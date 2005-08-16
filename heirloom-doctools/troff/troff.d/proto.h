/*
 * Portions Copyright (c) 2005 Gunnar Ritter, Freiburg i. Br., Germany
 *
 * Sccsid @(#)proto.h	1.2 (gritter) 8/16/05
 */

/* t6.c */
int width(register tchar);
void zapwcache(int);
int getcw(register int);
int abscw(int);
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
void casefp(void);
int setfp(int, int, char *);
void casecs(void);
void casebd(void);
void casevs(void);
void casess(void);
tchar xlss(void);
/* t10.c */
void ptinit(void);
void specnames(void);
int findch(register char *);
void ptout(register tchar);
tchar *ptout0(tchar *);
void ptps(void);
void ptfont(void);
void ptfpcmd(int, char *);
void ptlead(void);
void ptesc(void);
void newpage(int);
void pttrailer(void);
void ptstop(void);
void dostop(void);
