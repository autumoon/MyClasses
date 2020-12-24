//author:autumoon
//2019.08.22

#pragma once

#include "StdStrFile.h"

//��ı�־λ
#define	ORI_G	0x0001		//ԭ���ĵ�
#define	ADD_G	0x0002		//���ӵĵ�
#define	KEY_G	0x0004		//�ؼ��㣨���ظ���
#define	CHG_G	0x0008		//�ƶ��ĵ�
#define	PRD_G	0x0010		//�ؼ��㣨�ظ���
#define	GOO_G	0x0020		//�ƶ��ĺõ㣬�����ϲ����ٴ��ƶ�
#define	RND_G	0x0040		//����Ĺؼ���
#define	NMV_G	0x0080		//��ʱ����Ҫ�ƶ��ĵ�
#define	REL_G	0x0100		//�Ѿ��������λ�õĹؼ���
#define	INT_G	0x0200		//�������ཻ�ĵ�
#define	REK_G	0x0400		//�����������ߵĹؼ��㣬�ص���ά���ڶ�ά��������

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

	//ֻ��Ҫ�ж�����λ���Ƿ����
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

	//���
	double Area();
	
	int AddPts(int nIndex1, int nIndex2, std::vector<geo>& vGeoAdd){return AddGeoPts(m_vGeodetic, nIndex1, nIndex2, vGeoAdd);};
	int GetIndex(const geo& geoPt, const int& nAfterIndex = -1) const;
	//���ض���ĸ���
	int IniFromFile(const _tstring& strFilePath){return ReadGeosFromFile(strFilePath, m_vGeodetic);}
	int IniFromList(std::list<_tstring>& lContentInFile, const _tstring& strFilePath){return ReadGeosFromList(lContentInFile, strFilePath, m_vGeodetic);}
	int IniFromPtr(double* pX, double* pY, const int& nVertexNum);
	int IniFromPtr(double* pX, double* pY, int* pZ,const int& nVertexNum);
	//���浽�ļ�
	int Save2Rrlx(const _tstring& strFilePath){return SaveGeos2Rrlx(strFilePath, m_vGeodetic);}
	//�ڲ������ڴ棬�ⲿ�ǵ��ͷ�
	int ToPtr(double** pX, double** pY, int* pVertexNum, int**pZ = nullptr);
	size_t Count()const{return m_vGeodetic.size();}

public:
	static bool IsGeosClockwise(std::vector<geo>& vGeodetic);
	static int AddGeoPts(std::vector<geo>& vGeodetic, int nIndex1, int nIndex2, std::vector<geo>& vGeoAdd);
	static int ReadGeosFromFile(const _tstring& strFilePath, std::vector<geo>& vGeodetic);
	static int ReadGeosFromList(std::list<_tstring>& lContentInFile, const _tstring& strFilePath, std::vector<geo>& vGeodetic);
	static int SaveGeos2Rrlx(const _tstring& strFilePath, std::vector<geo>& vGeodetic);
};