/*
 * CDDL HEADER START
 *
 * The contents of this file are subject to the terms of the
 * Common Development and Distribution License, Version 1.0 only
 * (the "License").  You may not use this file except in compliance
 * with the License.
 *
 * You can obtain a copy of the license at usr/src/OPENSOLARIS.LICENSE
 * or http://www.opensolaris.org/os/licensing.
 * See the License for the specific language governing permissions
 * and limitations under the License.
 *
 * When distributing Covered Code, include this CDDL HEADER in each
 * file and include the License file at usr/src/OPENSOLARIS.LICENSE.
 * If applicable, add the following below this CDDL HEADER, with the
 * fields enclosed by brackets "[]" replaced with your own identifying
 * information: Portions Copyright [yyyy] [name of copyright owner]
 *
 * CDDL HEADER END
 */
/*
 * Copyright (c) 2005 Gunnar Ritter, Freiburg i. Br., Germany
 *
 * Sccsid @(#)main.c	1.2 (gritter) 6/14/05
 */
#if __GNUC__ >= 3 && __GNUC_MINOR__ >= 4 || __GNUC__ >= 4
#define	USED	__attribute__ ((used))
#elif defined __GNUC__
#define	USED	__attribute__ ((unused))
#else
#define	USED
#endif
static const char id[] USED = "@(#)sh.sl	1.51 (gritter) 7/3/05";
/* SLIST */
/*
args.c: * Sccsid @(#)args.c	1.5 (gritter) 6/16/05
blok.c: * Sccsid @(#)blok.c	1.7 (gritter) 6/16/05
bltin.c: * Sccsid @(#)bltin.c	1.10 (gritter) 7/3/05
brkincr.h: * Sccsid @(#)brkincr.h	1.4 (gritter) 6/15/05
cmd.c: * Sccsid @(#)cmd.c	1.4 (gritter) 6/15/05
ctype.c: * Sccsid @(#)ctype.c	1.5 (gritter) 6/15/05
ctype.h: * Sccsid @(#)ctype.h	1.5 (gritter) 6/15/05
defs.c: * Sccsid @(#)defs.c	1.4 (gritter) 6/15/05
defs.h: * Sccsid @(#)defs.h	1.20 (gritter) 6/30/05
dup.h: * Sccsid @(#)dup.h	1.4 (gritter) 6/15/05
echo.c: * Sccsid @(#)echo.c	1.9 (gritter) 7/2/05
error.c: * Sccsid @(#)error.c	1.6 (gritter) 6/22/05
expand.c: * Sccsid @(#)expand.c	1.6 (gritter) 6/22/05
fault.c: * Sccsid @(#)fault.c	1.11 (gritter) 6/22/05
func.c: * Sccsid @(#)func.c	1.4 (gritter) 6/15/05
getopt.c: * Sccsid @(#)getopt.c	1.7 (gritter) 6/22/05
hash.c: * Sccsid @(#)hash.c	1.6 (gritter) 6/26/05
hash.h: * Sccsid @(#)hash.h	1.4 (gritter) 6/15/05
hashserv.c: * Sccsid @(#)hashserv.c	1.4 (gritter) 6/15/05
io.c: * Sccsid @(#)io.c	1.5 (gritter) 6/15/05
jobs.c: * Sccsid @(#)jobs.c	1.13 (gritter) 6/23/05
mac.h: * Sccsid @(#)mac.h	1.6 (gritter) 6/19/05
macro.c: * Sccsid @(#)macro.c	1.8 (gritter) 6/16/05
main.c: * Sccsid @(#)main.c	1.8 (gritter) 6/22/05
mode.h: * Sccsid @(#)mode.h	1.5 (gritter) 6/15/05
msg.c: * Sccsid @(#)msg.c	1.10 (gritter) 6/17/05
name.c: * Sccsid @(#)name.c	1.13 (gritter) 6/16/05
name.h: * Sccsid @(#)name.h	1.4 (gritter) 6/15/05
print.c: * Sccsid @(#)print.c	1.11 (gritter) 6/19/05
pwd.c: * Sccsid @(#)pwd.c	1.6 (gritter) 6/15/05
service.c: * Sccsid @(#)service.c	1.10 (gritter) 6/26/05
setbrk.c: * Sccsid @(#)setbrk.c	1.4 (gritter) 6/15/05
stak.c: * Sccsid @(#)stak.c	1.5 (gritter) 6/15/05
stak.h: * Sccsid @(#)stak.h	1.5 (gritter) 6/15/05
string.c: * Sccsid @(#)string.c	1.5 (gritter) 6/16/05
strsig.c: * Sccsid @(#)strsig.c	1.9 (gritter) 6/30/05
sym.h: * Sccsid @(#)sym.h	1.4 (gritter) 6/15/05
test.c: * Sccsid @(#)test.c	1.9 (gritter) 6/29/05
timeout.h: * Sccsid @(#)timeout.h	1.4 (gritter) 6/15/05
ulimit.c: * Sccsid @(#)ulimit.c	1.10 (gritter) 6/20/05
umask.c: * Sccsid @(#)umask.c	1.1 (gritter) 6/16/05
word.c: * Sccsid @(#)word.c	1.7 (gritter) 6/22/05
xec.c: * Sccsid @(#)xec.c	1.6 (gritter) 6/30/05
*/
