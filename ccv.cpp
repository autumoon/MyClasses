/* ******* ccv.cpp **********
********* opencv常用操作函数实现 ********** */

/* author: autumoon */

#include "ccv.h"

CCvImage::CCvImage()
{
	ResetRect(m_rect);
	ResetRect(m_CRroi);
	IInput = NULL;
}

CCvImage::CCvImage(char szImgPath[], int flags/* = 1*/)
{
	ResetRect(m_rect);
	ResetRect(m_CRroi);
	IInput = NULL;

	m_Minput = cv::imread(szImgPath, flags);
}

CCvImage::CCvImage(CString strImgPath, int flags/* = 1*/)
{
	ResetRect(m_rect);
	ResetRect(m_CRroi);
	IInput = NULL;

	char* szImgPath;

#ifdef _UNICODE
	USES_CONVERSION;
	szImgPath = W2A(strImgPath);
#else
	szImgPath = (LPSTR)(LPCTSTR)strImgPath;
#endif

	m_Minput = cv::imread(szImgPath, flags);
}

double CCvImage::GetAvgGray(const cv::Mat& mSrc)
{
	IplImage gray = mSrc;
	CvScalar scalar = cvAvg(&gray);

	return scalar.val[0];
}

int CCvImage::AdaptiveFindThreshold(CvMat *dx, CvMat *dy, double *low, double *high)
{                                                                              
	CvSize size;                                                           
	IplImage *imge=0;                                                      
	int i,j;                                                               
	CvHistogram *hist;                                                     
	int hist_size = 255;                                                   
	float range_0[]={0,256};                                               
	float* ranges[] = { range_0 };                                         
	double PercentOfPixelsNotEdges = 0.7;                                  
	size = cvGetSize(dx);                                                  
	imge = cvCreateImage(size, IPL_DEPTH_32F, 1);                          
	// 计算边缘的强度, 并存于图像中                                        
	float maxv = 0;                                                        
	for(i = 0; i < size.height; i++ )                                      
	{                                                                      
		const short* _dx = (short*)(dx->data.ptr + dx->step*i);        
		const short* _dy = (short*)(dy->data.ptr + dy->step*i);        
		float* _image = (float *)(imge->imageData + imge->widthStep*i);
		for(j = 0; j < size.width; j++)                                
		{                                                              
			_image[j] = (float)(abs(_dx[j]) + abs(_dy[j]));        
			maxv = maxv < _image[j] ? _image[j]: maxv;             

		}                                                              
	}                                                                      
	if(maxv == 0){                                                         
		*high = 0;                                                     
		*low = 0;                                                      
		cvReleaseImage( &imge );                                       
		return -1;                                                        
	}                                                                      

	// 计算直方图                                                          
	range_0[1] = maxv;                                                     
	hist_size = (int)(hist_size > maxv ? maxv:hist_size);                  
	hist = cvCreateHist(1, &hist_size, CV_HIST_ARRAY, ranges, 1);          
	cvCalcHist( &imge, hist, 0, NULL );                                    
	int total = (int)(size.height * size.width * PercentOfPixelsNotEdges); 
	float sum=0;                                                           
	int icount = hist->mat.dim[0].size;                                    

	float *h = (float*)cvPtr1D( hist->bins, 0 );                           
	for(i = 0; i < icount; i++)                                            
	{                                                                      
		sum += h[i];                                                   
		if( sum > total )                                              
			break;                                                 
	}                                                                      
	// 计算高低门限                                                        
	*high = (i+1) * maxv / hist_size ;                                     
	*low = *high * 0.4;                                                    
	cvReleaseImage( &imge );                                               
	cvReleaseHist(&hist);

	return 0;
}    

