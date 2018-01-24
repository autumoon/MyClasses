#pragma once
#include "opencv.hpp"

//һ���߶Σ��������յ�
class COneSeg
{
public:
	COneSeg();
	COneSeg(cv::Vec4d v4d);
	COneSeg(cv::Point2d p1, cv::Point2d p2);
	COneSeg(const double& x1, const double& y1, const double& x2, const double& y2);
	~COneSeg();

public:
	double x1, y1, x2, y2;        //�߶εĶϵ�����

public:
	//�ж��߶����Ƿ��д���ָ�����ȵĵ�
	bool HasWhitePixs(cv::Mat& mSrcGray, const int& nBrightness = 255);
	//��ֱ�ߵĽ���
	bool IntersectionLine(COneSeg& os, cv::Point2d& pInter);
	//���߶εĽ���
	bool IntersectionSeg(COneSeg& os, cv::Point2d& pInter, const double& dDiff = 1.0);
	//���Ƿ����߶���
	bool IsPointInSeg(const cv::Point2d& pt, const double& dDiff = 1.0);

	//��ȡ�߶ε��е�
	cv::Point2d GetCenter();
	//��ȡ�߶����ڵ���С�������򣬶�����С���
	cv::Rect GetRect(const int& nMinGap = 8);
	//��ȡ�߶���ͼ���ڲ�����С�������򣬶�����С���
	cv::Rect GetRect(const int& nImgWidth, const int& nImgHeight, const int& nMinGap = 8);

	//��x������ļн�,-90��+90��
	double Angle();
	//�����߶��е�ľ���
	double CenterGap(COneSeg& os);
	//����
	double Length();

	double GapFromPoint(const cv::Point2d& pt);
	//б��
	double Slope();

	int DrawToMat(cv::Mat& mCanvas, const cv::Scalar& color = cv::Scalar(0, 0, 255), const int& thickness = 2);
	int Offset(const int& x, const int& y);
	static int ValidRect(cv::Rect& rc, const int& nWidth, const int& nHeight);

	std::string ToString();
	//���߶ξ��������е�ͼ���ϵĵ�
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

	//��������߶���x������ĽǶ�ƽ��ֵ
	double GetAvgAngle();

	int DrawToMat(cv::Mat& mCanvas, const cv::Scalar& color = cv::Scalar(0, 0, 255), const int& thickness = 2);

private:

};
