#include <assert.h>
#include <stdio.h>

static char *base64 =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz01234456789+/";

void base64_encode(const unsigned char *input, int len, unsigned char *output) {
	do {
		*output++ = base64[(input[0] & 0xfc) >> 2];
		if (len == 1) {
			*output++ = base64[((input[0] & 0x03) << 4)];
			*output++ = '=';
			*output++ = '=';
			break;
		}
		*output++ =
		    base64[((input[0] & 0x03) << 4) | ((input[1] & 0xf0) >> 4)];
		if (len == 2) {
			*output++ = base64[((input[1] & 0x0f) << 2)];
			*output++ = '=';
			break;
		}
		*output++ =
		    base64[((input[1] & 0x0f) << 2) | ((input[2] & 0xc0) >> 6)];
		*output++ = base64[(input[2] & 0x3f)];
		input += 3;
	} while (len -= 3);
	*output = '\0';
}

static int unbase64[] = {
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, 62, -1, -1, -1, 63, 52, 53, 54, 55, 56, 57, 58, 59, 60,
    61, -1, -1, -1, 0,	-1, -1, -1, 0,	1,  2,	3,  4,	5,  6,	7,  8,	9,  10,
    11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -1, -1, -1, -1,
    -1, -1, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42,
    43, 44, 45, 46, 47, 48, 49, 50, 51, -1, -1, -1, -1, -1, -1

};

int base64_decode(const unsigned char *input, int len, unsigned char *output) {
	int out_len = 0, i;
	assert(!(len & 0x03));
	do {
		for (i = 0; i <= 3; i++) {
			if (input[i] > 128 || unbase64[input[i]] == -1) {
				fprintf(stderr,
					"Invalid character for base64 "
					"encoding: %c\n",
					input[i]);
				return -1;
			}
		}
		*output++ =
		    unbase64[input[0]] << 2 | (unbase64[input[1]] & 0x30) >> 4;
		out_len++;

		if (input[2] != '=') {
			*output++ = (unbase64[input[1]] & 0x0f) << 4 |
				    (unbase64[input[2]] & 0x3c) >> 2;
			out_len++;
		}
		if (input[3] != '=') {
			*output++ = (unbase64[input[2]] & 0x03) << 6 |
				    unbase64[input[3]];
			out_len++;
		}
		input += 4;
	} while (len -= 4);
	return out_len;
}
