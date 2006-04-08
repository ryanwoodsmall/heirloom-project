/*
 * Portions Copyright (c) 2005 Gunnar Ritter, Freiburg i. Br., Germany
 *
 * Sccsid @(#)proto.h	1.32 (gritter) 4/8/06
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
int getascender(void);
int getdescender(void);
tchar setch(int);
tchar setabs(void);
int findft(register int, int);
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
void caseflig(void);
void casefp(void);
void casefps(void);
void casecs(void);
void casebd(void);
void casevs(void);
void casess(void);
tchar xlss(void);
tchar setuc0(int);
void casedummy(void);
#define	casetrack	casedummy
#define	casefallback	casedummy
#define	casehidechar	casedummy
#define	casefzoom	casedummy
#define	casekern	casedummy
#define	casepapersize	casedummy
#define	casemediasize	casedummy
#define	caselhang	casedummy
#define	caserhang	casedummy
#define	casekernpair	casedummy
#define	casekernbefore	casedummy
#define	casekernafter	casedummy
#define	caseftr		casedummy
#define	casefeature	casedummy
#define	casetrimat	casedummy
#define	casebleedat	casedummy
#define	casecropat	casedummy
#define	casefspacewidth	casedummy
#define	casefdeferlig	casedummy

#define	kernadjust(a, b)	0
#define	u2pts(i)		(i)
