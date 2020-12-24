#include "Resample.h"
#include <iostream>

#define AU_MIN(x,y)	((x) < (y) ? (x) : (y))
#define AU_MAX(x,y)	((x) > (y) ? (x) : (y))

#define BACK_GROUND_VALUE	0

int ResampleArray(int nSrcBufWidth, int nSrcBufHeight, int nSrcLeft, int nSrcTop, int nSrcRight, int nSrcBottom, unsigned char* pSrc, int nSrcBandNum,
	int nDstBufWidth, int nDstBufHeight, int nDstLeft, int nDstTop, int nDstRight, int nDstBottom, unsigned char* pDst, int nDstBandNum, int arrNewSeq[],
	int nSizeOfPointer /*= 1*/, int nSrcSkip /*= 0*/, int nDestSkip /*= 0*/)
{
	if (pSrc == nullptr || pDst == nullptr)
	{
		return -1;
	}

	//左上为小
	int nSrcWidth = nSrcRight - nSrcLeft;
	int nSrcHeight = nSrcBottom - nSrcTop;
	int nDstWidth = nDstRight - nDstLeft;
	int nDstHeight = nDstBottom - nDstTop;

	if (nSrcWidth < 0 || nSrcHeight < 0 || nDstWidth < 0 || nDstHeight < 0)
	{
		return -1;
	}

	double dWidthProp = (double)nDstWidth / nSrcWidth;
	double dHeightProp = (double)nDstHeight / nSrcHeight;

	//如果源地址和目标地址一样
	unsigned char* pSrcBak(pSrc);
	if (pSrc == pDst)
	{
		int nSrcMemSize = nSrcBufWidth * nSrcBufHeight * nSrcBandNum * nSizeOfPointer;
		pSrcBak = new unsigned char[nSrcMemSize];
		memcpy(pSrcBak, pSrc, nSrcMemSize);
		memset(pSrc, BACK_GROUND_VALUE, nSrcMemSize);
	}

	//采样
	const int nBandNum = AU_MIN(nDstBandNum, nSrcBandNum);
	for (int j = 0; j < nDstHeight; ++j)
	{
		int nyc = nDstTop + j;
		int nyp = nSrcTop + int(j / dHeightProp);
		if (nyp >= AU_MAX(0, nSrcTop) && nyp < AU_MIN(nSrcBottom, nSrcBufHeight)
			&& nyc >= AU_MAX(0, nDstTop) && nyc < AU_MIN(nDstBottom, nDstBufHeight))
		{
			unsigned char* pBufferDst = pDst + (nyc * nDstBufWidth) * nDstBandNum * nSizeOfPointer + nSrcSkip;
			unsigned char* pBufferSrc = pSrcBak + (nyp * nSrcBufWidth) * nSrcBandNum * nSizeOfPointer + nDestSkip;
			for (int i = 0; i < nDstWidth; ++i)
			{
				int nxc = nDstLeft + i;
				int nxp = nSrcLeft + int(i / dWidthProp);
				if(nxp >= AU_MAX(0, nSrcLeft) && nxp < AU_MIN(nSrcRight, nSrcBufWidth)
					&& nxc >= AU_MAX(0, nDstLeft) && nxc < AU_MIN(nDstRight, nDstBufWidth))
				{
					unsigned char* pSubBufferDst = pBufferDst + nxc * nDstBandNum * nSizeOfPointer;
					unsigned char* pSubBufferSrc = pBufferSrc + nxp * nSrcBandNum * nSizeOfPointer;
					//最多只改变前三个通道
					const int nDstBandCount = AU_MIN(nDstBandNum, 3);
					for (int k = 0; k < nDstBandCount; ++k)
					{
						memcpy(pSubBufferDst + k * nSizeOfPointer, pSubBufferSrc + arrNewSeq[k] * nSizeOfPointer, nSizeOfPointer);
					}
				}
			}
		}
	}
	
	if (pSrc == pDst && pSrcBak != nullptr)
	{
		delete[] pSrcBak;
		pSrcBak = nullptr;
	}

	return 0;
}
