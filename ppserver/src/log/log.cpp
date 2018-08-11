#include "log.h"
#include <stdarg.h>
#include <string.h>
 
//************************************
// Method:    CLog
// FullName:  CLog::CLog
// Access:    private 
// Returns:   
// Qualifier:
// Parameter: void
//************************************
 
CLog::CLog(void)
{
	strcpy(m_logdir, ".");
    m_File = NULL;
	//m_Lock = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_init(&m_Lock, NULL);
}
CLog::CLog(char *logdir)
{

	
	if(logdir){
		int m_logdir_len;
		strcpy(m_logdir, logdir);
		
		m_logdir_len = strlen(m_logdir);
		if(m_logdir[m_logdir_len-1] == '/')
			m_logdir[m_logdir_len-1]='\0';

	}

    m_File = NULL;
	//m_Lock = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_init(&m_Lock, NULL);
}
 
 
//************************************
// Method:    ~CLog
// FullName:  CLog::~CLog
// Access:    private 
// Returns:   
// Qualifier:
// Parameter: void
//************************************
 
 
CLog::~CLog(void)
{
	if(m_File)
		fclose(m_File);
    pthread_mutex_destroy(&m_Lock);
}
 
 
CLog *CLog::m_instance = NULL ;
CLog* CLog::Instance()
{
	if(NULL == m_instance)
	{
		m_instance = new CLog() ;
	}
	
	return m_instance ;
}

CLog* CLog::Instance(char *path)
{
	if(NULL == m_instance)
	{
		m_instance = new CLog(path) ;
	}
	
	return m_instance ;
}
/*
* 名称: Destory
*
* 参数: 
*
* 返回值: 
*
* 说明: 销毁CLog类唯一实例
*/
void  CLog::Destory()
{
	if(NULL != m_instance)
	{
		delete m_instance ;
		m_instance = NULL ;
	}
}

 
//************************************
// Method:    Logf
// FullName:  CLog::Logf
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: const char * lpFormat
// Parameter: ...
//************************************
 
void CLog::Logf(const char * lpFormat, ... )
{   
        //enter the lock
 
        pthread_mutex_lock(&m_Lock);
 
        GetFileFp();
 
        assert(m_File);
 
 
       //DWORD dwWrite = 0;  
 
        // write the time
        fwrite(szContentTitle, sizeof(char), strlen(szContentTitle), m_File);
 
        //::WriteFile(m_File, szContentTitle, strlen(szContentTitle), &dwWrite, NULL);
 
        // write the content
 
        va_list args;
        va_start(args, lpFormat);
        vfprintf(m_File, lpFormat, args);
        va_end(args);
 
        // write the enter under windows
 
        fwrite("\n\r", sizeof(char), 1, m_File);
        //::WriteFile(m_File, "\r\n", 2, &dwWrite, NULL);
        pthread_mutex_unlock(&m_Lock);
        return;
}
#if 0
struct tm { 
int tm_sec; /* 秒 – 取值区间为[0,59] */ 
int tm_min; /* 分 - 取值区间为[0,59] */ 
int tm_hour; /* 时 - 取值区间为[0,23] */ 
int tm_mday; /* 一个月中的日期 - 取值区间为[1,31] */ 
int tm_mon; /* 月份（从一月开始，0代表一月） - 取值区间为[0,11] */ 
int tm_year; /* 年份，其值等于实际年份减去1900 */ 
int tm_wday; /* 星期 – 取值区间为[0,6]，其中0代表星期天，1代表星期一，以此类推 */ 
int tm_yday; /* 从每年的1月1日开始的天数 – 取值区间为[0,365]，其中0代表1月1日，1代表1月2日，以此类推 */ 
int tm_isdst; /* 夏令时标识符，实行夏令时的时候，tm_isdst为正。不实行夏令时的进候，tm_isdst为0；不了解情况时，tm_isdst()为负。*/ 
} 
#endif
FILE * CLog::GetFileFp()
{
	struct tm *local; 
	struct tm *front_local; 
    time_t t; 
    t=time(NULL); 
    t=time(NULL); 
    local=localtime(&t); 
	front_local=localtime(&m_Time);
 
    sprintf(szContentTitle, "%04d:%02d:%02d %02d:%02d:%02d->", local->tm_year+1900, local->tm_mon, local->tm_mday, local->tm_hour, local->tm_min, local->tm_sec);

	if(m_File == NULL)
		goto crate_lable;

    if (local->tm_mday == front_local->tm_mday)
    {
        return m_File;
    }

	if (m_File != NULL)
    {
        fclose(m_File);
    }
	
crate_lable:
    m_Time = t;
 
    char szFileName [512] = {0};
    sprintf(szFileName, "%s/%04d-%02d-%02d.txt", m_logdir, local->tm_year+1900, local->tm_mon, local->tm_mday);
	
    m_File = fopen(szFileName, "a+");
    //m_File = CreateFile(szFileName, GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_ALWAYS , NULL, NULL);
    //SetFilePointer(m_File, 0, NULL, FILE_END);
    printf("create log file %s\n", szFileName);
 
    return m_File;
}



#if 0
#include <iostream>
#include "log.h"
#include <time.h>

 
// initialize here for thread-safe
 
CLog *log = CLog::Instance();
 
// the thread function
 
void * ThreadProc(void * param)
{
    int nFlag = 10000;
 
    while(nFlag-- > 0)
    {
        char szbuffer[256] = {0};
        sprintf(szbuffer, "%s_(HANDLE)_beginthreadex(NULL, 0, ThreadProc, lpThreadOne, NULL, NULL);HANDLE hThreadOne = (HANDLE)_beginthreadex(NULL, 0, ThreadProc, lpThreadOne, NULL, NULL);", (char *)param);
        log->Logf(szbuffer);
        log->Logf("%s_%s", (char *)param, "------------");
    }
    return 0;
}
char * lpThreadOne = "thread_one";
char * lpThreadTwo = "thread_two";
char * lpThreadThr = "thread_thr";
char * lpThreadfou = "thread_fou";
 
int main()
{
 
    // star
 
    time_t t; 
    t=time(NULL); 
 
	pthread_t Tid[4];

 
 	pthread_create(&Tid[0], NULL, ThreadProc, (void *)lpThreadOne);
 	pthread_create(&Tid[1], NULL, ThreadProc, (void *)lpThreadTwo);
 	pthread_create(&Tid[2], NULL, ThreadProc, (void *)lpThreadThr);
 	pthread_create(&Tid[3], NULL, ThreadProc, (void *)lpThreadfou);

	pthread_join(Tid[0], NULL);
	pthread_join(Tid[1], NULL);
	pthread_join(Tid[2], NULL);
	pthread_join(Tid[3], NULL);
     
 
 
    // you can use as this
 
    log->Logf("%s%d", "asd", 8);
 
	printf("time : %d\n", time(NULL)-t);
 
	printf("按任务键结束\n");
    getchar();
    return 0;
}
#endif
