//cpp文件
#include "ElapsedTime.h"  
#include <Windows.h>  

CElapsedTime::CElapsedTime()    
{    
	nInitialized = QueryPerformanceFrequency((LARGE_INTEGER*)&Frequency);    
}    

CElapsedTime::~CElapsedTime()    
{    
}    

bool CElapsedTime::Begin()    
{    
	if(!nInitialized)    
		return 0;  
	return QueryPerformanceCounter((LARGE_INTEGER*)&BeginTime) == TRUE;    
}  

bool CElapsedTime::End(int& nMin, int& nSecond, int& nMilliSecond)  
{    
	if(!nInitialized)    
		return false;  
	__int64 endtime;    
	QueryPerformanceCounter((LARGE_INTEGER*)&endtime);    
	__int64 elapsed = endtime - BeginTime;
	
	int nMilliSeconds = int((elapsed/(double)Frequency)*1000.0);  //单位毫秒

	nMin = (nMilliSeconds / 1000) / 60;
	nSecond = (nMilliSeconds / 1000) % 60;
	nMilliSecond = nMilliSeconds % 1000;

	return true;
}   

double CElapsedTime::End()  
{    
	if(!nInitialized)    
		return 0.0;  
	__int64 endtime;    
	QueryPerformanceCounter((LARGE_INTEGER*)&endtime);    
	__int64 elapsed = endtime - BeginTime;

	return ((double)elapsed/(double)Frequency)*1000.0;  //单位毫秒  
}    

bool CElapsedTime::Avaliable()  
{    
	return nInitialized == TRUE;    
} 
