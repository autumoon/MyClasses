/* ********************* Resample.h.h ************************
********* �������ĳ�������ز���������һ�������ĳ������ ********** */

/* author: autumoon */

#pragma once

//����Ϊunsigned char*ֻ��Ϊ�˷���ָ�룬ʵ�ʴ�С����nSrcSizeOfPointer����
//int arrNewSeq[3] = {0,1,2};��ʾȡԭͨ����123ͨ�����Դ�����
int ResampleArray(int nSrcBufWidth, int nSrcBufHeight, int nSrcLeft, int nSrcTop, int nSrcRight, int nSrcBottom, unsigned char* pSrc, int nSrcBandNum,
	int nDstBufWidth, int nDstBufHeight, int nDstLeft, int nDstTop, int nDstRight, int nDstBottom, unsigned char* pDst, int nDstBandNum, int arrNewSeq[],
	int nSizeOfPointer = 1, int nSrcSkip = 0, int nDestSkip = 0);