int CCvImage::AutoSizeMat(cv::Mat& Msrc, cv::Mat& Mdst, float& fProportion, int nScreenWidth/* = SCREEN_WIDTH*/, int nScreenHeight/* = SCREEN_HEIGHT*/)
{
	if (Msrc.cols <= nScreenWidth && Msrc.rows <= nScreenHeight)
	{
		Msrc.copyTo(Mdst);
		fProportion = 1.0;

		return 0;
	}

	if ((float)Msrc.cols / Msrc.rows >= (float)nScreenWidth / nScreenWidth)
	{
		fProportion = (float)Msrc.cols / nScreenWidth;
	}
	else
	{
		fProportion = (float)Msrc.rows / nScreenHeight;
	}

	cv::resize(Msrc, Mdst, cv::Size(Msrc.cols / fProportion, Msrc.rows / fProportion));

	return 0;
}

int CCvImage::CannyAutoThreshold(cv::Mat Msrc, cv::Mat& Mdst)
{
	double low, high;
	IplImage Isrc = Msrc;
	cv::Mat src;

	if (Msrc.channels() == 3)
	{
		cv::cvtColor(Msrc, src, CV_RGB2GRAY);
	}
	else
	{
		src = Msrc;
	}
	const int cn = src.channels();                                         
	cv::Mat dx(src.rows, src.cols, CV_16SC(cn), cv::BORDER_REPLICATE);                           
	cv::Mat dy(src.rows, src.cols, CV_16SC(cn), cv::BORDER_REPLICATE);                           

	cv::Sobel(src, dx, CV_16S, 1, 0, 3, 1, 0);
	cv::Sobel(src, dy, CV_16S, 0, 1, 3, 1, 0);

	CvMat _dx = dx, _dy = dy;                                              
	AdaptiveFindThreshold(&_dx, &_dy, &low, &high);
	cv::Canny(Msrc, Mdst, low, high);

	return 0;
}

int CCvImage::EasyCanny(cv::Mat Msrc, cv::Mat& Mdst, double threshold1/* = 100*/, double threshold2/* = 200*/)
{
	cv::Mat Mgray;
	if (Msrc.channels() == 3)
	{
		cv::cvtColor(Msrc, Mgray, CV_RGB2GRAY);
	}
	else
	{
		Mgray = Msrc;
	}

	cv::Canny(Mgray, Mdst, threshold1, threshold2);

	return 0;
}

int CCvImage::ErodeDilate(cv::Mat Msrc, cv::Mat& Mdst, int nPos)
{
	int n = nPos - 10;
	int an = n > 0 ? n : -n;
	cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(an * 2 + 1, an * 2 + 1), cv::Point(an, an));
	if( n < 0 )
		cv::erode(Msrc, Mdst, element);
	else
		cv::dilate(Msrc, Mdst, element);

	return 0;
}

int CCvImage::OpenClose(cv::Mat Msrc, cv::Mat& Mdst, int nPos)
{
	int n = nPos - 10;
	int an = n > 0 ? n : -n;
	cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(an * 2 + 1, an * 2 + 1), cv::Point(an, an));
	if( n < 0 )
		cv::morphologyEx(Msrc, Mdst, CV_MOP_OPEN, element);
	else
		cv::morphologyEx(Msrc, Mdst, CV_MOP_CLOSE, element);

	return 0;
}

int CCvImage::ShowImg(char szTitle[]/* = "显示图像"*/, bool bAutoClose/* = true*/)
{
	cv::imshow(szTitle, m_Minput);
	cv::waitKey();
	if (bAutoClose)
	{
		cv::destroyWindow(szTitle);
	}
	return 0;
}

int CCvImage::ShowImg(cv::Mat Msrc, char szTitle[]/* = "显示图像"*/, bool bAutoClose/* = true*/)
{
	cv::imshow(szTitle, Msrc);
	cv::waitKey();
	if (bAutoClose)
	{
		cv::destroyWindow(szTitle);
	}
	return 0;
}

int CCvImage::ShowImgAutoSize(char szTitle[]/* = "显示图像"*/, int nScreenWidth/* = 1920*/, int nScreenHeight/* = 1080*/, bool bAutoClose/* = true*/)
{
	return ShowImgAutoSize(m_Minput, szTitle, nScreenWidth, nScreenHeight, bAutoClose);
}

