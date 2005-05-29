/*	from Unix 7th Edition /usr/src/cmd/spell/spell.c	*/
/*
 * Copyright(C) Caldera International Inc. 2001-2002. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *   Redistributions of source code and documentation must retain the
 *    above copyright notice, this list of conditions and the following
 *    disclaimer.
 *   Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *   All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *      This product includes software developed or owned by Caldera
 *      International, Inc.
 *   Neither the name of Caldera International, Inc. nor the names of
 *    other contributors may be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * USE OF THE SOFTWARE PROVIDED FOR UNDER THIS LICENSE BY CALDERA
 * INTERNATIONAL, INC. AND CONTRIBUTORS ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL CALDERA INTERNATIONAL, INC. BE
 * LIABLE FOR ANY DIRECT, INDIRECT INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#if __GNUC__ >= 3 && __GNUC_MINOR__ >= 4 || __GNUC__ >= 4
#define	USED	__attribute__ ((used))
#elif defined __GNUC__
#define	USED	__attribute__ ((unused))
#else
#define	USED
#endif
static const char sccsid[] USED = "@(#)/usr/5lib/spell/spellprog.sl	1.10 (gritter) 5/29/05";

#include <string.h>

#include "spell.h"
#define DLEV 2

#if defined (__GLIBC__) && defined (_IO_getc_unlocked)
#undef	getchar
#define	getchar()	_IO_getc_unlocked(stdin)
#endif

static int	suffix(char *, int);
static int	nop(char *, char *, char *, int);
static int	strip(char *, char *, char *, int);
static int	s(char *, char *, char *, int);
static int	an(char *, char *, char *, int);
static int	ize(char *, char *, char *, int);
static int	y_to_e(char *, char *, char *, int);
static int	ily(char *, char *, char *, int);
static int	ncy(char *, char *, char *, int);
static int	bility(char *, char *, char *, int);
static int	i_to_y(char *, char *, char *, int);
static int	es(char *, char *, char *, int);
static int	metry(char *, char *, char *, int);
static int	tion(char *, char *, char *, int);
static int	CCe(char *, char *, char *, int);
static int	VCe(char *, char *, char *, int);
static char	*lookuppref(char **, char *);
static int	putsuf(char *, char *, int);
static int	putwrd(char *, char *, int);
static int	monosyl(char *, char *);
static char	*skipv(char *);
static int	vowel(int);
static void	ise(void);
static char	*ztos(char *);
static int	dict(char *, char *);

static struct suftab {
	char *suf;
	int (*p1)(char *, char *, char *, int);
	int n1;
	char *d1;
	char *a1;
	int (*p2)(char *, char *, char *, int);
	int n2;
	char *d2;
	char *a2;
} suftab[] = {
	{"ssen",ily,4,"-y+iness","+ness" },
	{"ssel",ily,4,"-y+i+less","+less" },
	{"se",s,1,"","+s",		es,2,"-y+ies","+es" },
	{"s'",s,2,"","+'s"},
	{"s",s,1,"","+s"},
	{"ecn",ncy,1,"","-t+ce"},
	{"ycn",ncy,1,"","-cy+t"},
	{"ytilb",nop,0,"",""},
	{"ytilib",bility,5,"-le+ility",""},
	{"elbaif",i_to_y,4,"-y+iable",""},
	{"elba",CCe,4,"-e+able","+able"},
	{"yti",CCe,3,"-e+ity","+ity"},
	{"ylb",y_to_e,1,"-e+y",""},
	{"yl",ily,2,"-y+ily","+ly"},
	{"laci",strip,2,"","+al"},
	{"latnem",strip,2,"","+al"},
	{"lanoi",strip,2,"","+al"},
	{"tnem",strip,4,"","+ment"},
	{"gni",CCe,3,"-e+ing","+ing"},
	{"reta",nop,0,"",""},
	{"re",strip,1,"","+r",		i_to_y,2,"-y+ier","+er"},
	{"de",strip,1,"","+d",		i_to_y,2,"-y+ied","+ed"},
	{"citsi",strip,2,"","+ic"},
	{"cihparg",i_to_y,1,"-y+ic",""},
	{"tse",strip,2,"","+st",	i_to_y,3,"-y+iest","+est"},
	{"cirtem",i_to_y,1,"-y+ic",""},
	{"yrtem",metry,0,"-ry+er",""},
	{"cigol",i_to_y,1,"-y+ic",""},
	{"tsigol",i_to_y,2,"-y+ist",""},
	{"tsi",VCe,3,"-e+ist","+ist"},
	{"msi",VCe,3,"-e+ism","+ist"},
	{"noitacif",i_to_y,6,"-y+ication",""},
	{"noitazi",ize,5,"-e+ation",""},
	{"rota",tion,2,"-e+or",""},
	{"noit",tion,3,"-e+ion","+ion"},
	{"naino",an,3,"","+ian"},
	{"na",an,1,"","+n"},
	{"evit",tion,3,"-e+ive","+ive"},
	{"ezi",CCe,3,"-e+ize","+ize"},
	{"pihs",strip,4,"","+ship"},
	{"dooh",ily,4,"-y+ihood","+hood"},
	{"luf",ily,3,"-y+iful","+ful"},
	{"ekil",strip,4,"","+like"},
	{ 0, 0, 0, 0, 0 }
};

static char *preftab[] = {
	"anti",
	"bio",
	"dis",
	"electro",
	"en",
	"fore",
	"hyper",
	"intra",
	"inter",
	"iso",
	"kilo",
	"magneto",
	"meta",
	"micro",
	"milli",
	"mis",
	"mono",
	"multi",
	"non",
	"out",
	"over",
	"photo",
	"poly",
	"pre",
	"pseudo",
	"re",
	"semi",
	"stereo",
	"sub",
	"super",
	"thermo",
	"ultra",
	"under",	/*must precede un*/
	"un",
	0
};

