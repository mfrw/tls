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
