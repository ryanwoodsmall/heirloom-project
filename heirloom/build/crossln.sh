#!/sbin/sh

# Sccsid @(#)crossln.sh	1.3 (gritter) 6/29/03

# Create a relative symbolic link.

usage() {
	echo "usage: $1 target linkname [root]" >&2
	exit 2
}

test $# != 2 -a $# != 3 && usage $0

case $3 in
'')	;;
*/)	;;
*)	set -- "$1" "$2" "$3/"
	;;
esac

case $1 in
/*)	;;
*)	rm -f -- "$2" && exec @LNS@ -- "$1" "$2" || exit ;;
esac

case $2 in
/*)	;;
..|../*)	echo "cannot resolve ../*" >&2; exit 1 ;;
.)	set -- "$1" "`pwd`" ${3+"$3"};;
*)	set -- "$1" "`pwd`/$2" ${3+"$3"};;
esac

test -d "$2" && test ! -d "$1" && set -- "$1" "$2/`basename $1`" ${3+"$3"}

test "x`dirname $1`" = "x`dirname $2`" && {
	rm -f -- "$2" && exec @LNS@ -- "`basename $1`" "$2" || exit
}

list= d=$2
while d=`dirname $d`; test "x$d" != x/ -a "x$d" != x.
do
	case $list in
	'')	list=.. ;;
	*)	list=../$list ;;
	esac
done

case $1 in
/*)	slash= ;;
*)	slash=/ ;;
esac

if test "x$3" != x
then
	s= r=$3
	while test "x$r" != x/ -a "x$r" != x.
	do
		s=../$s
		r=`dirname $r`
	done
	file=/`expr "$1" : "$3\\(.*\\)"`
	list=`expr "$list" : "$s\\(.*\\)"`
else
	file=$1
fi

rm -f -- "$2"
@LNS@ -- "$list$slash$file" "$2"
