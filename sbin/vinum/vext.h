/*-
 * Copyright (c) 1997, 1998
 *	Nan Yang Computer Services Limited.  All rights reserved.
 *
 *  This software is distributed under the so-called ``Berkeley
 *  License'':
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by Nan Yang Computer
 *      Services Limited.
 * 4. Neither the name of the Company nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * This software is provided ``as is'', and any express or implied
 * warranties, including, but not limited to, the implied warranties of
 * merchantability and fitness for a particular purpose are disclaimed.
 * In no event shall the company or contributors be liable for any
 * direct, indirect, incidental, special, exemplary, or consequential
 * damages (including, but not limited to, procurement of substitute
 * goods or services; loss of use, data, or profits; or business
 * interruption) however caused and on any theory of liability, whether
 * in contract, strict liability, or tort (including negligence or
 * otherwise) arising in any way out of the use of this software, even if
 * advised of the possibility of such damage.
 *
 */

/* $Id: vext.h,v 1.11 1999/03/02 04:10:00 grog Exp grog $ */

#define MAXARGS 64					    /* maximum number of args on a line */
#define PLEXINITSIZE 61440				    /* this is what the system does somewhere */

enum {
    KILOBYTE = 1024,
    MEGABYTE = 1048576,
    GIGABYTE = 1073741824
};

#if RAID5
#define VINUMMOD "Vinum"
#define WRONGMOD "vinum"				    /* don't want this one */
#else
#define VINUMMOD "vinum"
#define WRONGMOD "Vinum"				    /* don't want this one */
#endif

/* Prototype declarations */
void parseline(int c, char *args[]);			    /* parse a line with c parameters at args */
void checkentry(int index);
int haveargs(int);					    /* check arg, error message if not valid */
void vinum_create(int argc, char *argv[], char *arg0[]);
void vinum_read(int argc, char *argv[], char *arg0[]);
void vinum_modify(int argc, char *argv[], char *arg0[]);
void vinum_volume(int argc, char *argv[], char *arg0[]);
void vinum_plex(int argc, char *argv[], char *arg0[]);
void vinum_sd(int argc, char *argv[], char *arg0[]);
void vinum_drive(int argc, char *argv[], char *arg0[]);
void vinum_list(int argc, char *argv[], char *arg0[]);
void vinum_info(int argc, char *argv[], char *arg0[]);
void vinum_set(int argc, char *argv[], char *arg0[]);
void vinum_rm(int argc, char *argv[], char *arg0[]);
void vinum_init(int argc, char *argv[], char *arg0[]);
void vinum_resetconfig(int argc, char *argv[], char *arg0[]);
void vinum_start(int argc, char *argv[], char *arg0[]);
void continue_revive(int plexno);
void vinum_stop(int argc, char *argv[], char *arg0[]);
void vinum_makedev(int argc, char *argv[], char *arg0[]);
void vinum_help(int argc, char *argv[], char *arg0[]);
void vinum_quit(int argc, char *argv[], char *arg0[]);
void vinum_setdaemon(int argc, char *argv[], char *arg0[]);
void reset_volume_stats(int volno, int recurse);
void reset_plex_stats(int plexno, int recurse);
void reset_sd_stats(int sdno, int recurse);
void reset_drive_stats(int driveno);
void vinum_resetstats(int argc, char *argv[], char *arg0[]);
void vinum_attach(int argc, char *argv[], char *argv0[]);
void vinum_detach(int argc, char *argv[], char *argv0[]);
void vinum_rename(int argc, char *argv[], char *argv0[]);
void vinum_rename_2(char *, char *);
void vinum_replace(int argc, char *argv[], char *argv0[]);
void vinum_printconfig(int argc, char *argv[], char *argv0[]);
void vinum_saveconfig(int argc, char *argv[], char *argv0[]);
void vinum_label(int argc, char *argv[], char *arg0[]);
void vinum_ld(int argc, char *argv[], char *arg0[]);
void vinum_ls(int argc, char *argv[], char *arg0[]);
void vinum_lp(int argc, char *argv[], char *arg0[]);
void vinum_lv(int argc, char *argv[], char *arg0[]);
void start_daemon(void);
#ifdef VINUMDEBUG
void vinum_debug(int argc, char *argv[], char *arg0[]);
#endif
void make_devices(void);
void get_drive_info(struct drive *drive, int index);
void get_sd_info(struct sd *sd, int index);
void get_plex_sd_info(struct sd *sd, int plexno, int sdno);
void get_plex_info(struct plex *plex, int index);
void get_volume_info(struct volume *volume, int index);
int find_object(const char *name, enum objecttype *type);
char *lltoa(long long l, char *s);
void vinum_ldi(int, int);
void vinum_lvi(int, int);
void vinum_lpi(int, int);
void vinum_lsi(int, int);
int vinum_li(int object, enum objecttype type);
char *roughlength(long long bytes, int);
u_int64_t sizespec(char *spec);

extern int force;					    /* set to 1 to force some dangerous ops */
extern int verbose;					    /* set verbose operation */
extern int Verbose;					    /* very verbose operation */
extern int recurse;					    /* set recursion */
extern int stats;					    /* show statistics */
extern int dowait;					    /* wait for children to exit */

/* Structures to read kernel data into */
extern struct _vinum_conf vinum_conf;			    /* configuration information */

extern struct volume vol;
extern struct plex plex;
extern struct sd sd;
extern struct drive drive;

extern jmp_buf command_fail;				    /* return on a failed command */
extern int superdev;					    /* vinum super device */

extern int line;					    /* stdin line number for error messages */
extern int file_line;					    /* and line in input file (yes, this is tacky) */

extern char buffer[];					    /* buffer to read in to */
