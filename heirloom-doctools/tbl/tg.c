/*	Copyright (c) 1984, 1986, 1987, 1988, 1989 AT&T	*/
/*	  All Rights Reserved  	*/


/*
 * Copyright (c) 1980 Regents of the University of California.
 * All rights reserved. The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 */
     
/*
 * Copyright (c) 1983, 1984 1985, 1986, 1987, 1988, Sun Microsystems, Inc.
 * All Rights Reserved.
 */
  
/*	from OpenSolaris "tg.c	1.6	05/06/02 SMI"	 SVr4.0 1.1		*/

/*
 * Portions Copyright (c) 2005 Gunnar Ritter, Freiburg i. Br., Germany
 *
 * Sccsid @(#)tg.c	1.5 (gritter) 8/12/05
 */

 /* tg.c: process included text blocks */
# include "t..c"
# include <stdlib.h>
/* get_text was originally gettext and was renamed */
int 
get_text(char *sp, int ilin, int icol, char *fn, char *sz)
{
/* get a section of text */
char *line = NULL;
size_t linesize = 0;
int oname;
char *vs;
if (texname==0) error("Too many text block diversions");
if (textflg==0)
	{
	fprintf(tabout, ".nr %d \\n(.lu\n", SL); /* remember old line length */
	textflg=1;
	}
fprintf(tabout, ".eo\n");
fprintf(tabout, ".am %02d\n", icol+80);
fprintf(tabout, ".br\n");
fprintf(tabout, ".di %c+\n", texname);
rstofill();
if (fn && *fn) fprintf(tabout, ".nr %d \\n(.f\n.ft %s\n", S1, fn);
fprintf(tabout, ".ft \\n(.f\n"); /* protect font */
vs = vsize[stynum[ilin]][icol];
if ((sz && *sz) || (vs && *vs))
	{
	fprintf(tabout, ".nr %d \\n(.v\n", S2);
	if (vs==0 || *vs==0) vs= "\\n(.s+2";
	if (sz && *sz)
		fprintf(tabout, ".ps %s\n",sz);
	fprintf(tabout, ".vs %s\n",vs);
	fprintf(tabout, ".if \\n(%du>\\n(.vu .sp \\n(%du-\\n(.vu\n", S2,S2);
	}
if (cll[icol][0])
	fprintf(tabout, ".ll %sn\n", cll[icol]);
else
	fprintf(tabout, ".ll \\n(%du*%du/%du\n",SL,ctspan(ilin,icol),ncol+1);
fprintf(tabout,".if \\n(.l<\\n(%d .ll \\n(%du\n", icol+CRIGHT, icol+CRIGHT);
if (ctype(ilin,icol)=='a')
	fprintf(tabout, ".ll -2n\n");
fprintf(tabout, ".in 0\n");
while (gets1(&line, &linesize))
	{
	if (line[0]=='T' && line[1]=='}' && line[2]== tab) break;
	if (match("T}", line)) break;
	fprintf(tabout, "%s\n", line);
	}
if (fn && *fn) fprintf(tabout, ".ft \\n(%d\n", S1);
if (sz && *sz) fprintf(tabout, ".br\n.ps\n.vs\n");
fprintf(tabout, ".br\n");
fprintf(tabout, ".di\n");
fprintf(tabout, ".nr %c| \\n(dn\n", texname);
fprintf(tabout, ".nr %c- \\n(dl\n", texname);
fprintf(tabout, "..\n");
fprintf(tabout, ".ec \\\n");
/* copy remainder of line */
if (line[2])
	tcopy (sp, line+3);
else
	*sp=0;
oname=texname;
texname = texstr[++texct];
free(line);
return(oname);
}
void
untext(void)
{
rstofill();
fprintf(tabout, ".nf\n");
fprintf(tabout, ".ll \\n(%du\n", SL);
}
