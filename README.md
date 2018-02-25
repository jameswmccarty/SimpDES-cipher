# SimpDES-cipher
A C based implementation of the Simplified Data Encryption Standard (Simple DES) educational block cipher.

This program will allow a user to encrypt and decrypt files from the command line using a provided 10-bit key.

This standard uses a 10-bit key, and an 8-bit block size.  This is an implementation strictly for educational purposes, and should not be used in scenario where security is required.

A sample bash script to trial keys for a known plaintext attack (KPA) is provided to demonstrate the process.  The key size is small enough to be able to brute force a solution almost instantly.

# Background
For information on the design of Simple DES, I would recommend checking out the below links:

o https://en.wikipedia.org/wiki/Data_Encryption_Standard#Simplified_DES  
o http://mercury.webster.edu/aleshunas/COSC%205130/G-SDES.pdf  
o https://www.youtube.com/watch?v=fEKdpsCbtC8  

# How To Use This Program
Download all the project files.  They are:

o Makefile

o simpDES.c

o brute_force.sh

Compile the project with: `make`

Then run: `./simpDES -e|d inputfile outputfile hex_key`

i.e.:  
`./simpDES -e secret_stuff.pdf encrypted_secret_stuff.enc 0x0282`  
`./simpDES -d encrypted_secret_stuff.enc secret_stuff.pdf 0x0282`  

To execute the known plaintext attack, run: `./brute_force.sh ciphertextfile "Sample of Known plaintext."`

i.e.:  
`./brute_force.sh secretfile.enc "Lorem ipsum dolor situr adipiscing elit."`


