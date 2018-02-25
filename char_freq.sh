#!/bin/bash
# Example utility for conducting a known ciphertext
# only attack attack, with the knowledge that the
# encoded file is probably ASCII English text.
#
# Provided an encrypted file, output keys that
# return results that look like English text.

# Usage:
# ./char_freq.sh ciphertextfile
#
# This program makes use of the check_lang
# utility C program.  check_lang source may be
# modified and improved for use this this script
# i.e. modify character frequencies for detecting
# other languages.

if [[ -z "$1" ]]; then
	echo "Usage: ./char_freq.sh ciphertextfile"
	exit
fi

OUT="$(mktemp)"

for trialkey in {0..1023} #There are 2^10 possible keys
do
	x=`printf "0x%03X" $trialkey`
	./simpDES -d $1 $OUT $x
	./check_lang $OUT || echo "Candidate Key Found - $x"
done
rm $OUT