static int vflag;
static int xflag;
static char word[100];
static char original[100];
static char *deriv[40];
static char affix[40];

int
main(int argc,char **argv)
{
	register char *ep, *cp;
	register char *dp;
	int fold;
	int j;
	FILE *file, *found;
	if(!prime(argc,argv)) {
		fprintf(stderr,
		    "spell: cannot initialize hash table\n");
		exit(1);
	}
	found = fopen(argv[2],"w");
	for(argc-=3,argv+=3; argc>0 && argv[0][0]=='-'; argc--,argv++)
		switch(argv[0][1]) {
		case 'b':
			ise();
			break;
		case 'v':
			vflag++;
			break;
		case 'x':
			xflag++;
			break;
		}
	for(;; fprintf(file,"%s%s\n",affix,original)) {
		affix[0] = 0;
		file = found;
		for(ep=word;(*ep=j=getchar())!='\n';ep++)
			if(j == EOF)
				exit(0);
		for(cp=word,dp=original; cp<ep; )
			*dp++ = *cp++;
		*dp = 0;
		fold = 0;
		for(cp=word;cp<ep;cp++)
			if(islower(*cp))
				goto lcase;
		if(putsuf(ep,".",0))
			continue;
		++fold;
		for(cp=original+1,dp=word+1;dp<ep;dp++,cp++)
			*dp = Tolower(*cp);
lcase:
		if(putsuf(ep,".",0)||suffix(ep,0))
			continue;
		if(isupper(word[0])) {
			for(cp=original,dp=word; *dp = *cp++; dp++)
				if (fold) *dp = Tolower(*dp);
			word[0] = Tolower(word[0]);
			goto lcase;
		}
		file = stdout;
	}
	return 0;
}

