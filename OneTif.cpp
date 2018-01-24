#include "OneTif.h"

COneTif::COneTif()
{
	m_strTifPath = "";
	m_LBX = 0.0;
	m_LBY = 0.0;
	m_RTX = 0.0;
	m_RTY = 0.0;
	m_lfGSD = 0.0;

	m_nImgHeight = 0;
	m_nImgWidth = 0;
	m_nBandNum = 0;
	int m_nBPB = 1;
	int m_nBPP = 1;

	m_bHasOverviews = false;
	m_bTranto8bit = true;
	m_poDataset = NULL;
	m_eGDALType = GDT_Unknown;
	m_nDataType = Pixel_Byte;
	m_nBytesPerBand = 1;
	m_nOldBytesPerBand = 1;
	m_plut = NULL;
	m_plfCount = NULL;
	m_pHist = NULL;
}

COneTif::~COneTif()
{
	if (m_plut)
	{
		delete[] m_plut;
		m_plut = NULL;
	}

	if (m_plfCount)
	{
		delete[] m_plfCount;
		m_plfCount = NULL;
	}

	if (m_pHist)
	{
		delete[] m_pHist;
		m_pHist = NULL;
	}

	if (m_poDataset != NULL)
	{
		GDALClose((GDALDatasetH)m_poDataset);
		m_poDataset = NULL;
	}
}

