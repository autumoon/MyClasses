/* ******* ccv.h **********
********* opencv常用操作函数声明 ********** */

/* author: autumoon */

#pragma once

#include <afxdlgs.h> //打开文件
#include "opencv.hpp"

#define SCREEN_WIDTH     1920
#define SCREEN_HEIGHT    1080

class CCvImage
{
public:
	cv::Mat m_Minput; //使用cv::Mat类型

	//鼠标控制相关变量
	cv::Mat m_Mmask;
	cv::Mat m_Mmid;
	cv::Mat m_Msrc; //外部矩阵输入
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
	//图像操作
	double GetAvgGray(const cv::Mat& mSrc);

	int AdaptiveFindThreshold(CvMat *dx, CvMat *dy, double *low, double *high);
	int AutoSizeMat(cv::Mat& Msrc, cv::Mat& Mdst, float& fProportion, int nScreenWidth = SCREEN_WIDTH, int nScreenHeight = SCREEN_HEIGHT);
	int CannyAutoThreshold(cv::Mat Msrc, cv::Mat& Mdst);
	int EasyCanny(cv::Mat Msrc, cv::Mat& Mdst, double threshold1 = 100, double threshold2 = 200);
	int ErodeDilate(cv::Mat Msrc, cv::Mat& Mdst, int nPos); //nPos < 10 腐蚀，nPos > 10 膨胀
	int OpenClose(cv::Mat Msrc, cv::Mat& Mdst, int nPos); //nPos < 10 开运算，nPos > 10 闭运算
	int ShowImg(char szTitle[] = "显示图像", bool bAutoClose = true);
	int ShowImg(cv::Mat Msrc, char szTitle[] = "显示图像", bool bAutoClose = true); //显示某个矩阵
	int ShowImgAutoSize(char szTitle[] = "显示图像", int nScreenWidth = SCREEN_WIDTH, int nScreenHeight = SCREEN_HEIGHT,  bool bAutoClose = true);
	int ShowImgAutoSize(cv::Mat Msrc, char szTitle[] = "显示图像", int nScreenWidth = SCREEN_WIDTH, int nScreenHeight = SCREEN_HEIGHT,  bool bAutoClose = true);
	int SobelCalc(cv::Mat Msrc, cv::Mat& pMdst, bool bGray = false);

	//鼠标图像操作
	int DrawCurveOnImg(cv::Mat Msrc, char* szTitle = "画曲线", int nThick = 2);
	int DrawCurveOnBigImg(cv::Mat Msrc, int nScreenWidth = SCREEN_WIDTH, int nScreenHeight = SCREEN_HEIGHT, char* szTitle = "画曲线", int nThick = 2);
	int DrawLineOnImg(cv::Mat Msrc, CvPoint& pStart, CvPoint& pEnd, char* szTitle = "画直线", int nThick = 2);
	int DrawLineOnBigImg(cv::Mat Msrc, CvPoint& pStart, CvPoint& pEnd, int nScreenWidth = SCREEN_WIDTH, int nScreenHeight = SCREEN_HEIGHT, char* szTitle = "画直线", int nThick = 2);
	int DrawRectOnImg(cv::Mat Msrc, CvRect& rect, char* szTitle = "画矩形", int nThick = 2);
	int DrawRectOnBigImg(cv::Mat Msrc, CvRect& rect, int nScreenWidth = SCREEN_WIDTH, int nScreenHeight = SCREEN_HEIGHT, char* szTitle = "画矩形", int nThick = 2);
	int GetMaskByCurve(cv::Mat Msrc, cv::Mat& pDst, char* szTitle = "获取遮罩"); //通过曲线获取遮罩
	int GetMaskByDaub(cv::Mat Msrc, cv::Mat& pDst, int nRadius = 20, char* szTitle = "获取遮罩"); //通过涂抹获取遮罩

	//其他操作
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
	IplImage* IInput; //内部使用的IplImage指针
};

//回调函数
void on_mouse_curve(int event, int x, int y, int flags, void* pParameters);
void on_mouse_big_curve(int event, int x, int y, int flags, void* pParameters);
void on_mouse_curve_mask(int event, int x, int y, int flags, void* pParameters);
void on_mouse_daub(int event, int x, int y, int flags, void* pParameters);
void on_mouse_line(int event, int x, int y, int flags, void* pParameters);
void on_mouse_rect(int event, int x, int y, int flags, void* pParameters);
