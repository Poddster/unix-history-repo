#ifndef lint
static char sccsid[] = "@(#)condevs.c	5.8 (Berkeley) %G%";
#endif

/*
 * Here are various dialers to establish the machine-machine connection.
 * conn.c/condevs.c was glued together by Mike Mitchell.
 * The dialers were supplied by many people, to whom we are grateful.
 *
 * ---------------------------------------------------------------------
 * NOTE:
 * There is a bug that occurs at least on PDP11s due to a limitation of
 * setjmp/longjmp.   If the routine that does a setjmp is interrupted
 * and longjmp-ed to,  it loses its register variables (on a pdp11).
 * What works is if the routine that does the setjmp
 * calls a routine and it is the *subroutine* that is interrupted.
 *
 * Anyway, in conclusion, condevs.c is plagued with register variables
 * that are used inside
 * 	if (setjmp(...)) {
 * 		....
 * 	}
 *
 * THE FIX: Don't declare variables to be register
 */

#include "condevs.h"

struct condev condevs[] = {
	{ "DIR", "direct", diropn, nulldev, dircls },
#ifdef DATAKIT
	{ "DK", "datakit", dkopn, nulldev, nulldev },
#endif DATAKIT
#ifdef PNET
	{ "PNET", "pnet", pnetopn, nulldev, nulldev },
#endif PNET
#ifdef	UNETTCP
	{ "TCP", "TCP", unetopn, nulldev, unetcls },
#endif UNETTCP
#ifdef BSDTCP
	{ "TCP", "TCP", bsdtcpopn, nulldev, bsdtcpcls },
#endif BSDTCP
#ifdef MICOM
	{ "MICOM", "micom", micopn, nulldev, miccls },
#endif MICOM
#ifdef DN11
	{ "ACU", "dn11", Acuopn, dnopn, dncls },
#endif DN11
#ifdef HAYES
	{ "ACU", "hayes", Acuopn, hysopn, hyscls },
#endif HAYES
#ifdef HAYESQ	/* a version of hayes that doesn't use result codes */
	{ "ACU", "hayesq", Acuopn, hysqopn, hysqcls },
#endif HATESQ
#ifdef NOVATION
	{ "ACU", "novation", Acuopn, novopn, novcls},
#endif NOVATION
#ifdef DF02
	{ "ACU", "DF02", Acuopn, df2opn, df2cls },
#endif DF02
#ifdef VENTEL
	{ "ACU", "ventel", Acuopn, ventopn, ventcls },
#endif VENTEL
#ifdef VADIC
	{ "ACU", "vadic", Acuopn, vadopn, vadcls },
#endif VADIC
#ifdef VA212
	{ "ACU", "va212", Acuopn, va212opn, va212cls },
#endif VA212
#ifdef VA811S
	{ "ACU", "va811s", Acuopn, va811opn, va811cls },
#endif VA811S
#ifdef VA820
	{ "ACU", "va820", Acuopn, va820opn, va820cls },
	{ "WATS", "va820", Acuopn, va820opn, va820cls },
	{ "LOCAL", "va820", Acuopn, va820opn, va820cls },
#endif VA820
#ifdef RVMACS
	{ "ACU", "rvmacs", Acuopn, rvmacsopn, rvmacscls },
#endif RVMACS
#ifdef VMACS
	{ "ACU", "vmacs", Acuopn, vmacsopn, vmacscls },
#endif VMACS
#ifdef SYTEK
	{ "SYTEK", "sytek", sykopn, nulldev, sykcls },
#endif SYTEK

	/* Insert new entries before this line */
	{ NULL, NULL, NULL, NULL, NULL }
};

/*
 *	nulldev		a null device (returns CF_DIAL)
 */
nulldev()
{
	return CF_DIAL;
}

/*
 *	nodev		a null device (returns CF_NODEV)
 */
nodev()
{
	return CF_NODEV;
}

/*
 * Generic devices look through L-devices and call the CU_open routines for
 * appropriate devices.  Some things, like the tcp/ip interface, or direct
 * connect, do not use the CU_open entry.  ACUs must search to find the
 * right routine to call.
 */

/*
 *	diropn(flds)	connect to hardware line
 *
 *	return codes:
 *		> 0  -  file number  -  ok
 *		FAIL  -  failed
 */
