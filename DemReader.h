/* ******* DemReader.h **********
********* DemReader常用操作函数声明 ********** */

/* author: autumoon */

#pragma once
#include "StdStrFile.h"
#include "MfcStrFile.h"
#include "gdalUser.h"
#include <atlimage.h>

#define NoData -99999

typedef struct tagDEMHEADERINFO
{
	int nType;//类型 给0 
	double lfStartX;//起点 
	double lfStartY;// 
	double lfDx;//采样间隔 
	double lfDy;// 
	int nRow;//行 
	int nColumn;//列 
	double lfKapa;//旋转角，给0 
	int nProjection;//投影，给0 
	double lfHeight;//平均高度？ 
	double lfNoData;//无效数 
	int nScale;//缩放比 
	int nDot;//小数点位数 
	int nPixelSize;//占用的字节数 
}DEMHEADERINFO, *PDEMHEADERINFO;

class CDemReader
{
public:
	// 在文件头后面开始真实的高程值，浮点形式。
	//写入的顺序和文本格式的相同。
	CDemReader(const CString& strDem);
	~CDemReader();

private:
	const CString m_strDemPath;

public:
	char* m_DataMark;
	float m_Version;
	char* m_Unit;
	double m_Alfa;
	double m_Beta;
	double m_lfStartX;
	double m_lfStartY;
	double m_lfDx;
	double m_lfDy;
	int m_nRow;
	int m_nColumn;
	int m_nScale;
	double** m_pHeight;

public:
	inline bool CheckData(const double& dData);

	int ChangeAreaHeight(const int& nXoffset, const int& nYOffset, const int& nWidth, const int& nHeight, const double& dNewHeight);
	int CreateBmpByDem(const CString& strDemPath, const CString& strBmpPath);
	int CreateDemByPic(const CString& strPicPath);
	int CreateNewDem(const CString& strDemPath, const double& m_lfStartX, const double& m_lfStartY,
		const int& nRow, const int& nColumn,
		const int& nScale = 1000, const double& m_lfDx = 5.0, const double& m_lfDy = 5.0,
		const CString& strDataMark = _T("NSDTF-DEM"), const float& fVersion = 1.0, const CString& strUnit = _T("M"),
		const float& fAlfa = 0.000, const float& fBeta = 0.0000);
	int ReadDemData();
	int SaveDemData(const CString& strDem);
};
