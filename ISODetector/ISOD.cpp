#include "stdafx.h"
#include "ISOD.h"
#include <string>
#include <fstream>

//Конструктор
Isod::Isod()
{
	std::fstream fout("test_3.txt", std::ios::out);
	//------------------------------------Создание и инициализация элементов обьекта------------------------------------
	pixels = new unsigned int*[1024];
	for (int i = 0; i < 1024; i++) {
		pixels[i] = new unsigned int[1024];
	}
	//------------------------------------Создание пути шаблона------------------------------------
	std::wstring tempFilename;
	const std::size_t buf_len = 260;
	LPTSTR absPath = new TCHAR[buf_len];
	auto path_len = GetModuleFileName(GetModuleHandle(NULL), absPath, buf_len);
	HANDLE hBitmap;
	int counter;
	for (counter = path_len; counter != 0 && absPath[counter] != '\\'; counter--) {}
	absPath[counter] = '\\';														
	absPath[counter + 1] = NULL;

	//------------------------------------Расчёт отступа------------------------------------
	int offset = 54;
	unsigned int* offsetBuffer = new unsigned int[offset - 14];
	unsigned char bufferHandle[14];
	DWORD count = 0;
	//------------------------------------Считывание шаблона 0..9------------------------------------
	for (int i = 0; i < 10; i++) {
		tempFilename = absPath;
		tempFilename += L"symbTemplate\\";
		tempFilename += std::to_wstring(i);
		tempFilename += L".bmp";
		//------------------------------------Считываем Handler битмапы------------------------------------
		hBitmap = CreateFile(tempFilename.c_str(), GENERIC_READ | GENERIC_WRITE,
			0, NULL, OPEN_EXISTING, 0, NULL);
		if (hBitmap == INVALID_HANDLE_VALUE) {
			MessageBox(NULL, L"Cannot open template bmp file handler", L"Error", MB_OK);
		}		
		//------------------------------------Отступ------------------------------------
		if (!ReadFile(hBitmap, &bufferHandle, 14, &count, NULL)) {
			MessageBox(NULL, L"Cannot read bmp file handler", L"Error", MB_OK);
		}
		if (!ReadFile(hBitmap, offsetBuffer, offset - 14, &count, NULL)) {
			MessageBox(NULL, L"Cannot read template offset", L"Error", MB_OK);
		}
		//------------------------------------Считывание массива пикселей------------------------------------
		for (int rowCounter = TEMPLATE_HEIGHT - 1; rowCounter >= 0; rowCounter--) {
			for (int colCounter = 0; colCounter <= TEMPLATE_WIDTH; colCounter++) {
				if (!ReadFile(hBitmap, &this->symbTemp[i][rowCounter][colCounter], TEMPLATE_BYTES_PER_PIXEL, &count, NULL)) {
					MessageBox(NULL, L"Cannot read pixel", L"Error", MB_OK);
				}
			}
		}
		#if 0
			RebuildTemplate(this->symbTemp[i], TEMPLATE_HEIGHT, TEMPLATE_WIDTH);
			BlurTemplate(3, 15, TEMPLATE_HEIGHT, TEMPLATE_WIDTH, this->symbTemp[i]);
			std::wstring newFileName = std::to_wstring(i);
			newFileName += L".bmp";
			templateOut temp{
				this->symbTemp[i],
				newFileName,
				bufferHandle,
				offsetBuffer,
				offset,
				TEMPLATE_HEIGHT,
				TEMPLATE_WIDTH,
				TEMPLATE_BYTES_PER_PIXEL
			};
			OutCopyBlackAndWhite(temp);
		#endif
	}

	//------------------------------------Считывание шаблона A..Z------------------------------------
	for (int i = 0; i < 26; i++) {
		tempFilename = absPath;
		tempFilename += L"symbTemplate\\";
		tempFilename += std::to_wstring(i + 'A');
		tempFilename += L".bmp";
		//------------------------------------Считываем Handler битмапы------------------------------------
		hBitmap = CreateFile(tempFilename.c_str(), GENERIC_READ | GENERIC_WRITE,
			0, NULL, OPEN_EXISTING, 0, NULL);
		if (hBitmap == INVALID_HANDLE_VALUE) {
			MessageBox(NULL, L"Cannot open template bmp file handler", L"Error", MB_OK);
		}
		//------------------------------------Отступ------------------------------------
		if (!ReadFile(hBitmap, &bufferHandle, 14, &count, NULL)) {
			MessageBox(NULL, L"Cannot read bmp file handler", L"Error", MB_OK);
		}
		if (!ReadFile(hBitmap, offsetBuffer, offset - 14, &count, NULL)) {
			MessageBox(NULL, L"Cannot read template offset", L"Error", MB_OK);
		}
		//------------------------------------Считывание массива пикселей------------------------------------
		for (int rowCounter = TEMPLATE_HEIGHT - 1; rowCounter >= 0; rowCounter--) {
			for (int colCounter = 0; colCounter <= TEMPLATE_WIDTH; colCounter++) {
				if (!ReadFile(hBitmap, &this->symbTemp[i + 'A'][rowCounter][colCounter], TEMPLATE_BYTES_PER_PIXEL, &count, NULL)) {
					MessageBox(NULL, L"Cannot read pixel", L"Error", MB_OK);
				}

			}
		}
		#if 0
		RebuildTemplate(this->symbTemp[i + 'A'], TEMPLATE_HEIGHT, TEMPLATE_WIDTH);
		BlurTemplate(3, 15, TEMPLATE_HEIGHT, TEMPLATE_WIDTH, this->symbTemp[i + 'A']);
		std::wstring newFileName = std::to_wstring(i + 'A');
		newFileName += L".bmp";
		templateOut temp{
				this->symbTemp[i + 'A'],
				newFileName,
				bufferHandle,
				offsetBuffer,
				offset,
				TEMPLATE_HEIGHT,
				TEMPLATE_WIDTH,
				TEMPLATE_BYTES_PER_PIXEL
		};
		OutCopyBlackAndWhite(temp);
		#endif
	}
	fout.close();
	delete[] offsetBuffer;
	DeleteObject(hBitmap);
}