static int
suffix(char *ep,int lev)
{
	register struct suftab *t;
	register char *cp, *sp;
	lev += DLEV;
	deriv[lev] = deriv[lev-1] = 0;
	for(t= &suftab[0];sp=t->suf;t++) {
		cp = ep;
		while(*sp)
			if(*--cp!=*sp++)
				goto next;
		for(sp=cp; --sp>=word&&!vowel(*sp); ) ;
		if(sp<word)
			return(0);
		if((*t->p1)(ep-t->n1,t->d1,t->a1,lev+1))
			return(1);
		if(t->p2!=0) {
			deriv[lev] = deriv[lev+1] = 0;
			return((*t->p2)(ep-t->n2,t->d2,t->a2,lev));
		}
		return(0);
next:		;
	}
	return(0);
}

/*ARGSUSED*/
static int
nop(char *a, char *b, char *c, int d)
{
	return(0);
}

static int
strip(char *ep,char *d,char *a,int lev)
{
	return(putsuf(ep,a,lev)||suffix(ep,lev));
}

static int
s(char *ep,char *d,char *a,int lev)
{
	if(lev>DLEV+1)
		return(0);
	if(*ep=='s'&&ep[-1]=='s')
		return(0);
	return(strip(ep,d,a,lev));
}

static int
an(char *ep,char *d,char *a,int lev)
{
	if(!isupper(*word))	/*must be proper name*/
		return(0);
	return(putsuf(ep,a,lev));
}

static int
ize(char *ep,char *d,char *a,int lev)
{
	*ep++ = 'e';
	return(strip(ep,"",d,lev));
}

static int
y_to_e(char *ep,char *d,char *a,int lev)
{
	*ep++ = 'e';
	return(strip(ep,"",d,lev));
}

static int
ily(char *ep,char *d,char *a,int lev)
{
	if(ep[-1]=='i')
		return(i_to_y(ep,d,a,lev));
	else
		return(strip(ep,d,a,lev));
}

static int
ncy(char *ep,char *d,char *a,int lev)
{
	if(skipv(skipv(ep-1))<word)
		return(0);
	ep[-1] = 't';
	return(strip(ep,d,a,lev));
}

static int
bility(char *ep,char *d,char *a,int lev)
{
	*ep++ = 'l';
	return(y_to_e(ep,d,a,lev));
}

static int
i_to_y(char *ep,char *d,char *a,int lev)
{
	if(ep[-1]=='i') {
		ep[-1] = 'y';
		a = d;
	}
	return(strip(ep,"",a,lev));
}

static int
es(char *ep,char *d,char *a,int lev)
{
	if(lev>DLEV)
		return(0);
	switch(ep[-1]) {
	default:
		return(0);
	case 'i':
		return(i_to_y(ep,d,a,lev));
	case 's':
	case 'h':
	case 'z':
	case 'x':
		return(strip(ep,d,a,lev));
	}
}

static int
metry(char *ep,char *d,char *a,int lev)
{
	ep[-2] = 'e';
	ep[-1] = 'r';
	return(strip(ep,d,a,lev));
}

static int
tion(char *ep,char *d,char *a,int lev)
{
	switch(ep[-2]) {
	case 'c':
	case 'r':
		return(putsuf(ep,a,lev));
	case 'a':
		return(y_to_e(ep,d,a,lev));
	}
	return(0);
}

/*	possible consonant-consonant-e ending*/
static int
CCe(char *ep,char *d,char *a,int lev)
{
	switch(ep[-1]) {
	case 'l':
		if(vowel(ep[-2]))
			break;
		switch(ep[-2]) {
		case 'l':
		case 'r':
		case 'w':
			break;
		default:
			return(y_to_e(ep,d,a,lev));
		}
		break;
	case 's':
		if(ep[-2]=='s')
			break;
	case 'c':
	case 'g':
		if(*ep=='a')
			return(0);
	case 'v':
	case 'z':
		if(vowel(ep[-2]))
			break;
	case 'u':
		if(y_to_e(ep,d,a,lev))
			return(1);
		if(!(ep[-2]=='n'&&ep[-1]=='g'))
			return(0);
	}
	return(VCe(ep,d,a,lev));
}

