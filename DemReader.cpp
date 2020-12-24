/* ******* DemReader.cpp **********
********* DemReader常用操作函数实现 ********** */

/* author: autumoon */

#include "DemReader.h"

template <class T>
bool FindMaxMin(std::list<T>& lTs, T& Tmax, T& Tmin);

CDemReader::CDemReader(const CString& strDem) :m_strDemPath(strDem)
{
	m_DataMark = new char[100];
	m_Unit = new char[100];
}

CDemReader::~CDemReader()
{
	if (m_DataMark)
	{
		delete[]m_DataMark;
		m_DataMark = nullptr;
	}
	if (m_Unit)
	{
		delete[]m_Unit;
		m_Unit = nullptr;
	}
}

bool CDemReader::CheckData(const double& dData)
{
	return fabs(dData - NoData) > 0.001;
}

int CDemReader::ReadDemData()
{
	FILE *fp;
	const char* lpszFileName = CStdStr::ws2s(CMfcStrFile::CString2string(m_strDemPath)).c_str();
	fopen_s(&fp, lpszFileName, "rt");
	if (fp == NULL)
	{
		MessageBox(NULL, _T("文件无法打开！"), _T("提示"), MB_ICONERROR);
		return FALSE;
	}
	fscanf_s(fp, "%s", m_DataMark); //保存到数组当中
	if (strcmp(m_DataMark, "NSDTF-DEM") != 0)
	{
		MessageBox(NULL, _T("文件错误！"), _T("提示"), MB_ICONERROR);
		return FALSE;
	}
	//AfxMessageBox(m_DataMark); 
	fscanf_s(fp, "%f", &m_Version);//版本号 
	fscanf_s(fp, "%s", m_Unit);//单位(米) 
	fscanf_s(fp, "%lf", &m_Alfa);//α 
	fscanf_s(fp, "%lf", &m_Beta);//β 
	fscanf_s(fp, "%lf", &m_lfStartX);
	fscanf_s(fp, "%lf", &m_lfStartY);
	fscanf_s(fp, "%lf", &m_lfDx);
	fscanf_s(fp, "%lf", &m_lfDy);
	fscanf_s(fp, "%d", &m_nRow);
	fscanf_s(fp, "%d", &m_nColumn);
	fscanf_s(fp, "%d", &m_nScale);

	int j;
	m_pHeight = new double*[m_nRow];

	for (j = 0; j < m_nRow; j++)
	{
		m_pHeight[j] = new double[m_nColumn];
	}

	long x;
	for (int i = 0; i < m_nRow; i++)
	{
		for (int j = 0; j < m_nColumn; j++)
		{
			fscanf_s(fp, "%ld", &x);
			m_pHeight[i][j] = x;
			if (x == -99999)
			{
				m_pHeight[i][j] = (long)NoData;
			}
			else
				m_pHeight[i][j] = (double)x / m_nScale;
		}
	}

	fclose(fp);

	return 0;
}

int CDemReader::SaveDemData(const CString& strDem)
{
	std::list<_tstring> lOutput;
	lOutput.push_back(CMfcStrFile::CString2string(m_DataMark) + _T('\n'));
	lOutput.push_back(ToString(m_Version, 1) + _T('\n'));
	lOutput.push_back(CStdStr::s2ws(m_Unit) + _T('\n'));
	lOutput.push_back(ToString(m_Alfa, 4) + _T('\n'));
	lOutput.push_back(ToString(m_Beta, 4) + _T('\n'));
	lOutput.push_back(ToString(m_lfStartX) + _T('\n'));
	lOutput.push_back(ToString(m_lfStartY) + _T('\n'));
	lOutput.push_back(ToString(m_lfDx) + _T('\n'));
	lOutput.push_back(ToString(m_lfDy) + _T('\n'));
	lOutput.push_back(CStdTpl::ConvertToString(m_nRow) + _T('\n'));
	lOutput.push_back(CStdTpl::ConvertToString(m_nColumn) + _T('\n'));
	lOutput.push_back(CStdTpl::ConvertToString(m_nScale));

	_tstring strLine;

	for (int i = 0; i < m_nRow; i++)
	{
		for (int j = 0; j < m_nColumn; j++)
		{
			if (j % 10 == 0)
			{
				strLine += _T('\n');
				lOutput.push_back(strLine);
				strLine = _T("");
			}

			if (CheckData(m_pHeight[i][j]))
			{
				strLine += CStdTpl::ConvertToString(m_pHeight[i][j] * m_nScale) + _T(" ");
			}
			else
				strLine += CStdTpl::ConvertToString(m_pHeight[i][j]) + _T(" ");
		}
	}
	

	//增加最后一行
	strLine += '\n';
	lOutput.push_back(strLine);
	strLine = _T("");

	CStdFile::SaveTXTFile(CMfcStrFile::CString2string(strDem), lOutput);
	
	return 0;
}