//Загрузка изображения
BOOL Isod::LoadBitmap(wchar_t* fileName, BITMAP bitmap) {
	if (wcscpy_s(this->fileName, fileName) != 0) {
		return FALSE;
	}
	this->bitmap = bitmap;
	return TRUE;
}

//Перестройка шаблона
BOOL Isod::RebuildTemplate(templateArr &bitmap, int height, int width)
{
	int r = 0;
	int g = 0;
	int b = 0;
	const BYTE BackgroundColor = 127;
	for (int rowCounter = 0; rowCounter < height; rowCounter++) {
		for (int colCounter = 0; colCounter <= width; colCounter++) {
			if (bitmap[rowCounter][colCounter] >= 16777216) { // 16777216 == 01 00 00 00 == ?? RR GG BB удаление альфа канала
				bitmap[rowCounter][colCounter] = bitmap[rowCounter][colCounter] % 16777216;
			}
			r = bitmap[rowCounter][colCounter] / 65536;
			g = (bitmap[rowCounter][colCounter] % 65536) / 256;
			b = bitmap[rowCounter][colCounter] % 256;
			bitmap[rowCounter][colCounter] = (r + g + b) / 3;

			if (bitmap[rowCounter][colCounter] <= 128) {
				bitmap[rowCounter][colCounter] = 255;
			}
			else {
				bitmap[rowCounter][colCounter] = BackgroundColor;
			}
		}
	}
	return TRUE;
}

