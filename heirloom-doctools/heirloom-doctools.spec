#
# Sccsid @(#)heirloom-doctools.spec	1.11 (gritter) 9/10/05
#
Summary: The Heirloom Documentation Tools.
Name: heirloom-doctools
Version: 000000
Release: 1
License: Other
Source: %{name}-%{version}.tar.bz2
Group: System Environment/Base
Vendor: Gunnar Ritter <gunnarr@acm.org>
URL: <http://heirloom.sourceforge.net>
BuildRoot: %{_tmppath}/%{name}-root

%define	bindir		/usr/ucb
%define	mandir		/usr/share/man
%define	docdir		/usr/ucblib/doctools
%define	macdir		%{docdir}/tmac
%define	fntdir		%{docdir}/font
%define	tabdir		%{docdir}/nterm
%define	hypdir		%{docdir}/hyphen
%define	pstdir		%{docdir}/font/devpost/postscript
%define	pubdir		/usr/pub

%define	xcc		gcc
%define	cflags		'-O -fomit-frame-pointer'
%define	cppflags	'-D__NO_STRING_INLINES -D_GNU_SOURCE'
%define	yacc		'bison -y'

#
# Combine the settings defined above.
#
%define	makeflags	ROOT=%{buildroot} INSTALL=install YACC=%{yacc} MACDIR=%{macdir} FNTDIR=%{fntdir} TABDIR=%{tabdir} HYPDIR=%{hypdir} PUBDIR=%{pubdir} BINDIR=%{bindir} PSTDIR=%{pstdir} MANDIR=%{mandir} CC=%{xcc} CFLAGS=%{cflags} CPPFLAGS=%{cppflags}

%description
The Heirloom Documentation Tools provide troff, nroff, and related
utilities to format manual pages and other documents for output on
terminals and printers.

%prep
rm -rf %{buildroot}
%setup

%build
make %{makeflags}

%install
make %{makeflags} install

rm -f filelist.rpm
for f in %{bindir} %{macdir} %{fntdir} %{tabdir} %{hypdir} %{pstdir} %{pubdir}
do
	if test -d %{buildroot}/$f
	then
		(cd %{buildroot}/$f; find * -type f -o -type l) | sed "s:^:$f/:"
	else
		echo $f
	fi
done | sort -u | sed '
	1i\
%defattr(-,root,root)\
%{mandir}\
%doc README CHANGES FONTS NEWS PDFS LICENSE/*
' >filelist.rpm

%clean
cd .. && rm -rf %{_builddir}/%{name}-%{version}
rm -rf %{buildroot}

%files -f filelist.rpm
