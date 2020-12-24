/* ******* TfwReader.h **********
********* TfwReader常用操作函数声明 ********** */

/* author: autumoon */

#pragma once
#include "StdStrFile.h"

class CTfwReader
{
public:
	CTfwReader(const _tstring& strTfwPath);
	CTfwReader(std::vector<_tstring>& vTfwContent);
	CTfwReader(const double& dResolution, const double& dXCoordinate, const double& dYCoordinate);
	~CTfwReader();

public:
	_tstring m_strTfwPath;

	double m_dXResolution;
	double m_dXRotatePara;
	double m_dYRotatePara;
	double m_dYResolution;
	double m_dXCoordinate;
	double m_dYCoordinate;

public:
	int CalcGeoCoordinate(const int& nX, const int& nY, double& dX, double& dY);
	int CalcPixCoordinate(const double& dX, const double& dY, int& nX, int& nY, const bool& bNonnegative = true);
	int ReadData();
	int SaveAsTfwFile(const _tstring& strTfwSavePath);
	int ToStringVector(std::vector<_tstring>& arrTfw);

	static bool FindCommonRect(const double& dLDx1, const double& dLDy1, const double& dRTx1, const double& dRTy1,
		const double& dLDx2, const double& dLDy2, const double& dRTx2, const double& dRTy2, double& dLDxC, double& dLDyC, double& dRTxC, double& dRTyC);
};