int CCvImage::ShowImgAutoSize(cv::Mat Msrc, char szTitle[]/* = "显示图像"*/, int nScreenWidth/* = 1920*/, int nScreenHeight/* = 1080*/, bool bAutoClose/* = true*/)
{
	cv::Mat Mresize;
	float fProportion;
	AutoSizeMat(Msrc, Mresize, fProportion, nScreenWidth, nScreenHeight);

	cv::imshow(szTitle, Mresize);
	cv::waitKey(1000);
	if (bAutoClose)
	{
		cv::destroyWindow(szTitle);
	}

	return 0;
}

int CCvImage::SobelCalc(cv::Mat Msrc, cv::Mat& pMdst, bool bGray/* = false*/)
{    
	cv::Mat src, dst_x, dst_y, dst;

	if (bGray && Msrc.channels() == 3)
	{
		cv::cvtColor(Msrc, src, CV_RGB2GRAY);
	}
	else
	{
		src = Msrc;
	}

	cv::Sobel(src, dst_x, src.depth(), 1, 0);
	cv::Sobel(src, dst_y, src.depth(), 0, 1);
	cv::convertScaleAbs(dst_x, dst_x);
	cv::convertScaleAbs(dst_y, dst_y);
	cv::addWeighted(dst_x, 0.5, dst_y, 0.5, 0, dst);
	dst.copyTo(pMdst);

	return 0;  
}

int CCvImage::DrawCurveOnImg(cv::Mat Msrc, char* szTitle/* = "画曲线"*/, int nThick/* = 2*/)
{
	m_Mmid.release();
	m_Mmid = Msrc;
	//显示原图
	cv::imshow(szTitle, m_Mmid);

	//鼠标回调函数
	this->m_szTitle = szTitle;
	this->m_nThick = nThick;
	cvSetMouseCallback(szTitle, on_mouse_curve, this);
	cv::waitKey(0);

	cv::destroyWindow(szTitle);

	return 0;
}

void on_mouse_curve(int event, int x, int y, int flags, void* pParameters)
{
	CCvImage* pthis = (CCvImage*)pParameters;

	//获取相关的参数
	char* szTitle = pthis->m_szTitle;
	CvPoint& prev_pt = pthis->m_prev_pt;
	int nThick = pthis->m_nThick;
	cv::Mat& Mmid = pthis->m_Mmid;

	if(!Mmid.data)
		return;

	if ( event == CV_EVENT_LBUTTONUP || !(flags & CV_EVENT_FLAG_LBUTTON))  //判断事件为松开鼠标左键或者不是左拖拽
	{
		prev_pt = cvPoint(-1, -1);
	}
	else if (event == CV_EVENT_LBUTTONDOWN)  //判断为按下左键
	{
		prev_pt = cvPoint(x,y);
	}
	else if ( event == CV_EVENT_MOUSEMOVE && (flags & CV_EVENT_FLAG_LBUTTON))  //判断移动鼠标并且左拖拽
	{
		CvPoint pt = cvPoint(x, y);
		if ( prev_pt.x < 0)
		{
			prev_pt = pt;
		}
		line(Mmid, prev_pt, pt, cv::Scalar::all(255), nThick, 8, 0); //原图上划线
		prev_pt = pt;
		cv::imshow(szTitle, Mmid);
	}

	if (event == CV_EVENT_RBUTTONUP)
	{
		cv::destroyWindow(szTitle);
	}
}

int CCvImage::DrawCurveOnBigImg(cv::Mat Msrc,
	int nScreenWidth/* = SCREEN_WIDTH*/, int nScreenHeight/* = SCREEN_HEIGHT*/, char* szTitle/* = "画曲线"*/, int nThick/* = 2*/)
{
	m_Mmid.release();
	m_Msrc.release();
	float fProportion;
	AutoSizeMat(Msrc, m_Mmid, fProportion, nScreenWidth, nScreenHeight);
	m_Msrc = Msrc;

	//显示原图
	cv::imshow(szTitle, m_Mmid);

	//鼠标回调函数
	this->m_szTitle = szTitle;
	this->m_nThick = nThick;
	this->m_fProportion = fProportion;
	cvSetMouseCallback(szTitle, on_mouse_big_curve, this);
	cv::waitKey(0);

	cv::destroyWindow(szTitle);

	return 0;
}