int CDemReader::CreateNewDem(const CString& strDemPath, const double& dStartX, const double& dStartY,
	const int& nRow, const int& nColumn,
	const int& nScale /* = 1000*/, const double& dDx /*= 5.0*/, const double& dDy /*= 5.0*/,
	const CString& strDataMark /*= _T("NSDTF-DEM")*/, const float& fVersion /*= 1.0*/, const CString& strUnit /*= _T("M")*/,
	const float& fAlfa /*= 0.000*/, const float& fBeta /*= 0.0000*/)
{
	std::list<_tstring> lOutput;
	lOutput.push_back(CMfcStrFile::CString2string(strDataMark) + _T('\n'));
	lOutput.push_back(ToString(fVersion, 1) + _T('\n'));
	lOutput.push_back(CStdStr::s2ws(m_Unit) + _T('\n'));
	lOutput.push_back(ToString(fAlfa, 4) + _T('\n'));
	lOutput.push_back(ToString(fBeta, 4) + _T('\n'));
	lOutput.push_back(ToString(dStartX) + _T('\n'));
	lOutput.push_back(ToString(dStartY) + _T('\n'));
	lOutput.push_back(ToString(dDx) + _T('\n'));
	lOutput.push_back(ToString(dDy) + _T('\n'));
	lOutput.push_back(CStdTpl::ConvertToString(nRow) + _T('\n'));
	lOutput.push_back(CStdTpl::ConvertToString(nColumn) + _T('\n'));
	lOutput.push_back(CStdTpl::ConvertToString(nScale));

	_tstring strLine;

	for (int i = 0; i < m_nRow; i++)
	{
		for (int j = 0; j < m_nColumn; j++)
		{
			if (j % 10 == 0)
			{
				strLine += _T('\n');
				lOutput.push_back(strLine);
				strLine = _T("");
			}
			strLine += CStdTpl::ConvertToString(NoData) + _T(" ");
		}
	}

	//增加最后一行
	strLine += '\n';
	lOutput.push_back(strLine);
	strLine = _T("");

	CStdFile::SaveTXTFile(CMfcStrFile::CString2string(strDemPath), lOutput);

	return 0;
}

int CDemReader::CreateDemByPic(const CString& strPicPath)
{
	CGdalUser gu;
	gu.m_strFilePath = CStdStr::ws2s(CMfcStrFile::CString2string(strPicPath));
	gu.InitializeRead(gu.m_strFilePath.c_str());

	double dStartX = 0;
	double dStartY = 0;
	//在每个像素之间的距离代表1m的时候，不需要通过tfw文件更新参数
	m_nRow = gu.m_nImgHeight / 5;
	m_nColumn = gu.m_nImgWidth / 5;

	_tstring sPicPath = CMfcStrFile::CString2string(strPicPath);
	CString strTfwPath = (CStdStr::GetDirOfFile(sPicPath) + _T("\\") + CStdStr::GetNameOfFile(sPicPath, false) + _T(".tfw")).c_str();
	CString strDemPath = (CStdStr::GetDirOfFile(sPicPath) + _T("\\") + CStdStr::GetNameOfFile(sPicPath, false) + _T(".dem")).c_str();

	if (CStdFile::IfAccessFile(CMfcStrFile::CString2string(strTfwPath)))
	{
		//存在tfw文件的时候，需要更新参数
	}

	CreateNewDem(strDemPath, dStartX, dStartY, m_nRow, m_nColumn);

	return 0;
}

int CDemReader::ChangeAreaHeight(const int& nXoffset, const int& nYOffset, const int& nWidth, const int& nHeight, const double& dNewHeight)
{
	for (int i = 0; i < m_nRow; i++)
	{
		for (int j = 0; j < m_nColumn; j++)
		{
			if ( j >= nXoffset && j <= nXoffset + nWidth && i >= nYOffset && i <= nYOffset + nHeight)
			{
				m_pHeight[i][j] = dNewHeight;
			}
		}
	}

	return 0;
}

int CDemReader::CreateBmpByDem(const CString& strDemPath, const CString& strBmpPath)
{
	ReadDemData();

	int nWidth = m_nColumn;
	int nHeight = m_nRow;

	std::list<double> ldArray;
	for (int i = 0; i < m_nRow; i++)
	{
		for (int j = 0; j < m_nColumn; j++)
		{
			if (CheckData(m_pHeight[i][j]))
			{
				ldArray.push_back(m_pHeight[i][j]);
			}
		}
	}

	double dMaxHeight;
	double dMinHeight;
	FindMaxMin(ldArray, dMaxHeight, dMinHeight);


	CImage *image = new CImage();
	image->Create(nWidth, nHeight, 32);
	CDC *pDC = CDC::FromHandle(image->GetDC());
	pDC->FillSolidRect(0, 0, nWidth, nHeight, RGB(255, 255, 255));
	for (int i = 0; i < nWidth; i++)
	{
		for (int j = 0; j < nHeight; j++)
		{
			//设置每个像素点的RGB
			if (CheckData(m_pHeight[j][i]))
			{
				double dGray = (m_pHeight[j][i] - dMinHeight) / (dMaxHeight - dMinHeight) * 255;
				image->SetPixelRGB(i, j, (BYTE)dGray, (BYTE)dGray, (BYTE)dGray);
			}
			else
			{
				image->SetPixelRGB(i, j, 0, 0, 0);
			}
		}
	}
	image->Save(strBmpPath);

	return 0;
}

template <class T>
bool FindMaxMin(std::list<T>& lTs, T& Tmax, T& Tmin)
{
	std::list<T>::iterator it = lTs.begin();
	Tmax = Tmin = *it;

	for (; it != lTs.end(); ++it)
	{
		if (*it > Tmax)
		{
			Tmax = *it;
		}
		else if (*it < Tmin)
		{
			Tmin = *it;
		}
	}

	return Tmax == Tmin;
}


