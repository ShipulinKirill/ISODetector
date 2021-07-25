#ifndef ISOD_H
#define ISOD_H

#include <windows.h>
#include <map>
#include "ISODconsts.h"

typedef unsigned int templateArr[TEMPLATE_HEIGHT][TEMPLATE_WIDTH] ;

class Isod
{
private:
	unsigned int**pixels;
	BITMAP bitmap;
	wchar_t fileName[MAX_BUFFER_SIZE];
	std::map <char, templateArr> symbTemp;
	BOOL OutCopyBlackAndWhite(templateOut temp);
	BOOL RebuildTemplate(templateArr &bitmap, int height, int width);
	BOOL CleanBitmap();
	int ErrorFunction(templateArr mask, double delta, int Left, int Top);
	void MakeLighter(unsigned int Depth, unsigned int Step, unsigned int SourcePixel, unsigned int &NearPixel);
	void BlurTemplate(unsigned int Depth, unsigned int Step, int Width, int Height, templateArr &bitmap);
	BOOL RebuildBitmap(unsigned int** bitmap,int height, int width);
	BOOL MonochromeBitmap();
public:
	wchar_t ownerCode[4];
	wchar_t number[7];
	wchar_t typeCode[4];
	Isod();
	BOOL LoadBitmap(wchar_t *fileName, BITMAP bitmap);
	BOOL GetDetect();
	~Isod();
};

#endif