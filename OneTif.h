#pragma once
#include <string>
#include <vector>
#include "gdal.h"
#include "gdal_priv.h"
#include <io.h>

enum _PIC_TYPE_
{
	Pixel_Byte = 0,
	Pixel_Int16 = 1,
	Pixel_SInt16 = 2,
	Pixel_Int32 = 3,
	Pixel_SInt32 = 4,
	Pixel_Float = 5,
	Pixel_Double = 6,
	Pixel_Int64 = 7,
	Pixel_SInt64 = 8
};

class COneTif
{
public:
	COneTif();
	~COneTif();

public:
	std::string m_strTifPath;

	double m_LBX;
	double m_LBY;
	double m_RTX;
	double m_RTY;
	double m_lfGSD;

	int m_nImgHeight;
	int m_nImgWidth;
	int m_nBandNum;

	int m_nBPB;
	int m_nBPP;

public:
	bool Initialize(bool b16Trans8 = true);
	bool ReadImage(int nStartCol, int nStartRow, int nWidth, int nHeight, unsigned char* pData, int nBufferWidth, int nBufferHeight);
	bool readImageGDAL(unsigned char **pImageData, int &width, int &height, int &nChannels, const char *filePath);
	bool WriteImageGDAL(const char* pDstImgFileName, unsigned char *pImageData, int width, int height, int nChannels);
	char* findImageTypeGDAL(char *pDstImgFileName);
	int ReadImg(int nSrcLeft, int nSrcTop, int nSrcRight, int nSrcBottom, unsigned char* pBuf, int nBufWid, int nBufHeight, int nBandNum,
		int nDestLeft, int nDestTop, int nDestRight, int nDestBottom, int nSrcSkip, int nDestSkip);

private:
	bool m_bHasOverviews;
	bool m_bTranto8bit;
	GDALDataset *m_poDataset;
	GDALDataType m_eGDALType;
	_PIC_TYPE_ m_nDataType;
	int m_nBytesPerBand;
	int m_nOldBytesPerBand;
	unsigned char* m_plut;
	double* m_plfCount;
	double* m_pHist;
};
