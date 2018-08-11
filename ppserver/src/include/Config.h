/*
* Copyright(c) 2008,  
* All rights reserved
*
* �ļ�����:  Config.h
* ժ    Ҫ:  ϵͳ������CConfigͷ�ļ�
*
* ��ǰ�汾:  1.0
* ��    ��:  
* �������:  2008��10��19��
*/

#if !defined(PAYCENTER_CONFIG_H__INCLUDED_)
#define PAYCENTER_CONFIG_H__INCLUDED_


#include <stdio.h>
#include "configPublic.h"
#include "utils.h"

using namespace std ;

#define NOT_FOUND                   0x80000000
#define TRUCK_CODE_NUM_MAX 1000
#define VERSION_NUM_MAX 100


class  CConfig  
{
private:
	CConfig();
	CConfig(char *CPath);
	virtual ~CConfig() ;

	static CConfig* m_instance ;
public:

	int CConfigInitResault;


//*****************************socket
  struct str_split urlFilter;
	char rootUrl[256];
	int nMaxConnNum ;
//��λ��
	int timeOut;
	int getSocketConfig();
//***********************************************

//************************���ݿ�����
	char DBHost[32];
	char DBUser[32];
	char DBPassWord[32];
	char DBName[32];
	unsigned int DBPort;
	char DBSocket[32];
	unsigned long DBClientFlag;
	int getDBConfig();
//********************************************




//*******************SYSTEM:
	FILE * log_fd;
	char LOGDIR[256];
	char OUTPUTDIR[256];
	char OUTPUTDIR_FINISH_FLAG[15];
	int getSystemConfig();
//&*********************************************


	static CConfig* Instance();
	static CConfig* Instance(char *name);
	int cfg_init();
	void  Destory();
	int ConfGetChildNumber(const char *name);
};

extern CConfig *GConfig;


#endif // !defined(PAYCENTER_CONFIG_H__INCLUDED_)
