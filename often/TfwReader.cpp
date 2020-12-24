/* ******* TfwReader.cpp **********
********* TfwReader常用操作函数实现 ********** */

/* author: autumoon */

#include "TfwReader.h"

CTfwReader::CTfwReader(const _tstring& strTfwPath) :m_strTfwPath(strTfwPath)
{
	ReadData();
}

CTfwReader::CTfwReader(std::vector<_tstring>& vTfwContent)
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
	std::vector<_tstring> vTfwContent;
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

int CTfwReader::ToStringVector(std::vector<_tstring>& arrTfw)
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

int CTfwReader::SaveAsTfwFile( const _tstring& strTfwSavePath )
{
	std::list<_tstring> lContent;
	lContent.push_back(ToString(m_dXResolution) + _T('\n'));
	lContent.push_back(ToString(m_dXRotatePara) + _T('\n'));
	lContent.push_back(ToString(m_dYRotatePara) + _T('\n'));
	lContent.push_back(ToString(m_dYResolution) + _T('\n'));
	lContent.push_back(ToString(m_dXCoordinate) + _T('\n'));
	lContent.push_back(ToString(m_dYCoordinate) + _T('\n'));

	CStdFile::SaveTXTFile(strTfwSavePath, lContent);

	return 0;
}

bool CTfwReader::FindCommonRect(const double& dLDx1, const double& dLDy1, const double& dRTx1, const double& dRTy1,
	const double& dLDx2, const double& dLDy2, const double& dRTx2, const double& dRTy2, double& dLDxC, double& dLDyC, double& dRTxC, double& dRTyC)
{
	dLDxC = dLDx1 > dLDx2 ? dLDx1 : dLDx2;
	dRTyC = dRTy1 < dRTy2 ? dRTy1 : dRTy2;
	dRTxC = dRTx1 < dRTx2 ? dRTx1 : dRTx2;
	dLDyC = dLDy1 > dLDy2 ? dLDy1 : dLDy2;

	if (dLDxC >= dRTxC)
	{
		return false;
	}
	if (dRTyC <= dLDyC)
	{
		return false;
	}

	return true;
}