diropn(flds)
register char *flds[];
{
	register int dcr, status;
	struct Devices dev;
	char dcname[20];
	FILE *dfp;
#ifdef VMSDTR	/* Modem control on vms(works dtr) */
	int modem_control;
	short iosb[4]; 
	int sys$qiow();	/* use this for long reads on vms */
	int ret;
	long mode[2];
	modem_control = 0;
#endif
	dfp = fopen(DEVFILE, "r");
	ASSERT(dfp != NULL, "CAN'T OPEN", DEVFILE, 0);
	while ((status = rddev(dfp, &dev)) != FAIL) {
#ifdef VMSDTR	/* Modem control on vms(works dtr) */
		/* If we find MOD in the device type field we go into action */
		if (strcmp(dev.D_type, "MOD") == SAME) {
			modem_control = 1;
		        DEBUG(7, "Setting Modem control to %d",modem_control);
		}
		if (strcmp(flds[F_CLASS], dev.D_class) != SAME)
				continue;
		/*
		 * Modem control on vms(works dtr) Take anything in MOD class.
	  	 * It probably should work differently anyway so we can have
		 *  multiple hardwired lines.
		 */
		if (!modem_control&&strcmp(flds[F_PHONE], dev.D_line) != SAME)
#else !VMSDTR
		if (strcmp(flds[F_CLASS], dev.D_class) != SAME)
			continue;
		if (strcmp(flds[F_PHONE], dev.D_line) != SAME)
#endif !VMSDTR
			continue;
		if (mlock(dev.D_line) != FAIL)
			break;
	}
	fclose(dfp);
	if (status == FAIL) {
		logent("DEVICE", "NO");
		return CF_NODEV;
	}

	sprintf(dcname, "/dev/%s", dev.D_line);
	if (setjmp(Sjbuf)) {
		delock(dev.D_line);
		return CF_DIAL;
	}
	signal(SIGALRM, alarmtr);
	alarm(10);
	getnextfd();
	errno = 0;
        DEBUG(4,"Opening %s",dcname);
	dcr = open(dcname, 2); /* read/write */
#ifdef VMSDTR	/* Modem control on vms(works dtr) */
	fflush(stdout);
	if (modem_control) { /* Did we have MOD in the device type field ? */
		/* Sense the current terminal setup and save it */
		if ((ret = sys$qiow(_$EFN,(fd_fab_pointer[dcr]->fab).fab$l_stv,
			IO$_SENSEMODE,iosb,0,0,mode,8,0,0,0,0))
				!= SS$_NORMAL) {
			DEBUG(7, "ret status on sense failed on Modem sense=%x<", ret);
			return CF_DIAL;
		}
		mode[1] |= TT$M_MODEM; /* Or in modem control(DTR) */
		/* Now set the new terminal characteristics */
		/* This is temporary and will go away when we let go of it */
		if ((ret = sys$qiow(_$EFN,(fd_fab_pointer[dcr]->fab).fab$l_stv,
			IO$_SETMODE,iosb,0,0,mode,8,0,0,0,0))
				!= SS$_NORMAL) {
			DEBUG(7, "ret status on sense failed on Modem setup=%x<", ret);
			return CF_DIAL;
		}
	}
#endif VMSDTR
	next_fd = -1;
	if (dcr < 0 && errno == EACCES)
		logent(dcname, "CAN'T OPEN");
	alarm(0);
	if (dcr < 0) {
		delock(dev.D_line);
		return CF_DIAL;
	}
	fflush(stdout);
	fixline(dcr, dev.D_speed);
	strcpy(devSel, dev.D_line);	/* for latter unlock */
	CU_end = dircls;
	return dcr;
}

dircls(fd)
register int fd;
{
	if (fd > 0) {
		close(fd);
		delock(devSel);
	}
}

/***
 *	Acuopn - open an ACU and dial the number.  The condevs table
 *		will be searched until a dialing unit is found that is
 *		free.
 *
 *	return codes:	>0 - file number - o.k.
 *			FAIL - failed
 */
char devSel[20];	/* used for later unlock() */

