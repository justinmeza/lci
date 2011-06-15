#!/bin/sh
# Usage: testDir.sh PROGRAM DIR

IFS=' 	
'
PATH="/usr/local/bin:/usr/bin:/bin"
export PATH

# Set up options
OPTS=
while getopts qm opt
do
	case $opt in
	q)	OPTS="$OPTS -q"
		;;
	m)	OPTS="$OPTS -m"
		;;
	esac
done
# Remove options, leave arguments
shift $((OPTIND - 1))

find $2 -name *.lol | sort -t'/' -n -k1 -k2 -k3 -k4 -k5 -k6 -k7 -k8 -k9 -k10 | xargs -n 1 ./testFile.sh$OPTS $1 && echo "Passed all tests!" && exit 0

exit 1
