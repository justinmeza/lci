#!/bin/bash
# Usage: testFile.sh PROGRAM FILE

IFS=' 	
'
PATH="/usr/local/bin:/usr/bin:/bin"
export PATH

# Set up options
QUIET= MEMCHK=
ERR=1
MEMERR=127
while getopts qm opt
do
	case $opt in
	q)	QUIET=true
		;;
	m)	MEMCHK="valgrind -q --leak-check=full --error-exitcode=$MEMERR"
		;;
	esac
done
# Remove options, leave arguments
shift $((OPTIND - 1))

PROGRAM="$1"
TESTFILE="$2"
TESTNAME=${TESTFILE%.*}
INFILE="$TESTNAME.in"
OUTFILE="$TESTNAME.out"
ERRFILE="$TESTNAME.err"

# Check for test file
if [ ! -f $TESTFILE ]
then
	printf "Unable to find test file ($TESTFILE)!\nExiting...\n\n"
	exit 1
else
	test ! $QUIET && printf "Found test file ($TESTFILE)!\n"
fi

# Check for input file
IN=
if [ ! -f $INFILE ]
then
	test ! $QUIET && printf "Not using an input file ($INFILE)...\n"
else
	test ! $QUIET && printf "Found input file ($INFILE)!\n"
	IN="< $INFILE"
fi

# Check for error file
if [ ! -f $ERRFILE ]
then
	test ! $QUIET && printf "Not using an error file ($ERRFILE)...\n"
	# Check for output file
	if [ ! -f $OUTFILE ]
	then
		printf "Unable to find output file ($OUTFILE)!\nExiting...\n\n"
		exit 1
	else
		test ! $QUIET && printf "Found output file ($OUTFILE)!\n"
	fi
	# Run the test
	TMPFILE=$(mktemp /tmp/temp.XXXXXX) && test ! $QUIET && printf "Using temporary output file ($TMPFILE)...\n"
	eval "$MEMCHK $PROGRAM $TESTFILE $IN > $TMPFILE"
	RESULT=$?
	# Check that program exited normally
	if [ $RESULT -eq 0 ]
	then
		eval "diff $OUTFILE $TMPFILE"
		RESULT=$?
		rm -f $TMPFILE
		# Check the result
		if [ $RESULT -eq 0 ]
		then
			test ! $QUIET && printf "$TESTNAME: Success!\n\n"
			exit 0
		else
			printf "$TESTNAME: Failure!\n(Check diff output for differences.)\n('<' indicates lines from $OUTFILE that do not appear in program output.)\n('>' indicates lines from program output that do not appear in $OUTFILE.)\n\n"
			exit 1
		fi
	elif [ $RESULT -eq $MEMERR ]
	then
		printf "$TESTNAME: Failure!\n(Memory leak detected, check program output for more information.)\n\n"
		exit 1
	else
		printf "$TESTNAME: Failure!\n(You did *not* specify an error file, $ERRFILE, signifying you wanted the program to succeed; however, the program failed with exit value $RESULT.)\n\n"
		rm -f $TMPFILE
		exit 1
	fi
else
	test ! $QUIET && printf "Found error file ($ERRFILE)!\n"
	# Check for output file
	if [ -f $OUTFILE ]
	then
		printf "Warning: When checking for errors, output file ($OUTFILE) will be ignored!\n"
	fi
	# Run the test
	if [ $QUIET ]
	then
		eval "$MEMCHK $PROGRAM $TESTFILE" 2> /dev/null
		RESULT=$?
	else
		eval "$MEMCHK $PROGRAM $TESTFILE"
		RESULT=$?
	fi
	# Check the result
	if [ $RESULT -eq 0 ]
	then
		printf "$TESTNAME: Failure!\n(You specified an error file, $ERRFILE, signifying you wanted the program to fail; however, the program did not fail.)\n\n"
		exit 1
	elif [ $RESULT -eq $MEMERR ]
	then
		printf "$TESTNAME: Failure!\n(Memory leak detected, check program output for more information.)\n\n"
		exit 1
	elif [ $RESULT -eq $ERR ]
	then
		test ! $QUIET && printf "$TESTNAME: Success!\n\n"
		exit 0
	else
		printf "$TESTNAME: Failure!\n(Unknown exit code, check program output for more information.)\n\n"
		exit 1
	fi
fi
