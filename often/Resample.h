/* ********************* Resample.h.h ************************
********* 将数组的某个区域重采样到另外一个数组的某个区域 ********** */

/* author: autumoon */

#pragma once

//定义为unsigned char*只是为了方便指针，实际大小根据nSrcSizeOfPointer决定
//int arrNewSeq[3] = {0,1,2};表示取原通道的123通道，以此类推
int ResampleArray(int nSrcBufWidth, int nSrcBufHeight, int nSrcLeft, int nSrcTop, int nSrcRight, int nSrcBottom, unsigned char* pSrc, int nSrcBandNum,
	int nDstBufWidth, int nDstBufHeight, int nDstLeft, int nDstTop, int nDstRight, int nDstBottom, unsigned char* pDst, int nDstBandNum, int arrNewSeq[],
	int nSizeOfPointer = 1, int nSrcSkip = 0, int nDestSkip = 0);
