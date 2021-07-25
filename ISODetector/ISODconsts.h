#ifndef ISODCONSTS_H
#define ISODCONSTS_H

#define MAX_BUFFER_SIZE 150
#define TEMPLATE_WIDTH 40
#define TEMPLATE_HEIGHT 80
#define TEMPLATE_BYTES_PER_PIXEL 3

struct templateOut {
	templateArr& bitmap;
	std::wstring filename;
	unsigned char* bufferHandle;
	unsigned int* offsetBuffer; 
	int offset; 
	int height; 
	int width; 
	int bpp;
};

#endif