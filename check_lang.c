#include<stdio.h>
#include<stdlib.h>
#include<string.h>

/* Letter frequencies for the English language
 * values from https://en.wikipedia.org/wiki/Letter_frequency.
 * Only the most frequent 5 letters are evaluated for
 * simplicity.
 */

#define EFREQ 12.7
#define TFREQ 9.0
#define AFREQ 8.1
#define OFREQ 7.5
#define IFREQ 6.9

/* consider a margin of error */
#define FREQ_BUF 3.0

double char_hist[26];

int chk_bnds(double test, double base, double margin) {
	double low = base - margin;
	double high = base + margin;
	if(test >= low && test <= high)
		return 1;
	return 0;
}

int test_lang() {
	int i;
	double total = 0.;

	/* normalize histogram */
	for(i=0;i<26;i++) {
		total += char_hist[i];
	}
	if(total <= 0.) { /* histogram was empty */
		return 0;     /* not an English file */
	}
	for(i=0;i<26;i++) {
		char_hist[i] /= total;
		char_hist[i] *= 100.0;
	}

	/*printf("E: %.2f\nT: %.2f\nA: %.2f\nO: %.2f\nI: %.2f\n",
		char_hist[4], char_hist[19], char_hist[0], char_hist[14], char_hist[8]);*/ 

	/* verify all freqs fall within bounds */
	if(!chk_bnds(char_hist[4], EFREQ, FREQ_BUF))
		return 0;
	if(!chk_bnds(char_hist[19], TFREQ, FREQ_BUF))
		return 0;
	if(!chk_bnds(char_hist[0], AFREQ, FREQ_BUF))
		return 0;
	if(!chk_bnds(char_hist[14], OFREQ, FREQ_BUF))
		return 0;
	if(!chk_bnds(char_hist[8], IFREQ, FREQ_BUF))
		return 0;

	/*printf("all checks passed.\n");*/
	return 1;
}

void count(unsigned char input) {
	if(input >= 65 && input <= 90) { /* upper case */
		char_hist[input-65] += 1.0;	
	} else if (input >= 97 && input <= 122) { /* lower case */
		char_hist[input-97] += 1.0;
	}
}

void print_usage(char **argv)
{
	printf ("check-lang\n");
	printf ("-----------------------\n\n");
	printf ("Determine if a given file is ASCII English text based on character frequency.\n");
	printf ("usage:\n");
	printf ("%s inputfile\n\n", argv[0]);
	printf ("Program exit status is 1 if English is detected, 0 otherwise.\n");
	fflush (stdout);
}

int main(int argc, char **argv) {
	
	FILE *in = NULL;
	unsigned char pt;
	
    if(argc != 2) {
		print_usage(argv);
		exit(0);
	}
	if(!strcmp (argv[1], "-h"))
	{
		print_usage(argv);
		exit(0);
	}
	if((in = fopen(argv[1], "rb")) == NULL)
	{
		printf ("Error opening input file %s.\n", argv[1]);
		exit(EXIT_FAILURE);
	}

	while(1) {
		/* read an input block (a single 8-bit byte) */
		if(fread(&pt,1,sizeof(char),in) == 0) { 
				break; /* reached EOF */
			}
		count(pt);
	}

	fclose(in); /* completed reading file */

	return test_lang();
}

