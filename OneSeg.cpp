#include "OneSeg.h"

COneSeg::COneSeg()
{
}

COneSeg::COneSeg(cv::Vec4d v4d)
{
	x1 = v4d[0];
	y1 = v4d[1];
	x2 = v4d[2];
	y2 = v4d[3];
}

COneSeg::COneSeg(cv::Point2d p1, cv::Point2d p2)
{
	x1 = p1.x;
	y1 = p1.y;
	x2 = p2.x;
	y2 = p2.y;
}

COneSeg::COneSeg(const double& x1, const double& y1, const double& x2, const double& y2)
{
	this->x1 = x1;
	this->y1 = y1;
	this->x2 = x2;
	this->y2 = y2;
}

COneSeg::~COneSeg()
{

}

bool COneSeg::HasWhitePixs( cv::Mat& mSrcGray, const int& nBrightness /*= 255*/ )
{
	cv::LineIterator iterator(mSrcGray, cv::Point((int)x1, (int)y1), cv::Point((int)x2, (int)y2));
	std::vector<cv::Point> vLinePts;
	int count = iterator.count;
	for(int i = 0; i < count; i++, ++iterator )
	{
		uchar* ptr = *iterator;
		if (ptr[0] >= nBrightness)
		{
			return true;
		}
	}

	return false;
}

bool COneSeg::IntersectionLine(COneSeg& os, cv::Point2d& pInter)
{
	//第一条直线的参数
	double a0 = y1 - y2;
	double b0 = x2 - x1;
	double c0 = x1 * y2 - x2 * y1;

	double x3 = os.x1;
	double y3 = os.y1;
	double x4 = os.x2;
	double y4 = os.y2;

	//第二条直线的参数
	double a1 = y3 - y4;
	double b1 = x4 - x3;
	double c1 = x3 * y4 - x4 * y3;

	//求交点
	double D = a0 * b1 - a1 * b0;

	if (fabs(D) < 0.00001)
	{
		//无交点
		return false;
	}

	pInter.x = (b0 * c1 - b1 * c0) / D;
	pInter.y = (c0 * a1 - c1 * a0) / D;

	return true;
}

bool COneSeg::IntersectionSeg(COneSeg& os, cv::Point2d& pInter, const double& dDiff/* = 1.0*/)
{
	bool bRes = IntersectionLine(os, pInter);
	return bRes && IsPointInSeg(pInter, dDiff) && os.IsPointInSeg(pInter, dDiff);
}

bool COneSeg::IsPointInSeg(const cv::Point2d& pt, const double& dDiff/* = 1.0*/)
{
	//考虑到图像的特殊性，不需要斜率相同，只需要用距离判断即可
	return TwoPointsGap(pt, cv::Point2d(x1, y1)) + TwoPointsGap(pt, cv::Point2d(x2, y2)) - Length() <= dDiff;
}

cv::Point2d COneSeg::GetCenter()
{
	return cv::Point2d((x1 + x2) / 2, (y1 + y2) / 2);
}

double COneSeg::Angle()
{
	return atan(Slope()) / CV_PI * 180;
}

double COneSeg::Slope()
{
	if (fabs(x1 - x2) <= 0.0001)
	{
		return DBL_MAX;
	}

	return (y1 - y2) / (x1 - x2);
}

double COneSeg::Length()
{
	return sqrt((y1 - y2) * (y1 - y2) + (x1 - x2) * (x1 - x2));
}

int COneSeg::DrawToMat(cv::Mat& mCanvas, const cv::Scalar& color /*= cv::Scalar(0, 0, 255)*/, const int& thickness /*= 2*/)
{
	cv::line(mCanvas, cv::Point((int)x1, (int)y1), cv::Point((int)x2, (int)y2), color, thickness);

	return 0;
}

cv::Rect COneSeg::GetRect(const int& nMinGap /*= 8*/)
{
	//只是得到矩形区域，可能为负
	int x = int(x1 < x2 ? x1 : x2);
	if (abs(y2 - y1) <= nMinGap)
	{
		int y = int((y1 + y2) / 2 - nMinGap / 2);
		int width = int(abs(x1 - x2));
		if (width <= nMinGap)
		{
			x = int((x1 + x2) / 2 - nMinGap / 2);
			width = nMinGap;
		}

		int height = nMinGap;
		return cv::Rect(x, y, width, height);
	}
	else
	{
		int y = int(y1 < y2 ? y1 : y2);
		int width = int(abs(x1 - x2));
		if (width <= nMinGap)
		{
			x = int((x1 + x2) / 2 - nMinGap / 2);
			width = nMinGap;
		}
		int height = int(abs(y1 - y2));
		return cv::Rect(x, y, width, height);
	}
}

