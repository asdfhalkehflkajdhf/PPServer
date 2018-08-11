#ifndef LOG_H__
#define LOG_H__
 
 
 
#include <assert.h>
#include <stdio.h>
#include <pthread.h>
#include <time.h>

 
#define CONTENT_TITLE 32
 

 
class CLog
{
 
private:
 
    // constructor
 
    CLog(void);
    CLog (char *logdir);
 
 
    //destructor
 
    ~CLog(void);
 
 
 
    //overloaded 
 
    const CLog & operator = (const CLog &);
 	static CLog* m_instance ;

private:
 
	pthread_mutex_t m_Lock;
 
     
    // file descriptor
 
    FILE * m_File;
 
 
    //create time
 
    time_t m_Time;
 
     
    // content title
 
    char szContentTitle[CONTENT_TITLE];

 	char m_logdir[256];
 
private:
     
     
    // get file handle
 
    FILE * GetFileFp();
 
 
public:
	/*
	* 名称: Instance
	*
	* 参数: 
	*
	*/

 	static	CLog* Instance() ;
 	static	CLog* Instance(char *path) ;

	/*
	* 名称: Destory
	*
	* 参数: 
	*
	* 返回值: 
	*
	* 说明: 销毁类唯一实例
	*/
	static	void  Destory() ;
 
    // log a formate string
 
    void Logf(const char * lpFormat, ...);
 
 
};
 
 
 
#endif // Log_h__
