/*
 * Copyright (c) 1994 The Regents of the University of California.
 * Copyright (c) 1994 Jan-Simon Pendry.
 * All rights reserved.
 *
 * This code is derived from software donated to Berkeley by
 * Jan-Simon Pendry.
 *
 * %sccs.include.redist.c%
 *
 *	@(#)union.h	8.8 (Berkeley) %G%
 */

struct union_args {
	char		*target;	/* Target of loopback  */
	int		mntflags;	/* Options on the mount */
};

#define UNMNT_ABOVE	0x0001		/* Target appears below mount point */
#define UNMNT_BELOW	0x0002		/* Target appears below mount point */
#define UNMNT_REPLACE	0x0003		/* Target replaces mount point */
#define UNMNT_OPMASK	0x0003

struct union_mount {
	struct vnode	*um_uppervp;
	struct vnode	*um_lowervp;
	struct ucred	*um_cred;	/* Credentials of user calling mount */
	int		um_cmode;	/* cmask from mount process */
	int		um_op;		/* Operation mode */
};

#ifdef KERNEL

/*
 * DEFDIRMODE is the mode bits used to create a shadow directory.
 */
#define VRWXMODE (VREAD|VWRITE|VEXEC)
#define VRWMODE (VREAD|VWRITE)
#define UN_DIRMODE ((VRWXMODE)|(VRWXMODE>>3)|(VRWXMODE>>6))
#define UN_FILEMODE ((VRWMODE)|(VRWMODE>>3)|(VRWMODE>>6))

/*
 * A cache of vnode references
 */
struct union_node {
	LIST_ENTRY(union_node)	un_cache;	/* Hash chain */
	struct vnode		*un_vnode;	/* Back pointer */
	struct vnode	        *un_uppervp;	/* overlaying object */
	struct vnode	        *un_lowervp;	/* underlying object */
	struct vnode		*un_dirvp;	/* Parent dir of uppervp */
	struct vnode		*un_pvp;	/* Parent vnode */
	char			*un_path;	/* saved component name */
	int			un_hash;	/* saved un_path hash value */
	int			un_openl;	/* # of opens on lowervp */
	unsigned int		un_flags;
	off_t			un_uppersz;	/* size of upper object */
	off_t			un_lowersz;	/* size of lower object */
#ifdef DIAGNOSTIC
	pid_t			un_pid;
#endif
};

#define UN_WANT		0x01
#define UN_LOCKED	0x02
#define UN_ULOCK	0x04		/* Upper node is locked */
#define UN_KLOCK	0x08		/* Keep upper node locked on vput */
#define UN_CACHED	0x10		/* In union cache */

extern int union_allocvp __P((struct vnode **, struct mount *,
				struct vnode *, struct vnode *,
				struct componentname *, struct vnode *,
				struct vnode *));
extern int union_copyfile __P((struct vnode *, struct vnode *,
					struct ucred *, struct proc *));
extern int union_copyup __P((struct union_node *, int, struct ucred *,
				struct proc *));
extern int union_dowhiteout __P((struct union_node *, struct ucred *,
					struct proc *));
extern int union_mkshadow __P((struct union_mount *, struct vnode *,
				struct componentname *, struct vnode **));
extern int union_mkwhiteout __P((struct union_mount *, struct vnode *,
				struct componentname *, char *));
extern int union_vn_create __P((struct vnode **, struct union_node *,
				struct proc *));
extern int union_cn_close __P((struct vnode *, int, struct ucred *,
				struct proc *));
extern void union_removed_upper __P((struct union_node *un));
extern struct vnode *union_lowervp __P((struct vnode *));
extern void union_newlower __P((struct union_node *, struct vnode *));
extern void union_newupper __P((struct union_node *, struct vnode *));
extern void union_newsize __P((struct vnode *, off_t, off_t));

#define	MOUNTTOUNIONMOUNT(mp) ((struct union_mount *)((mp)->mnt_data))
#define	VTOUNION(vp) ((struct union_node *)(vp)->v_data)
#define	UNIONTOV(un) ((un)->un_vnode)
#define	LOWERVP(vp) (VTOUNION(vp)->un_lowervp)
#define	UPPERVP(vp) (VTOUNION(vp)->un_uppervp)
#define OTHERVP(vp) (UPPERVP(vp) ? UPPERVP(vp) : LOWERVP(vp))

extern int (**union_vnodeop_p)();
extern struct vfsops union_vfsops;
#endif /* KERNEL */
