#ifndef DECOMPRESS_UNLZ4_H
#define DECOMPRESS_UNLZ4_H

<<<<<<< HEAD
int unlz4(unsigned char *inbuf, int len,
	int(*fill)(void*, unsigned int),
	int(*flush)(void*, unsigned int),
	unsigned char *output,
	int *pos,
=======
int unlz4(unsigned char *inbuf, long len,
	long (*fill)(void*, unsigned long),
	long (*flush)(void*, unsigned long),
	unsigned char *output,
	long *pos,
>>>>>>> android-3.18
	void(*error)(char *x));
#endif