Acuopn(flds)
register char *flds[];
{
    char phone[MAXPH+1];
    register struct condev *cd;
    register int fd, acustatus;
    register FILE *dfp;
    struct Devices dev;
    int retval = CF_NODEV;

    exphone(flds[F_PHONE], phone);
    devSel[0] = '\0';
    DEBUG(4, "Dialing %s\n", phone);
    dfp = fopen(DEVFILE, "r");
    ASSERT(dfp != NULL, "Can't open", DEVFILE, 0);

    acustatus = 0;	/* none found, none locked */
    for(cd = condevs; cd->CU_meth != NULL; cd++) {
	if (snccmp(flds[F_LINE], cd->CU_meth) == SAME) {
	    rewind(dfp);
	    while(rddev(dfp, &dev) != FAIL) {
		/*
		 * for each ACU L.sys line, try at most twice
		 * (TRYCALLS) to establish carrier.  The old way tried every
		 * available dialer, which on big sites takes forever!
		 * Sites with a single auto-dialer get one try.
		 * Sites with multiple dialers get a try on each of two
		 * different dialers.
		 * To try 'harder' to connect to a remote site,
		 * use multiple L.sys entries.
		 */
		if (acustatus > TRYCALLS)
			continue;
		if (strcmp(flds[F_CLASS], dev.D_class) != SAME)
		    continue;
		if (snccmp(flds[F_LINE], dev.D_type) != SAME)
		    continue;
		if (dev.D_brand[0] == '\0')
		    logent("Acuopn","No 'brand' name on ACU");
		else if (snccmp(dev.D_brand, cd->CU_brand) != SAME)
		    continue;
		if (acustatus < 1)
			acustatus = 1;	/* has been found */
		if (mlock(dev.D_line) == FAIL)
		    continue;

		DEBUG(4, "Using %s\n", cd->CU_brand);
		acustatus++;
		fd = (*(cd->CU_open))(phone, flds, &dev);
		if (fd > 0) {
		    CU_end = cd->CU_clos;   /* point CU_end at close func */
		    fclose(dfp);
		    strcpy(devSel, dev.D_line);   /* save for later unlock() */
		    return fd;
		    }
		delock(dev.D_line);
		retval = CF_DIAL;
		}
	    }
	}
    fclose(dfp);
    if (acustatus == 0)
	logent("L-devices", "No appropriate ACU");
    if (acustatus == 1)
	logent("DEVICE", "NO");
    return retval;
}

#if defined(VENTEL) || defined(NOVATION)
/*
 * uucpdelay:  delay execution for numerator/denominator seconds.
 */

#ifdef INTERVALTIMER
#define uucpdelay(num,denom) intervaldelay(1000000*num/denom)
#include <sys/time.h>
catch alarm sig
SIGALRM
struct itimerval itimerval;
itimerval.itimer_reload =
itimerval.rtime.itimer_interval =
itimerval.rtime.itimer_value =
settimer(ITIMER_REAL, &itimerval);
pause();
alarm comes in
turn off timer.
#endif INTERVALTIMER

#ifdef FASTTIMER
#define uucpdelay(num,denom) nap(60*num/denom)
/*	Sleep in increments of 60ths of second.	*/
nap (time)
	register int time;
{
	static int fd;

	if (fd == 0)
		fd = open (FASTTIMER, 0);

	read (fd, 0, time);
}
#endif FASTTIMER

#ifdef FTIME
#define uucpdelay(num,denom) ftimedelay(1000*num/denom)
#include <sys/timeb.h>
ftimedelay(n)
{
	static struct timeb loctime;
	register i = loctime.millitm;

	ftime(&loctime);
	while (abs((int)(loctime.millitm - i))<n) ftime(&loctime)
		;
}
#endif FTIME

#ifdef BUSYLOOP
#define uucpdelay(num,denom) busyloop(CPUSPEED*num/denom)
#define CPUSPEED 1000000	/* VAX 780 is 1MIPS */
#define	DELAY(n)	{ register long N = (n); while (--N > 0); }
busyloop(n)
{
	DELAY(n);
}
#endif BUSYLOOP

slowrite(fd, str)
register char *str;
{
	DEBUG(6, "slowrite ", CNULL);
	while (*str) {
		DEBUG(6, "%c", *str);
		uucpdelay(1,10);	/* delay 1/10 second */
		write(fd, str, 1);
		str++;
	}
	DEBUG(6, "\n", CNULL);
}
#endif VENTEL || NOVATION