//Перестройка битмапы
BOOL Isod::RebuildBitmap(unsigned int** bitmap, int height, int width)
{
	int r = 0;
	int g = 0;
	int b = 0;
	for (int rowCounter = 0; rowCounter < height; rowCounter++) {
		for (int colCounter = 0; colCounter <= width; colCounter++) {
			if (bitmap[rowCounter][colCounter] >= 16777216) { // 16777216 == 01 00 00 00 == ?? RR GG BB удаление альфа канала
				bitmap[rowCounter][colCounter] = bitmap[rowCounter][colCounter] % 16777216;
			}
			r = bitmap[rowCounter][colCounter] / 65536;
			g = (bitmap[rowCounter][colCounter] % 65536) / 256;
			b = bitmap[rowCounter][colCounter] % 256;
			bitmap[rowCounter][colCounter] = (r + g + b)/3; //градации серого
		}
	}
	return TRUE;
}

//Высветление
void Isod::MakeLighter(unsigned int Depth, unsigned int Step, unsigned int SourcePixel, unsigned int &NearPixel)
{
	if (SourcePixel > NearPixel + Step)
			NearPixel = SourcePixel - Step;
}

//Размытие шаблона
void Isod::BlurTemplate(unsigned int Depth, unsigned int Step, int Width, int Height, templateArr &bitmap)
{
	for (int rows = 1; rows <= Width - 1; rows++)
		for (int cols = 1; cols < Height - 1; cols++)
			if (bitmap[rows][cols] >= (128 + Step*Depth))
			{
				//Для каждого пикселя пытаемся осветлить 8 соседних с ним!
				MakeLighter(Depth, Step, bitmap[rows][cols], bitmap[rows - 1][cols - 1]);
				MakeLighter(Depth, Step, bitmap[rows][cols], bitmap[rows - 1][cols + 0]);
				MakeLighter(Depth, Step, bitmap[rows][cols], bitmap[rows - 1][cols + 1]);
				MakeLighter(Depth, Step, bitmap[rows][cols], bitmap[rows + 0][cols - 1]);
				MakeLighter(Depth, Step, bitmap[rows][cols], bitmap[rows + 0][cols + 1]);
				MakeLighter(Depth, Step, bitmap[rows][cols], bitmap[rows + 1][cols - 1]);
				MakeLighter(Depth, Step, bitmap[rows][cols], bitmap[rows + 1][cols + 0]);
				MakeLighter(Depth, Step, bitmap[rows][cols], bitmap[rows + 1][cols + 1]);
			}
}

BOOL Isod::CleanBitmap() {
	return TRUE;
}

//Вывод чёрно-белой битмапы
BOOL Isod::OutCopyBlackAndWhite(templateOut temp)
{
	//------------------------------------Вывод копии серые оттенки------------------------------------------------------
	HANDLE hbitmapOut;
	DWORD count = 0;
	hbitmapOut = CreateFile(temp.filename.c_str(),
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		CREATE_ALWAYS,
		0,
		NULL);
	if (hbitmapOut == INVALID_HANDLE_VALUE) {
		MessageBox(NULL, L"Cannot create CopyBlackAndWhite bmp file handler", L"Error", MB_OK);
		return FALSE;
	}

	if (!WriteFile(hbitmapOut, temp.bufferHandle, 14, &count, NULL)) {
		MessageBox(NULL, L"Cannot write CopyBlackAndWhite bmp file handler", L"Error", MB_OK);
		return FALSE;
	}

	if (!WriteFile(hbitmapOut, temp.offsetBuffer, temp.offset - 14, &count, NULL)) {
		MessageBox(NULL, L"Cannot write CopyBlackAndWhite bmp file info", L"Error", MB_OK);
		return FALSE;
	}

	for (int rowCounter = temp.height - 1; rowCounter >= 0; rowCounter--) {
		for (int colCounter = 0; colCounter <= temp.width; colCounter++) {
			for (int k = 0; k < temp.bpp; k++) {
				if (!WriteFile(hbitmapOut, &temp.bitmap[rowCounter][colCounter], 1, &count, NULL)) {
					MessageBox(NULL, L"Cannot read CopyBlackAndWhite pixel", L"Error", MB_OK);
					return FALSE;
				}
			}
		}
	}
	DeleteObject(hbitmapOut);
}

