/*
 *	Portions Copyright (c) 2005 Gunnar Ritter, Freiburg i. Br., Germany
 *
 *	Sccsid @(#)pt.h	1.37 (gritter) 7/3/06
 */

/* n10.c */
extern void	ptinit(void);
extern char	*skipstr(char *);
extern char	*getstr(char *, char *);
extern char	*getint(char *, int *);
extern void	specnames(void);
extern int	findch(register char *);
extern void	twdone(void);
extern void	ptout1(void);
extern char	*plot(char *);
extern void	move(void);
extern void	ptlead(void);
extern void	dostop(void);
extern void	newpage();
extern void	pttrailer();
/* n6.c */
extern int	width(register tchar);
extern int	getascender(void);
extern int	getdescender(void);
extern tchar	setch(int);
extern tchar	setabs(void);
extern int	findft(register int, int);
extern void	caseps(void);
extern void	mchbits(void);
extern void	setps(void);
extern tchar	setht(void);
extern tchar	setslant(void);
extern void	caseft(void);
extern void	setfont(int);
extern void	setwd(void);
extern tchar	vmot(void);
extern tchar	hmot(void);
extern tchar	mot(void);
extern tchar	sethl(int);
extern tchar	makem(int);
extern tchar	getlg(tchar);
extern void	caselg(void);
extern void	caseflig(void);
extern void	casefp(void);
extern void	casefps(void);
extern void	casecs(void);
extern void	casebd(void);
extern void	casevs(void);
extern void	casess(void);
extern tchar	xlss(void);
extern tchar	setuc0(int);
extern tchar	setanchor(void);
extern tchar	setlink(void);
extern void	casedummy(void);
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
#define	casefkern	casedummy
#define	caseminss	casedummy
#define	caselspadj	casedummy

#define	kernadjust(a, b)	0
#define	u2pts(i)		(i)
