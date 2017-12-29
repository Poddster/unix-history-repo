/*-
 * Copyright (c) 2017 Netflix, Inc
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <sys/cdefs.h>
__FBSDID("$FreeBSD$");

#include <sys/param.h>
#include <ctype.h>
#include <devinfo.h>
#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/linker.h>
#include <sys/module.h>
#include <sys/stat.h>
#include <sys/sysctl.h>

/* options descriptor */
static struct option longopts[] = {
	{ "all",		no_argument,		NULL,	'a' },
	{ "dump",		no_argument,		NULL,	'd' },
	{ "unbound",		no_argument,		NULL,	'u' },
	{ "verbose",		no_argument,		NULL,	'v' },
	{ NULL,			0,			NULL,	0 }
};

static int all_flag;
static int dump_flag;
static int unbound_flag;
static int verbose_flag;

static void *hints;
static void *hints_end;

static void
read_linker_hints(void)
{
	char fn[MAXPATHLEN];
	struct stat sb;
	char *modpath, *p, *q;
	size_t buflen;
	int fd;

	if (sysctlbyname("kern.module_path", NULL, &buflen, NULL, 0) < 0)
		errx(1, "Can't find kernel module path.");
	modpath = malloc(buflen);
	if (modpath == NULL)
		err(1, "Can't get memory for modpath.");
	if (sysctlbyname("kern.module_path", modpath, &buflen, NULL, 0) < 0)
		errx(1, "Can't find kernel module path.");
	p = modpath;
	while ((q = strsep(&p, ";")) != NULL) {
		snprintf(fn, sizeof(fn), "%s/linker.hints", q);
		fd = open(fn, O_RDONLY);
		if (fd < 0) {
			if (errno == ENOENT)
				continue;
			err(1, "Can't open %s for reading", fn);
		}
		if (fstat(fd, &sb) != 0)
			err(1, "Can't fstat %s\n", fn);
		hints = malloc(sb.st_size);
		if (hints == NULL)
			err(1, "not enough space to read hints file of %ju bytes", (uintmax_t)sb.st_size);
		if (read(fd, hints, sb.st_size) != sb.st_size)
			err(1, "Can't read in %ju bytes from %s", (uintmax_t)sb.st_size, fn);
		close(fd);
		break;
	}
	if (q == NULL) {
		warnx("Can't read linker hints file.");
		free(hints);
		hints = NULL;
		return;
	}
	if (*(int *)(intptr_t)hints != LINKER_HINTS_VERSION) {
		warnx("Linker hints version %d doesn't match expected %d.",
		    *(int *)(intptr_t)hints, LINKER_HINTS_VERSION);
		free(hints);
		hints = NULL;
	}
	if (hints != NULL)
		hints_end = (void *)((intptr_t)hints + (intptr_t)sb.st_size);
}

static int
getint(void **ptr)
{
	int *p = *ptr;
	int rv;

	p = (int *)roundup2((intptr_t)p, sizeof(int));
	rv = *p++;
	*ptr = p;
	return rv;
}

static void
getstr(void **ptr, char *val)
{
	int *p = *ptr;
	char *c = (char *)p;
	int len = *(uint8_t *)c;

	memcpy(val, c + 1, len);
	val[len] = 0;
	c += len + 1;
	*ptr = (void *)c;
}

static int
pnpval_as_int(const char *val, const char *pnpinfo)
{
	int rv;
	char key[256];
	char *cp;

	if (pnpinfo == NULL)
		return -1;

	cp = strchr(val, ';');
	key[0] = ' ';
	if (cp == NULL)
		strlcpy(key + 1, val, sizeof(key) - 1);
	else {
		memcpy(key + 1, val, cp - val);
		key[cp - val + 1] = '\0';
	}
	strlcat(key, "=", sizeof(key));
	if (strncmp(key + 1, pnpinfo, strlen(key + 1)) == 0)
		rv = strtol(pnpinfo + strlen(key + 1), NULL, 0);
	else {
		cp = strstr(pnpinfo, key);
		if (cp == NULL)
			rv = -1;
		else
			rv = strtol(cp + strlen(key), NULL, 0);
	}
	return rv;
}

static void
quoted_strcpy(char *dst, const char *src)
{
	char q = ' ';

	if (*src == '\'' || *src == '"')
		q = *src++;
	while (*src && *src != q)
		*dst++ = *src++; // XXX backtick quoting
	*dst++ = '\0';
	// XXX overflow
}

static char *
pnpval_as_str(const char *val, const char *pnpinfo)
{
	static char retval[256];
	char key[256];
	char *cp;

	if (pnpinfo == NULL) {
		*retval = '\0';
		return retval;
	}

	cp = strchr(val, ';');
	key[0] = ' ';
	if (cp == NULL)
		strlcpy(key + 1, val, sizeof(key) - 1);
	else {
		memcpy(key + 1, val, cp - val);
		key[cp - val + 1] = '\0';
	}
	strlcat(key, "=", sizeof(key));
	if (strncmp(key + 1, pnpinfo, strlen(key + 1)) == 0)
		quoted_strcpy(retval, pnpinfo + strlen(key + 1));
	else {
		cp = strstr(pnpinfo, key);
		if (cp == NULL)
			strcpy(retval, "MISSING");
		else
			quoted_strcpy(retval, cp + strlen(key));
	}
	return retval;
}