void on_mouse_big_curve(int event, int x, int y, int flags, void* pParameters)
{
	CCvImage* pthis = (CCvImage*)pParameters;

	//获取相关的参数
	char* szTitle = pthis->m_szTitle;
	CvPoint& prev_pt = pthis->m_prev_pt;
	int nThick = pthis->m_nThick;
	cv::Mat& Mmid = pthis->m_Mmid;

	if(!Mmid.data)
		return;

	if ( event == CV_EVENT_LBUTTONUP || !(flags & CV_EVENT_FLAG_LBUTTON))  //判断事件为松开鼠标左键或者不是左拖拽
	{
		prev_pt = cvPoint(-1, -1);
	}
	else if (event == CV_EVENT_LBUTTONDOWN)  //判断为按下左键
	{
		prev_pt = cvPoint(x,y);
	}
	else if ( event == CV_EVENT_MOUSEMOVE && (flags & CV_EVENT_FLAG_LBUTTON))  //判断移动鼠标并且左拖拽
	{
		CvPoint pt = cvPoint(x, y);
		if ( prev_pt.x < 0)
		{
			prev_pt = pt;
		}
		line(Mmid, prev_pt, pt, cv::Scalar::all(255), nThick, 8, 0); //缩小图上划线

		CvPoint t_prev_pt = prev_pt, t_pt = pt;
		pthis->ResizePoint(t_prev_pt, pthis->m_fProportion);
		pthis->ResizePoint(t_pt, pthis->m_fProportion);
		line(pthis->m_Msrc, t_prev_pt, t_pt, cv::Scalar::all(255), nThick, 8, 0); //原图上划线
		prev_pt = pt;
		cv::imshow(szTitle, Mmid);
	}

	if (event == CV_EVENT_RBUTTONUP)
	{
		cv::destroyWindow(szTitle);
	}
}

int CCvImage::DrawLineOnImg(cv::Mat Msrc, CvPoint& pStart, CvPoint& pEnd, char* szTitle/* = "画直线"*/, int nThick/* = 2*/)
{
	m_Mmid.release();
	Msrc.copyTo(m_Mmid);
	//显示原图
	cv::imshow(szTitle, m_Mmid);

	//鼠标回调函数
	this->m_szTitle = szTitle;
	this->m_nThick = nThick;
	cvSetMouseCallback(szTitle, on_mouse_line, this);
	cv::waitKey(0);

	pStart = m_prev_pt;
	pEnd = m_curr_pt;
	cv::destroyWindow(szTitle);

	return 0;
}

int CCvImage::DrawLineOnBigImg(cv::Mat Msrc, CvPoint& pStart, CvPoint& pEnd,
	int nScreenWidth/* = SCREEN_WIDTH*/, int nScreenHeight/* = SCREEN_HEIGHT*/, char* szTitle/* = "画直线"*/, int nThick/* = 2*/)
{
	m_Mmid.release();
	float fProportion;
	AutoSizeMat(Msrc, m_Mmid, fProportion, nScreenWidth, nScreenHeight);
	//显示原图
	cv::imshow(szTitle, m_Mmid);

	//鼠标回调函数
	this->m_szTitle = szTitle;
	this->m_nThick = nThick;
	cvSetMouseCallback(szTitle, on_mouse_line, this);
	cv::waitKey(0);

	pStart = m_prev_pt;
	pEnd = m_curr_pt;
	ResizePoint(pStart, fProportion);
	ResizePoint(pEnd, fProportion);

	cv::destroyWindow(szTitle);

	return 0;
}

