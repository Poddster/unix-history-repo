/*
 * Copyright (c) 1997-2001 Erez Zadok
 * Copyright (c) 1990 Jan-Simon Pendry
 * Copyright (c) 1990 Imperial College of Science, Technology & Medicine
 * Copyright (c) 1990 The Regents of the University of California.
 * All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Jan-Simon Pendry at Imperial College, London.
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
 *    must display the following acknowledgment:
 *      This product includes software developed by the University of
 *      California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *      %W% (Berkeley) %G%
 *
 * $Id: amfs_linkx.c,v 1.3.2.1 2001/01/10 03:23:02 ezk Exp $
 *
 */

/*
 * Symbol-link file system, with test that the target of the link exists.
 */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif /* HAVE_CONFIG_H */
#include <am_defs.h>
#include <amd.h>

/* forward declarations */
static int amfs_linkx_mount(am_node *mp);

/*
 * linkx operations
 */
struct am_ops amfs_linkx_ops =
{
  "linkx",
  amfs_link_match,
  0,				/* amfs_linkx_init */
  amfs_linkx_mount,
  0,
  amfs_auto_fumount,
  amfs_link_fumount,
  amfs_error_lookuppn,
  amfs_error_readdir,
  0,				/* amfs_linkx_readlink */
  0,				/* amfs_linkx_mounted */
  0,				/* amfs_linkx_umounted */
  find_amfs_auto_srvr,
  FS_MBACKGROUND
};


static int
amfs_linkx_mount(am_node *mp)
{
  /*
   * Check for existence of target.
   */
  struct stat stb;
  char *ln;

  if (mp->am_link)
    ln = mp->am_link;
  else				/* should never occur */
    ln = mp->am_mnt->mf_mount;

  /*
   * Use lstat, not stat, since we don't
   * want to know if the ultimate target of
   * a symlink chain exists, just the first.
   */
  if (lstat(ln, &stb) < 0)
    return errno;

  return 0;
}
