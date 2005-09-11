/*
 * Portions Copyright (c) 2005 Gunnar Ritter, Freiburg i. Br., Germany
 *
 * Sccsid @(#)proto.h	1.23 (gritter) 9/11/05
 */

/* t6.c */
int width(register tchar);
void zapwcache(int);
int getcw(register int);
int abscw(int);
int kernadjust(tchar, tchar);
void xbits(register tchar, int);
tchar setch(int);
tchar setabs(void);
int findft(register int);
void caseps(void);
void casps1(register int);
int findps(register int);
void mchbits(void);
void setps(void);
tchar setht(void);
tchar setslant(void);
void caseft(void);
void setfont(int);
void setwd(void);
tchar vmot(void);
tchar hmot(void);
tchar mot(void);
tchar sethl(int);
tchar makem(register int);
tchar getlg(tchar);
void caselg(void);
void caseflig(void);
void casefp(void);
int setfp(int, int, char *);
void casecs(void);
void casebd(void);
void casevs(void);
void casess(void);
tchar xlss(void);
void casetrack(void);
void casefallback(void);
void casehidechar(void);
void casefzoom(void);
void casekern(void);
void casepapersize(void);
void caseladj(void);
void caseradj(void);
int un2tr(int, int *);
int tr2un(tchar, int);
int pts2u(int);
double u2pts(int);
/* t10.c */
void ptinit(void);
void specnames(void);
int findch(register char *);
void ptout(register tchar);
tchar *ptout0(tchar *, tchar *);
void ptps(void);
void ptfont(void);
void ptfpcmd(int, char *);
void ptlead(void);
void ptesc(void);
void newpage(int);
void ptsupplyfont(char *, char *);
void pttrailer(void);
void ptstop(void);
void dostop(void);
