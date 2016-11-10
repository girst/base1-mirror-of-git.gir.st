/* base1 en- and decoder with usage similar to GNU's base64.
(C) 2016 Tobias Girstmair, http://isticktoit.net/
Released under the GNU GPL v3. See LICENSE for details. */

#define _XOPEN_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <stdint.h>

#define VERSION "0.1"

#define DEFAULT_WRAP 72

#define COPYRIGHT "(C) 2016 Tobias Girstmair, GPLv3"
#define HELP_TEXT \
	"Usage: %s [OPTION]... [FILE]\n"\
	"Options: -d (decode), -i (ignore-garbage), -w COLS (wrap, default %d)"\
	" -t (test) -v (version) -h (help)\n"

#define BASE ((1<<CHAR_BIT)) //2 ^ (bits of a byte)

struct ops {
	FILE* f;
	int d; /*decode*/
	int i; /*ignore-garbage*/
	int w; /*wrap*/
	int t; /*test*/
};

size_t highestOneBitPosition(unsigned long long a);
int multiplication_is_safe(unsigned long long a, unsigned long long b);
int addition_is_safe(unsigned long long a, unsigned long long b);

void do_encode (FILE* in, FILE* out, int wrap_column, int test_only);
void do_decode (FILE* in, FILE* out, int ignore_garbage);


int main (int argc, char** argv) {
	struct ops op = {stdin, 0, 0, DEFAULT_WRAP, 0};
	int opt;

	opterr=0; /* suppress default error messages */
	while ( (opt = getopt(argc, argv, "diw:tvh")) != -1)  {
		switch (opt) {
		case 'd': op.d = 1; break;
		case 'i': op.i = 1; break;
		case 'w': op.w = atoi (optarg); break;
		case 't': op.t = 1; break;
		case 'v':
			fprintf (stderr, "base1 %s\n%s\n", VERSION, COPYRIGHT);
			return 0;
		case 'h':
			fprintf (stderr, HELP_TEXT, argv[0], DEFAULT_WRAP);
			return 0;
		default:
			fprintf (stderr, "unknown option '-%c'.\n", optopt);
			return 1;
		}
	}
	if (argc-optind > 1) {
		fprintf (stderr, "%s: extra operand '%s'. \n", argv[0], argv[argc-1]);
		return 1;
	} else if (optind < argc) {
		if (strcmp (argv [optind], "-") != 0)
			op.f = fopen (argv[optind], "rb");
	}

	if (op.d) {
		do_decode (op.f, stdout, op.i);
	} else {
		do_encode (op.f, stdout, op.w, op.t);
	}

	if (op.f != stdin) fclose (op.f);

	return 0;
}




void do_encode (FILE* in, FILE* out, int wrap_column, int test_only) {
	int in_len = 0;
	unsigned long long out_len = 0;

	for (int in_char; (in_char = getc(in)) != EOF; in_len++) {
		if (!multiplication_is_safe (out_len, BASE)){
			puts ("overflowed.");
			return;
		}
		out_len *= BASE;
		if (!addition_is_safe (out_len, in_char)) {
			puts ("overflowed.");
			return;
		}
		out_len += in_char;
	}

	if (test_only) {
		printf ("Length of output:\t%llu\nMaximum value of ULL:\t%llu\n", out_len, ULLONG_MAX);
		return;
	}

	unsigned long long block_size = 1;

	for (int i = 0; i < in_len; i++) {
		out_len += block_size;
		block_size *= BASE;
	}

	for (unsigned long long i = 0; i < out_len; i++)
		fprintf (out, "%s%c", wrap_column&&!(i%wrap_column)&&i?"\n":"", 'A');
		//TODO: use faster function
}

void do_decode (FILE* in, FILE* out, int ignore_garbage) {
	unsigned long long in_len = 0;

	for (int in_char; (in_char = getc (in)) != EOF;) {
		if (in_char == ' ' || in_char == '\n' || in_char == '\t') {
			continue;
		} else if (in_char == 'A') {
			in_len++;
		} else if (!ignore_garbage) {
			fprintf (stderr, "Unrecognized glyph %c\n", in_char);
			return;
		}
		// TODO: check for overflow!
	}

	int bin_len = 0;
	unsigned long long block_size = 1;
	while (in_len >= block_size) {
		in_len -= block_size;
		bin_len++;
		block_size *= BASE;
	}

	char b;
	char out_buf[bin_len];
	for (int byte_num = bin_len-1; byte_num >= 0; byte_num--) {
		b = in_len & (BASE - 1);
		out_buf[byte_num] = b;
		in_len -= b;
		in_len /= BASE;
	}

	for (int i = 0; i < bin_len; i++) putc (out_buf[i], out);
}

/* http://stackoverflow.com/a/199455 */
int addition_is_safe(unsigned long long a, unsigned long long b) {
	size_t a_bits=highestOneBitPosition(a), b_bits=highestOneBitPosition(b);
	return (a_bits<64 && b_bits<64);
}

int multiplication_is_safe(unsigned long long a, unsigned long long b) {
	size_t a_bits=highestOneBitPosition(a), b_bits=highestOneBitPosition(b);
	return (a_bits+b_bits<=64);
}
size_t highestOneBitPosition(unsigned long long a) {
	size_t bits=0;
	while (a!=0) {
		++bits;
		a>>=1;
	};
	return bits;
}