/*	possible consonant-vowel-consonant-e ending*/
static int
VCe(char *ep,char *d,char *a,int lev)
{
	char c;
	c = ep[-1];
	if(c=='e')
		return(0);
	if(!vowel(c) && vowel(ep[-2])) {
		c = *ep;
		*ep++ = 'e';
		if(putsuf(ep,d,lev)||suffix(ep,lev))
			return(1);
		ep--;
		*ep = c;
	}
	return(strip(ep,d,a,lev));
}

static char *
lookuppref(char **wp,char *ep)
{
	register char **sp;
	register char *bp,*cp;
	for(sp=preftab;*sp;sp++) {
		bp = *wp;
		for(cp= *sp;*cp;cp++,bp++)
			if(Tolower(*bp)!=*cp)
				goto next;
		for(cp=bp;cp<ep;cp++) 
			if(vowel(*cp)) {
				*wp = bp;
				return(*sp);
			}
next:	;
	}
	return(0);
}

static int
putsuf(char *ep,char *a,int lev)
{
	register char *cp;
	char *bp;
	register char *pp;
	int val = 0;
	char space[20];
	deriv[lev] = a;
	if(putwrd(word,ep,lev))
		return(1);
	bp = word;
	pp = space;
	deriv[lev+1] = pp;
	while(cp=lookuppref(&bp,ep)) {
		*pp++ = '+';
		while(*pp = *cp++)
			pp++;
		if(putwrd(bp,ep,lev+1)) {
			val = 1;
			break;
		}
	}
	deriv[lev+1] = deriv[lev+2] = 0;
	return(val);
}

static int
putwrd(char *bp,char *ep,int lev)
{
	register int i, j;
	char duple[3];
	if(ep-bp<=1)
		return(0);
	if(vowel(*ep)) {
		if(monosyl(bp,ep))
			return(0);
	}
	i = dict(bp,ep);
	if(i==0&&vowel(*ep)&&ep[-1]==ep[-2]&&monosyl(bp,ep-1)) {
		ep--;
		deriv[++lev] = duple;
		duple[0] = '+';
		duple[1] = *ep;
		duple[2] = 0;
		i = dict(bp,ep);
	}
	if(vflag==0||i==0)
		return(i);
	j = lev;
	do {
		if(deriv[j])
			strcat(affix,deriv[j]);
	} while(--j>0);
	strcat(affix,"\t");
	return(i);
}


static int
monosyl(char *bp,char *ep)
{
	if(ep<bp+2)
		return(0);
	if(vowel(*--ep)||!vowel(*--ep)
		||ep[1]=='x'||ep[1]=='w')
		return(0);
	while(--ep>=bp)
		if(vowel(*ep))
			return(0);
	return(1);
}

static char *
skipv(char *s)
{
	if(s>=word&&vowel(*s))
		s--;
	while(s>=word&&!vowel(*s))
		s--;
	return(s);
}

static int
vowel(int c)
{
	switch(Tolower(c)) {
	case 'a':
	case 'e':
	case 'i':
	case 'o':
	case 'u':
	case 'y':
		return(1);
	}
	return(0);
}

/* crummy way to Britishise */
static void
ise(void)
{
	register struct suftab *p;
	for(p = suftab;p->suf;p++) {
		p->suf = ztos(p->suf);
		p->d1 = ztos(p->d1);
		p->a1 = ztos(p->a1);
	}
}
static char *
ztos(char *s)
{
	char	*t = strdup(s);
	for(s=t;*s;s++)
		if(*s=='z')
			*s = 's';
	return t;
}

static int
dict(char *bp,char *ep)
{
	register char *wp;
	long h;
	register long *lp;
	register int i;
	if(xflag)
		printf("=%.*s\n",(int)(ep-bp),bp);
	for(i=0; i<NP; i++) {
		for (wp = bp, h = 0, lp = pow2[i]; wp < ep; ++wp, ++lp)
			h += *wp * *lp;
		h += '\n' * *lp;
		h %= p[i];
		if(get(h)==0)
			return(0);
	}
	return(1);
}
