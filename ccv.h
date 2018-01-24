/* ******* ccv.h **********
********* opencv���ò����������� ********** */

/* author: autumoon */

#pragma once

#include <afxdlgs.h> //���ļ�
#include "opencv.hpp"

#define SCREEN_WIDTH     1920
#define SCREEN_HEIGHT    1080

class CCvImage
{
public:
	cv::Mat m_Minput; //ʹ��cv::Mat����

	//��������ر���
	cv::Mat m_Mmask;
	cv::Mat m_Mmid;
	cv::Mat m_Msrc; //�ⲿ��������
	char* m_szTitle;
	CvPoint m_curr_pt;
	CvPoint m_prev_pt;
	CvRect m_rect;
	CvRect m_CRroi;
	float m_fProportion;
	int m_nThick;

public:
	CCvImage();
	CCvImage(cv::Mat mat);
	CCvImage(IplImage* pImage);
	CCvImage(char szImgPath[], int flags = 1);
	CCvImage(CString strImgPath, int flags = 1);

public:
	//ͼ�����
	double GetAvgGray(const cv::Mat& mSrc);

	int AdaptiveFindThreshold(CvMat *dx, CvMat *dy, double *low, double *high);
	int AutoSizeMat(cv::Mat& Msrc, cv::Mat& Mdst, float& fProportion, int nScreenWidth = SCREEN_WIDTH, int nScreenHeight = SCREEN_HEIGHT);
	int CannyAutoThreshold(cv::Mat Msrc, cv::Mat& Mdst);
	int EasyCanny(cv::Mat Msrc, cv::Mat& Mdst, double threshold1 = 100, double threshold2 = 200);
	int ErodeDilate(cv::Mat Msrc, cv::Mat& Mdst, int nPos); //nPos < 10 ��ʴ��nPos > 10 ����
	int OpenClose(cv::Mat Msrc, cv::Mat& Mdst, int nPos); //nPos < 10 �����㣬nPos > 10 ������
	int ShowImg(char szTitle[] = "��ʾͼ��", bool bAutoClose = true);
	int ShowImg(cv::Mat Msrc, char szTitle[] = "��ʾͼ��", bool bAutoClose = true); //��ʾĳ������
	int ShowImgAutoSize(char szTitle[] = "��ʾͼ��", int nScreenWidth = SCREEN_WIDTH, int nScreenHeight = SCREEN_HEIGHT,  bool bAutoClose = true);
	int ShowImgAutoSize(cv::Mat Msrc, char szTitle[] = "��ʾͼ��", int nScreenWidth = SCREEN_WIDTH, int nScreenHeight = SCREEN_HEIGHT,  bool bAutoClose = true);
	int SobelCalc(cv::Mat Msrc, cv::Mat& pMdst, bool bGray = false);

	//���ͼ�����
	int DrawCurveOnImg(cv::Mat Msrc, char* szTitle = "������", int nThick = 2);
	int DrawCurveOnBigImg(cv::Mat Msrc, int nScreenWidth = SCREEN_WIDTH, int nScreenHeight = SCREEN_HEIGHT, char* szTitle = "������", int nThick = 2);
	int DrawLineOnImg(cv::Mat Msrc, CvPoint& pStart, CvPoint& pEnd, char* szTitle = "��ֱ��", int nThick = 2);
	int DrawLineOnBigImg(cv::Mat Msrc, CvPoint& pStart, CvPoint& pEnd, int nScreenWidth = SCREEN_WIDTH, int nScreenHeight = SCREEN_HEIGHT, char* szTitle = "��ֱ��", int nThick = 2);
	int DrawRectOnImg(cv::Mat Msrc, CvRect& rect, char* szTitle = "������", int nThick = 2);
	int DrawRectOnBigImg(cv::Mat Msrc, CvRect& rect, int nScreenWidth = SCREEN_WIDTH, int nScreenHeight = SCREEN_HEIGHT, char* szTitle = "������", int nThick = 2);
	int GetMaskByCurve(cv::Mat Msrc, cv::Mat& pDst, char* szTitle = "��ȡ����"); //ͨ�����߻�ȡ����
	int GetMaskByDaub(cv::Mat Msrc, cv::Mat& pDst, int nRadius = 20, char* szTitle = "��ȡ����"); //ͨ��ͿĨ��ȡ����

	//��������
	int OutputMat(const cv::Mat& mSrc, const CString& strTxtPath);
	int ResetRect(CvRect& rect, int nValue = -1);
	int ResizePoint(CvPoint& Csrc, float fProportion);
	int ResizeRect(CvRect& Csrc, float fProportion);
	int ResizeRect(CvRect& rect, int nExpandWidthPixs = 0, int nExpandHeightPixs = 0);
	int ResizeRect(CvRect& rect, int nLeft, int nRight, int nUp, int nDown);
	int ValidExpandValue(const CvRect& CRroi, int& nLeft, int& nRight, int& nUp, int& nDown, int nWidth, int nHeight);
	int ValidRect(CvRect& rect, const int nWidth, const int nHeight);
	int ValidRect(CvRect& rectSmall, const int nRectWidth, const int nRectHeight, const int nPicWidth, const int nPicHeight);

	long ValusPixCounts(const cv::Mat& mSrc, const int& nValue = 255);

private:
	IplImage* IInput; //�ڲ�ʹ�õ�IplImageָ��
};

//�ص�����
void on_mouse_curve(int event, int x, int y, int flags, void* pParameters);
void on_mouse_big_curve(int event, int x, int y, int flags, void* pParameters);
void on_mouse_curve_mask(int event, int x, int y, int flags, void* pParameters);
void on_mouse_daub(int event, int x, int y, int flags, void* pParameters);
void on_mouse_line(int event, int x, int y, int flags, void* pParameters);
void on_mouse_rect(int event, int x, int y, int flags, void* pParameters);