//Монохромирование битмапы
BOOL Isod::MonochromeBitmap()
{
	HANDLE hbitmap;
	std::fstream fout("test_1.txt", std::ios::out);
	//------------------------------------Считываем Handler битмапы------------------------------------
	hbitmap = CreateFile(this->fileName, GENERIC_READ | GENERIC_WRITE,
						0, NULL, OPEN_EXISTING, 0, NULL);
	if (hbitmap == INVALID_HANDLE_VALUE) {
		MessageBox(NULL, L"Cannot open bmp file handler", L"Error", MB_OK);
		return FALSE;
	}
	
	unsigned char bufferHandle[14];
	DWORD count = 0;
	if (!ReadFile(hbitmap, &bufferHandle, 14, &count, NULL)) {
		MessageBox(NULL, L"Cannot read bmp file handler", L"Error", MB_OK);
		return FALSE;
	}
	//------------------------------------Расчёт отступа------------------------------------
	int offset = (bufferHandle[10] << 0) | (bufferHandle[11] << 8) | (bufferHandle[12] << 16) | (bufferHandle[13] << 24);
	int width = this->bitmap.bmWidth;
	int height = this->bitmap.bmHeight;
	int bytesPerPixel = this->bitmap.bmBitsPixel / 8;
	//------------------------------------Отступ------------------------------------
	unsigned int*offsetBuffer = new unsigned int[offset - 14];
	if (!ReadFile(hbitmap, offsetBuffer, offset - 14, &count, NULL)) {
		MessageBox(NULL, L"Cannot read offset", L"Error", MB_OK);
		return FALSE;
	}
	//------------------------------------Считывание массива пикселей------------------------------------
	for (int rowCounter = height - 1; rowCounter >= 0; rowCounter--) {
		for (int colCounter = 0; colCounter <= width; colCounter++) {
			if (!ReadFile(hbitmap, &pixels[rowCounter][colCounter], bytesPerPixel, &count, NULL)) {
				MessageBox(NULL, L"Cannot read pixel", L"Error", MB_OK);
				return FALSE;
			}
		}
	}
	
	if (!RebuildBitmap(this->pixels, this->bitmap.bmHeight, this->bitmap.bmWidth)) {
		MessageBox(NULL, L"Cannot rebuild bitmap", L"Error", MB_OK);
		return FALSE;
	}
	//------------------------------------Очистка------------------------------------
	fout.close();
	delete[] offsetBuffer;
	DeleteObject(hbitmap);
	return TRUE;
} 

//Определение
BOOL Isod::GetDetect() //всё менять
{
	MonochromeBitmap();
	std::fstream fout("test_2.txt", std::ios::out);
	int width = this->bitmap.bmWidth;
	int height = this->bitmap.bmHeight;
	//------------------------------------Заполнение массивов------------------------------------
	
	fout.close();
	return TRUE;
}

BOOL Isod::NewWave() 
{

}

//Функция ошибки при распознании
int Isod::ErrorFunction(templateArr mask, double delta, int Left, int Top)
{
	int Result = 0;
	for (int row = 0; row < TEMPLATE_HEIGHT; row++)
		for (int col = 0; col <= TEMPLATE_WIDTH; col++)
		{
			//прибавляем к ошибке модуль разности
			Result += abs(static_cast<int>(this->pixels[Left + row][Top + col] - mask[static_cast<int>(row * delta)][static_cast<int>(col * delta)]));
		}
	return Result;
}

//Деструктор
Isod::~Isod()
{
	for (int i = 0; i < 1024; i++) {
		delete[] pixels[i];
	}
	delete[] pixels;
	symbTemp.clear();
}
