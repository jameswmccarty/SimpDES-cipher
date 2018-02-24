#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define HI_4(x) ((x >> 4) & 0x0F)
#define LO_4(x) (x & 0x0F)
#define HI_5(x) ((x >> 5) & 0x1F)
#define LO_5(x) (x & 0x1F)
#define KEYMASK 0x03FF

/* Simp-DES S-boxes */
const unsigned char S0[4][4] = {
	{1,0,3,2},
	{3,2,1,0},
	{0,2,1,3},
	{3,1,3,2}
};

const unsigned char S1[4][4] = {
	{0,1,2,3},
	{2,0,1,3},
	{3,0,1,0},
	{2,1,0,3}
};

/* IP -- Initial Permutation *
 *
 * Permutate the 8-bit input vector
 * bit pattern defined as:
 * inpt: 8 7 6 5 4 3 2 1 
 * out:  2 6 3 1 4 8 5 7
 * *******************************/
unsigned char IP(char input) {
	int i = 8;
	int vect[8] = {2,0,6,1,3,5,7,4};
	unsigned char out = 0x00;
	while(i --> 0) {
		if(input & (0x01<<i)) 
			out |= (0x01<<vect[i]);
	}
	return out;
}

/* IP^(-1) -- Initial Permutation Negate *
 *
 * Final inverse permutation of 8-bit input
 * bit pattern defined as:
 * inpt: 8 7 6 5 4 3 2 1
 * out:  4 1 3 5 7 2 8 6
 * *******************************/
unsigned char IPneg(char input) {
	int i = 8;
	int vect[8] = {1,3,0,4,7,5,2,6};
	unsigned char out = 0x00;
	while(i --> 0) {
		if(input & (0x01<<i)) 
			out |= (0x01<<vect[i]);
	}
	return out;
}

/* P10 -- Permutation *
 *
 * Permutate the 10-bit input vector
 * bit pattern defined as:
 * inpt: 10 9 8 7 6 5  4 3 2 1
 * out:  3  5 2 7 4 10 1 9 8 6
 * *******************************/
unsigned int P10(int input) {
	int i = 10;
	int vect[10] = {4,2,1,6,0,8,5,9,7,3};
	unsigned int out = 0x00;
	while(i --> 0) {
		if(input & (0x01<<i)) 
			out |= (0x01<<vect[i]);
	}
	return out;
}

/* P8 -- Selective Permutation *
 *
 * Permutate the 10-bit input vector
 * and output an 8-bit pattern defined as:
 * inpt: 1  2 3 4 5 6 7 8 9  10
 * out:  -  - 6 3 7 4 8 5 10 9
 * *******************************/
unsigned char P8(int input) {
	int i = 10;
	int vect[8] = {6,4,2,7,5,3,0,1};
	unsigned char out = 0x00;
	while(i --> 2) {
		if(input & (0x01<<(9-i))) 
			out |= (0x01<<vect[i-2]);	
	}
	return out;
}

/* Expand / Permutation
 *
 * Function maps a 4-bit input vector with
 * bits 4 3 2 1 and outputs an 8-bit vector
 * with bit order:
 * 4 1 2 3 2 3 4 1
 *
 * The lowest four bits of the input char are
 * used for this expansion...any high bits are
 * discarded. */
unsigned char EP(char input) {
	unsigned char out   = 0x00;
	unsigned char ONE   = 0x41; /* 0100 | 0001 */
	unsigned char TWO   = 0x28; /* 0010 | 1000 */
	unsigned char THREE = 0x14; /* 0001 | 0100 */
	unsigned char FOUR  = 0x82; /* 1000 | 0010 */
	
	if(input & 0x08) out |= ONE;
	if(input & 0x04) out |= TWO;
	if(input & 0x02) out |= THREE;
	if(input & 0x01) out |= FOUR;

	return out;
}

/* LS -- Left Shift
 *
 * Function rotates an input vector
 * to the left by the number of bits 
 * specified by 'shift', with wraparound.
 *
 * !!! NOTE !!!
 * - The output is a 5-bit vector 
 *
 * This is not a general purpose left shift.
 * Only 5-bits are considered. */
unsigned char LS(char input, int shift) {
	unsigned char output = 0x00;
	shift %= 5; /* consider only 5-bit vector */
	input &= 0x1F; /* keep lowest five */
  output |= (char) (( ((int) input) << shift ) & 0x1F);
  output |= (char) ( ((unsigned int) input) >> (5-shift) );
	return output;
}

/* SW -- Swap
 *
 * Function exchanges lowest 4 bits with 
 * highest 4 bits. */
unsigned char SW(char input) {
	return (input << 4) | ((input >> 4) & 0x0F);
}

/* key_exp -- Key Expansion
 *
 * Provided a 10-bit input key, populate the
 * 8-bit keys at k1 and k2 by performing
 * the 2 key expansion rounds. */
void key_exp(int key, unsigned char *k1, unsigned char *k2) {
	int tmp;
	tmp = P10(key);
	tmp = LS(HI_5(tmp), 1) << 5 | LS(LO_5(tmp), 1);
	*k1  = P8(tmp);
	tmp = LS(HI_5(tmp), 2) << 5 | LS(LO_5(tmp), 2);
	*k2  = P8(tmp);
}

/* P4 -- Permutation *
 *
 * Permutate the 4-bit input vector
 * bit pattern defined as:
 * inpt: 1 2 3 4
 * out:  2 4 3 1
 */
unsigned char P4(char input) {
	int i = 4;
	int vect[4] = {2,1,3,0};
	unsigned char out = 0x00;
	while(i --> 0) {
		if(input & (0x01<<i)) 
			out |= (0x01<<vect[i]);
	}
	return out;
}

