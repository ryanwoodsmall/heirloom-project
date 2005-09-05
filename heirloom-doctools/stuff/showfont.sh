#!/sbin/sh

#
# This script uses troff to print the characters and character names of
# one or more PostScript fonts. It accepts the AFM files of the respective
# fonts as arguments, and expects these files to be in the current directory.
# If matching PFB, PFA, or T42 files also exist in the current directory,
# they are included.
#

# Sccsid @(#)showfont.sh	1.3 (gritter) 9/5/05

for i
do (
	base=`expr "$i" : '\(.*\)\.afm'`
	if test -f "$base.pfa"
	then
		supply=$base.pfa
	elif test -f "$base.pfb"
	then
		supply=$base.pfb
	elif test -f "$base.t42"
	then
		supply=$base.t42
	else
		unset supply
	fi
	cat <<-!
		.nr PE 10.8i
		.fp 0 X $i $supply
		.ps 10
		.vs 14
		.ta 12pC 24p
		.de NC
		.	sp |6P
		.	po +8P
		..
		.wh \\n(PEu NC
		.sp 6P
	!
	nawk <"$i" '
		$1 == "FontName" {
			print ".mk S"
			print ".sp |4P"
			printf("\\fH\\s(12'"$i"' \\(em %s\n", $2)
			print ".sp |\\nSu"
		}
		$1 == "StartCharMetrics" {
			state = 1
		}
		state == 1 && $1 == "C" && n < 255 && \
				match($0, /(^|;)[ 	]*N[	]*/) {
			name = substr($0, RSTART+RLENGTH+1)
			match(name, /[ 	;]/)
			name = substr(name, 1, RSTART-1)
			printf("\t\\s(11\\fX\\[%s]\t\\s8\\fH%s\n",\
				name, name)
			print ".br"
			n++
		}
		state == 1 && $1 == "EndCharMetrics" {
			state = 0
		}
	'
	cat <<-!
		.wh \\n(PEu
	!
   ) | TROFFONTS=. troff -x
done | dpost