cv::Rect COneSeg::GetRect(const int& nImgWidth, const int& nImgHeight, const int& nMinGap /*= 8*/)
{
	cv::Rect roi = GetRect(nMinGap);
	
	ValidRect(roi, nImgWidth, nImgHeight);

	return roi;
}

double COneSeg::CenterGap(COneSeg& os)
{
	cv::Point2d p1 = GetCenter();
	cv::Point2d p2 = os.GetCenter();

	return TwoPointsGap(p1, p2);
}

double COneSeg::GapFromPoint(const cv::Point2d& pt)
{
	double k = Slope();

	if (k == DBL_MAX)
	{
		//垂直的时候直接返回差值
		return fabs(x1 - pt.x);
	}

	double b = y1 - k * x1;

	return fabs(k * pt.x - pt.y + b) / sqrt( 1 + k * k);
}

CSegGroup::CSegGroup()
{
}

CSegGroup::~CSegGroup()
{
}

double CSegGroup::GetAvgAngle()
{
	double dSum = 0;
	for (int i = 0; i < vOs.size(); ++i)
	{
		dSum += vOs[i].Angle();
	}

	return dSum / vOs.size();
}

int CSegGroup::DrawToMat(cv::Mat& mCanvas, const cv::Scalar& color /*= cv::Scalar(0, 0, 255)*/, const int& thickness /*= 2*/)
{
	for (int i = 0; i < vOs.size(); ++i)
	{
		vOs[i].DrawToMat(mCanvas, color, thickness);
	}

	return (int)vOs.size();
}

int COneSeg::Offset(const int& x, const int& y)
{
	x1 += x;
	y1 += y;
	x2 += x;
	y2 += y;

	return 0;
}

std::string COneSeg::ToString()
{
	std::string sCoors, sTmp;
	std::stringstream ss1, ss2, ss3, ss4;
	ss1 << x1;
	ss1 >> sTmp;
	sCoors += "x1 = ";
	sCoors += sTmp + " ";

	ss2 << y1;
	ss2 >> sTmp;
	sCoors += "y1 = ";
	sCoors += sTmp + " ";

	ss3 << x2;
	ss3 >> sTmp;
	sCoors += "x2 = ";
	sCoors += sTmp + " ";

	ss4 << y2;
	ss4 >> sTmp;
	sCoors += "y2 = ";
	sCoors += sTmp + " ";

	return sCoors;
}

std::vector<cv::Point> COneSeg::GetLinePoints()
{
	std::vector<cv::Point> vOutput;
	int width = int(MAX(x1, x2) + 1);
	int height = int(MAX(y1, y2) + 1);

	cv::Mat image(cv::Size(width, height), CV_8UC1);
	cv::LineIterator iterator(image, cv::Point((int)x1, (int)y1), cv::Point((int)x2, (int)y2));
	int count = iterator.count;
	for (int i = 0; i < count; i++)
	{
		/* print the pixel coordinates: demonstrates how to calculate the coordinates */
		{
			int offset, x, y;
			/* assume that ROI is not set, otherwise need to take it into account. */
			offset = int(iterator.ptr - (uchar*)(image.data));
			y = int(offset / image.step);
			x = int((offset - y*image.step) / (sizeof(uchar))/* size of pixel */);
			vOutput.push_back(cv::Point(x, y));
		}
		iterator++;
	}

	return vOutput;
}

double COneSeg::TwoPointsGap(const cv::Point2d& p1, const cv::Point2d& p2)
{
	return TwoPointsGap(p1.x, p1.y, p2.x, p2.y);
}

double COneSeg::TwoPointsGap(const double& x1, const double& y1, const double& x2, const double& y2)
{
	return sqrt((y1 - y2) * (y1 - y2) + (x1 - x2) * (x1 - x2));
}

int COneSeg::ValidRect(cv::Rect& roi, const int& nWidth, const int& nHeight)
{
	if (roi.x < 0 || roi.x > nWidth - 1)
	{
		roi.x = 0;
	}

	if (roi.x + roi.width > nWidth)
	{
		roi.width = nWidth - roi.x;
	}

	if (roi.y < 0 || roi.y > nHeight - 1)
	{
		roi.y = 0;
	}

	if (roi.y + roi.height > nHeight)
	{
		roi.height = nHeight - roi.y;
	}

	return 0;
}


