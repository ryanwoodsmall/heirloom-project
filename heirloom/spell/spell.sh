#
#	from Unix 7th Edition /bin/spell */
#
# Copyright(C) Caldera International Inc. 2001-2002. All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#   Redistributions of source code and documentation must retain the
#    above copyright notice, this list of conditions and the following
#    disclaimer.
#   Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
#   All advertising materials mentioning features or use of this software
#    must display the following acknowledgement:
#      This product includes software developed or owned by Caldera
#      International, Inc.
#   Neither the name of Caldera International, Inc. nor the names of
#    other contributors may be used to endorse or promote products
#    derived from this software without specific prior written permission.
#
# USE OF THE SOFTWARE PROVIDED FOR UNDER THIS LICENSE BY CALDERA
# INTERNATIONAL, INC. AND CONTRIBUTORS ``AS IS'' AND ANY EXPRESS OR
# IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL CALDERA INTERNATIONAL, INC. BE
# LIABLE FOR ANY DIRECT, INDIRECT INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
# BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
# WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
# OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
# EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
# B flags, D dictionary, F files, H history, S stop, V data for -v
#
# Sccsid @(#)spell.sh	1.12 (gritter) 12/1/03

PATH=@DEFBIN@:@SV3BIN@:$PATH export PATH

libdir=@DEFLIB@/spell

H_SPELL=${H_SPELL-@SPELLHIST@}
test -w "$H_SPELL" || H_SPELL=/dev/null
T_SPELL=/tmp/spell.$$
V_SPELL=/dev/null
B_SPELL=
lfile=/dev/null
trap "rm -f $T_SPELL; exit" 0 1 2 13 15
while :
do
	while getopts abvx A
	do
		case $A in
		v)	B_SPELL="$B_SPELL -v"
			V_SPELL=${T_SPELL}a ;;
		a)	;;
		b)	D_SPELL=${D_SPELL-$libdir/hlistb}
			B_SPELL="$B_SPELL -b" ;;
		x)	B_SPELL="$B_SPELL -x" ;;
		*)	echo "usage: `basename $0` [-v] [-b] [-x] [+local_file] [ files ]" >&2
			exit 2 ;;
		esac
	done
	test $OPTIND -gt 1 && shift `expr $OPTIND - 1`
	OPTIND=1
	case $1 in
	+*)
		lfile=`expr x"$1" : 'x+\(.*\)'`
		if test -z "$lfile"
		then
			echo "`basename $0` cannot identify local spell file" >&2
			exit 1
		elif test ! -r "$lfile"
		then
			echo "`basename $0` cannot read $lfile" >&2
			exit 1
		fi
		shift
		;;
	*)
		break
	esac
done

mkfifo $T_SPELL || exit
deroff -w ${@+"$@"} |\
  sort -u |\
  $libdir/spellprog ${S_SPELL-$libdir/hstop} $T_SPELL |\
  $libdir/spellprog ${D_SPELL-$libdir/hlista} $V_SPELL $B_SPELL |\
  sort -u +0f +0 $T_SPELL - |\
  fgrep -v -x -f "$lfile" |\
  tee -a $H_SPELL
who am i >>$H_SPELL 2>/dev/null
case $V_SPELL in
/dev/null)	exit
esac
sed '/^\./d' $V_SPELL | sort -u +1f +0
