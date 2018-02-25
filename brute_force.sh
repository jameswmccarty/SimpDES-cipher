#!/bin/bash
# Example utility for conducting a known plaintext attack
# (KPA) with a brute-force approach.
#
# Provided an encrypted file, and a sample plaintext
# 'crib', recover the key that was used to encrypt
# the file.
# 
# With Simp-DES, there are only 2^10 possible keys
# to evaluate, making this approach a reasonable
# method.

# Usage:
# ./brute_force.sh ciphertextfile "Sample of known plaintext."
#
# Program echos candidate keyvalues, where plaintext appears
# in the output of decoded files.

if [[ -z "$1" || -z "$2" ]]; then
	echo "Usage: ./brute_force.sh ciphertextfile \"Sample of known plaintext.\""
	exit
fi

if [ ! -f "$1" ]; then
	echo "$1 not found."
	exit
fi

OUT="$(mktemp)"

for trialkey in {0..1023} #There are 2^10 possible keys
do
	x=`printf "0x%03X" $trialkey`
	./simpDES -d $1 $OUT $x
	if grep -q "$2" $OUT ; then
		echo "Candidate Key Found - $x"
	fi
done
rm $OUT
