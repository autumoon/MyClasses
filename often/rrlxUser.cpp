#include "rrlxUser.h"


int CRrlxUser::IniFromPtr(double* pX, double* pY, const int& nVertexNum)
{
	if (pX == nullptr || pY == nullptr || nVertexNum <= 0)
	{
		return 0;
	}

	m_vGeodetic.clear();

	for (int i = 0; i < nVertexNum; ++i)
	{
		m_vGeodetic.push_back(geo(pX[i], pY[i]));
	}

	return nVertexNum;
}

int CRrlxUser::IniFromPtr(double* pX, double* pY, int* pZ,const int& nVertexNum)
{
	if (pX == nullptr || pY == nullptr || pZ == nullptr || nVertexNum <= 0)
	{
		return 0;
	}

	m_vGeodetic.clear();

	for (int i = 0; i < nVertexNum; ++i)
	{
		m_vGeodetic.push_back(geo(pX[i], pY[i], pZ[i]));
	}

	return nVertexNum;
}

double CRrlxUser::Area()
{
	size_t n = Count();
	if (n < 3)
	{
		return 0.0;
	}

	double ans = m_vGeodetic[n - 1].x * m_vGeodetic[0].y - m_vGeodetic[n - 1].y * m_vGeodetic[0].x;
	for(int i=0;i<n-1;++i)
		ans += m_vGeodetic[i].x * m_vGeodetic[i + 1].y - m_vGeodetic[i].y * m_vGeodetic[i + 1].x;

	return abs(ans)*1.0/2.0;
}

int CRrlxUser::GetIndex(const geo& geoPt, const int& nAfterIndex /*= -1*/) const
{
	size_t nPtCount = m_vGeodetic.size();
	for (int i = nAfterIndex + 1; i >= 0 && i < nPtCount; ++i)
	{
		if (m_vGeodetic[i] == geoPt)
		{
			return i;
		}
	}

	return -1;
}

int CRrlxUser::ToPtr(double** pX, double** pY, int* pVertexNum, int**pZ /*= nullptr*/)
{
	if (pX == nullptr || pY == nullptr || pVertexNum == nullptr)
	{
		return -1;
	}

	size_t nPtCount = m_vGeodetic.size();
	*pX = new double[nPtCount]();
	*pY = new double[nPtCount]();
	//按需要输出
	if (pZ)
	{
		*pZ = new int[nPtCount]();
	}

	*pVertexNum = nPtCount;

	if (pZ)
	{
		for (size_t i = 0; i < nPtCount; ++i)
		{
			(*pX)[i] = m_vGeodetic[i].x;
			(*pY)[i] = m_vGeodetic[i].y;
			(*pZ)[i] = m_vGeodetic[i].z;
		}
	}
	else
	{
		for (size_t i = 0; i < nPtCount; ++i)
		{
			(*pX)[i] = m_vGeodetic[i].x;
			(*pY)[i] = m_vGeodetic[i].y;
		}
	}

	return nPtCount;
}

bool CRrlxUser::IsGeosClockwise(std::vector<geo>& vGeodetic)
{
	//沿着多边形的边求曲线积分,若积分为正,则是沿着边界曲线正方向(逆时针),反之为顺时针
	double d = 0.0;
	const size_t nSize = vGeodetic.size();

	if (nSize == 0)
	{
		return false;
	}

	for (int i = 0; i < nSize - 1; ++i)
	{
		d += (vGeodetic[i + 1].y + vGeodetic[i].y)*(vGeodetic[i + 1].x - vGeodetic[i].x);
	}

	d += (vGeodetic[0].y + vGeodetic[nSize - 1].y)*(vGeodetic[0].x - vGeodetic[nSize - 1].x);

	return -0.5 * d < 0.0;
}

