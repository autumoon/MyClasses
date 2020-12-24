#pragma once 

/* ������������ʱ����� */
class CElapsedTime      
{
public:
	CElapsedTime();
	virtual ~CElapsedTime();      
public:    
	bool Avaliable();
	bool Begin();
	bool End(int& nMin, int& nSecond, int& nMilliSecond);
	double End();

private:    
	int nInitialized;
	__int64 Frequency;
	__int64 BeginTime;

};