/* perform bit-wise manipulations to access correct
 * value of S-box S0 */
unsigned char getS0(char input) {
	unsigned int row,col,tmp;
	/* bits 1 and 4 of input specify row */	
	tmp = input & 0x08; /* isolate bit 4 */
	row = (tmp>>2) | (input & 0x01); /* shift bit 4 down, retain bit 1 */

	/* bits 2 and 3 of input specify col */
	tmp = input & 0x06; /* 0000 0110 - isolate bits 2 and 3 */
	col = tmp>>1;

	return S0[row][col];
}

/* perform bit-wise manipulations to access correct
 * value of S-box S1 */
unsigned char getS1(char input) {
	unsigned int row,col,tmp;
	/* bits 1 and 4 of input specify row */	
	tmp = input & 0x08; /* isolate bit 4 */
	row = (tmp>>2) | (input & 0x01); /* shift bit 4 down, retain bit 1 */

	/* bits 2 and 3 of input specify col */
	tmp = input & 0x06; /* 0000 0110 - isolate bits 2 and 3 */
	col = tmp>>1;

	return S1[row][col];
}

/* f_k -- Function F-sub K
 *
 * Performs encryption function on a given
 * 8-bit input vector.  Only the high bits
 * are transformed, which is why there is
 * a Swap function called between rounds.
 *
 * Accepts input vector and round key.
 */
unsigned char f_k(char input, char k) {
	unsigned char tmp = 0x00;

	/* ------ Enter F Box -------- */
	
	tmp = EP(LO_4(input)); /* expand low input bits */
	tmp ^= k; /* apply the round key */
	tmp = P4((getS0(HI_4(tmp)) << 2) | 
			     (getS1(LO_4(tmp)))); 
	/* S-box outputs permutated with P4 */
	/* ------- Exit F Box --------- */
	tmp ^= HI_4(input);
	return (tmp << 4) | LO_4(input);
}

/* Encrypt accepts an 8-bit block and a 
 * 10-bit key, and returns the encoded
 * 8-bit cipher text.
 */
unsigned char encrypt(unsigned char plain, int key) {
	unsigned char k1, k2; /* round keys */
	/* Computer Key Expansion first */
	key_exp(key, &k1, &k2);
	/* Apply initial permutation to plain text */
	plain = IP(plain);
	/* apply first round function */
	plain = f_k(plain,k1);
	/* Swap bytes */
	plain = SW(plain);
	/* apply second round function */
	plain = f_k(plain,k2);
	/* undo the initial permutation */
	return IPneg(plain);
}

/* The decryption process is the reverse
 * of encryption process above.
 */
unsigned char decrypt(unsigned char cipher, int key) {
	unsigned char k1, k2;
	/* Computer Key Expansion first */
	key_exp(key, &k1, &k2);
	/* Apply initial permutation to cipher text */
	cipher = IP(cipher);
	/* apply first round function */
	cipher = f_k(cipher,k2);
	/* Swap bytes */
	cipher = SW(cipher);
	/* apply second round function */
	cipher = f_k(cipher,k1);
	/* undo the initial permutation */
	return IPneg(cipher);
}

void print_usage(char **argv)
{
	printf ("Simple-DES block cipher\n");
	printf ("-----------------------\n\n");
	printf ("An educational implementation of DES with an 8-bit block size and 10-bit keysize.\n");
	printf ("usage:\n");
	printf ("%s -e|d inputfile outputfile hex_key\n\n", argv[0]);
	printf ("Examples:\n");
	printf ("%s -e secret_stuff.pdf encrypted_secret_stuff.enc 0x0282\n", argv[0]);
	printf ("%s -d encrypted_secret_stuff.enc secret_stuff.pdf 0x0282\n", argv[0]);
	fflush (stdout);
}

int main(int argc, char **argv) {
	
	FILE *in      = NULL;
	FILE *out     = NULL;
	int key       = 0x00;
	int enc_flag  = 1; /* encrypt by default.  check for flag */
	unsigned char pt,ct;
	
    if(argc != 5) {
		print_usage(argv);
		exit(0);
	}
	if(strcmp (argv[1], "-e") && strcmp (argv[1], "-d"))
	{
		printf ("Error...must specify Encryption or Decryption.\n\n");
		print_usage(argv);
		exit(EXIT_FAILURE);
	}
	if(!strcmp(argv[1], "-d")) enc_flag = 0;
	/* open streams for binary mode */	
	if((in = fopen(argv[2], "rb")) == NULL)
	{
		printf ("Error opening input file %s.\n", argv[2]);
		exit(EXIT_FAILURE);
	}
	if((out = fopen(argv[3], "wb")) == NULL)
	{
		printf ("Error opening output file %s.\n", argv[3]);
		exit(EXIT_FAILURE);
	}
	
	key = strtol(argv[4],NULL,16); /* read key as a hex value */
	key &= KEYMASK; /* retain 10-bits only */

	if(enc_flag) {
		while(1) {
			/* read an input block (a single 8-bit byte) */
			if(fread(&pt,1,sizeof(char),in) == 0) { /* reached EOF */
				break;
			}
			ct = encrypt(pt,key);
			fwrite(&ct,sizeof(char),1,out); /* write out block */
	}} 
	else {
		while(1) {
			/* read an input block (a single 8-bit byte) */
			if(fread(&ct,1,sizeof(char),in) == 0) { /* reached EOF */
				break;
			}
			pt = decrypt(ct,key);
			fwrite(&pt,sizeof(char),1,out); /* write out block */
	}}

	fclose(in);
	fclose(out);
	return 0;
}

