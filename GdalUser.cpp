#include ".\GdalUser.h"

CGdalUser::CGdalUser()
{
	m_LBX = 0.0;
	m_LBY = 0.0;
	m_RTX = 0.0;
	m_RTY = 0.0;
	m_lfGSD = 1.0;

	m_nDataType = GDT_Unknown;

	m_nImgWidth = -1;
	m_nImgHeight = -1;
	m_nBandNum = -1;
	m_nBPB = -1;
	m_nBPP = -1;
	m_strFilePath = "";

	m_bTranto8bit = false;
	m_plfCount = nullptr;
	m_pHist = nullptr;
	m_poDataset = nullptr;
	m_nOldBytesPerBand = 0;
	m_plut = nullptr;
}

CGdalUser::~CGdalUser()
{
	if (m_poDataset != nullptr)
	{
		GDALClose((GDALDatasetH)m_poDataset);
		m_poDataset = nullptr;
	}
}

bool CGdalUser::InitializeCreate(const char* szFilePath, GDALDataType nDataType, const int& widthDst, const int& heightDst, const int& nBandNum)
{
	if (szFilePath == nullptr || widthDst <= 0 ||heightDst <= 0 || nBandNum <= 0 || IsImgReady())
	{
		return false;
	}

	GDALAllRegister();
	char *GType = NULL;
	GType = findImageTypeGDAL((char*)szFilePath);
	if (GType == NULL)
	{
		return false;
	}
	GDALDriver *pMemDriver = NULL;
	pMemDriver = GetGDALDriverManager()->GetDriverByName("MEM");
	if (pMemDriver == NULL)
	{
		return false;
	}
	if (nDataType == GDT_Unknown)
	{
		nDataType = GDT_Byte;
	}
	m_nDataType = nDataType;
	m_nImgWidth = widthDst;
	m_nImgHeight = heightDst;
	m_nBandNum = nBandNum;

	m_nBPB = GetBPB(nDataType);
	m_nOldBytesPerBand = m_nBPB;
	m_nBPP = m_nBPB * nBandNum;

	GDALDataset* pMemDataSet = pMemDriver->Create("", m_nImgWidth, m_nImgHeight, m_nBandNum, nDataType, NULL);
	GDALDriver *pDstDriver = NULL;
	pDstDriver = (GDALDriver *)GDALGetDriverByName(GType);
	if (pDstDriver == NULL)
	{
		return false;
	}
	m_poDataset = pDstDriver->CreateCopy(szFilePath, pMemDataSet, FALSE, NULL, NULL, NULL);

	GDALClose(pMemDataSet);

	return m_poDataset != nullptr;
}