int CRrlxUser::AddGeoPts(std::vector<geo>& vGeodetic, int nIndex1, int nIndex2, std::vector<geo>& vGeoAdd)
{
	size_t nOldCount = vGeodetic.size();

	//排除错误情况
	if (nIndex1 < 0 || nIndex1 >= nOldCount || nIndex2 < 0 || nIndex2 >= nOldCount || nIndex1 == nIndex2)
	{
		return -1;
	}

	const geo gStart = vGeodetic[nIndex1];
	const geo gEnd = vGeodetic[nIndex2];

	//实际要添加的点，可能需要去掉关键点
	std::vector<geo> vGeos;
	bool bStart = false;
	for (size_t i = 0; i < nOldCount; ++i)
	{
		geo gCur = vGeoAdd[i];

		if (bStart)
		{
			if (gCur != gEnd)
			{
				vGeos.push_back(gCur);
			}
			else
			{
				bStart = false;
				break;
			}
		}
		else if (!bStart && gCur != gStart)
		{
			bStart = true;
			vGeos.push_back(gCur);
		}
	}

	//将实际需要添加的点添加到指定位置
	if (nIndex1 < nIndex2)
	{
		//此时保留了关键点n1和n2
		vGeodetic.erase(vGeodetic.begin() + nIndex1 + 1, vGeodetic.begin() + nIndex2);
		vGeodetic.insert(vGeodetic.begin() + nIndex1 + 1, vGeos.begin(), vGeos.end());
	}
	else
	{
		vGeodetic.erase(vGeodetic.begin() + nIndex1 + 1, vGeodetic.end());
		vGeodetic.erase(vGeodetic.begin(), vGeodetic.begin() + nIndex2);
		vGeodetic.insert(vGeodetic.end(), vGeos.begin(), vGeos.end());
	}

	return 0;
}

int CRrlxUser::ReadGeosFromFile(const _tstring& strFilePath, std::vector<geo>& vGeodetic)
{
	std::list<_tstring> lConetentInFile;
	CStdFile::ParseTXTFile(strFilePath, lConetentInFile);

	return ReadGeosFromList(lConetentInFile, strFilePath, vGeodetic);
}

int CRrlxUser::ReadGeosFromList(std::list<_tstring>& lContentInFile, const _tstring& strFilePath, std::vector<geo>& vGeodetic)
{
	if (lContentInFile.size() == 0)
	{
		return -1;
	}

	std::list<_tstring>::iterator it_c = lContentInFile.begin();
	int nSize = 0;
	CStdTpl::ConvertFromString(nSize, *it_c);
	++it_c;
	int nIndex = 0;
	for (; it_c != lContentInFile.end() && nIndex < nSize; ++it_c)
	{
		geo g;
		std::string coor = CStdStr::ws2s(*it_c);
		sscanf_s(coor.c_str(), "%lf%lf%d", &g.x, &g.y, &g.z);
		vGeodetic.push_back(g);
		++nIndex;
	}

	//如果存在相邻的点重合，则去掉其中一个点，注意首尾也可能重合
	for (size_t i = 0; i < vGeodetic.size() - 1; ++i)
	{
		if (vGeodetic[i] == vGeodetic[i + 1])
		{
			vGeodetic.erase(vGeodetic.begin() + i--);
		}
	}

	//如果首尾相同，则需要去掉首个
	if (vGeodetic.size() > 1 && *vGeodetic.begin() == *vGeodetic.rbegin())
	{
		vGeodetic.erase(vGeodetic.begin());
	}

	return (int)vGeodetic.size();
}

int CRrlxUser::SaveGeos2Rrlx(const _tstring& strFilePath, std::vector<geo>& vGeodetic)
{
	std::vector<_tstring> arrFileContent;
	arrFileContent.push_back(ToString(static_cast<int>(vGeodetic.size())) + _T("\n"));
	for (int i = 0; i < vGeodetic.size(); ++i)
	{
		arrFileContent.push_back(ToString(vGeodetic[i].x) + _T("   ") + ToString(vGeodetic[i].y) + _T("   ") + ToString(vGeodetic[i].z) + _T("\n"));
	}
	CStdFile::SaveTXTFile(strFilePath, arrFileContent);

	return 0;
}