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
	//�����ļ���ʼ��
	bool InitializeCreate(const char* szFilePath, GDALDataType nDataType, const int& widthDst, const int& heightDst, const int& nBandNum);
	//��ȡ�ļ���ʼ��
	bool InitializeRead(const char* szFilePath, const bool& b16Trans8 = false);

	//���½�Ϊ��ʼ���
	bool ReadImage(int nStartCol, int nStartRow, int nWidth, int nHeight, unsigned char* pData, int nBufferWidth, int nBufferHeight);
	bool WriteImage(int nStartCol, int nStartRow, int nWidth, int nHeight, unsigned char* pData, int nBufferWidth, int nBufferHeight);

	//�Ѿ�׼���ö�ȡ����д��
	inline bool IsImgReady();
	int ReadImg(int nSrcLeft, int nSrcTop, int nSrcRight, int nSrcBottom, unsigned char* pBuf, int nBufWid, int nBufHeight, int nBandNum,
		int nDestLeft, int nDestTop, int nDestRight, int nDestBottom, int nSrcSkip, int nDestSkip);
	int WriteImg(int nSrcLeft, int nSrcTop, int nSrcRight, int nSrcBottom, unsigned char* pBuf, int nBufWid, int nBufHeight, int nBandNum,
		int nDestLeft, int nDestTop, int nDestRight, int nDestBottom, int nSrcSkip, int nDestSkip);

	//һ����ȫ����ȡ�����Ͻ�Ϊ��㣬�ڲ������ڴ棬�ǵ��ⲿdelete[]�������ڴ�й¶
	static bool ReadImageGDAL(const char* pFilePath, unsigned char **pImageData, GDALDataType nDataType, int& widthDst, int& heightDst, int& nBandNum);
	//һ����ȫ��д�룬���Ͻ�Ϊ��㣬�ڲ�ֻ���ڴ棬�ǵ��ⲿdelete[]�������ڴ�й¶
	static bool WriteImageGDAL(const char* pFilePath, unsigned char *pImageData, GDALDataType nDataType, const int& widthDst, const int& heightDst, const int& nBandNum);
	static char* findImageTypeGDAL(char *pDstImgFileName);
	//���ݸ�ʽ���ͻ�ȡռ���ֽ���
	static int GetBPB(const GDALDataType& nDataType);
	//���ݵ�ͨ��������������BGRתRGB, arrNewSeq = {2,1,0}����
	static int ReseqChannels(unsigned char* pData, const int& nRows, const int& nCols, const int& nBandNum, const int& nBPB, int arrNewSeq[]);
	//����y������з�ת
	static int VerticalReverse(unsigned char* pData, const int& nRows, const int& nCols, const int& nBandNum, const int& nBPB);

private:
	bool m_bTranto8bit;
	double* m_plfCount;
	double* m_pHist;

	GDALDataset *m_poDataset;

	int m_nOldBytesPerBand;

	unsigned char* m_plut;
};
