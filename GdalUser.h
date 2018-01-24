#pragma once
#include <string>
#include <vector>
#include "gdal.h"
#include "gdal_priv.h"
#include <io.h>

#ifdef _DEBUG
#pragma comment(lib, "gdal\\x64\\gdal_i.lib")
#else
#pragma comment(lib, "gdal\\x64\\gdal_i.lib")
#endif

class CGdalUser
{
public:
	CGdalUser();
	~CGdalUser();

public:
	double m_LBX;
	double m_LBY;
	double m_RTX;
	double m_RTY;
	double m_lfGSD;

	GDALDataType m_nDataType;

	int m_nImgWidth;
	int m_nImgHeight;
	int m_nBandNum;
	int m_nBPB;
	int m_nBPP;

	std::string m_strFilePath;

public:
	//创建文件初始化
	bool InitializeCreate(const char* szFilePath, GDALDataType nDataType, const int& widthDst, const int& heightDst, const int& nBandNum);
	//读取文件初始化
	bool InitializeRead(const char* szFilePath, const bool& b16Trans8 = false);

	//左下角为起始点的
	bool ReadImage(int nStartCol, int nStartRow, int nWidth, int nHeight, unsigned char* pData, int nBufferWidth, int nBufferHeight);
	bool WriteImage(int nStartCol, int nStartRow, int nWidth, int nHeight, unsigned char* pData, int nBufferWidth, int nBufferHeight);

	//已经准备好读取或者写入
	inline bool IsImgReady();
	int ReadImg(int nSrcLeft, int nSrcTop, int nSrcRight, int nSrcBottom, unsigned char* pBuf, int nBufWid, int nBufHeight, int nBandNum,
		int nDestLeft, int nDestTop, int nDestRight, int nDestBottom, int nSrcSkip, int nDestSkip);
	int WriteImg(int nSrcLeft, int nSrcTop, int nSrcRight, int nSrcBottom, unsigned char* pBuf, int nBufWid, int nBufHeight, int nBandNum,
		int nDestLeft, int nDestTop, int nDestRight, int nDestBottom, int nSrcSkip, int nDestSkip);

	//一次性全部读取，左上角为起点，内部分配内存，记得外部delete[]，否则内存泄露
	static bool ReadImageGDAL(const char* pFilePath, unsigned char **pImageData, GDALDataType nDataType, int& widthDst, int& heightDst, int& nBandNum);
	//一次性全部写入，左上角为起点，内部只读内存，记得外部delete[]，否则内存泄露
	static bool WriteImageGDAL(const char* pFilePath, unsigned char *pImageData, GDALDataType nDataType, const int& widthDst, const int& heightDst, const int& nBandNum);
	static char* findImageTypeGDAL(char *pDstImgFileName);
	//根据格式类型获取占用字节数
	static int GetBPB(const GDALDataType& nDataType);
	//数据的通道重新排序，例如BGR转RGB, arrNewSeq = {2,1,0}即可
	static int ReseqChannels(unsigned char* pData, const int& nRows, const int& nCols, const int& nBandNum, const int& nBPB, int arrNewSeq[]);
	//数据y方向进行翻转
	static int VerticalReverse(unsigned char* pData, const int& nRows, const int& nCols, const int& nBandNum, const int& nBPB);

private:
	bool m_bTranto8bit;
	double* m_plfCount;
	double* m_pHist;

	GDALDataset *m_poDataset;

	int m_nOldBytesPerBand;

	unsigned char* m_plut;
};
