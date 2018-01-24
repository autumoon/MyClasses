#pragma once
#include "opencv.hpp"

//一条线段，有起点和终点
class COneSeg
{
public:
	COneSeg();
	COneSeg(cv::Vec4d v4d);
	COneSeg(cv::Point2d p1, cv::Point2d p2);
	COneSeg(const double& x1, const double& y1, const double& x2, const double& y2);
	~COneSeg();

public:
	double x1, y1, x2, y2;        //线段的断点坐标

public:
	//判断线段上是否有大于指定亮度的点
	bool HasWhitePixs(cv::Mat& mSrcGray, const int& nBrightness = 255);
	//两直线的交点
	bool IntersectionLine(COneSeg& os, cv::Point2d& pInter);
	//两线段的交点
	bool IntersectionSeg(COneSeg& os, cv::Point2d& pInter, const double& dDiff = 1.0);
	//点是否在线段上
	bool IsPointInSeg(const cv::Point2d& pt, const double& dDiff = 1.0);

	//获取线段的中点
	cv::Point2d GetCenter();
	//获取线段所在的最小矩形区域，定义最小宽度
	cv::Rect GetRect(const int& nMinGap = 8);
	//获取线段在图像内部的最小矩形区域，定义最小宽度
	cv::Rect GetRect(const int& nImgWidth, const int& nImgHeight, const int& nMinGap = 8);

	//与x轴正向的夹角,-90到+90度
	double Angle();
	//两条线段中点的距离
	double CenterGap(COneSeg& os);
	//长度
	double Length();

	double GapFromPoint(const cv::Point2d& pt);
	//斜率
	double Slope();

	int DrawToMat(cv::Mat& mCanvas, const cv::Scalar& color = cv::Scalar(0, 0, 255), const int& thickness = 2);
	int Offset(const int& x, const int& y);
	static int ValidRect(cv::Rect& rc, const int& nWidth, const int& nHeight);

	std::string ToString();
	//求线段经过的所有的图像上的点
	std::vector<cv::Point> GetLinePoints();

private:
	double TwoPointsGap(const cv::Point2d& p1, const cv::Point2d& p2);
	double TwoPointsGap(const double& x1, const double& y1, const double& x2, const double& y2);
};


class CSegGroup
{
public:
	CSegGroup();
	~CSegGroup();

public:
	std::vector<COneSeg> vOs;

	//计算该组线段与x轴正向的角度平均值
	double GetAvgAngle();

	int DrawToMat(cv::Mat& mCanvas, const cv::Scalar& color = cv::Scalar(0, 0, 255), const int& thickness = 2);

private:

};
