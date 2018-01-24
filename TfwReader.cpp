/* ******* TfwReader.cpp **********
********* TfwReader常用操作函数实现 ********** */

/* author: autumoon */

#include "TfwReader.h"

CTfwReader::CTfwReader(const std::string& strTfwPath) :m_strTfwPath(strTfwPath)
{
	ReadData();
}

CTfwReader::CTfwReader(std::vector<std::string>& vTfwContent)
{
	if (vTfwContent.size() < 6)
	{
		return;
	}
	CStdTpl::ConvertFromString(m_dXResolution, vTfwContent[0]);
	CStdTpl::ConvertFromString(m_dXRotatePara, vTfwContent[1]);
	CStdTpl::ConvertFromString(m_dYRotatePara, vTfwContent[2]);
	CStdTpl::ConvertFromString(m_dYResolution, vTfwContent[3]);
	CStdTpl::ConvertFromString(m_dXCoordinate, vTfwContent[4]);
	CStdTpl::ConvertFromString(m_dYCoordinate, vTfwContent[5]);
}

CTfwReader::CTfwReader(const double& dResolution, const double& dXCoordinate, const double& dYCoordinate)
{
	m_dXResolution = dResolution;
	m_dXRotatePara = 0.0;
	m_dYRotatePara = 0.0;
	m_dYResolution = -dResolution;
	m_dXCoordinate = dXCoordinate;
	m_dYCoordinate = dYCoordinate;
}

CTfwReader::~CTfwReader()
{

}

int CTfwReader::ReadData()
{
	std::vector<std::string> vTfwContent;
	CStdFile::ParseTXTFile(m_strTfwPath, vTfwContent);
	if (vTfwContent.size() < 6)
	{
		return -1;
	}

	CStdTpl::ConvertFromString(m_dXResolution, vTfwContent[0]);
	CStdTpl::ConvertFromString(m_dXRotatePara, vTfwContent[1]);
	CStdTpl::ConvertFromString(m_dYRotatePara, vTfwContent[2]);
	CStdTpl::ConvertFromString(m_dYResolution, vTfwContent[3]);
	CStdTpl::ConvertFromString(m_dXCoordinate, vTfwContent[4]);
	CStdTpl::ConvertFromString(m_dYCoordinate, vTfwContent[5]);

	return 0;
}

int CTfwReader::ToStringVector(std::vector<std::string>& arrTfw)
{
	arrTfw.clear();
	
	arrTfw.push_back(ToString(m_dXResolution));
	arrTfw.push_back(ToString(m_dXRotatePara));
	arrTfw.push_back(ToString(m_dYRotatePara));
	arrTfw.push_back(ToString(m_dYResolution));
	arrTfw.push_back(ToString(m_dXCoordinate));
	arrTfw.push_back(ToString(m_dYCoordinate));

	return 0;
}

int CTfwReader::CalcGeoCoordinate(const int& nX, const int& nY, double& dX, double& dY)
{
	dX = m_dXResolution * nX + nY * m_dYRotatePara + m_dXCoordinate;
	dY = m_dYResolution * nY + nX * m_dXRotatePara + m_dYCoordinate;

	return 0;
}

int CTfwReader::CalcPixCoordinate(const double& dX, const double& dY, int& nX, int& nY, const bool& bNonnegative/* = true*/)
{
	//为了简化计算，此时并没有考虑旋转的问题
	nX = int((dX - m_dXCoordinate) / m_dXResolution + 0.5);
	nY = int((dY - m_dYCoordinate) / m_dYResolution + 0.5);
	
	if (bNonnegative && nX < 0)
	{
		nX = 0;
	}

	if (bNonnegative && nY < 0)
	{
		nY = 0;
	}

	return 0;
}

int CTfwReader::SaveAsTfwFile( const std::string& strTfwSavePath )
{
	std::list<std::string> lContent;
	lContent.push_back(ToString(m_dXResolution) + '\n');
	lContent.push_back(ToString(m_dXRotatePara) + '\n');
	lContent.push_back(ToString(m_dYRotatePara) + '\n');
	lContent.push_back(ToString(m_dYResolution) + '\n');
	lContent.push_back(ToString(m_dXCoordinate) + '\n');
	lContent.push_back(ToString(m_dYCoordinate) + '\n');

	CStdFile::SaveTXTFile(strTfwSavePath, lContent);

	return 0;
}