void on_mouse_line(int event, int x, int y, int flags, void* pParameters)
{
	CCvImage* pthis = (CCvImage*)pParameters;

	//获取相关的参数
	char* szTitle = pthis->m_szTitle;
	CvPoint curr_pt;
	CvPoint prev_pt = pthis->m_prev_pt; //注意这里不是引用
	int nThick = pthis->m_nThick;
	cv::Mat& Mmid = pthis->m_Mmid;

	if(!Mmid.data)
		return;

	cv::Mat Mori;

	if ( event == CV_EVENT_LBUTTONUP || !(flags & CV_EVENT_FLAG_LBUTTON))  //判断事件为松开鼠标左键或者不是左拖拽
	{
		prev_pt = cvPoint(-1, -1);
	}
	else if (event == CV_EVENT_LBUTTONDOWN)  //判断为按下左键
	{
		prev_pt = cvPoint(x,y);
	}
	else if ( event == CV_EVENT_MOUSEMOVE && (flags & CV_EVENT_FLAG_LBUTTON))  //判断移动鼠标并且左拖拽
	{
		CvPoint pt = cvPoint(x, y);
		if ( prev_pt.x < 0)
		{
			prev_pt = pt;
		}
		Mori.release();
		Mmid.copyTo(Mori);
		line(Mori, prev_pt, pt, cv::Scalar::all(255), nThick, 8, 0); //原图上划线
		curr_pt = pt;
		pthis->m_prev_pt = prev_pt;
		pthis->m_curr_pt = pt;
		cv::imshow(szTitle, Mori);
	}

	if (event == CV_EVENT_RBUTTONUP)
	{
		cv::destroyWindow(szTitle);
	}
}

int CCvImage::DrawRectOnImg(cv::Mat Msrc, CvRect& rect, char* szTitle/* = "画矩形"*/, int nThick/* = 2*/)
{
	m_Mmid.release();
	Msrc.copyTo(m_Mmid);

	//显示原图
	cv::imshow(szTitle, m_Mmid);

	//鼠标回调函数
	this->m_szTitle = szTitle;
	this->m_nThick = nThick;
	cvSetMouseCallback(szTitle, on_mouse_rect, this);
	cv::waitKey(0);

	cv::destroyWindow(szTitle);

	rect = this->m_rect;

	return 0;
}

int CCvImage::DrawRectOnBigImg(cv::Mat Msrc, CvRect& rect,
	int nScreenWidth/* = SCREEN_WIDTH*/, int nScreenHeight/* = SCREEN_HEIGHT*/, char* szTitle/* = "画矩形"*/, int nThick/* = 2*/)
{
	m_Msrc.release();
	m_Mmid.release();
	m_Msrc = Msrc;
	AutoSizeMat(Msrc, m_Mmid, m_fProportion, nScreenWidth, nScreenHeight);

	//显示原图
	cv::imshow(szTitle, m_Mmid);

	//鼠标回调函数
	this->m_szTitle = szTitle;
	this->m_nThick = nThick;
	cvSetMouseCallback(szTitle, on_mouse_rect, this);
	cv::waitKey(0);

	cv::destroyWindow(szTitle);

	rect = this->m_rect;
	ResizeRect(rect, m_fProportion);

	return 0;
}

