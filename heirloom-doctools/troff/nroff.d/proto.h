/*
 * Portions Copyright (c) 2005 Gunnar Ritter, Freiburg i. Br., Germany
 *
 * Sccsid @(#)proto.h	1.8 (gritter) 8/21/05
 */

/* n10.c */
void ptinit(void);
char *skipstr(char *);
char *getstr(char *, char *);
char *getint(char *, int *);
void specnames(void);
int findch(register char *);
void twdone(void);
void ptout1(void);
char *plot(char *);
void move(void);
void ptlead(void);
void dostop(void);
void newpage();
void pttrailer();
/* n6.c */
int width(register tchar);
tchar setch(int);
tchar setabs(void);
int findft(register int);
void caseps(void);
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
tchar makem(int);
tchar getlg(tchar);
void caselg(void);
void casefp(void);
void casecs(void);
void casebd(void);
void casevs(void);
void casess(void);
tchar xlss(void);
void casefpost(void);
void casetkf(void);
void casefallback(void);
void casehidechar(void);
void casefzoom(void);