static void
search_hints(const char *bus, const char *dev, const char *pnpinfo)
{
	char val1[256], val2[256];
	int ival, len, ents, i, notme, mask, bit, v, found;
	void *ptr, *walker;
	char *lastmod = NULL, *cp, *s;

	walker = hints;
	getint(&walker);
	found = 0;
	while (walker < hints_end) {
		len = getint(&walker);
		ival = getint(&walker);
		ptr = walker;
		switch (ival) {
		case MDT_VERSION:
			getstr(&ptr, val1);
			ival = getint(&ptr);
			getstr(&ptr, val2);
			if (dump_flag)
				printf("Version: if %s.%d kmod %s\n", val1, ival, val2);
			break;
		case MDT_MODULE:
			getstr(&ptr, val1);
			getstr(&ptr, val2);
			if (lastmod)
				free(lastmod);
			lastmod = strdup(val2);
			if (dump_flag)
				printf("Module %s in %s\n", val1, val2);
			break;
		case MDT_PNP_INFO:
			if (!dump_flag && !unbound_flag && lastmod && strcmp(lastmod, "kernel") == 0)
				break;
			getstr(&ptr, val1);
			getstr(&ptr, val2);
			ents = getint(&ptr);
			if (bus && strcmp(val1, bus) != 0)
				break;
			if (dump_flag)
				printf("PNP info for bus %s format %s %d entries (%s)\n",
				    val1, val2, ents, lastmod);
			for (i = 0; i < ents; i++) {
				if (dump_flag)
					printf("   ");
				cp = val2;
				notme = 0;
				mask = -1;
				bit = -1;
				do {
					switch (*cp) {
					case 'I':
					case 'J':
					case 'G':
					case 'L':
					case 'M':
						ival = getint(&ptr);
						if (dump_flag) {
							printf("%#x:", ival);
							break;
						}
						if (bit >= 0 && ((1 << bit) & mask) == 0)
							break;
						v = pnpval_as_int(cp + 2, pnpinfo);
						switch (*cp) {
						case 'J':
							if (ival == -1)
								break;
							/*FALLTHROUGH*/
						case 'I':
							if (v != ival && ival != 0)
								notme++;
							break;
						case 'G':
							if (v < ival)
								notme++;
							break;
						case 'L':
							if (v > ival)
								notme++;
							break;
						case 'M':
							mask = ival;
							break;
						}
						break;
					case 'D':
					case 'Z':
						getstr(&ptr, val1);
						if (dump_flag) {
							printf("'%s':", val1);
							break;
						}
						if (*cp == 'D')
							break;
						s = pnpval_as_str(cp + 2, pnpinfo);
						if (strcmp(s, val1) != 0)
							notme++;
						break;
					default:
						break;
					}
					bit++;
					cp = strchr(cp, ';');
					if (cp)
						cp++;
				} while (cp && *cp);
				if (dump_flag)
					printf("\n");
				else if (!notme) {
					if (!unbound_flag) {
						if (all_flag)
							printf("%s: ", *dev ? dev : "unattached" );
						printf("%s\n", lastmod);
					}
					found++;
				}
			}
			break;
		default:
			if (dump_flag)
				printf("Unknown Type %d len %d\n", ival, len);
			break;
		}
		walker = (void *)(len - sizeof(int) + (intptr_t)walker);
	}
	if (unbound_flag && found == 0 && *pnpinfo) {
		if (verbose_flag)
			printf("------------------------- ");
		printf("%s on %s pnpinfo %s", *dev ? dev : "unattached", bus, pnpinfo);
		if (verbose_flag)
			printf(" -------------------------");
		printf("\n");
	}
	free(lastmod);
}

static int
find_unmatched(struct devinfo_dev *dev, void *arg)
{
	struct devinfo_dev *parent;
	char *bus, *p;

	do {
		if (!all_flag && dev->dd_name[0] != '\0')
			break;
		if (!(dev->dd_flags & DF_ENABLED))
			break;
		parent = devinfo_handle_to_device(dev->dd_parent);
		bus = strdup(parent->dd_name);
		p = bus + strlen(bus) - 1;
		while (p >= bus && isdigit(*p))
			p--;
		*++p = '\0';
		if (verbose_flag)
			printf("Searching %s %s bus at %s for pnpinfo %s\n",
			    dev->dd_name, bus, dev->dd_location, dev->dd_pnpinfo);
		search_hints(bus, dev->dd_name, dev->dd_pnpinfo);
		free(bus);
	} while (0);

	return (devinfo_foreach_device_child(dev, find_unmatched, arg));
}

static void
usage(void)
{

	errx(1, "devmatch [-adv]");
}

int
main(int argc, char **argv)
{
	struct devinfo_dev *root;
	int ch;

	while ((ch = getopt_long(argc, argv, "aduv",
		    longopts, NULL)) != -1) {
		switch (ch) {
		case 'a':
			all_flag++;
			break;
		case 'd':
			dump_flag++;
			break;
		case 'u':
			unbound_flag++;
			break;
		case 'v':
			verbose_flag++;
			break;
		default:
			usage();
		}
	}
	argc -= optind;
	argv += optind;

	if (argc >= 1)
		usage();

	read_linker_hints();
	if (dump_flag) {
		search_hints(NULL, NULL, NULL);
		exit(0);
	}

	if (devinfo_init())
		err(1, "devinfo_init");
	if ((root = devinfo_handle_to_device(DEVINFO_ROOT_DEVICE)) == NULL)
		errx(1, "can't find root device");
	devinfo_foreach_device_child(root, find_unmatched, (void *)0);
	devinfo_free();
}