bool COneTif::Initialize(bool b16Trans8)
{
	const char *pszFilename = m_strTifPath.c_str();
	GDALAllRegister();
	CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");   //设置支持中文路径  

	m_poDataset = (GDALDataset *)GDALOpen(pszFilename, GA_ReadOnly);

	if (m_poDataset == NULL)
	{
		return false;
	}
	double adfGeoTransform[6] = { 0 };
	m_nImgWidth = m_poDataset->GetRasterXSize();
	m_nImgHeight = m_poDataset->GetRasterYSize();
	m_nBandNum = m_poDataset->GetRasterCount();

	m_eGDALType = m_poDataset->GetRasterBand(1)->GetRasterDataType();

	if (m_poDataset->GetRasterBand(1)->GetOverviewCount() != 0)
	{
		m_bHasOverviews = true;
	}

	switch (m_eGDALType)
	{
	case 1:
	{
			  m_nDataType = Pixel_Byte;
			  m_nBytesPerBand = 1;
			  break;
	}
	case 2:
	{
			  m_nDataType = Pixel_Int16;
			  m_nBytesPerBand = 2;
			  break;
	}
	case 3:
	{
			  m_nDataType = Pixel_SInt16;
			  m_nBytesPerBand = 2;
			  break;
	}
	case 4:
	{
			  m_nDataType = Pixel_Int32;
			  m_nBytesPerBand = 4;
			  break;
	}
	case 5:
	{
			  m_nDataType = Pixel_SInt32;
			  m_nBytesPerBand = 4;
			  break;
	}
	case 6:
	{
			  m_nDataType = Pixel_Float;
			  m_nBytesPerBand = 4;
			  break;
	}
	case 7:
	{
			  m_nDataType = Pixel_Double;
			  m_nBytesPerBand = 8;
			  break;
	}
	}

	m_nBPB = m_nBytesPerBand;
	m_nOldBytesPerBand = m_nBPB;
	m_nBPP = m_nBandNum * m_nBPB;

	if (m_poDataset->GetGeoTransform(adfGeoTransform) == CE_None)
	{
		//左上角
		m_LBX = adfGeoTransform[0];
		m_RTY = adfGeoTransform[3];
		m_lfGSD = adfGeoTransform[1];
	}
	else
	{
		return false;
	}

	//计算出的值
	m_RTX = m_LBX + m_lfGSD * m_nImgWidth;
	m_LBY = m_RTY - m_lfGSD * m_nImgHeight;

	if (m_nBPB == 2 && b16Trans8 == true)
	{
		m_bTranto8bit = true;
	}
	else
	{
		m_bTranto8bit = false;
	}

	if (m_nBPB == 2 && b16Trans8 == true)
	{
		int nBandNum = m_nBandNum;
		if (m_plut)
		{
			delete[] m_plut;
			m_plut = NULL;
		}

		m_plut = new unsigned char[nBandNum * 65536];
		memset(m_plut, 0, sizeof(unsigned char)* 65536 * nBandNum);

		//保存lut文件
		std::string szFileName = m_strTifPath + ".czb";
		if (_access(szFileName.c_str(), 0) == -1)
		{
			const int nBlockLength = 5000;
			double dWidthIniProp = (double)nBlockLength / m_nImgWidth;
			double dHeightIniProp = (double)nBlockLength / m_nImgHeight;
			double dIniScale = dWidthIniProp < dHeightIniProp ? dWidthIniProp : dHeightIniProp;
			int nIniWidth = int(m_nImgWidth * dIniScale + 0.5);
			int nIniHeight = int(m_nImgHeight * dIniScale + 0.5);

			if (dIniScale >= 1.0)
			{
				//此时不需要缩放
				dIniScale = 1.0;
				nIniWidth = m_nImgWidth;
				nIniHeight = m_nImgHeight;
			}

			unsigned char *m_pBufferPy = new unsigned char[nIniWidth * nIniHeight * nBandNum * m_nBPB];
			memset(m_pBufferPy, 0, nIniWidth * nIniHeight * nBandNum * m_nBPB);

			const int nBlock = 1024;
			unsigned char *m_pBlockBuffer = new unsigned char[m_nImgWidth * nBandNum * m_nBPB * nBlock];
			memset(m_pBlockBuffer, 0, sizeof(unsigned char)* m_nImgWidth * nBandNum * m_nBPB * nBlock);

			int nBPP = m_nBPB * nBandNum;

			COneTif tif;
			tif.m_strTifPath = pszFilename;
			tif.Initialize(false);

			int nRowBlockNum = (m_nImgHeight + nBlock - 1) / nBlock;
			for (int j = 0; j < nRowBlockNum; ++j)
			{
				memset(m_pBlockBuffer, 0, sizeof(unsigned char)* m_nImgWidth * nBandNum * m_nBPB * nBlock);
				tif.ReadImage(0, j * nBlock, m_nImgWidth, nBlock, m_pBlockBuffer, m_nImgWidth, nBlock);

				for (int m = 0; m < nIniHeight; m++)
				{
					int nSrcRows = int(m / dIniScale + 0.5) - j * nBlock;
					if (nSrcRows >= nBlock || nSrcRows < 0)
					{
						continue;
					}
					unsigned char *pBufferPyIndex = m_pBufferPy + m * nIniWidth * nBPP;
					unsigned char *pBufferBlockIndex = m_pBlockBuffer + nSrcRows * m_nImgWidth * nBPP;
					for (int n = 0; n < nIniWidth; n++)
					{
						int nSrcCols = int(n / dIniScale + 0.5);
						if (nSrcCols >= m_nImgWidth)
						{
							continue;
						}
						unsigned char *pSubBufferPyIndex = pBufferPyIndex + n * nBPP;
						unsigned char *pSubBufferBlockIndex = pBufferBlockIndex + nSrcCols * nBPP;

						memcpy(pSubBufferPyIndex, pSubBufferBlockIndex, nBPP);
					}
				}
			}
			delete[] m_pBlockBuffer;
			m_pBlockBuffer = NULL;

			m_pHist = new double[65536 * nBandNum];
			memset(m_pHist, 0, sizeof(double)* 65536 * nBandNum);
			for (int j = 0; j < nIniHeight; ++j)
			{
				unsigned short *pBufferIndex = (unsigned short*)m_pBufferPy + j * nIniWidth * nBandNum;
				for (int i = 0; i < nIniWidth; ++i)
				{
					unsigned short *pSubBufferIndex = pBufferIndex + i * nBandNum;
					for (int k = 0; k < nBandNum; ++k)
					{
						m_pHist[k * 65536 + pSubBufferIndex[k]] += 1.0;
					}
				}
			}

			if (m_plfCount)
			{
				delete[] m_plfCount;
				m_plfCount = NULL;
			}

			m_plfCount = new double[10];
			memset(m_plfCount, 0, sizeof(double)* 10);
			for (int i = 0; i < nBandNum; i++)
			{
				for (int j = 1; j < 65536; j++)
				{
					m_plfCount[i] += m_pHist[i * 65536 + j];
				}
			}

			double lfMinThreshold = 0.001, lfMaxThreshold = 0.001;

			for (int i = 0; i < nBandNum; i++)
			{
				double lfTmpCount = 0.0001;
				int nMinCut = 1, nMaxCut = 65535;
				for (int j = 1; j < 65536; j++)
				{
					lfTmpCount += m_pHist[i * 65536 + j];
					if (lfTmpCount / m_plfCount[i] > lfMinThreshold)
					{
						nMinCut = j;
						break;
					}
				}
				int nMinValue = 0, nMaxValue = 0;
				for (int j = 1; j < 65536; j++)
				{
					if (m_pHist[i * 65536 + j] > 1e-3)
					{
						nMinValue = j;
						break;
					}
				}
				for (int j = 65534; j > 0; j--)
				{
					if (m_pHist[i * 65536 + j] > 1e-3)
					{
						nMaxValue = j;
						break;
					}
				}

				lfTmpCount = 0.0001;
				for (int j = 65534; j > 0; j--)
				{
					lfTmpCount += m_pHist[i * 65536 + j];
					if (lfTmpCount / m_plfCount[i] > lfMaxThreshold)
					{
						nMaxCut = j;
						break;
					}
				}
				for (int j = 1; j < nMinCut; j++)
				{
					m_plut[i * 65536 + j] = 1;
				}
				for (int j = nMinCut; j <= nMaxCut; j++)
				{
					m_plut[i * 65536 + j] = MAX(1, MIN(253, (int)(251.0 * ((double)j - nMinCut) / ((double)nMaxCut - nMinCut) + 2)));
				}
				for (int j = nMaxCut + 1; j < 65536; j++)
				{
					m_plut[i * 65536 + j] = 254;
				}
			}

			delete[] m_plfCount;
			m_plfCount = NULL;
			delete[] m_pHist;
			m_pHist = NULL;
			delete[] m_pBufferPy;
			m_pBufferPy = NULL;

			FILE *pFile;
			if (fopen_s(&pFile, szFileName.c_str(), "wb") == 0)
			{
				fwrite(m_plut, sizeof(unsigned char), 65536 * m_nBandNum, pFile);
				// 		for (int i = 0; i < 65536; ++i)
				// 		{
				// 			fprintf(pFile, "%d\n", m_pClrLut[i]);
				// 		}
			}
			fclose(pFile);
		}
		else
		{
			//读取lut文件
			FILE *pFile;
			if (fopen_s(&pFile, szFileName.c_str(), "rb") == 0)
			{
				fread(m_plut, sizeof(unsigned char), 65536 * m_nBandNum, pFile);
			}
			fclose(pFile);
		}

	}
	return true;
}

