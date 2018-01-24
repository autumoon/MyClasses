#include "cxImageUser.h"
#include "StdStrFile.h"

int CxImageUser::m_nJpgQuality = 90;

bool CxImageUser::GetExifTime(std::string fileName, std::string& strTime)
{
	EXIFINFO* pExifInfo;

	CxImage  image; 
	image.Load(CStdStr::s2ws(fileName).c_str(), CXIMAGE_FORMAT_JPG); 
	if (image.IsValid())
	{
		pExifInfo = image.GetExifInfo();

		if (pExifInfo->IsExif)
		{
			//获取时间等信息
			if (pExifInfo->DateTime[0])
			{
				strTime = CStdStr::Trim(pExifInfo->DateTime);
				//如果为空也返回假
				return strTime.length() != 0;
			}
			else
			{
				return false;
			}

		}
		else
		{
			return false;
		}
	}

	return true;    
}

bool CxImageUser::ico2pic(const std::wstring& strSrcFile, const std::wstring& strDstFile, const int& nWidth /*= -1*/, const int& nHeight /*= -1*/)
{
	const std::string& strDstSuffix = CStdStr::ToUpperLower(CStdStr::GetSuffixOfFile(CStdStr::ws2s(strDstFile), false));
	ENUM_CXIMAGE_FORMATS dstFormat = (ENUM_CXIMAGE_FORMATS)CxImage::GetTypeIdFromName(CStdStr::s2ws(strDstSuffix).c_str());

	CxImage  image; 
	image.Load(strSrcFile.c_str(), CXIMAGE_FORMAT_ICO); 
	if (image.IsValid())
	{ 
		int nPageCount = image.GetNumFrames();//得到ico文件的页数
		int nBigFrame = 0;
		int w = -1, h = -1;
		for(int i = 0; i < nPageCount; i++)
		{
			if ( 0 != i)
			{
				image.SetFrame(i);			//设置当前帧为i编号图像
				image.Load(strSrcFile.c_str());
				int w_tmp = image.GetWidth();
				int h_tmp = image.GetHeight();
				if (w_tmp >= w && h_tmp >= h_tmp)
				{
					nBigFrame = i;
					w = w_tmp;
					h = h_tmp;
				}
			}
			else
			{
				image.SetFrame(i);			//设置当前帧为i编号图像
				image.Load(strSrcFile.c_str());
				w = image.GetWidth();
				h = image.GetHeight();
			}
		}

		image.SetFrame(nBigFrame);
		image.Load(strSrcFile.c_str());

		if(!image.IsGrayScale() && dstFormat != CXIMAGE_FORMAT_GIF)
			image.IncreaseBpp(24); 

		if (dstFormat == CXIMAGE_FORMAT_JPG)
		{
			//如果是jpg则设置压缩质量
			image.SetJpegQuality(m_nJpgQuality);
		}

		if (dstFormat == CXIMAGE_FORMAT_GIF)
		{
			image.DecreaseBpp(8, true);
		}

		if (nWidth != -1 && nHeight != -1 && nWidth > 0 && nHeight > 0)
		{
			image.Resample(nWidth, nHeight);
		}

		image.Save(strDstFile.c_str(), dstFormat); 
	}

	return true;
}

bool CxImageUser::pic2pic(const std::wstring& strSrcFile, const std::wstring& strDstFile, const int& nWidth /*= -1*/, const int& nHeight /*= -1*/)
{
	//常见的格式互相转换
	const std::string& strSrcSuffix = CStdStr::ToUpperLower(CStdStr::GetSuffixOfFile(CStdStr::ws2s(strSrcFile), false));
	const std::string& strDstSuffix = CStdStr::ToUpperLower(CStdStr::GetSuffixOfFile(CStdStr::ws2s(strDstFile), false));
	ENUM_CXIMAGE_FORMATS srcFormat = (ENUM_CXIMAGE_FORMATS)CxImage::GetTypeIdFromName(CStdStr::s2ws(strSrcSuffix).c_str());
	ENUM_CXIMAGE_FORMATS dstFormat = (ENUM_CXIMAGE_FORMATS)CxImage::GetTypeIdFromName(CStdStr::s2ws(strDstSuffix).c_str());

	//gif调整尺寸，为了保持动画需要特殊处理
	if (srcFormat == CXIMAGE_FORMAT_GIF && dstFormat == CXIMAGE_FORMAT_GIF)
	{
		return ResizeGif(strSrcFile, strDstFile, nWidth, nHeight);
	}

	//ico格式转换，因为多尺寸的原因需要特殊处理
	if (srcFormat == CXIMAGE_FORMAT_ICO)
	{
		return ico2pic(strSrcFile, strDstFile, nWidth, nHeight);
	}

	CxImage  image; 
	image.Load(strSrcFile.c_str(), srcFormat); 
	if (image.IsValid())
	{ 
		if(!image.IsGrayScale() && dstFormat != CXIMAGE_FORMAT_GIF)
			image.IncreaseBpp(24); 

		if (dstFormat == CXIMAGE_FORMAT_JPG)
		{
			//如果是jpg则设置压缩质量
			image.SetJpegQuality(m_nJpgQuality);
		}

		if (dstFormat == CXIMAGE_FORMAT_GIF)
		{
			image.DecreaseBpp(8, true);
		}

		if (nWidth != -1 && nHeight != -1 && nWidth > 0 && nHeight > 0)
		{
			image.Resample(nWidth, nHeight);
		}

		image.Save(strDstFile.c_str(), dstFormat); 
	}

	return true;
}

bool CxImageUser::ResizeGif(const std::wstring& strSrcFile, const std::wstring& strDstFile, const int& nWidth, const int& nHeight)
{
#if CXIMAGE_SUPPORT_DECODE && CXIMAGE_SUPPORT_ENCODE && CXIMAGE_SUPPORT_GIF
	CxImage img;
	img.Load(strSrcFile.c_str(), CXIMAGE_FORMAT_GIF);

	if (!img.IsValid())
	{
		return false;
	}

	//这样做只能保留第一帧
	//img.Resample(nWidth, nHeight);
	//img.Save(strDstFile.c_str(), CXIMAGE_FORMAT_GIF);

	int iNumFrames = img.GetNumFrames();
	CxImage** imgSave = new CxImage*[iNumFrames];

	for (int i = 0; i < iNumFrames; i++)
	{
		CxImage* newImage = new CxImage();
		newImage->SetFrame(i);
		newImage->Load(strSrcFile.c_str(), CXIMAGE_FORMAT_GIF);
		newImage->Resample(nWidth, nHeight);
		if (0 == newImage->GetNumColors())
		{
			imgSave[i]->DecreaseBpp(8, true);
		}
		imgSave[i] = newImage;
	}

	CxIOFile hFile;
	std::string Method = "wb";
	std::wstring  stempmd = CStdStr::s2ws(Method);
	LPCWSTR wMethod = stempmd.c_str();
	bool BFlag = hFile.Open(strDstFile.c_str(), wMethod);

	CxImageGIF multiimage;

	multiimage.SetLoops(-1);
	multiimage.SetFrameDelay(img.GetFrameDelay());
	multiimage.SetDisposalMethod(img.GetDisposalMethod());
	multiimage.Encode(&hFile, imgSave, iNumFrames, false, false);

	hFile.Close();

	//释放资源
	for (int i = 0; i < iNumFrames; ++i)
	{
		if (imgSave[i])
		{
			delete imgSave[i];
			imgSave[i] = nullptr;
		}
	}
	if (imgSave)
	{
		delete[] imgSave;
		imgSave = nullptr;
	}

#endif

	return true;
}


