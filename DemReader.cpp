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
	}
	if (m_Unit)
	{
		delete[]m_Unit;
	}
}

bool CDemReader::CheckData(const double& dData)
{
	return fabs(dData - NoData) > 0.001;
}

int CDemReader::ReadDemData()
{
	FILE *fp;
	const char* lpszFileName = sdf.CString2Char(m_strDemPath);
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
	std::list<CString> lOutput;
	lOutput.push_back(CString(m_DataMark) + '\n');
	lOutput.push_back(sdf.Float2Cstring(m_Version, 1) + '\n');
	lOutput.push_back(CString(m_Unit) + '\n');
	lOutput.push_back(sdf.Float2Cstring(m_Alfa, 4) + '\n');
	lOutput.push_back(sdf.Float2Cstring(m_Beta, 4) + '\n');
	lOutput.push_back(sdf.Float2Cstring(m_lfStartX) + '\n');
	lOutput.push_back(sdf.Float2Cstring(m_lfStartY) + '\n');
	lOutput.push_back(sdf.Float2Cstring(m_lfDx) + '\n');
	lOutput.push_back(sdf.Float2Cstring(m_lfDy) + '\n');
	lOutput.push_back(sdf.Int2Cstring(m_nRow) + '\n');
	lOutput.push_back(sdf.Int2Cstring(m_nColumn) + '\n');
	lOutput.push_back(sdf.Int2Cstring(m_nScale));

	CString strLine;

	for (int i = 0; i < m_nRow; i++)
	{
		for (int j = 0; j < m_nColumn; j++)
		{
			if (j % 10 == 0)
			{
				strLine += '\n';
				lOutput.push_back(strLine);
				strLine = "";
			}

			if (CheckData(m_pHeight[i][j]))
			{
				strLine += sdf.Int2Cstring(m_pHeight[i][j] * m_nScale) + _T(" ");
			}
			else
				strLine += sdf.Int2Cstring(m_pHeight[i][j]) + _T(" ");
		}
	}
	

	//增加最后一行
	strLine += '\n';
	lOutput.push_back(strLine);
	strLine = "";

	sdf.SaveTXTFile(strDem, lOutput);
	
	return 0;
}

int CDemReader::CreateNewDem(const CString& strDemPath, const double& dStartX, const double& dStartY,
	const int& nRow, const int& nColumn,
	const int& nScale /* = 1000*/, const double& dDx /*= 5.0*/, const double& dDy /*= 5.0*/,
	const CString& strDataMark /*= _T("NSDTF-DEM")*/, const float& fVersion /*= 1.0*/, const CString& strUnit /*= _T("M")*/,
	const float& fAlfa /*= 0.000*/, const float& fBeta /*= 0.0000*/)
{
	std::list<CString> lOutput;
	lOutput.push_back(CString(strDataMark) + '\n');
	lOutput.push_back(sdf.Float2Cstring(fVersion, 1) + '\n');
	lOutput.push_back(CString(m_Unit) + '\n');
	lOutput.push_back(sdf.Float2Cstring(fAlfa, 4) + '\n');
	lOutput.push_back(sdf.Float2Cstring(fBeta, 4) + '\n');
	lOutput.push_back(sdf.Float2Cstring(dStartX) + '\n');
	lOutput.push_back(sdf.Float2Cstring(dStartY) + '\n');
	lOutput.push_back(sdf.Float2Cstring(dDx) + '\n');
	lOutput.push_back(sdf.Float2Cstring(dDy) + '\n');
	lOutput.push_back(sdf.Int2Cstring(nRow) + '\n');
	lOutput.push_back(sdf.Int2Cstring(nColumn) + '\n');
	lOutput.push_back(sdf.Int2Cstring(nScale));

	CString strLine;

	for (int i = 0; i < m_nRow; i++)
	{
		for (int j = 0; j < m_nColumn; j++)
		{
			if (j % 10 == 0)
			{
				strLine += '\n';
				lOutput.push_back(strLine);
				strLine = "";
			}
			strLine += sdf.Int2Cstring(NoData) + _T(" ");
		}
	}

	//增加最后一行
	strLine += '\n';
	lOutput.push_back(strLine);
	strLine = "";

	sdf.SaveTXTFile(strDemPath, lOutput);

	return 0;
}

int CDemReader::CreateDemByPic(const CString& strPicPath)
{
	CGdalUser gu(strPicPath);
	//gu.Initialnize();

	double dStartX = 0;
	double dStartY = 0;
	//在每个像素之间的距离代表1m的时候，不需要通过tfw文件更新参数
	m_nRow = gu.m_nRasterYSize / 5;
	m_nColumn = gu.m_nRasterXSize / 5;

	CString strTfwPath = sdf.GetDirOfFile(strPicPath) + "\\" + sdf.GetNameOfFile(strPicPath, false) + _T(".tfw");
	CString strDemPath = sdf.GetDirOfFile(strPicPath) + "\\" + sdf.GetNameOfFile(strPicPath, false) + _T(".dem");

	if (sdf.IfExistFile(strTfwPath))
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
				image->SetPixelRGB(i, j, dGray, dGray, dGray);
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


