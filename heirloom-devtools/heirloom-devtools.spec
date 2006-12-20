#
# Sccsid @(#)heirloom-devtools.spec	1.4 (gritter) 12/20/06
#
Summary: The Heirloom Development Tools.
Name: heirloom-devtools
Version: 000000
Release: 1
License: Other
Source: %{name}-%{version}.tar.bz2
Group: System Environment/Base
Vendor: Gunnar Ritter <gunnarr@acm.org>
URL: <http://heirloom.sourceforge.net>
BuildRoot: %{_tmppath}/%{name}-root

%define	bindir		/usr/ccs/bin
%define	susdir		/usr/5bin/posix
%define	libdir		/usr/ccs/lib
%define	mandir		/usr/ccs/share/man

%define	xcc		gcc
%define	cflags		'-O -fomit-frame-pointer'
%define	cppflags	'-D__NO_STRING_INLINES -D_GNU_SOURCE'

#
# Combine the settings defined above.
#
%define	makeflags	ROOT=%{buildroot} INSTALL=install BINDIR=%{bindir} SUSDIR=%{susdir} LIBDIR=%{libdir} MANDIR=%{mandir} CC=%{xcc} CFLAGS=%{cflags} CPPFLAGS=%{cppflags}

%description
The Heirloom Development Tools provide yacc, lex, m4, and SCCS, as
portable derivatives of the utilities released by Sun as part of
OpenSolaris. The OpenSolaris utilities were in turn derived from
the original Unix versions, and are assumed be conforming
implementations of the POSIX standard.

%prep
rm -rf %{buildroot}
%setup

%build
make %{makeflags}

%install
make %{makeflags} install

rm -f filelist.rpm
for f in %{bindir} %{susdir} %{libdir}
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
%doc README CHANGES LICENSE/*
' >filelist.rpm

%clean
cd .. && rm -rf %{_builddir}/%{name}-%{version}
rm -rf %{buildroot}

%files -f filelist.rpm
