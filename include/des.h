#ifndef DES_H
#define DES_H

#define DES_BLOCK_SIZE 8  // 64 bits, defined in the standard
#define DES_KEY_SIZE 8	  // 56 bits used, but must supply 64 (8 are ignored)
#define EXPANSION_BLOCK_SIZE 6
#define PC1_KEY_SIZE 7
#define SUBKEY_SIZE 6

#define GET_BIT(array, bit) ((array)[(int)((bit) / 8)] & (0x80 >> ((bit) % 8)))
#define SET_BIT(array, bit) ((array)[(int)((bit) / 8)] |= (0x80 >> ((bit) % 8)))
#define CLEAR_BIT(array, bit) \
	((array)[(int)((bit) / 8)] &= ~(0x80 >> ((bit) % 8)))

typedef enum { OP_ENCRYPT, OP_DECRYPT } op_type;

void des_encrypt(const unsigned char *, const int, unsigned char *,
		 const unsigned char *);

#endif /* end of include guard: DES_H */
