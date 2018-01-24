/* ******* GdiplusUserU.cpp **********
********* GDI+图像操作函数实现 ********** */

/* author: autumoon */

#include "GdiplusUserU.h"

CGdiplusUser::CGdiplusUser()
{
	GdiplusStartup(&m_GdiplusToken, &m_Gdistart, NULL); 
}

CGdiplusUser::~CGdiplusUser()
{
	GdiplusShutdown(m_GdiplusToken);
}

int CGdiplusUser::GetImageCLSID(const WCHAR* format, CLSID* pCLSID) 
{ //得到格式为format的图像文件的编码值，访问该格式图像的COM组件的GUID值保存在pCLSID中 
	UINT num = 0; 
	UINT size = 0; 
	ImageCodecInfo* pImageCodecInfo = NULL; 
	GetImageEncodersSize(&num, &size); 
	if(size == 0) 
		return FALSE; // 编码信息不可用 
	//分配内存 
	pImageCodecInfo = (ImageCodecInfo*)(malloc(size)); 
	if(pImageCodecInfo == NULL) 
		return FALSE; // 分配失败 
	//获得系统中可用的编码方式的所有信息 
	GetImageEncoders(num, size, pImageCodecInfo); 
	//在可用编码信息中查找format格式是否被支持 
	for(UINT i = 0; i < num; ++i) 
	{ //MimeType：编码方式的具体描述 
		if( wcscmp(pImageCodecInfo[i].MimeType, format) == 0 ) 
		{ 
			*pCLSID = pImageCodecInfo[i].Clsid; 
			free(pImageCodecInfo); 
			return TRUE; 
		} 
	} 
	free(pImageCodecInfo); 
	return FALSE; 
}

int CGdiplusUser::ConvertOneImage(const CString& strLoadFile, const CString& strSaveFile)
{
	CString strLoadExt,strSaveExt;//获取打开和保存的图像文件后缀名
	int nLenLoadExt = strLoadFile.ReverseFind('.');
	int nLenSaveExt = strSaveFile.ReverseFind('.');
	strLoadExt = strLoadFile.Right(strLoadFile.GetLength()-nLenLoadExt-1);
	strSaveExt = strSaveFile.Right(strSaveFile.GetLength()-nLenSaveExt-1);
	strLoadExt.MakeLower();
	strSaveExt.MakeLower();
	//判断文件后缀名是否正确
	if (strSaveExt!="bmp"&&strSaveExt!="jpg"&&strSaveExt!="jpeg"&&strSaveExt!="png"\
		&&strSaveExt!="tiff"&&strSaveExt!="gif")
	{
		return FALSE;
	}
	if("jpg" == strSaveExt)
		strSaveExt = "jpeg";

	CLSID encoderClsid ;
	Image *image = NULL;

	//加载文件
	image = Bitmap::FromFile(strLoadFile,TRUE);
	if (image==NULL)
	{
		return FALSE;
	}
	WCHAR Wctype[1024]=L"image/";
	wcscat(Wctype,strSaveExt);
	///读图片CLSID///////////////////////////////////////////////////////////////////////
	GetImageCLSID(Wctype,&encoderClsid);

	//保存图片
	BOOL bRes = TRUE;
	if(Ok != image->Save(strSaveFile,&encoderClsid,NULL))
	{
		MessageBox(NULL, _T("转换失败!"), _T("提示"), MB_ICONWARNING);
		bRes = FALSE;
	}

	delete image;
	return bRes;
}

int CGdiplusUser::ConvertImageInDir(const CString& strDirSrc, const CString& strDirDst, const CString& strSuffix/* = _T(".jpg")*/)
{
	CStringArray arrFiles;
	sdf.ReadDirFiles(strDirSrc, &arrFiles, ".bmp");
	sdf.ReadDirFiles(strDirSrc, &arrFiles, ".jpg");
	sdf.ReadDirFiles(strDirSrc, &arrFiles, ".jpeg");
	sdf.ReadDirFiles(strDirSrc, &arrFiles, ".png");
	sdf.ReadDirFiles(strDirSrc, &arrFiles, ".tiff");
	sdf.ReadDirFiles(strDirSrc, &arrFiles, ".gif");

	sdf.CreateMuliteDirectory(strDirDst);
	for (int i = 0; i < arrFiles.GetCount(); ++i)
	{
		CString strDstFile = strDirDst + "\\" + sdf.GetNameOfFile(arrFiles[i], false) + strSuffix;
		ConvertOneImage(arrFiles[i], strDstFile);
	}

	return 0;
}
