/* ******* GdiplusUserU.cpp **********
********* GDI+ͼ���������ʵ�� ********** */

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
{ //�õ���ʽΪformat��ͼ���ļ��ı���ֵ�����ʸø�ʽͼ���COM�����GUIDֵ������pCLSID�� 
	UINT num = 0; 
	UINT size = 0; 
	ImageCodecInfo* pImageCodecInfo = NULL; 
	GetImageEncodersSize(&num, &size); 
	if(size == 0) 
		return FALSE; // ������Ϣ������ 
	//�����ڴ� 
	pImageCodecInfo = (ImageCodecInfo*)(malloc(size)); 
	if(pImageCodecInfo == NULL) 
		return FALSE; // ����ʧ�� 
	//���ϵͳ�п��õı��뷽ʽ��������Ϣ 
	GetImageEncoders(num, size, pImageCodecInfo); 
	//�ڿ��ñ�����Ϣ�в���format��ʽ�Ƿ�֧�� 
	for(UINT i = 0; i < num; ++i) 
	{ //MimeType�����뷽ʽ�ľ������� 
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
	CString strLoadExt,strSaveExt;//��ȡ�򿪺ͱ����ͼ���ļ���׺��
	int nLenLoadExt = strLoadFile.ReverseFind('.');
	int nLenSaveExt = strSaveFile.ReverseFind('.');
	strLoadExt = strLoadFile.Right(strLoadFile.GetLength()-nLenLoadExt-1);
	strSaveExt = strSaveFile.Right(strSaveFile.GetLength()-nLenSaveExt-1);
	strLoadExt.MakeLower();
	strSaveExt.MakeLower();
	//�ж��ļ���׺���Ƿ���ȷ
	if (strSaveExt!="bmp"&&strSaveExt!="jpg"&&strSaveExt!="jpeg"&&strSaveExt!="png"\
		&&strSaveExt!="tiff"&&strSaveExt!="gif")
	{
		return FALSE;
	}
	if("jpg" == strSaveExt)
		strSaveExt = "jpeg";

	CLSID encoderClsid ;
	Image *image = NULL;

	//�����ļ�
	image = Bitmap::FromFile(strLoadFile,TRUE);
	if (image==NULL)
	{
		return FALSE;
	}
	WCHAR Wctype[1024]=L"image/";
	wcscat(Wctype,strSaveExt);
	///��ͼƬCLSID///////////////////////////////////////////////////////////////////////
	GetImageCLSID(Wctype,&encoderClsid);

	//����ͼƬ
	BOOL bRes = TRUE;
	if(Ok != image->Save(strSaveFile,&encoderClsid,NULL))
	{
		MessageBox(NULL, _T("ת��ʧ��!"), _T("��ʾ"), MB_ICONWARNING);
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
