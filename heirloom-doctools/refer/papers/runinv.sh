#       Copyright(c) 1988, Sun Microsystems, Inc.
#       All Rights Reserved

# from OpenSolaris "runinv	1.3	98/06/26 SMI" 

#
# Portions Copyright (c) 2005 Gunnar Ritter, Freiburg i. Br., Germany
#
# Sccsid %W% (gritter) %G%
#

PATH=/usr/5bin:$PATH export PATH

@REFDIR@/mkey R* | @REFDIR@/inv -v -h997 -n Xind
mv Xind.ia Ind.ia
mv Xind.ib Ind.ib
mv Xind.ic Ind.ic