bool COneTif::ReadImage(int nStartCol, int nStartRow, int nWidth, int nHeight, unsigned char* pData, int nBufferWidth, int nBufferHeight)
{
	if (m_nBandNum == 1)
	{
		return 	ReadImg(nStartCol, nStartRow, nStartCol + nWidth, nStartRow + nHeight, pData, nBufferWidth, nBufferHeight, m_nBandNum, 0, 0, nBufferWidth, nBufferHeight, 0, 0) == 0;
	}
	return 	ReadImg(nStartCol, nStartRow, nStartCol + nWidth, nStartRow + nHeight, pData, nBufferWidth, nBufferHeight, m_nBandNum, 0, 0, nBufferWidth, nBufferHeight, -1, 0) == 0;
}

int COneTif::ReadImg(int nSrcLeft, int nSrcTop, int nSrcRight, int nSrcBottom, unsigned char* pBuf, int nBufWid, int nBufHeight, int nBandNum,
	int nDestLeft, int nDestTop, int nDestRight, int nDestBottom, int nSrcSkip, int nDestSkip)
{
	int m_nWidth = m_nImgWidth;
	int m_nHeight = m_nImgHeight;

	double lfScale = (nDestRight - nDestLeft) / (double)(nSrcRight - nSrcLeft);

	if (nSrcLeft >= m_nWidth || nSrcTop >= m_nHeight || nSrcRight <= 0 || nSrcBottom <= 0)
	{
		return 1;
	}

	int nStartRowOffset = 0, nStartColOffset = 0, nEndRowOffset = 0, nEndColOffset = 0;

	if (nSrcLeft < 0)
	{
		nStartColOffset = -nSrcLeft;
	}
	if (nSrcTop < 0)
	{
		nStartRowOffset = -nSrcTop;
	}
	if (nSrcRight > m_nWidth)
	{
		nEndColOffset = m_nWidth - nSrcRight;
	}
	if (nSrcBottom > m_nHeight)
	{
		nEndRowOffset = m_nHeight - nSrcBottom;
	}

	nSrcLeft += nStartColOffset;
	nSrcRight += nEndColOffset;
	nSrcTop += nStartRowOffset;
	nSrcBottom += nEndRowOffset;

	nDestLeft += int(nStartColOffset*lfScale);
	nDestRight += int(nEndColOffset*lfScale);
	nDestTop += int(nStartRowOffset*lfScale);
	nDestBottom += int(nEndRowOffset*lfScale);

	if (nSrcSkip == -1)
	{
		if (nBandNum != m_nBandNum)
		{
			return 1;
		}

		int *bandmap = new int[nBandNum];
		for (int i = 0; i < nBandNum; ++i)
		{
			bandmap[i] = i + 1;
		}

		if (!m_bTranto8bit)
		{
			CPLErr er = m_poDataset->RasterIO(GF_Read, nSrcLeft, m_nHeight - nSrcBottom, nSrcRight - nSrcLeft, nSrcBottom - nSrcTop,
				pBuf + (nDestBottom - 1) * nBufWid * m_nBPP + nDestLeft * m_nBPP, nDestRight - nDestLeft, nDestBottom - nDestTop, m_eGDALType, nBandNum, bandmap,
				m_nBPP, -m_nBPP * nBufWid, m_nBPB);
			if (CE_Failure == er)
			{
				return 1;
			}
		}
		else
		{
			unsigned char *temBuf = new unsigned char[m_nOldBytesPerBand*m_nBandNum*nBufHeight*nBufWid];
			memset(temBuf, 0, m_nOldBytesPerBand*m_nBandNum*nBufHeight*nBufWid);
			CPLErr er = m_poDataset->RasterIO(GF_Read, nSrcLeft, m_nHeight - nSrcBottom, nSrcRight - nSrcLeft, nSrcBottom - nSrcTop,
				temBuf + (nDestBottom - 1) * nBufWid * m_nOldBytesPerBand*nBandNum + nDestLeft * m_nOldBytesPerBand*nBandNum, nDestRight - nDestLeft, nDestBottom - nDestTop, m_eGDALType, nBandNum, bandmap,
				m_nOldBytesPerBand*nBandNum, -m_nOldBytesPerBand*nBandNum * nBufWid, m_nOldBytesPerBand);
			if (CE_Failure == er)
			{
				return 1;
			}

			switch (m_nDataType)
			{
			case Pixel_Int16:
			{
								unsigned short* pp = (unsigned short*)temBuf;
								for (int j = nDestTop; j < nDestBottom; ++j)
								{
									int nRowOffset = j * nBufWid * nBandNum;

									unsigned char *pBufferIndex = pBuf + nRowOffset;
									unsigned short *ppIndex = pp + nRowOffset;
									for (int i = nDestLeft; i < nDestRight; ++i)
									{
										int nColOffset = i * nBandNum;

										unsigned char *pSubBufferIndex = pBufferIndex + nColOffset;
										unsigned short *ppSubIndex = ppIndex + nColOffset;

										int bBackGround = TRUE;
										for (int nb = 0; nb < nBandNum; ++nb)
										{
											if (ppSubIndex[nb] != 0)
											{
												bBackGround = FALSE;
												break;
											}
										}
										if (bBackGround == TRUE)
										{
											memset(pSubBufferIndex, 0, nBandNum);
										}
										else
										{
											for (int nb = 0; nb < nBandNum; ++nb)
											{
												pSubBufferIndex[nb] = m_plut[nb * 65536 + ppSubIndex[nb]];
											}
										}
									}
								}
								break;
			}
			case Pixel_SInt16:
			{
								 short* pp = (short*)temBuf;
								 for (int j = nDestTop; j < nDestBottom; ++j)
								 {
									 for (int i = nDestLeft; i < nDestRight; ++i)
									 {
										 for (int nb = 0; nb < m_nBandNum; ++nb)
										 {
											 pBuf[j*nBufWid*nBandNum + i*nBandNum + nb] = m_plut[nb * 65536 + pp[j*nBufWid*nBandNum + i*nBandNum + nb]];
										 }
									 }
								 }
								 break;
			}
			}
			delete[]temBuf;
			temBuf = NULL;
		}

		delete[] bandmap;
		bandmap = NULL;
	}
	else
	{
		if (nDestSkip > nBandNum - 1)
		{
			return 1;
		}
		if (!m_bTranto8bit)
		{
			CPLErr er = m_poDataset->GetRasterBand(nSrcSkip + 1)->RasterIO(GF_Read, nSrcLeft, m_nHeight - nSrcBottom, nSrcRight - nSrcLeft, nSrcBottom - nSrcTop,
				pBuf + (nDestBottom - 1) * nBufWid * nBandNum* m_nBPB + nDestLeft * nBandNum * m_nBPB + nDestSkip * m_nBPB, nDestRight - nDestLeft, nDestBottom - nDestTop, m_eGDALType,
				nBandNum * m_nBPB, -nBandNum * m_nBPB * nBufWid);
			if (CE_Failure == er)
			{
				return 1;
			}
		}
		else
		{
			unsigned char *temBuf = new unsigned char[m_nOldBytesPerBand*nBufHeight*nBufWid];
			CPLErr er = m_poDataset->GetRasterBand(nSrcSkip + 1)->RasterIO(GF_Read, nSrcLeft, m_nHeight - nSrcBottom, nSrcRight - nSrcLeft, nSrcBottom - nSrcTop,
				temBuf + (nDestBottom - 1) * nBufWid * m_nOldBytesPerBand + nDestLeft * m_nOldBytesPerBand, nDestRight - nDestLeft, nDestBottom - nDestTop, m_eGDALType,
				m_nOldBytesPerBand, -m_nOldBytesPerBand * nBufWid);
			if (CE_Failure == er)
			{
				return 1;
			}
			switch (m_nDataType)
			{
			case Pixel_Int16:
			{
								unsigned short* pp = (unsigned short*)temBuf;
								for (int j = nDestTop; j < nDestBottom; ++j)
								{
									for (int i = nDestLeft; i < nDestRight; ++i)
									{
										pBuf[j*nBufWid*nBandNum + i*nBandNum + nDestSkip] = m_plut[nSrcSkip * 65536 + pp[j*nBufWid + i]];
									}
								}
								break;
			}
			case Pixel_SInt16:
			{
								 short* pp = (short*)temBuf;
								 for (int j = nDestTop; j < nDestBottom; ++j)
								 {
									 for (int i = nDestLeft; i < nDestRight; ++i)
									 {
										 pBuf[j*nBufWid*nBandNum + i*nBandNum + nDestSkip] = m_plut[nSrcSkip * 65536 + pp[j*nBufWid + i]];
									 }
								 }
								 break;
			}
			}
			delete[]temBuf;
			temBuf = NULL;
		}
	}

	return 0;
}