bool CGdalUser::InitializeRead(const char* szFilePath, const bool& b16Trans8/* = false*/)
{
	if (IsImgReady())
	{
		return false;
	}

	m_strFilePath = std::string(szFilePath);
	const char *pszFilename = szFilePath;
	GDALAllRegister();
	CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");   //设置支持中文路径  

	m_poDataset = (GDALDataset *)GDALOpen(pszFilename, GA_ReadOnly);

	if (m_poDataset == nullptr)
	{
		return false;
	}

	//获取基本信息
	m_nImgWidth = m_poDataset->GetRasterXSize();
	m_nImgHeight = m_poDataset->GetRasterYSize();
	m_nBandNum = m_poDataset->GetRasterCount();
	//这里假设每个通道的类型都相同
	m_nDataType = m_poDataset->GetRasterBand(1)->GetRasterDataType();

	m_nBPB = GetBPB(m_nDataType);
	m_nOldBytesPerBand = m_nBPB;
	m_nBPP = m_nBandNum * m_nBPB;
	double adfGeoTransform[6] = { 0 };
	if (m_poDataset->GetGeoTransform(adfGeoTransform) == CE_None)
	{
		//左上角
		m_LBX = adfGeoTransform[0];
		m_RTY = adfGeoTransform[3];
		m_lfGSD = adfGeoTransform[1];
	}

	//计算出的值
	m_RTX = m_LBX + m_lfGSD * m_nImgWidth;
	m_LBY = m_RTY - m_lfGSD * m_nImgHeight;

	m_bTranto8bit = m_nBPB == 2 && b16Trans8 == true;

	if (m_nBPB == 2 && m_bTranto8bit == true)
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
		std::string szFileName = m_strFilePath + ".czb";
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

			CGdalUser tif;
			tif.InitializeRead(szFilePath, false);

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
				//for (int i = 0; i < 65536; ++i)
				//{
				//	fprintf(pFile, "%d\n", m_plut[i]);
				//}
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

bool CGdalUser::IsImgReady()
{
	return m_poDataset != nullptr;
}

bool CGdalUser::ReadImageGDAL(const char* pFilePath, unsigned char **pImageData, GDALDataType nDataType, int& widthDst, int& heightDst, int& nBandNum)
{
	if (pFilePath == nullptr || pImageData == nullptr)
	{
		return false;
	}

	if (nDataType == GDT_Unknown)
	{
		nDataType = GDT_Byte;
	}

	int nBPB = GetBPB(nDataType);

	GDALAllRegister();
	int width = 0, height = 0;
	GDALDataset *poDataset = NULL;
	poDataset = (GDALDataset*)GDALOpen(pFilePath, GA_ReadOnly);
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
	nBandNum = poDataset->GetRasterCount();

	unsigned char **img = new unsigned char*[nBandNum];
	for (i = 0; i < nBandNum; i++)
	{
		img[i] = new unsigned char[widthDst * heightDst * nBPB];
	}
	for (i = 1; i <= nBandNum; ++i)
	{
		//默认读取顺序
		pBand = poDataset->GetRasterBand(i);
		pBand->RasterIO(GF_Read,
			0, 0,						//nXOff,nYOff:从左上角坐标point(nXOff,nYOff)处读取图像数据  		
			width, height,				//nXSize,nXSize:要读取的图像数据尺寸，注意可以不是band的实际尺寸，这样就是读取roi区域数据 		
			img[i - 1],					//pData:读取的数据即将存储的目的地址。  		
			widthDst, heightDst,		//nBufXSize,nBufYSize:目的地址处图像的尺寸，如果与roi尺寸不一致，则缩放。  		
			nDataType,					//eBufType:读取图像后，将要存储为的类型  		
			0,							//nPixelSpace:控制同一行相邻像素间隔，0代表使用目的数据类型大小  		
			0);							//nLineSpace:控制相邻行的行间隔，0代表使用[目的数据类型大小 * nXsize]  		
	}
	GDALClose(poDataset);

	//将多个通道组合成单个输出通道
	*pImageData = new unsigned char[widthDst*heightDst* nBandNum * nBPB];
	for (int k = 0; k < nBandNum; ++k)
	{
		unsigned char* pBandData = img[k];
		for (int j = 0; j < heightDst; ++j)
		{
			unsigned char* pRowReadData = pBandData + j * widthDst * nBPB;
			unsigned char* pRowWriteData = *pImageData + j * widthDst * nBandNum * nBPB;
			for (int i = 0; i < widthDst; ++i)
			{
				unsigned char* pColReadData = pRowReadData + i * nBPB;
				unsigned char* pColWriteData = pRowWriteData + i * nBandNum * nBPB;
				//赋值读取
				memcpy(pColWriteData + k * nBPB, pColReadData, nBPB);
			}
		}
	}

	for (i = 0; i < nBandNum; ++i)
	{
		delete[] img[i];
		img[i] = nullptr;
	}
	delete[]img;
	img = nullptr;

	return true;
}

bool CGdalUser::WriteImageGDAL(const char* pFilePath, unsigned char *pImageData, GDALDataType nDataType, const int& widthDst, const int& heightDst, const int& nBandNum)
{
	if ((pFilePath == NULL || pImageData == NULL || widthDst <1 || heightDst < 1 || nBandNum < 1))
	{
		return false;
	}

	GDALAllRegister();
	char *GType = NULL;
	GType = findImageTypeGDAL((char*)pFilePath);
	if (GType == NULL)
	{
		return false;
	}
	GDALDriver *pMemDriver = NULL;
	pMemDriver = GetGDALDriverManager()->GetDriverByName("MEM");
	if (pMemDriver == NULL)
	{
		return false;
	}
	if (nDataType == GDT_Unknown)
	{
		nDataType = GDT_Byte;
	}
	int nBPB = GetBPB(nDataType);
	GDALDataset* pMemDataSet = pMemDriver->Create("", widthDst, heightDst, nBandNum, nDataType, NULL);
	GDALRasterBand *pBand = NULL;
	int nLineCount = widthDst * nBandNum * nBPB;
	unsigned char *ptr1 = (unsigned char *)pImageData;

	for (int i = 1; i <= nBandNum; i++)
	{
		//按顺序存入，如果是BGR需要在外部进行波段转换
		pBand = pMemDataSet->GetRasterBand(i);
		pBand->RasterIO(GF_Write,
			0,
			0,
			widthDst,
			heightDst,
			ptr1 + (i - 1) * nBPB,
			widthDst,
			heightDst,
			nDataType,
			nBandNum * nBPB,
			nLineCount);
	}

	GDALDriver *pDstDriver = NULL;
	pDstDriver = (GDALDriver *)GDALGetDriverByName(GType);
	if (pDstDriver == NULL)
	{
		return false;
	}

	GDALDataset* Dstds = pDstDriver->CreateCopy(pFilePath, pMemDataSet, FALSE, NULL, NULL, NULL);

	GDALClose(pMemDataSet);
	GDALClose(Dstds);

	return Dstds != nullptr;
}

bool CGdalUser::ReadImage(int nStartCol, int nStartRow, int nWidth, int nHeight, unsigned char* pData, int nBufferWidth, int nBufferHeight)
{
	//-1代表读取所有通道
	int nSrcSkip = m_nBandNum == 1 ? 0 : -1;

	return 	ReadImg(nStartCol, nStartRow, nStartCol + nWidth, nStartRow + nHeight, pData, nBufferWidth, nBufferHeight, m_nBandNum, 0, 0, nBufferWidth, nBufferHeight, nSrcSkip, 0) == 0;
}

bool CGdalUser::WriteImage(int nStartCol, int nStartRow, int nWidth, int nHeight, unsigned char* pData, int nBufferWidth, int nBufferHeight)
{
	//-1代表写入所有通道
	int nSrcSkip = m_nBandNum == 1 ? 0 : -1;

	return 	WriteImg(nStartCol, nStartRow, nStartCol + nWidth, nStartRow + nHeight, pData, nBufferWidth, nBufferHeight, m_nBandNum, 0, 0, nBufferWidth, nBufferHeight, nSrcSkip, 0) == 0;
}


int CGdalUser::ReadImg(int nSrcLeft, int nSrcTop, int nSrcRight, int nSrcBottom, unsigned char* pBuf, int nBufWid, int nBufHeight, int nBandNum,
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
				pBuf + (nDestBottom - 1) * nBufWid * m_nBPP + nDestLeft * m_nBPP, nDestRight - nDestLeft, nDestBottom - nDestTop, m_nDataType, nBandNum, bandmap,
				m_nBPP, -m_nBPP * nBufWid, m_nBPB);
			if (CE_Failure == er)
			{
				return 1;
			}
		}
		else
		{
			unsigned char *temBuf = new unsigned char[m_nOldBytesPerBand * m_nBandNum*nBufHeight * nBufWid];
			memset(temBuf, 0, m_nOldBytesPerBand * m_nBandNum * nBufHeight * nBufWid);
			CPLErr er = m_poDataset->RasterIO(GF_Read, nSrcLeft, m_nHeight - nSrcBottom, nSrcRight - nSrcLeft, nSrcBottom - nSrcTop,
				temBuf + (nDestBottom - 1) * nBufWid * m_nOldBytesPerBand*nBandNum + nDestLeft * m_nOldBytesPerBand*nBandNum, nDestRight - nDestLeft, nDestBottom - nDestTop, m_nDataType, nBandNum, bandmap,
				m_nOldBytesPerBand*nBandNum, -m_nOldBytesPerBand*nBandNum * nBufWid, m_nOldBytesPerBand);
			if (CE_Failure == er)
			{
				return 1;
			}

			switch (m_nDataType)
			{
			case GDT_UInt16:
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
			case GDT_Int16:
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
				pBuf + (nDestBottom - 1) * nBufWid * nBandNum* m_nBPB + nDestLeft * nBandNum * m_nBPB + nDestSkip * m_nBPB, nDestRight - nDestLeft, nDestBottom - nDestTop, m_nDataType,
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
				temBuf + (nDestBottom - 1) * nBufWid * m_nOldBytesPerBand + nDestLeft * m_nOldBytesPerBand, nDestRight - nDestLeft, nDestBottom - nDestTop, m_nDataType,
				m_nOldBytesPerBand, -m_nOldBytesPerBand * nBufWid);
			if (CE_Failure == er)
			{
				return 1;
			}
			switch (m_nDataType)
			{
			case GDT_UInt16:
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
			case GDT_Int16:
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

int CGdalUser::WriteImg(int nSrcLeft, int nSrcTop, int nSrcRight, int nSrcBottom, unsigned char* pBuf, int nBufWid, int nBufHeight, int nBandNum,
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

		CPLErr er = m_poDataset->RasterIO(GF_Write, nSrcLeft, m_nHeight - nSrcBottom, nSrcRight - nSrcLeft, nSrcBottom - nSrcTop,
			pBuf + (nDestBottom - 1) * nBufWid * m_nBPP + nDestLeft * m_nBPP, nDestRight - nDestLeft, nDestBottom - nDestTop, m_nDataType, nBandNum, bandmap,
			m_nBPP, -m_nBPP * nBufWid, m_nBPB);
		if (CE_Failure == er)
		{
			return 1;
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

		CPLErr er = m_poDataset->GetRasterBand(nSrcSkip + 1)->RasterIO(GF_Write, nSrcLeft, m_nHeight - nSrcBottom, nSrcRight - nSrcLeft, nSrcBottom - nSrcTop,
			pBuf + (nDestBottom - 1) * nBufWid * nBandNum* m_nBPB + nDestLeft * nBandNum * m_nBPB + nDestSkip * m_nBPB, nDestRight - nDestLeft, nDestBottom - nDestTop, m_nDataType,
			nBandNum * m_nBPB, -nBandNum * m_nBPB * nBufWid);
		if (CE_Failure == er)
		{
			return 1;
		}
	}

	return 0;
}


char* CGdalUser::findImageTypeGDAL(char *pDstImgFileName)
{
	char* cf = strrchr(pDstImgFileName, '.');
	if (cf == nullptr)
	{
		return nullptr;
	}
	char *dstExtension = strlwr( cf + 1);
	char *Gtype = NULL;
	if (0 == strcmp(dstExtension, "bmp")) Gtype = "BMP";
	else if (0 == strcmp(dstExtension, "jpg") || 0 == strcmp(dstExtension, "jpeg")) Gtype = "JPEG";
	else if (0 == strcmp(dstExtension, "png")) Gtype = "PNG";
	else if (0 == strcmp(dstExtension, "tif")) Gtype = "GTiff";
	else if (0 == strcmp(dstExtension, "gif")) Gtype = "GIF";
	//以下是不常用的格式
	else if (0 == strcmp(dstExtension, "img")) Gtype = "HFA";
	else if (0 == strcmp(dstExtension, "bt")) Gtype = "BT";
	else if (0 == strcmp(dstExtension, "ecw")) Gtype = "ECW";
	else if (0 == strcmp(dstExtension, "fits")) Gtype = "FITS";
	else if (0 == strcmp(dstExtension, "hdf")) Gtype = "HDF4";
	else if (0 == strcmp(dstExtension, "hdr")) Gtype = "EHdr";
	else Gtype = nullptr;

	return Gtype;
}

int CGdalUser::GetBPB(const GDALDataType& nDataType)
{
	int nBPB = 0;
	switch (nDataType)
	{
	case 1:
		{
			//nDataType = GDT_Byte;
			nBPB = 1;
			break;
		}
	case 2:
		{
			//nDataType = GDT_UInt16;
			nBPB = 2;
			break;
		}
	case 3:
		{
			//nDataType = GDT_Int16;
			nBPB = 2;
			break;
		}
	case 4:
		{
			//nDataType = GDT_UInt32;
			nBPB = 4;
			break;
		}
	case 5:
		{
			//nDataType = GDT_Int32;
			nBPB = 4;
			break;
		}
	case 6:
		{
			//nDataType = GDT_Float32;
			nBPB = 4;
			break;
		}
	case 7:
		{
			//nDataType = GDT_Float64;
			nBPB = 8;
			break;
		}
	default:
		return nBPB;
	}

	return nBPB;
}

int CGdalUser::ReseqChannels(unsigned char* pData, const int& nRows, const int& nCols, const int& nBandNum, const int& nBPB, int arrNewSeq[])
{
	if (pData == nullptr || nRows <= 0 || nCols <= 0 || nBandNum <= 0 || nBPB <= 0)
	{
		return -1;
	}

	unsigned char* pTmp = nullptr;
	const int& nBufferSize = nBandNum * nBPB;
	pTmp = new unsigned char[nBufferSize];
	//memset(pTmp, 0, nBufferSize);

	for (int j = 0; j < nRows; ++j)
	{
		unsigned char* pRowData = pData + j * nCols * nBandNum * nBPB;
		for (int i = 0; i < nCols; ++i)
		{
			unsigned char* pColData = pRowData + i * nBandNum * nBPB;
			for (int k = 0; k < nBandNum; ++k)
			{
				memcpy(pTmp + k * nBPB, pColData + arrNewSeq[k] * nBPB, nBPB);
			}
			for (int k = 0; k < nBandNum; ++k)
			{
				memcpy(pColData + k * nBPB, pTmp + k * nBPB, nBPB);
			}
		}
	}

	if (pTmp)
	{
		delete[] pTmp;
		pTmp = nullptr;
	}

	return 0;
}

int CGdalUser::VerticalReverse(unsigned char* pData, const int& nRows, const int& nCols, const int& nBandNum, const int& nBPB)
{
	if (pData == nullptr || nRows <= 0 || nCols <= 0 || nBandNum <= 0 || nBPB <= 0)
	{
		return -1;
	}

	unsigned char* pTmp = nullptr;
	const int& nBufferSize = nRows * nCols * nBandNum * nBPB;
	pTmp = new unsigned char[nBufferSize];
	memcpy(pTmp, pData, nBufferSize);

	for (int j = 0; j < nRows; ++j)
	{
		unsigned char* pRowReadData = pTmp + (nRows - 1 - j) * nCols * nBandNum * nBPB;
		unsigned char* pRowWriteData = pData + j * nCols * nBandNum * nBPB;
		for (int i = 0; i < nCols; ++i)
		{
			unsigned char* pColReadData = pRowReadData + i * nBandNum * nBPB;
			unsigned char* pColWriteData = pRowWriteData + i * nBandNum * nBPB;
			for (int k = 0; k < nBandNum; ++k)
			{
				memcpy(pColWriteData + k * nBPB, pColReadData + k * nBPB, nBPB);
			}
		}
	}

	if (pTmp)
	{
		delete[] pTmp;
		pTmp = nullptr;
	}

	return 0;
}
