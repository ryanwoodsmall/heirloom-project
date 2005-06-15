#
# Sccsid @(#)heirloom-sh.spec	1.2 (gritter) 6/16/05
#
Summary: The Heirloom Bourne Shell.
Name: heirloom-sh
Version: 000000
Release: 1
License: Other
Source: %{name}-%{version}.tar.bz2
Group: System Environment/Base
Vendor: Gunnar Ritter <Gunnar.Ritter@pluto.uni-freiburg.de>
URL: <http://heirloom.sourceforge.net>
BuildRoot: %{_tmppath}/%{name}-root

%define	usr		/usr
%define	defbin		%{usr}/5bin
%define	sv3bin		%{defbin}

%define	mandir		%{usr}/share/man/5man

%define	lns		ln

%define	xcc		gcc
%define	cflags		'-O -fomit-frame-pointer'
%define	cppflags	'-D__NO_STRING_INLINES -D_GNU_SOURCE'

#
# Combine the settings defined above.
#
%define	makeflags	ROOT=%{buildroot} DEFBIN=%{defbin} SV3BIN=%{sv3bin} MANDIR=%{mandir} CC=%{xcc} CFLAGS=%{cflags} CPPFLAGS=%{cppflags} LNS=%{lns} UCBINST=install

%description
The Heirloom Bourne Shell is a derivative of the traditional Unix shell
as found on SVR4 implementations.

%prep
rm -rf %{buildroot}
%setup

%build
make %{makeflags}

%install
make %{makeflags} install

%clean
cd .. && rm -rf %{_builddir}/%{name}-%{version}
rm -rf %{buildroot}

%files
%defattr(-,root,root)
%doc CALDERA.LICENSE CHANGES OPENSOLARIS.LICENSE README
%{sv3bin}/*