/******************************************************************************
函数名：
readImageGDAL
功能：
读取图像
参数：
unsigned char **pImageData - 指向图像数据指针的指针，将由new操作符动态创建，需要在函数外部由调用者使用delete[]销毁，否则内存泄露
int &width,int &height     - 图像宽度、高度，由于是引用，可以作为返回值。
nChannels                  - 图像通道，可选值为1或3。1代表灰度图像，3代表RGB图像,-1表示按照图像自身通道数目读取。
const char *filePath       - 图像文件路径名称
说明：******************************************************************************/
bool COneTif::readImageGDAL(unsigned char **pImageData, int &widthDst, int &heightDst, int &nChannels, const char *filePath)
{
	int width = 0, height = 0;
	GDALAllRegister();
	GDALDataset *poDataset = NULL;
	poDataset = (GDALDataset*)GDALOpen(filePath, GA_ReadOnly);
	if (poDataset == NULL)
	{
		GDALClose(poDataset);
		return false;
	}
	width = poDataset->GetRasterXSize();
	height = poDataset->GetRasterYSize();
	if (widthDst <= 0 && heightDst <= 0)
	{
		widthDst = width;
		heightDst = height;
	}

	GDALRasterBand* pBand;
	int i = 0;
	int nRastercount = poDataset->GetRasterCount();
	if (nRastercount == 1)		//只有1个通道，则为灰度图像  
	{
		nChannels = 1;
		pBand = poDataset->GetRasterBand(1);
		*pImageData = new unsigned char[widthDst * heightDst];
		pBand->RasterIO(GF_Read,
			0, 0,				//nXOff,nYOff:从左上角坐标point(nXOff,nYOff)处读取图像数据  
			width, height,		//nXSize,nXSize:要读取的图像数据尺寸，注意可以不是band的实际尺寸，这样就是读取roi区域数据  
			*pImageData,		//pData:读取的数据即将存储的目的地址。  
			widthDst, heightDst,//nBufXSize,nBufYSize:目的地址处图像的尺寸，如果与roi尺寸不一致，则缩放。  
			GDT_Byte,			//eBufType:读取图像后，将要存储为的类型  
			0,					//nPixelSpace:控制同一行相邻像素间隔，0代表使用目的数据类型大小  
			0);					//nLineSpace:控制相邻行的行间隔，0代表使用[目的数据类型大小 * nXsize]  
		GDALClose(poDataset);
		return true;
	}
	else if (nRastercount == 3 && (nChannels == 3 || nChannels < 0)) //有3个通道，并且输出为RGB图像  
	{
		nChannels = 3;
		*pImageData = new unsigned char[nRastercount * widthDst * heightDst];
		for (i = 1; i <= nRastercount; ++i)
		{
			//GDAL内band存储顺序为RGB，需要转换为我们一般的BGR存储，即低地址->高地址为:B G R  
			unsigned char *pImageOffset = *pImageData + i - 1;
			GDALRasterBand* pBand = poDataset->GetRasterBand(nRastercount - i + 1);

			pBand->RasterIO(
				GF_Read,
				0, 0,
				width, height,
				pImageOffset,
				widthDst, heightDst,
				GDT_Byte,
				3,
				0);
		}
		GDALClose(poDataset);
		return true;
	}
	else if (nRastercount == 3 && nChannels == 1) //有3个通道，但是要求输出灰度图像  
	{
		unsigned char **img = new unsigned char*[nRastercount];
		for (i = 0; i < nRastercount; i++)
		{
			img[i] = new unsigned char[widthDst * heightDst];
		}
		for (i = 1; i <= nRastercount; ++i)
		{
			//GDAL内band存储顺序为RGB，需要转换为我们一般的BGR存储，即低地址->高地址为:B G R  
			pBand = poDataset->GetRasterBand(nRastercount - i + 1);
			pBand->RasterIO(GF_Read,
				0, 0,
				width, height,
				img[i - 1],
				widthDst, heightDst,
				GDT_Byte,
				0,
				0);
		}
		GDALClose(poDataset);
		*pImageData = new unsigned char[widthDst*heightDst];
		for (int r = 0; r < heightDst; ++r)
		{
			for (int c = 0; c < widthDst; ++c)
			{
				int t = (r*widthDst + c);
				//r g b分量依次占:0.299 0.587 0.144,可简化为3:6:1  
				//img[1.2.3]依次对应BGR  
				(*pImageData)[t] = (img[2][t] * 3 + img[1][t] * 6 + img[0][t] + 5) / 10;
			}
		}

		for (i = 0; i < nRastercount; ++i)
		{
			delete[] img[i];
		}
		delete[]img; img = NULL;
		return true;
	}
	else
	{
		return false;
	}
}

