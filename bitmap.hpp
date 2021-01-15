//
//  Copyright (c) XiaoStore. All rights reserved.
//

#pragma once

bool Rgb2BmpFile(
	const char *pFile,
	const void *pBuffer,
	int nWidth, int nHeight, int nBitCount = 32,
	BITMAPINFOHEADER *pBitMapInfoHeader = NULL,
	BITMAPFILEHEADER *pBitMapFileHeader = NULL
	)
{
	/* align */
	const int c_nBitsPerByte = 8;
	const int c_nAlignBytes = 4;
	int nAlignGaps = c_nAlignBytes * c_nBitsPerByte - 1;
	int widthStep = (((nWidth * nBitCount) + nAlignGaps) & (~nAlignGaps)) / c_nBitsPerByte;

	BITMAPINFOHEADER bih;
	if (pBitMapInfoHeader)
	{
		memcpy(&bih, pBitMapInfoHeader, sizeof(bih));
	}
	else
	{
		bih.biSize = 40;       // header size  
		bih.biWidth = nWidth;
		bih.biHeight = nHeight;
		bih.biPlanes = 1;
		bih.biBitCount = nBitCount;     // RGB encoded, 8/16/24/32 bit  
		bih.biCompression = BI_RGB;   // no compression
		bih.biSizeImage = widthStep * nHeight;
		bih.biXPelsPerMeter = 0;
		bih.biYPelsPerMeter = 0;
		bih.biClrUsed = 0;
		bih.biClrImportant = 0;
	}

	BITMAPFILEHEADER bhh;
	if (pBitMapFileHeader)
	{
		memcpy(&bhh, pBitMapFileHeader, sizeof(bhh));
	}
	else
	{
		bhh.bfType = ((WORD)('M' << 8) | 'B');  //'BM'  
		bhh.bfSize = (DWORD)sizeof(BITMAPFILEHEADER)+(DWORD)sizeof(BITMAPINFOHEADER)+
			widthStep * nHeight;
		bhh.bfReserved1 = 0;
		bhh.bfReserved2 = 0;
		bhh.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER)+(DWORD)sizeof(BITMAPINFOHEADER);
	}

	DeleteFileA(pFile);

	HANDLE hFile;
	DWORD dwNumberOfBytesWritten;
	hFile = CreateFileA(pFile, GENERIC_WRITE,
		/*FILE_SHARE_WRITE*/0, NULL, OPEN_ALWAYS,
		/*FILE_ATTRIBUTE_HIDDEN*/FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		//DWORD dwLastError = GetLastError();
		return FALSE;
	}

	SetFilePointer(hFile, 0, NULL, FILE_END);

	BOOL BRet = WriteFile(hFile, (LPSTR)&bhh, sizeof(BITMAPFILEHEADER),
		&dwNumberOfBytesWritten, NULL);
	BRet = WriteFile(hFile, (LPSTR)&bih, sizeof(BITMAPINFOHEADER), &dwNumberOfBytesWritten, NULL);

	DWORD nNumberOfBytesToWrite = widthStep * nHeight;
	BRet = WriteFile(hFile, pBuffer, nNumberOfBytesToWrite, &dwNumberOfBytesWritten, NULL);

	CloseHandle(hFile);

	return TRUE;
}