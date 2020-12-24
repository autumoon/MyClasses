/* ******* DemReader.h **********
********* DemReader���ò����������� ********** */

/* author: autumoon */

#pragma once
#include "StdStrFile.h"
#include "MfcStrFile.h"
#include "gdalUser.h"
#include <atlimage.h>

#define NoData -99999

typedef struct tagDEMHEADERINFO
{
	int nType;//���� ��0 
	double lfStartX;//��� 
	double lfStartY;// 
	double lfDx;//������� 
	double lfDy;// 
	int nRow;//�� 
	int nColumn;//�� 
	double lfKapa;//��ת�ǣ���0 
	int nProjection;//ͶӰ����0 
	double lfHeight;//ƽ���߶ȣ� 
	double lfNoData;//��Ч�� 
	int nScale;//���ű� 
	int nDot;//С����λ�� 
	int nPixelSize;//ռ�õ��ֽ��� 
}DEMHEADERINFO, *PDEMHEADERINFO;

class CDemReader
{
public:
	// ���ļ�ͷ���濪ʼ��ʵ�ĸ߳�ֵ��������ʽ��
	//д���˳����ı���ʽ����ͬ��
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