void on_mouse_rect(int event, int x, int y, int flags, void* pParameters)
{
	CCvImage* pthis = (CCvImage*)pParameters;

	//获取相关的参数
	char* szTitle = pthis->m_szTitle;
	CvPoint& prev_pt = pthis->m_prev_pt;
	CvRect& rect = pthis->m_rect;
	CvRect& RoiRect = pthis->m_CRroi;
	int nThick = pthis->m_nThick;
	cv::Mat& Mmid = pthis->m_Mmid;
	cv::Mat& Msrc = pthis->m_Msrc;

	if(!Mmid.data)
		return;

	cv::Mat Mori;

	if ( event == CV_EVENT_LBUTTONUP || !(flags & CV_EVENT_FLAG_LBUTTON))  //判断事件为松开鼠标左键或者不是左拖拽
	{
		prev_pt = cvPoint(-1, -1);
	}
	else if (event == CV_EVENT_LBUTTONDOWN)  //判断为按下左键
	{
		prev_pt = cvPoint(x,y);
	}
	else if ( event == CV_EVENT_MOUSEMOVE && (flags & CV_EVENT_FLAG_LBUTTON))  //判断移动鼠标并且左拖拽
	{
		if (RoiRect.width == -1 || RoiRect.height == -1)
		{
			CvPoint pt = cvPoint(x, y);
			if ( prev_pt.x < 0)
			{
				prev_pt = pt;
			}
			Mori.release();
			Mmid.copyTo(Mori);
			rectangle(Mori, prev_pt, pt, cv::Scalar::all(255), nThick);
			rect = cvRect(prev_pt.x, prev_pt.y, pt.x - prev_pt.x, pt.y - prev_pt.y);
			cv::imshow(szTitle, Mori);
		}
		else
		{
			if (pthis->m_Msrc.cols != pthis->m_Mmid.cols || pthis->m_Msrc.rows != pthis->m_Mmid.rows)
			{
				IplImage Isrc = pthis->m_Msrc;
				CvPoint pt = cvPoint(x, y);

				cv::Mat Mr(pthis->m_Msrc, RoiRect);
				Mr.copyTo(Mori);
				rectangle(Mori, prev_pt, pt, cv::Scalar::all(255), nThick);
				rect = cvRect(RoiRect.x + prev_pt.x, RoiRect.y + prev_pt.y, pt.x - prev_pt.x, pt.y - prev_pt.y);
				pthis->ResizeRect(rect, 1 / pthis->m_fProportion);
				cv::imshow(szTitle, Mori);
			}
		}
	}

	if (event == CV_EVENT_LBUTTONDBLCLK)
	{
		if (RoiRect.width == -1 || RoiRect.height == -1)
		{
			if (pthis->m_Msrc.cols != pthis->m_Mmid.cols || pthis->m_Msrc.rows != pthis->m_Mmid.rows)
			{
				IplImage Isrc = pthis->m_Msrc;
				CvPoint pt = cvPoint(x, y);
				RoiRect = cvRect(x * pthis->m_fProportion - pthis->m_Mmid.cols / 2, y  * pthis->m_fProportion - pthis->m_Mmid.rows / 2, pthis->m_Mmid.cols, pthis->m_Mmid.rows);
				pthis->ValidRect(RoiRect, pthis->m_Mmid.cols, pthis->m_Mmid.rows, pthis->m_Msrc.cols, pthis->m_Msrc.rows);

				cvSetImageROI(&Isrc, RoiRect);
				cvShowImage(szTitle, &Isrc);
				cvResetImageROI(&Isrc);
			}
		}
		else
		{
			Mori.release();
			Mmid.copyTo(Mori);
			cv::imshow(szTitle, Mori);
			pthis->ResetRect(RoiRect);
		}
	}

	if (event == CV_EVENT_RBUTTONUP)
	{
		cv::destroyWindow(szTitle);
	}
}

int CCvImage::GetMaskByCurve(cv::Mat Msrc, cv::Mat& Mdst, char* szTitle/* = "获取遮罩"*/)
{
	m_Mmid.release();
	Msrc.copyTo(m_Mmid);

	cv::Mat mask(Msrc.rows, Msrc.cols, CV_8U, cv::Scalar(255));
	m_Mmask.release();
	m_Mmask = mask;

	//显示原图
	cv::imshow(szTitle, m_Mmid);

	//鼠标回调函数
	this->m_szTitle = szTitle;
	cvSetMouseCallback(szTitle, on_mouse_curve_mask, this);
	cv::waitKey(0);

	m_Mmask.copyTo(Mdst);
	cv::destroyWindow(szTitle);

	return 0;
}

