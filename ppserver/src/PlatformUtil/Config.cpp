#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "Config.h"


static int g_Config_debug_switch = 1;
#define G_Config_DEBUG(format,...) \
if (g_Config_debug_switch ){                                    \
	printf(format, ##__VA_ARGS__);\
}

void switch_to_s(char *src)
{
	char *p=src;
	if(p == NULL)return;
	p=strchr(p, '-');
	while(p){
		*p='_';
		p=strchr(p, '-');
	}
	return ;
}

int show_config_info()
{
    G_Config_DEBUG("-- %s\n", __func__);
	ConfNode *child1 = NULL;
	ConfNode *child2 = NULL;
	ConfNode *child3 = NULL;
    	ConfNode *root = ConfGetRootNode();

	TAILQ_FOREACH(child1, &root->head, next) 
	{
		G_Config_DEBUG("A %s:%s\n",child1->name, child1->val);
		
		TAILQ_FOREACH(child2, &child1->head, next) 
		{
			G_Config_DEBUG("B \t%s:%s\n",child2->name, child2->val);
			TAILQ_FOREACH(child3, &child2->head, next) 
			{
				G_Config_DEBUG("C \t\t%s:%s\n",child3->name, child3->val);
				
			}
		}
	}
	return 0;
}

//return group_number:sucess -1:fail
int CConfig::ConfGetChildNumber(const char *name)
{
	ConfNode *child = NULL;
	ConfNode *node = NULL;
	int count = 0;

	node = ConfGetNode(name);
	TAILQ_FOREACH(child, &node->head, next) 
	{
		count++;
	}

	return count;
}
int CConfig::getDBConfig()
{
	ConfNode *child = NULL;
	ConfNode *node = NULL;


	DBHost[32];
	DBUser[32];
	DBPassWord[32];
	DBName[32];
	DBPort=0;
	DBSocket[32]='\0';
	DBClientFlag=0;

	char *val=NULL;
	
      G_Config_DEBUG("\n\n-- %s\n", __func__);
	node = ConfGetNode("DBServer");
	//TAILQ_FOREACH(child, &node->head, next) 
	{
		ConfGetChildValue(node, (char *)"DBHost", &val);
		if(val == NULL){
			G_Config_DEBUG("*************** DBHost is error\n");
			return -1;
		}
		strcpy(DBHost, val);

		ConfGetChildValue(node, (char *)"DBUser", &val);
		if(val == NULL){
			G_Config_DEBUG("*************** DBUser is error\n");
			return -1;
		}
		strcpy(DBUser, val);
		switch_to_s(DBUser);
		
		ConfGetChildValue(node, (char *)"DBPassWord", &val);
		if(val == NULL){
			G_Config_DEBUG("*************** DBPassWord is error\n");
			return -1;
		}
		strcpy(DBPassWord, val);
		
		ConfGetChildValue(node, (char *)"DBName", &val);
		if(val == NULL){
			G_Config_DEBUG("*************** DBName is error\n");
			return -1;
		}
		strcpy(DBName, val);
	
		ConfGetChildValue(node, (char *)"DBPort", &val);
		if(val != NULL){
			DBPort = atoi(val);
		}

		ConfGetChildValue(node, (char *)"DBSocket", &val);
		if(val != NULL){
			strcpy(DBSocket, val);
		}
	}

	G_Config_DEBUG("DBHost                       =%s\n", DBHost);
	G_Config_DEBUG("DBUser                       =%s\n", DBUser);
	G_Config_DEBUG("DBPasswd                     =%s\n", DBPassWord);
	G_Config_DEBUG("DBName                       =%s\n", DBName);
	G_Config_DEBUG("DBPort                       =%u\n", DBPort);
	G_Config_DEBUG("DBSocket                     =%s\n\n", DBSocket);
	
	return 0;	

}

int CConfig::getSocketConfig()
{
	ConfNode *child = NULL;
	ConfNode *node = NULL;
      G_Config_DEBUG("\n\n-- %s\n", __func__);

	node = ConfGetNode("TimeOut");
	timeOut= 1;
	if(node != NULL) timeOut= atoi(node->val);
	else G_Config_DEBUG("*************** TimeOut default 1\n");
		
	

	rootUrl[0]='\0';
	node = ConfGetNode("RootURL");
	if(node!= NULL)strcpy(rootUrl, node->val);
	else {
		G_Config_DEBUG("*************** RootURL is error\n");
		return -1;
	}

	node = ConfGetNode("UrlFilter");
	if(node!= NULL)str_split_func(&urlFilter, node->val, '|');
	else {
		G_Config_DEBUG("*************** UrlFilter is error\n");
		return -1;
	}

	
	nMaxConnNum= 1 ;
	node = ConfGetNode("MaxConnNum");
	if(node!= NULL)nMaxConnNum = atoi(node->val);
	

	G_Config_DEBUG("TimeOut        =%d\n", timeOut);
	G_Config_DEBUG("RootURL        =%s\n", rootUrl);
	G_Config_DEBUG("MaxConnNum     =%d\n", nMaxConnNum);

	return 0;	

}
int CConfig::getSystemConfig()
{
	ConfNode *child = NULL;
	ConfNode *node = NULL;
	char *val=NULL;

      G_Config_DEBUG("\n\n-- %s\n", __func__);
	
	node = ConfGetNode("SYSTEM");
	//TAILQ_FOREACH(child, &node->head, next) 
	{
		ConfGetChildValue(node, (char *)"LOGDIR", &val);
		if(val == NULL){
			G_Config_DEBUG("*************** LOGDIR is error\n");
			return -1;
		}
		strcpy(LOGDIR, val);
		
		ConfGetChildValue(node, (char *)"OUTPUTDIR", &val);
		if(val == NULL){
			G_Config_DEBUG("*************** OUTPUTDIR is error\n");
			return -1;
		}
		strcpy(OUTPUTDIR, val);
		
		ConfGetChildValue(node, (char *)"OUTPUT-FINISH-FLAG", &val);
		if(val == NULL){
			G_Config_DEBUG("*************** OUTPUT-FINISH-FLAG is error\n");
			return -1;
		}
		strcpy(OUTPUTDIR_FINISH_FLAG, val);

		
	}
	G_Config_DEBUG("LOGDIR                               =%s\n", LOGDIR);
	G_Config_DEBUG("OUTPUTDIR                            =%s\n", OUTPUTDIR);

	return 0;	

}

/*
	返回值0成功；-1失败
*/
int CConfig::cfg_init()
{

	ConfNode *child = NULL;
	ConfNode *node = NULL;
	int ret;

	ret =getDBConfig();
	if(ret)return -1;


	ret =getSocketConfig();
	if(ret)return -1;

	ret =getSystemConfig();
	if(ret)return -1;

	
	return 0;
}


CConfig* CConfig::m_instance = NULL;

CConfig::CConfig()
{
	ConfInit();
	if (ConfYamlLoadFile("./conf.yaml") != 0){
		CConfigInitResault = -1;
		printf( "Invalid format of configuration files [./conf.yaml], please check\n");
		printf( "NOTE:　configuration file Don't make the 'TAB' and the '_'\n");
	}else{
		CConfigInitResault = cfg_init();
		if( CConfigInitResault != 0){
		printf( "Invalid format of configuration files [./conf.yaml], please check\n");
			printf( "NOTE:　configuration file Don't make the 'TAB' and the '_'\n");
		}
	}

	//show_config_info();

	
	ConfDeInit();
}


CConfig::CConfig(char *CPath)
{


	ConfInit();
	if (ConfYamlLoadFile(CPath) != 0){
		CConfigInitResault = -1;
		printf( "Invalid format of configuration files [%s], please check\n", CPath);
		printf( "NOTE:　configuration file Don't make the 'TAB' and the '_'\n");
	}else{
		CConfigInitResault = cfg_init();
		if( CConfigInitResault != 0){
		printf( "Invalid format of configuration files [%s], please check\n", CPath);
			printf( "NOTE:　configuration file Don't make the 'TAB' and the '_'\n");
		}
	}

	//show_config_info();

	
	ConfDeInit();



}

CConfig::~CConfig()
{
	str_split_free(&urlFilter);

}


CConfig* CConfig::Instance()
{
	if( NULL == m_instance )
	{
		m_instance = new CConfig(NULL);
	}
	
	return m_instance ;

}

CConfig* CConfig::Instance(char *name)
{
	if( NULL == m_instance )
	{
		m_instance = new CConfig(name);
	}
	
	return m_instance ;

}
void  CConfig::Destory()
{
	if(NULL != m_instance)
	{
		delete m_instance ;
		m_instance = NULL ;
	}
}