char* COneTif::findImageTypeGDAL(char *pDstImgFileName)
{
	char *dstExtension = strlwr(strrchr(pDstImgFileName, '.') + 1);
	char *Gtype = NULL;
	if (0 == strcmp(dstExtension, "bmp")) Gtype = "BMP";
	else if (0 == strcmp(dstExtension, "jpg")) Gtype = "JPEG";
	else if (0 == strcmp(dstExtension, "png")) Gtype = "PNG";
	else if (0 == strcmp(dstExtension, "tif")) Gtype = "GTiff";
	else if (0 == strcmp(dstExtension, "gif")) Gtype = "GIF";
	else Gtype = NULL;

	return Gtype;
}

bool COneTif::WriteImageGDAL(const char* pDstImgFileName, unsigned char *pImageData, int width, int height, int nChannels)
{
	if ((pDstImgFileName == NULL || pImageData == NULL || width <1 || height < 1 || nChannels < 1))
	{
		return false;
	}

	GDALAllRegister();
	char *GType = NULL;
	GType = findImageTypeGDAL((char*)pDstImgFileName);
	if (GType == NULL)  { return false; }

	GDALDriver *pMemDriver = NULL;
	pMemDriver = GetGDALDriverManager()->GetDriverByName("MEM");
	if (pMemDriver == NULL) { return false; }

	GDALDataset * pMemDataSet = pMemDriver->Create("", width, height, nChannels, GDT_Byte, NULL);
	GDALRasterBand *pBand = NULL;
	int nLineCount = width * nChannels;
	unsigned char *ptr1 = (unsigned char *)pImageData;
	for (int i = 1; i <= nChannels; i++)
	{
		pBand = pMemDataSet->GetRasterBand(nChannels - i + 1);
		pBand->RasterIO(GF_Write,
			0,
			0,
			width,
			height,
			ptr1 + i - 1,
			width,
			height,
			GDT_Byte,
			nChannels,
			nLineCount);
	}

	GDALDriver *pDstDriver = NULL;
	pDstDriver = (GDALDriver *)GDALGetDriverByName(GType);
	if (pDstDriver == NULL) { return false; }

	GDALDataset* Dstds = pDstDriver->CreateCopy(pDstImgFileName, pMemDataSet, FALSE, NULL, NULL, NULL);

	GDALClose(pMemDataSet);
	GDALClose(Dstds);

	return true;
}