void on_mouse_curve_mask(int event, int x, int y, int flags, void* pParameters)
{
	CCvImage* pthis = (CCvImage*)pParameters;

	//获取相关的参数
	char* szTitle = pthis->m_szTitle;
	CvPoint& prev_pt = pthis->m_prev_pt;
	CvRect& rect = pthis->m_rect;
	cv::Mat& Mmid = pthis->m_Mmid;
	cv::Mat& Mmask = pthis->m_Mmask;

	if(!Mmid.data)
		return;

	if ( event == CV_EVENT_LBUTTONUP || !(flags & CV_EVENT_FLAG_LBUTTON))  //判断事件为松开鼠标左键或者不是左拖拽
	{
		prev_pt = cvPoint(-1, -1);
	}
	else if (event == CV_EVENT_LBUTTONDOWN)  //判断为按下左键
	{
		prev_pt = cvPoint(x,y);
	}
	else if ( event == CV_EVENT_MOUSEMOVE && (flags & CV_EVENT_FLAG_LBUTTON))  //判断移动鼠标并且左拖拽
	{
		CvPoint pt = cvPoint(x, y);
		if ( prev_pt.x < 0)
		{
			prev_pt = pt;
		}
		line(Mmask, prev_pt, pt, cv::Scalar(0), 2, 8, 0); //模板上划线
		line(Mmid, prev_pt, pt, cv::Scalar::all(255), 2, 8, 0);          //原图上划线
		prev_pt = pt;
		cv::imshow(szTitle, Mmid);
	}
	if (event == CV_EVENT_RBUTTONUP)
	{
		cv::floodFill(Mmask, cv::Point(x, y), cv::Scalar(0));//填充抠图模板
	}
}

int CCvImage::GetMaskByDaub(cv::Mat Msrc, cv::Mat& Mdst, int nRadius/* = 20*/, char* szTitle/* = "获取遮罩"*/)
{
	m_Mmid.release();
	Msrc.copyTo(m_Mmid);

	cv::Mat mask(Msrc.rows, Msrc.cols, CV_8U, cv::Scalar(0));
	m_Mmask.release();
	m_Mmask = mask;

	//显示原图
	cv::imshow(szTitle, m_Mmid);

	//鼠标回调函数
	this->m_szTitle = szTitle;
	this->m_nThick = nRadius;
	cvSetMouseCallback(szTitle, on_mouse_daub, this);
	cv::waitKey(0);

	m_Mmask.copyTo(Mdst);
	cv::destroyWindow(szTitle);

	return 0;
}

void on_mouse_daub(int event, int x, int y, int flags, void* pParameters)
{
	CCvImage* pthis = (CCvImage*)pParameters;

	//获取相关的参数
	char* szTitle = pthis->m_szTitle;
	CvPoint& prev_pt = pthis->m_prev_pt;
	int nThick = pthis->m_nThick;
	cv::Mat& Mmid = pthis->m_Mmid;

	if(!Mmid.data)
		return;

	if ( event == CV_EVENT_LBUTTONUP || !(flags & CV_EVENT_FLAG_LBUTTON))  //判断事件为松开鼠标左键或者不是左拖拽
	{
		prev_pt = cvPoint(-1, -1);
	}
	else if (event == CV_EVENT_LBUTTONDOWN)  //判断为按下左键
	{
		prev_pt = cvPoint(x,y);
	}
	else if ( event == CV_EVENT_MOUSEMOVE && (flags & CV_EVENT_FLAG_LBUTTON))  //判断移动鼠标并且左拖拽
	{
		CvPoint pt = cvPoint(x, y);
		if ( prev_pt.x < 0)
		{
			prev_pt = pt;
		}
		line(pthis->m_Mmask, prev_pt, pt, cv::Scalar(255), nThick, 8, 0); //模板上划线
		line(Mmid, prev_pt, pt, cv::Scalar::all(255), nThick, 8, 0);          //原图上划线
		prev_pt = pt;
		cv::imshow(szTitle, Mmid);
	}

	if (event == CV_EVENT_RBUTTONUP)
	{
		cv::destroyWindow(szTitle);
	}
}


int CCvImage::OutputMat(const cv::Mat& mSrc, const CString& strTxtPath)
{
	std::vector<CString> vContent;
	for (int j = 0; j < mSrc.rows; ++j)
	{
		const uchar* data = mSrc.ptr<uchar>(j);
		CString strLine;
		for (int i = 0; i < mSrc.cols; ++i)
		{
			CString strPix;
			strPix.Format(_T("%d"), data[i]);
			strLine += strPix + ' ';
		}
		vContent.push_back(strLine + '\n');
	}

	CStdioFile file;
	file.Open(strTxtPath, CFile::modeCreate | CFile::modeReadWrite);

	for (std::vector<CString>::iterator it = vContent.begin(); it != vContent.end(); ++it)
	{
		file.WriteString(*it);
	}
	file.Close();

	return 0;
}


