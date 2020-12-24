//author:autumoon
//2019.08.22

#pragma once

#include "StdStrFile.h"

//点的标志位
#define	ORI_G	0x0001		//原来的点
#define	ADD_G	0x0002		//增加的点
#define	KEY_G	0x0004		//关键点（不重复）
#define	CHG_G	0x0008		//移动的点
#define	PRD_G	0x0010		//关键点（重复）
#define	GOO_G	0x0020		//移动的好点，基本上不会再次移动
#define	RND_G	0x0040		//特殊的关键点
#define	NMV_G	0x0080		//暂时不需要移动的点
#define	REL_G	0x0100		//已经保持相对位置的关键点
#define	INT_G	0x0200		//导致自相交的点
#define	REK_G	0x0400		//用于重新走线的关键点，特点是维数在二维或者以上

typedef struct _geo
{
	double x;
	double y;
	int z;
	unsigned int nFlag;

	_geo(){}

	_geo(double x, double y)
	{
		this->x = x;
		this->y = y;
		this->z = 0;
		this->nFlag = ORI_G;
	}

	_geo(double x, double y, int z)
	{
		this->x = x;
		this->y = y;
		this->z = z;
		this->nFlag = ORI_G;
	}

	_geo(double x, double y, int z, int nFlag)
	{
		this->x = x;
		this->y = y;
		this->z = z;
		this->nFlag = nFlag;
	}

	//只需要判断坐标位置是否相等
	bool operator== (const _geo& T)const
	{
		return fabs(x - T.x) < 1e-7 && fabs(y - T.y) < 1e-7;
	}

	bool operator!= (const _geo& T)const
	{
		return fabs(x - T.x) > 1e-7 || fabs(y - T.y) > 1e-7;
	}

}geo;

class CRrlxUser
{
public:
	CRrlxUser(){};
	~CRrlxUser(){m_vGeodetic.clear();};

public:
	_tstring m_stRrlxPath;
	std::vector<geo> m_vGeodetic;

public:
	bool IsClockwise(){return IsGeosClockwise(m_vGeodetic);}

	//面积
	double Area();
	
	int AddPts(int nIndex1, int nIndex2, std::vector<geo>& vGeoAdd){return AddGeoPts(m_vGeodetic, nIndex1, nIndex2, vGeoAdd);};
	int GetIndex(const geo& geoPt, const int& nAfterIndex = -1) const;
	//返回定点的个数
	int IniFromFile(const _tstring& strFilePath){return ReadGeosFromFile(strFilePath, m_vGeodetic);}
	int IniFromList(std::list<_tstring>& lContentInFile, const _tstring& strFilePath){return ReadGeosFromList(lContentInFile, strFilePath, m_vGeodetic);}
	int IniFromPtr(double* pX, double* pY, const int& nVertexNum);
	int IniFromPtr(double* pX, double* pY, int* pZ,const int& nVertexNum);
	//保存到文件
	int Save2Rrlx(const _tstring& strFilePath){return SaveGeos2Rrlx(strFilePath, m_vGeodetic);}
	//内部分配内存，外部记得释放
	int ToPtr(double** pX, double** pY, int* pVertexNum, int**pZ = nullptr);
	size_t Count()const{return m_vGeodetic.size();}

public:
	static bool IsGeosClockwise(std::vector<geo>& vGeodetic);
	static int AddGeoPts(std::vector<geo>& vGeodetic, int nIndex1, int nIndex2, std::vector<geo>& vGeoAdd);
	static int ReadGeosFromFile(const _tstring& strFilePath, std::vector<geo>& vGeodetic);
	static int ReadGeosFromList(std::list<_tstring>& lContentInFile, const _tstring& strFilePath, std::vector<geo>& vGeodetic);
	static int SaveGeos2Rrlx(const _tstring& strFilePath, std::vector<geo>& vGeodetic);
};