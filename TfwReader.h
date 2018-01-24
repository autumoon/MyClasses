/* ******* TfwReader.h **********
********* TfwReader常用操作函数声明 ********** */

/* author: autumoon */

#pragma once
#include "StdStrFile.h"

class CTfwReader
{
public:
	CTfwReader(const std::string& strTfwPath);
	CTfwReader(std::vector<std::string>& vTfwContent);
	CTfwReader(const double& dResolution, const double& dXCoordinate, const double& dYCoordinate);
	~CTfwReader();

public:
	std::string m_strTfwPath;

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
	int SaveAsTfwFile(const std::string& strTfwSavePath);
	int ToStringVector(std::vector<std::string>& arrTfw);
};