int CCvImage::ResetRect(CvRect& rect, int nValue/* = -1*/)
{
	rect.x = nValue;
	rect.y = nValue;
	rect.width = nValue;
	rect.height = nValue;

	return 0;
}

int CCvImage::ResizePoint(CvPoint& Csrc, float fProportion)
{
	Csrc.x *= fProportion;
	Csrc.y *= fProportion;

	return 0;
}

int CCvImage::ResizeRect(CvRect& Csrc, float fProportion)
{
	Csrc.x *= fProportion;
	Csrc.y *= fProportion;
	Csrc.width *= fProportion;
	Csrc.height *= fProportion;

	return 0;
}

int CCvImage::ResizeRect(CvRect& rect, int nExpandWidthPixs/* = 0*/, int nExpandHeightPixs/* = 0*/)
{
	rect.x -= nExpandWidthPixs;
	rect.y -= nExpandHeightPixs;
	rect.width += nExpandWidthPixs * 2;
	rect.height += nExpandHeightPixs * 2;

	return 0;
}

int CCvImage::ResizeRect(CvRect& rect, int nLeft, int nRight, int nUp, int nDown)
{
	rect.x -= nLeft;
	rect.y -= nUp;
	rect.width += nLeft + nRight;
	rect.height += nUp + nDown;

	return 0;
}

int CCvImage::ValidExpandValue(const CvRect& CRroi, int& nLeft, int& nRight, int& nUp, int& nDown, int nWidth, int nHeight)
{
	if (CRroi.x < nLeft)
	{
		nLeft = CRroi.x;
	}

	if (CRroi.x + CRroi.width + nRight > nWidth)
	{
		nRight = nWidth - CRroi.x - CRroi.width;
	}

	if (CRroi.y < nUp)
	{
		nUp = CRroi.y;
	}

	if (CRroi.y + CRroi.height + nDown > nHeight)
	{
		nDown = nHeight - CRroi.y - CRroi.height;
	}

	return 0;
}

int CCvImage::ValidRect(CvRect& rect, int nWidth, int nHeight)
{
	if (rect.width < 0 && rect.height < 0 && rect.x >= 0 && rect.x <= nWidth && rect.y >= 0 && rect.y <= nWidth)
	{
		//反向操作
		rect.x += rect.width;
		rect.y += rect.height;
		rect.width = -rect.width;
		rect.height = -rect.height;
	}

	if (rect.x < 0)
	{
		rect.x = 0;
	}

	if (rect.x > nWidth)
	{
		rect.x = nWidth;
	}

	if (rect.y > nHeight)
	{
		rect.y = nHeight;
	}

	if (rect.y < 0)
	{
		rect.y = 0;
	}

	if (rect.x + rect.width > nWidth)
	{
		rect.width = nWidth - rect.x;
	}

	if (rect.y + rect.height > nHeight)
	{
		rect.height = nHeight - rect.y;
	}

	return 0;
}

int CCvImage::ValidRect(CvRect& rectSmall, const int nRectWidth, const int nRectHeight, const int nPicWidth, const int nPicHeight)
{
	ValidRect(rectSmall, nPicWidth, nPicHeight);

	if (rectSmall.x + nRectWidth > nPicWidth)
	{
		rectSmall.x = nPicWidth - nRectWidth;
		rectSmall.width = nRectWidth;
	}

	if (rectSmall.y + nRectHeight > nPicHeight)
	{
		rectSmall.y = nPicHeight -  nRectHeight;
		rectSmall.height = nRectHeight;
	}

	return 0;
}

long CCvImage::ValusPixCounts(const cv::Mat& mSrc, const int& nValue/* = 255*/)
{
	long lCount = 0;

	for (int j = 0; j < mSrc.rows; ++j)
	{
		const uchar* data = mSrc.ptr<uchar>(j);
		for (int i = 0; i < mSrc.cols; ++i)
		{
			if (data[i] == nValue)
			{
				++lCount;
			}
		}
	}

	return lCount;
}

