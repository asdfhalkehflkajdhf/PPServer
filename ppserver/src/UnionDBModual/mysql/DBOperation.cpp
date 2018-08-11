/*
* Copyright(c) 2008, 北京一鸣神州科技有限公司 
* All rights reserved
*
* 文件名称:  DBOperation.cpp
* 摘    要:  CDBOperation实现
*
* 当前版本:  1.0
* 作    者:  姜福来
* 完成日期:  2008年10月21日
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>
#include <mysql/mysql.h>

#include "Config.h"
#include "DBOperation.h"


static int g_db_debug_switch = 1;
#define G_DB_DEBUG(format,...) \
if (g_db_debug_switch ){                                    \
	printf(format, ##__VA_ARGS__);\
}


int mysql_get_field_key_val(MYSQL_RES *res, MYSQL_ROW row, const char *fieldname, int *val);
int mysql_get_field_key_val(MYSQL_RES *res, MYSQL_ROW row, const char *fieldname, long *val);
int mysql_get_field_key_val(MYSQL_RES *res, MYSQL_ROW row, const char *fieldname, long long *val);
int mysql_get_field_key_val(MYSQL_RES *res, MYSQL_ROW row, const char *fieldname, char *val);
int mysql_get_field_key_val(MYSQL_RES *res, MYSQL_ROW row, const char *fieldname, char *val, int val_len);

//获取执行sql语句修改行数
int mysql_query_get_res(MYSQL *DB, const char *sql, int *res);
int mysql_query_get_res(MYSQL *DB, const char *sql);
//获取执行sql语句结果集
int mysql_query_get_res(MYSQL *DB, const char *sql, MYSQL_RES **res);

int mysql_query_free_res(MYSQL *DB, MYSQL_RES *res);
int mysql_query_free_res(MYSQL *DB);



CDBOperation* CDBOperation::m_instance = NULL;

CDBOperation::CDBOperation()
{
	CConfig* pConfig = CConfig::Instance() ;
	OpenDB();
}
CDBOperation* CDBOperation::Instance()
{
	if( NULL == m_instance )
	{
		m_instance = new CDBOperation();
	}
	
	return m_instance ;

}

void  CDBOperation::Destory()
{
	if(NULL != m_instance)
	{
		delete m_instance ;
		m_instance = NULL ;
	}
}

CDBOperation::~CDBOperation()
{
	if(m_pDBAccess != NULL)
	{
		CloseDB() ;
	}
}

bool CDBOperation::OpenDB()
{
	CConfig* pConfig = CConfig::Instance() ;

	survival_time = (int )time(NULL);
	
	m_pDBAccess =(void *)mysql_init(NULL);
	if (NULL == m_pDBAccess) {  
G_DB_DEBUG("%d | %d | open db mysql_init fail %s %s %s\n",(unsigned int)pthread_self(), __LINE__,  pConfig->DBHost, pConfig->DBUser, pConfig->DBName);
		return false;
	}  
	//开启mysql多线程时mysql_library_init必须执行对应在关闭时执行mysql_library_end
	if (mysql_thread_init()) {  
G_DB_DEBUG("%d | %d | open db mysql_library_init fail %s %s %s\n",(unsigned int)pthread_self(), __LINE__,  pConfig->DBHost, pConfig->DBUser, pConfig->DBName);
		return false;
	}  

	if(!mysql_real_connect((MYSQL *)m_pDBAccess, pConfig->DBHost, pConfig->DBUser, pConfig->DBPassWord, pConfig->DBName, pConfig->DBPort, NULL, CLIENT_MULTI_STATEMENTS)){ 
		G_DB_DEBUG( (char *) "连接数据库[%s to %s]失败，错误原因是:%s", pConfig->DBName, pConfig->DBHost, mysql_error((MYSQL *)m_pDBAccess));
G_DB_DEBUG("%d | %d | open db mysql_real_connect fail %s %s %s\n",(unsigned int)pthread_self(), __LINE__,  pConfig->DBHost, pConfig->DBUser, pConfig->DBName);
		return false;
	} else { 
		mysql_autocommit((MYSQL *)m_pDBAccess, 0);
		if (mysql_query_get_res((MYSQL *)m_pDBAccess, "set NAMES gbk")) // 如果失败
		{
			G_DB_DEBUG( (char *)"连接数据库失败，错误原因是:%s", mysql_error((MYSQL *)m_pDBAccess));
			return false;
		}
		char value = 1; 
		mysql_options((MYSQL *)m_pDBAccess, MYSQL_OPT_RECONNECT, (char*)&value); 
G_DB_DEBUG("%d | %d | open db ok %s %s %s\n",(unsigned int)pthread_self(), __LINE__,  pConfig->DBHost, pConfig->DBUser, pConfig->DBName);
		return true; 

	}
}
bool CDBOperation::OpenDB( const char *host, const char *user, const char *passwd, const char *db, unsigned int port, const char *unix_socket)
{
	m_pDBAccess =(void *)mysql_init(NULL);
	if (NULL == m_pDBAccess) {  
		G_DB_DEBUG( (char *) "连接数据库init 失败");
G_DB_DEBUG("%d | %d | open db mysql_init fail %s %s %s\n",(unsigned int)pthread_self(), __LINE__, host, user, db);
		return false;
	}  
	//开启mysql多线程时mysql_library_init必须执行对应在关闭时执行mysql_library_end
	if (mysql_thread_init()) {  
		G_DB_DEBUG( (char *) "连接数据库could not initialize MySQL library 失败");
G_DB_DEBUG("%d | %d | open db mysql_library_init fail %s %s %s\n",(unsigned int)pthread_self(), __LINE__, host, user, db);
		return false;
	}  

	if(!mysql_real_connect((MYSQL *)m_pDBAccess, host, user, passwd, db, port, NULL, CLIENT_MULTI_STATEMENTS)){ 
		G_DB_DEBUG( (char *) "Failed to connect to mysql database %s on %s.\n", db, host); 
G_DB_DEBUG("%d | %d | open db mysql_real_connect fail %s %s %s\n",(unsigned int)pthread_self(), __LINE__, host, user, db);
		return false;
	} else { 
		mysql_autocommit((MYSQL *)m_pDBAccess, 0);
		if (mysql_query_get_res((MYSQL *)m_pDBAccess, "set NAMES gbk")) // 如果失败
		{
			G_DB_DEBUG( (char *)"连接数据库失败，错误原因是:%s", mysql_error((MYSQL *)m_pDBAccess));
			return false;
		}
		char value = 1; 
		mysql_options((MYSQL *)m_pDBAccess, MYSQL_OPT_RECONNECT, (char*)&value); 
G_DB_DEBUG("%d | %d | open db ok %s %s %s\n",(unsigned int)pthread_self(), __LINE__, host, user, db);
		return true; 
	}
}
void CDBOperation::CloseDB()
{
	if(m_pDBAccess){
		if(	mysql_commit((MYSQL *)m_pDBAccess)){
G_DB_DEBUG("%d | %d | close db commit erro\n",(unsigned int)pthread_self(), __LINE__);
		}
		mysql_close((MYSQL *)m_pDBAccess);
		m_pDBAccess = NULL ;
		mysql_thread_end();
G_DB_DEBUG("%d | %d | close db %d\n",(unsigned int)pthread_self(), __LINE__, 	(int)time(NULL) - survival_time );
	}	
}


/*
返回非0:需要处理
返回　0:不需要处理
*/
int CDBOperation::CrawlerRecord(char *dir, char *host, char *page)
{
	char url[512];

	char szSql[1024] = {0} ;
	int nUpdateCount;
	//MYSQL_RES *res; 
	//MYSQL_ROW row;
	int ret=0;
	
	if(strcmp(dir, "/")) sprintf(url, "http://%s/%s/%s\n", host, dir, strcmp(page, "@")?page:"");  
	else  sprintf(url, "http://%s%s%s\n", host, dir, strcmp(page, "@")?page:"");  

	sprintf(szSql, "call InsertCrawlerRecord( '%s')",url) ;
	G_DB_DEBUG("%d | %d | %s\n",(unsigned int)pthread_self(), __LINE__, szSql);

	// 执行失败时，按没找到结果处理
	if(mysql_query_get_res((MYSQL *)m_pDBAccess, szSql)){
		G_DB_DEBUG( (char *)"get info error [%s %s]", szSql, mysql_error((MYSQL *)m_pDBAccess)) ;
		return	-1;
	}	

	return ret;

}


/*
返回非0:需要处理
返回　0:不需要处理
*/
int CDBOperation::isNeedToHandle(char *dir, char *host, char *page)
{
	char url[512];

	char szSql[1024] = {0} ;
	int nUpdateCount;
	MYSQL_RES *res; 
	MYSQL_ROW row;
	int ret;
	
	if(strcmp(dir, "/")) sprintf(url, "http://%s/%s/%s\n", host, dir, strcmp(page, "@")?page:"");  
	else  sprintf(url, "http://%s%s%s\n", host, dir, strcmp(page, "@")?page:"");  

	sprintf(szSql, "CALL GetIsNeedToHandleSate( '%s')",url) ;
	G_DB_DEBUG("%d | %d | %s\n",(unsigned int)pthread_self(), __LINE__, szSql);

	// 执行失败时，按没找到结果处理
	if(mysql_query_get_res((MYSQL *)m_pDBAccess, szSql, &res)){
		G_DB_DEBUG( (char *)"get info error [%s %s]", szSql, mysql_error((MYSQL *)m_pDBAccess)) ;
		return	-1;
	}	
	if (!res) { 
		G_DB_DEBUG( (char *)"get info error [%s] -2",szSql);
		mysql_query_free_res((MYSQL *)m_pDBAccess, res);
		return -2;
	}
	//没有找到结果
	if((row = mysql_fetch_row(res)) == NULL){
		G_DB_DEBUG( (char *)"get info error [%s] -3",szSql);
		mysql_query_free_res((MYSQL *)m_pDBAccess, res);
		return -3;
	}
	ret = atoi(row[0]);
	mysql_query_free_res((MYSQL *)m_pDBAccess, res);
	return ret;

}

#if 0
int CDBOperation::UpdateBankWorkKey(ST_TranData* stpTranData)
{	
	char szSql[512]={0};
	MYSQL_RES *res; 
	MYSQL_ROW row;
	int nRet;

	if(stpTranData->szTerminalID_41[0]=='\0'  || stpTranData->szMerchantID_42[0]=='\0'){
		G_DB_DEBUG( (char *)"更新银行密钥失败") ;
		memcpy(stpTranData->szRetCode_39,"E6",2);
		return -1;
	}
	sprintf(szSql, "UPDATE tab_platkey SET MacKey='%s', PinKey='%s', SignTime=NOW() WHERE PlatTermNo='%s' AND PlatMerchantNo='%s'", 
		stpTranData->szBankMacKey,stpTranData->szBankPInKey, stpTranData->szTerminalID_41,stpTranData->szMerchantID_42);
G_DB_DEBUG("%d | %d | %s\n",(unsigned int)pthread_self(), __LINE__, szSql);

	if(mysql_query((MYSQL *)m_pDBAccess, szSql)){
		G_DB_DEBUG( (char *)"更新银行密钥失败[%s %s]", szSql, mysql_error((MYSQL *)m_pDBAccess)) ;
		memcpy(stpTranData->szRetCode_39,"E6",2);
		mysql_rollback((MYSQL *)m_pDBAccess);
		return -1;
	}

	nRet = mysql_affected_rows((MYSQL *)m_pDBAccess);
	if(nRet <=0 ){
		G_DB_DEBUG( (char *)"更新银行密钥失败[%s |结果未找到]", szSql) ;
		memcpy(stpTranData->szRetCode_39,"E6",2);
		mysql_commit((MYSQL *)m_pDBAccess);
		return -1;
	}
	sprintf(szSql, "UPDATE  tab_platmerterm SET BatchNo = '%s' WHERE PlatTermNo = '%s' AND PlatMerchantID = '%s'", 
		stpTranData->szBatchNo, stpTranData->szTerminalID_41,stpTranData->szMerchantID_42);
G_DB_DEBUG("%d | %d | %s\n",(unsigned int)pthread_self(), __LINE__, szSql);

	if(mysql_query((MYSQL *)m_pDBAccess, szSql)){
		G_DB_DEBUG( (char *)"更新银行密钥失败[%s %s]", szSql, mysql_error((MYSQL *)m_pDBAccess)) ;
		memcpy(stpTranData->szRetCode_39,"E6",2);
		mysql_rollback((MYSQL *)m_pDBAccess);
		return -1;
	}
	
	nRet = mysql_affected_rows((MYSQL *)m_pDBAccess);
	if(nRet <0 ){
		G_DB_DEBUG( (char *)"更新银行密钥失败[%s |结果未找到]", szSql) ;
		memcpy(stpTranData->szRetCode_39,"E6",2);
		mysql_commit((MYSQL *)m_pDBAccess);
		return -1;
	}

	mysql_commit((MYSQL *)m_pDBAccess);
	return 0;

}

int CDBOperation::UpdateBankWorkKey(char* pMerchantID, char* pTermID, char* pPinKey, char* pMacKey, char* pBatchNo)
{	
	char szSql[512]={0};
	MYSQL_RES *res; 
	MYSQL_ROW row;
	int nRet;

	if(pTermID[0]=='\0'  || pMerchantID[0]=='\0'){
		G_DB_DEBUG( (char *)"更新银行密钥失败") ;
		return -1;
	}
	sprintf(szSql, "UPDATE tab_platkey SET MacKey='%s', PinKey='%s', SignTime=NOW() WHERE PlatTermNo='%s' AND PlatMerchantNo='%s'", 
		pMacKey,pPinKey, pTermID,pMerchantID);
G_DB_DEBUG("%d | %d | %s\n",(unsigned int)pthread_self(), __LINE__, szSql);

	if(mysql_query((MYSQL *)m_pDBAccess, szSql)){
		G_DB_DEBUG( (char *)"更新银行密钥失败[%s %s]", szSql, mysql_error((MYSQL *)m_pDBAccess)) ;
		mysql_rollback((MYSQL *)m_pDBAccess);
		return -1;
	}

	nRet = mysql_affected_rows((MYSQL *)m_pDBAccess);
	if(nRet <=0 ){
		G_DB_DEBUG( (char *)"更新银行密钥失败[%s |结果未找到]", szSql) ;
		mysql_commit((MYSQL *)m_pDBAccess);
		return -1;
	}
	sprintf(szSql, "UPDATE  tab_platmerterm SET BatchNo = '%s' WHERE PlatTermNo = '%s' AND PlatMerchantID = '%s'", 
		pBatchNo, pTermID,pMerchantID);
G_DB_DEBUG("%d | %d | %s\n",pthread_self(), __LINE__, szSql);

	if(mysql_query((MYSQL *)m_pDBAccess, szSql)){
		G_DB_DEBUG( (char *)"更新银行密钥失败[%s %s]", szSql, mysql_error((MYSQL *)m_pDBAccess)) ;
		mysql_rollback((MYSQL *)m_pDBAccess);
		return -1;
	}
	
	nRet = mysql_affected_rows((MYSQL *)m_pDBAccess);
	if(nRet <=0 ){
		G_DB_DEBUG( (char *)"更新银行密钥失败[%s |结果未找到]", szSql) ;
		mysql_commit((MYSQL *)m_pDBAccess);
		return -1;
	}

	mysql_commit((MYSQL *)m_pDBAccess);
	return 0;

}



int CDBOperation::GetBankWorkKey(char* pMerchantID, char* pTermID, int& nBankIndex, char* pPinKey, char* pMacKey, char* pTmkKey)
{	
	char szSql[512] = { 0 };
	MYSQL_RES *res; 
	MYSQL_ROW row;

	sprintf(szSql,"select KeyIndex,PinKey,MacKey,TMK from tab_platkey where PlatTermNo='%s' and PlatMerchantNo='%s' limit 0,1", pTermID, pMerchantID);
G_DB_DEBUG("%d | %d | %s\n",pthread_self(), __LINE__, szSql);

	try
	{
		if(mysql_query_get_res((MYSQL *)m_pDBAccess, szSql, &res)){
			G_DB_DEBUG( (char *)"获取GetBankWorkKey失败[%s %s]", szSql, mysql_error((MYSQL *)m_pDBAccess)) ;
			return -1;
		}
		if (res) { 
			int i, j;
			int index;

			if((row = mysql_fetch_row(res)) == NULL){
				G_DB_DEBUG( (char *)"获取GetBankWorkKey失败[%s |结果未找到]", szSql) ;
				mysql_query_free_res((MYSQL *)m_pDBAccess, res);
				return -1;
			}


			if(mysql_get_field_key_val(res, row, "KeyIndex", &nBankIndex) < 0){
				G_DB_DEBUG( (char *)"获取GetBankWorkKey失败[%s]",szSql);
				mysql_query_free_res((MYSQL *)m_pDBAccess, res);
				return -1;
			}
			
			if(mysql_get_field_key_val(res, row, "PinKey", pPinKey)  < 0){
				G_DB_DEBUG( (char *)"获取GetBankWorkKey失败[%s]",szSql);
				mysql_query_free_res((MYSQL *)m_pDBAccess, res);
				return -1;
			}
			
			if(mysql_get_field_key_val(res, row, "MacKey", pMacKey)  < 0){
				G_DB_DEBUG( (char *)"获取GetBankWorkKey失败[%s]",szSql);
				mysql_query_free_res((MYSQL *)m_pDBAccess, res);
				return -1;
			}
			
			if(mysql_get_field_key_val(res, row, "TMK", pTmkKey)  < 0){
				G_DB_DEBUG( (char *)"获取GetBankWorkKey失败[%s]",szSql);
				mysql_query_free_res((MYSQL *)m_pDBAccess, res);
				return -1;
			}

			if (strlen(pPinKey) <= 0)
			{
				mysql_query_free_res((MYSQL *)m_pDBAccess, res);
				return -2;
			}
			mysql_query_free_res((MYSQL *)m_pDBAccess, res);
			return 0;
			
		}else{
			G_DB_DEBUG( (char *)"获取GetBankWorkKey失败[%s %s]", szSql, mysql_error((MYSQL *)m_pDBAccess)) ;
			mysql_query_free_res((MYSQL *)m_pDBAccess, res);
			return -1;
		}
	}
	catch (...)
	{
		G_DB_DEBUG( (char *)"获取GetBankWorkKey失败[%s]",szSql);
		return -1;
	}	

	return 0;
}

int CDBOperation::UpdateTermWorkKey(ST_TranData* stpTranData)
{
	
	char szSql[512]={0};
	int  nRet=0;

	sprintf(szSql, "UPDATE tab_termkey SET MacKey='%s', PinKey='%s', TdkKey='%s', SignTime=NOW()  WHERE TermNo='%s'", stpTranData->szTermMacKey,stpTranData->szTermPInKey,stpTranData->szTermPInKey, stpTranData->szTermNo);
G_DB_DEBUG("%d | %d | %s\n",pthread_self(), __LINE__, szSql);

	if(mysql_query_get_res((MYSQL *)m_pDBAccess, szSql) != 0){
		G_DB_DEBUG( (char *)"更新终端密钥失败[%s %s]", szSql, mysql_error((MYSQL *)m_pDBAccess)) ;
		memcpy(stpTranData->szRetCode_39,"E6",2);
		return -1;
	}

	return 0;
}

int CDBOperation::GetTermInfo(ST_TranData* pstTranData)
{
	char szSql[1024*3] = { 0 };
	int nRet=0;
	MYSQL_RES *res; 
	MYSQL_ROW row;

	if(pstTranData->isGetTermInfo){
		return 0;
	}
	pstTranData->isGetTermInfo = 1;

	sprintf(szSql,"CALL GETTermInfo('%s', '%s')",pstTranData->szTermNo, pstTranData->szTranType);
G_DB_DEBUG("%d | %d | %s\n",pthread_self(), __LINE__, szSql);

	try
	{
		if(mysql_query_get_res((MYSQL *)m_pDBAccess, szSql, &res)){
			G_DB_DEBUG( (char *)"获取终端信息失败[%s %s]", szSql, mysql_error((MYSQL *)m_pDBAccess)) ;
			memcpy(pstTranData->szRetCode_39,"D1",2);
			return -1;
		}	

		if (!res) { 
			memcpy(pstTranData->szRetCode_39,"D1",2);
			G_DB_DEBUG( (char *)"获取终端信息失败[%s]", szSql) ;
			mysql_query_free_res((MYSQL *)m_pDBAccess, res);
			return -1;
		}
		
		if((row = mysql_fetch_row(res)) == NULL){
			memcpy(pstTranData->szRetCode_39,"D1",2);
			mysql_query_free_res((MYSQL *)m_pDBAccess, res);
			return -1;
		}

		nRet=atoi(row[0]);
		if(nRet!=0)
		{
			memcpy(pstTranData->szRetCode_39,"D1",2);
			if(nRet==1)
			{
				memcpy(pstTranData->szRetCode_39,"D1",2);
			}
			else if(nRet==2)
			{
				memcpy(pstTranData->szRetCode_39,"D2",2);
			}
			else if(nRet==3)
			{
				memcpy(pstTranData->szRetCode_39,"D3",2);
			}
			else if(nRet==4)
			{
				memcpy(pstTranData->szRetCode_39,"E9",2);
			}
			else if(nRet==5)
			{
				memcpy(pstTranData->szRetCode_39,"D6",2);
			}
			else if(nRet==6)
			{
				memcpy(pstTranData->szRetCode_39,"D7",2);
			}
			else if(nRet==7 )
			{
				memcpy(pstTranData->szRetCode_39,"A0",2);
			}
			else if(nRet==8 )
			{
				memcpy(pstTranData->szRetCode_39,"A0",2);
			}
			G_DB_DEBUG( (char *)"获取终端信息失败[%s][%s]", pstTranData->szRetCode_39, szSql) ;

			mysql_query_free_res((MYSQL *)m_pDBAccess, res);

			return nRet;
		}


		mysql_get_field_key_val(res, row, "CanPrint", &pstTranData->nCanPrint);
		mysql_get_field_key_val(res, row, "ProgramType", &pstTranData->nProgramType);
		mysql_get_field_key_val(res, row, "UpdateFlag", &pstTranData->nUpdateFlag);
		mysql_get_field_key_val(res, row, "TermType", &pstTranData->nTermType);
		
		mysql_get_field_key_val(res, row, "PlatTermNo", pstTranData->szTerminalID_41 ,sizeof(pstTranData->szTerminalID_41));
		mysql_get_field_key_val(res, row, "PlatMerchantNo", pstTranData->szMerchantID_42,sizeof(pstTranData->szMerchantID_42));
		mysql_get_field_key_val(res, row, "BankKeyIndex", &pstTranData->nBankKeyIndex);
		mysql_get_field_key_val(res, row, "PlatMacK", pstTranData->szBankMacKey,sizeof(pstTranData->szBankMacKey));

		mysql_get_field_key_val(res, row, "PlatTMK", pstTranData->szBankTMKey,sizeof(pstTranData->szBankTMKey));
		mysql_get_field_key_val(res, row, "PlatPinK", pstTranData->szBankPInKey,sizeof(pstTranData->szBankPInKey));
		mysql_get_field_key_val(res, row, "TermKeyIndex", &pstTranData->nTermKeyIndex);
		mysql_get_field_key_val(res, row, "TermTMK", pstTranData->szTermTMKey,sizeof(pstTranData->szTermTMKey));

		mysql_get_field_key_val(res, row, "TermMaK", pstTranData->szTermMacKey,sizeof(pstTranData->szTermMacKey));
		mysql_get_field_key_val(res, row, "TermPinK", pstTranData->szTermPInKey,sizeof(pstTranData->szTermPInKey));
		mysql_get_field_key_val(res, row, "TermTdK", pstTranData->szTermTdkKey,sizeof(pstTranData->szTermTdkKey));
		mysql_get_field_key_val(res, row, "BatchNo", pstTranData->szBatchNo,sizeof(pstTranData->szBatchNo));
		
		mysql_get_field_key_val(res, row, "Enctype", pstTranData->szEnctype,sizeof(pstTranData->szEnctype));
		mysql_get_field_key_val(res, row, "MerchantName", pstTranData->szMerchantName,sizeof(pstTranData->szMerchantName));

		mysql_query_free_res((MYSQL *)m_pDBAccess, res);
		
		return 0;
	}
	catch (...)
	{
		memcpy(pstTranData->szRetCode_39,"D1",2);
		return -1;
	}

}
#if 0
int CDBOperation::GetBankSerialNo(ST_TranData* stpTranData)
{
	long nSerno = 0 ;
	long nRet = 0 ;
	char szSql[512] = {0} ;
	MYSQL_RES *res; 
	MYSQL_ROW row;

	sprintf(szSql, "CALL GetBankSerialNo('%s', '%s')",stpTranData->szTerminalID_41, stpTranData->szMerchantID_42) ;
G_DB_DEBUG("%d | %d | %s\n",pthread_self(), __LINE__, szSql);

	try
	{
		if(mysql_query_get_res((MYSQL *)m_pDBAccess, szSql, &res)){
			G_DB_DEBUG( (char *)"获取流水号异常[%s %s]", szSql, mysql_error((MYSQL *)m_pDBAccess)) ;
			memcpy(stpTranData->szRetCode_39,"D4",2);
			return -1;
		}	

		if (!res) { 
			G_DB_DEBUG( (char *)"获取流水号异常[%s %s]", szSql, mysql_error((MYSQL *)m_pDBAccess)) ;
			memcpy(stpTranData->szRetCode_39,"D4",2);
			mysql_query_free_res((MYSQL *)m_pDBAccess, res);
			return -1;
		}
		if((row = mysql_fetch_row(res)) == NULL){
			G_DB_DEBUG( (char *)"获取流水号异常[%s |结果未找到]", szSql) ;
			memcpy(stpTranData->szRetCode_39,"D4",2);
			mysql_query_free_res((MYSQL *)m_pDBAccess, res);
			return -1;
		}

		if(atoi(row[0]) <= 0){
			G_DB_DEBUG( (char *)"获取流水号异常[%s %s]", szSql, mysql_error((MYSQL *)m_pDBAccess)) ;
			memcpy(stpTranData->szRetCode_39,"D4",2);
			mysql_query_free_res((MYSQL *)m_pDBAccess, res);
			return -1;
		}
		mysql_get_field_key_val(res, row, "SerialNo", &nSerno);
		sprintf(stpTranData->szTraceNumber_11, "%06d", nSerno);
		mysql_query_free_res((MYSQL *)m_pDBAccess, res);
		return 0;
	}
	catch(...)
	{
		G_DB_DEBUG( (char *)"获取流水号异常[%s]", szSql) ;
		memcpy(stpTranData->szRetCode_39,"D4",2);
		return -1 ;
	}
}

int CDBOperation::GetBankSerialNo(const char* pMerchantID, const char* pTermID, char* pSerNo, char* pBatchNo)
{
	long nSerno = 0 ;
	char szBatchNo[7] = {0};
	long nRet = 0 ;
	char szSql[512] = {0} ;
	MYSQL_RES *res; 
	MYSQL_ROW row;

	sprintf(szSql, "call GetBankSerialNo('%s', '%s')", pTermID, pMerchantID) ;
G_DB_DEBUG("%d | %d | %s\n",pthread_self(), __LINE__, szSql);

	try
	{
		if(mysql_query_get_res((MYSQL *)m_pDBAccess, szSql, &res)){
			G_DB_DEBUG( (char *)"获取流水号异常[%s %s]", szSql, mysql_error((MYSQL *)m_pDBAccess)) ;
			return -1;
		}	

		if (!res) { 
			G_DB_DEBUG( (char *)"获取流水号异常[%s %s]", szSql, mysql_error((MYSQL *)m_pDBAccess)) ;
			mysql_query_free_res((MYSQL *)m_pDBAccess, res);
			return -1;
		}
		if((row = mysql_fetch_row(res)) == NULL){
			G_DB_DEBUG( (char *)"获取流水号异常[%s |结果未找到]", szSql) ;
			mysql_query_free_res((MYSQL *)m_pDBAccess, res);
			return -1;
		}		
		if(atoi(row[0]) <= 0){
			G_DB_DEBUG( (char *)"获取流水号异常[%s %s]", szSql, mysql_error((MYSQL *)m_pDBAccess)) ;
			mysql_query_free_res((MYSQL *)m_pDBAccess, res);
			return -1;
		}
		mysql_get_field_key_val(res, row, "SerialNo", &nSerno);
		sprintf(pSerNo, "%06d", nSerno);
		mysql_get_field_key_val(res, row, "BatchNo", szBatchNo, 6);
		sprintf(pBatchNo, "%06d", atoi(szBatchNo));
		mysql_query_free_res((MYSQL *)m_pDBAccess, res);
		return 0;

	}
	catch(...)
	{
		G_DB_DEBUG( (char *)"获取流水号异常[%s]", szSql) ;
		return -1 ;
	}
}
#else
int	CDBOperation::GetBankSerialNo(ST_TranData* stpTranData)
{
	MYSQL *pDbObj = (MYSQL *)m_pDBAccess;
	MYSQL_RES *pRes = NULL; //result
	MYSQL_ROW mysql_row; //row
	long nSerialNo = 0;
	char szBatchNo[7] = {0};
	int nBatchAddFlag = 0;
	
	char szSql[512] = {0};
	sprintf(szSql, "select SerialNo, BatchNo from tab_platmerterm where PlatMerchantID='%s' and PlatTermNo='%s' for UPDATE", 
			stpTranData->szMerchantID_42, stpTranData->szTerminalID_41);
G_DB_DEBUG("%d | %d | %s\n",pthread_self(), __LINE__, szSql);

	if (mysql_query(pDbObj, szSql)) // 如果失败
	{
		G_DB_DEBUG( (char *)"Call GetBankSerialNo 失败，错误原因:%s", mysql_error(pDbObj));
		memcpy(stpTranData->szRetCode_39,"D4",2);
		mysql_rollback((MYSQL *)m_pDBAccess);
		return -1;
	}

	pRes = mysql_use_result(pDbObj);
	if(pRes)
	{
		if((mysql_row = mysql_fetch_row(pRes)) != NULL)
		{
			if (mysql_row[0])
				nSerialNo = atol(mysql_row[0]);
			if (mysql_row[1])
				memcpy(szBatchNo, mysql_row[1], 6);
		}
		else
		{
			mysql_free_result(pRes);
			mysql_rollback((MYSQL *)m_pDBAccess);
			return -1;
		}
	}
	else
	{
		mysql_free_result(pRes);
		mysql_rollback((MYSQL *)m_pDBAccess);
		return -1;
	}
	
	mysql_free_result(pRes);
	
	if (nSerialNo >= 999999)
	{
		nSerialNo = 1;
		if (nBatchAddFlag == 1)
		{
			if (atol(szBatchNo) >= 999999)
				memcpy(szBatchNo, "000001", 6);
			else
				sprintf(szBatchNo, "%06ld", (atol(szBatchNo)+1));
		}
	}
	else
		nSerialNo += 1;
		
	sprintf(stpTranData->szTraceNumber_11, "%06ld", nSerialNo);
	strcpy(stpTranData->szBatchNo, szBatchNo);
	
	memset(szSql, 0, sizeof(szSql));
	sprintf(szSql, "UPDATE tab_platmerterm SET SerialNo=%ld, BatchNo='%s' where PlatMerchantID='%s' and PlatTermNo='%s'", 
			nSerialNo, szBatchNo,  stpTranData->szMerchantID_42, stpTranData->szTerminalID_41);
G_DB_DEBUG("%d | %d | %s\n",pthread_self(), __LINE__, szSql);
	if (mysql_query(pDbObj, szSql)) // 如果失败
	{
		G_DB_DEBUG( (char *)"UPDATE tab_platmerterm 失败，错误原因:%s", mysql_error(pDbObj));
		mysql_rollback((MYSQL *)m_pDBAccess);
		memcpy(stpTranData->szRetCode_39,"D4",2);
		return -1;
	}
	
	mysql_commit(pDbObj);
	
	return 0;
}

int	CDBOperation::GetBankSerialNo(const char* pPlatMerchantID, const char* pPlatTermNo, char* pSerialNo, char* pBatchNo)
{
	MYSQL *pDbObj = (MYSQL *)m_pDBAccess;
	MYSQL_RES *pRes = NULL; //result
	MYSQL_ROW mysql_row; //row
	long nSerialNo = 0;
	char szBatchNo[7] = {0};
	int nBatchAddFlag = 0;
	
	char szSql[512] = {0};
	sprintf(szSql, "select SerialNo, BatchNo from tab_platmerterm where PlatMerchantID='%s' and PlatTermNo='%s' for UPDATE", 
			pPlatMerchantID, pPlatTermNo);
G_DB_DEBUG("%d | %d | %s\n",pthread_self(), __LINE__, szSql);

	if (mysql_query(pDbObj, szSql)) // 如果失败
	{
		G_DB_DEBUG( (char *)"Call GetBankSerialNo 失败，错误原因:%s", mysql_error(pDbObj));
		mysql_rollback((MYSQL *)m_pDBAccess);
		return -1;
	}

	pRes = mysql_use_result(pDbObj);
	if(pRes)
	{
		if((mysql_row = mysql_fetch_row(pRes)) != NULL)
		{
			if (mysql_row[0])
				nSerialNo = atol(mysql_row[0]);
			if (mysql_row[1])
				memcpy(szBatchNo, mysql_row[1], 6);
		}
		else
		{
			mysql_free_result(pRes);
			mysql_rollback((MYSQL *)m_pDBAccess);
			return -1;
		}
	}
	else
	{
		mysql_free_result(pRes);
		mysql_rollback((MYSQL *)m_pDBAccess);
		return -1;
	}
	
	mysql_free_result(pRes);
	
	if (nSerialNo >= 999999)
	{
		nSerialNo = 1;
		if (nBatchAddFlag == 1)
		{
			if (atol(szBatchNo) >= 999999)
				memcpy(szBatchNo, "000001", 6);
			else
				sprintf(szBatchNo, "%06ld", (atol(szBatchNo)+1));
		}
	}
	else
		nSerialNo += 1;
		
	
	sprintf(pSerialNo, "%06ld", nSerialNo) ;
	sprintf(pBatchNo, "%06ld", atol(szBatchNo));
	
	memset(szSql, 0, sizeof(szSql));
	sprintf(szSql, "UPDATE tab_platmerterm SET SerialNo=%ld, BatchNo='%s' where PlatMerchantID='%s' and PlatTermNo='%s'", 
			nSerialNo, szBatchNo,  pPlatMerchantID, pPlatTermNo);
G_DB_DEBUG("%d | %d | %s\n",pthread_self(), __LINE__, szSql);
	if (mysql_query(pDbObj, szSql)) // 如果失败
	{
		G_DB_DEBUG( (char *)"UPDATE tab_platmerterm 失败，错误原因:%s", mysql_error(pDbObj));
		mysql_rollback((MYSQL *)m_pDBAccess);
		return -1;
	}
	
	mysql_commit(pDbObj);
	
	return 0;
}

#endif

void CDBOperation::GetErrInfo(const char* pErrCode,char* pErrInfo)
{
	char szSql[256] = { 0 } ;
	char szErrInfo[512] = { 0 };
	MYSQL_RES *res; 
	MYSQL_ROW row;

	sprintf(szSql,"select ErrMeaning from tab_errinfo where ErrCode='%s'",pErrCode);
G_DB_DEBUG("%d | %d | %s\n",pthread_self(), __LINE__, szSql);

	try
	{
		if(mysql_query_get_res((MYSQL *)m_pDBAccess, szSql, &res)){
			G_DB_DEBUG( (char *)"获取错误信息失败[%s %s]", szSql, mysql_error((MYSQL *)m_pDBAccess)) ;
			strcat(szErrInfo,"交易失败，请稍后重试");
			return ;
		}

		if (!res) { 
			G_DB_DEBUG( (char *)"获取错误信息失败[%s]", szSql);
			sprintf(szErrInfo,"交易失败，请稍后重试");
			mysql_query_free_res((MYSQL *)m_pDBAccess, res);
			return ;
		}
		if((row = mysql_fetch_row(res)) == NULL){
			G_DB_DEBUG( (char *)"获取错误信息失败[%s |结果未找到]", szSql);
			sprintf(szErrInfo,"交易失败，请稍后重试");
			mysql_query_free_res((MYSQL *)m_pDBAccess, res);
			return ;
		}
		
		mysql_get_field_key_val(res, row, "ErrMeaning", szErrInfo);
		mysql_query_free_res((MYSQL *)m_pDBAccess, res);
	}
	catch (...)
	{
		G_DB_DEBUG( (char *)"获取错误信息失败[%s]", szSql) ;
		sprintf(szErrInfo,"交易失败，请稍后重试");
	}
	memcpy(pErrInfo,szErrInfo,sizeof(szErrInfo));	
}

void CDBOperation::GetErrShowInfo(const char* pErrCode,char* pErrInfo)
{
	char szSql[256] = { 0 } ;
	MYSQL_RES *res; 
	MYSQL_ROW row;

	sprintf(szSql,"select ShowInfo from tab_errinfo where ErrCode='%s' ",pErrCode);
G_DB_DEBUG("%d | %d | %s\n",pthread_self(), __LINE__, szSql);

	try
	{
		if(mysql_query_get_res((MYSQL *)m_pDBAccess, szSql, &res)){
			G_DB_DEBUG( (char *)"获取错误信息失败[%s %s]", szSql, mysql_error((MYSQL *)m_pDBAccess)) ;
			sprintf(pErrInfo,"交易失败，请稍后重试");
			return ;
		}

		if (!res) { 
			G_DB_DEBUG( (char *)"获取错误信息失败[%s]", szSql);
			sprintf(pErrInfo,"交易失败，请稍后重试");
			mysql_query_free_res((MYSQL *)m_pDBAccess, res);
			return ;
		}
		if((row = mysql_fetch_row(res)) == NULL){
			G_DB_DEBUG( (char *)"获取错误信息失败[%s |结果未找到]", szSql);
			sprintf(pErrInfo,"交易失败，请稍后重试");
			mysql_query_free_res((MYSQL *)m_pDBAccess, res);
			return ;
		}			

		mysql_get_field_key_val(res, row, "ShowInfo", pErrInfo);
		mysql_query_free_res((MYSQL *)m_pDBAccess, res);
		return ;
	}
	catch (...)
	{
		G_DB_DEBUG( (char *)"获取错误信息失败[%s]", szSql) ;
		sprintf(pErrInfo,"交易失败，请稍后重试");
	}

}

void CDBOperation::RecordTranData(ST_TranData* stpTranData)
{
	char szSql[1024] = {0} ;
	int nUpdateCount;
	MYSQL_RES *res; 
	MYSQL_ROW row;


	sprintf(szSql, "CALL RecordTradeData( '%s', '%s', '%s', '%s', '%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%d','%s','%s','%s','%s','%s','%d','%s','%s','%s')",
		stpTranData->szTermNo,stpTranData->szTranType,stpTranData->szAmount_4,stpTranData->szInAccount,stpTranData->szOutAccount_2,
		stpTranData->szTermSerno,stpTranData->szTraceNumber_11,stpTranData->szLocalDate,stpTranData->szLocalTime,stpTranData->szBankDate_13,
		stpTranData->szBankTime_12,stpTranData->szRetCode_39,stpTranData->szReferenceNumber_37,stpTranData->szThirdRetCode,
		stpTranData->szTerminalID_41,stpTranData->szMerchantID_42,stpTranData->szBatchNo,stpTranData->nCancelFlag,stpTranData->szAuthCode_38, 
		stpTranData->szOriginalSerNo, stpTranData->szOriginalTranDate,stpTranData->szAcquiringIDCode_32, stpTranData->szCardSequenceNumber_23, 
		stpTranData->nIsICCardTrade, stpTranData->szEquipmentID, stpTranData->szIssuerNo, stpTranData->szAcquirerNo) ;
G_DB_DEBUG("%d | %d | %s\n",pthread_self(), __LINE__, szSql);


	if(strlen(stpTranData->szTermNo) <= 0 || strlen(stpTranData->szTranType)<=0 || strlen(stpTranData->szAmount_4)<=0
		|| strlen(stpTranData->szLocalDate)<= 0 || strlen(stpTranData->szLocalTime)<=0 || strlen(stpTranData->szRetCode_39)<=0){
			G_DB_DEBUG( (char *)"记账失败[%s]", szSql) ;
			return  ;
	}
	
	try
	{
		if(mysql_query_get_res((MYSQL *)m_pDBAccess, szSql, &res)){
			G_DB_DEBUG( (char *)"记账失败[%s %s]", szSql, mysql_error((MYSQL *)m_pDBAccess)) ;
			return  ;
		}
		if (!res) { 
			G_DB_DEBUG( (char *)"记账失败[%s |结果未找到]", szSql) ;
			mysql_query_free_res((MYSQL *)m_pDBAccess, res);
			return ;
		}

		if((row = mysql_fetch_row(res)) == NULL){
			G_DB_DEBUG( (char *)"记账失败[%s |结果未找到]", szSql) ;
			mysql_query_free_res((MYSQL *)m_pDBAccess, res);
			return ;
		}			
		nUpdateCount = atoi(row[0]);
		mysql_query_free_res((MYSQL *)m_pDBAccess, res);
		if(nUpdateCount != 0){
			G_DB_DEBUG( (char *)"记账失败[%s]", szSql) ;
		}
		return ;		
	}
	catch(...)
	{
		G_DB_DEBUG( (char *)"插入交易记录异常[%s]", szSql) ;
		return ;
	}
}

void CDBOperation::RecordScriptData(ST_TranData* stpTranData)
{
	char szSql[1024] = {0} ;

	sprintf(szSql, "CALL RecordScriptData('%s', '%s', '%s', '%s', '%s','%s','%s','%s','%s','%s','%s','%d','%s','%s','%s','%s','%d')",
		stpTranData->szTermNo, stpTranData->szAmount_4, stpTranData->szTermSerno,stpTranData->szTraceNumber_11, stpTranData->szBankDate_13,
		stpTranData->szBankTime_12,stpTranData->szOutAccount_2, stpTranData->szRetCode_39, stpTranData->szTerminalID_41, 
		stpTranData->szMerchantID_42,stpTranData->szBatchNo,stpTranData->nIsNeedToSendScript, stpTranData->szAuthCode_38, 
		stpTranData->szProcessingCode_3, stpTranData->szCardSequenceNumber_23, stpTranData->szAcquiringIDCode_32, stpTranData->nPinInPutType) ;
G_DB_DEBUG("%d | %d | %s\n",pthread_self(), __LINE__, szSql);


	if(strlen(stpTranData->szTermNo)<=0 || strlen(stpTranData->szAmount_4)<=0 || strlen(stpTranData->szRetCode_39)!=2){
			G_DB_DEBUG( (char *)"记录脚本信息失败[%s]", szSql) ;
			return  ;
	}


	try
	{
		if(mysql_query_get_res((MYSQL *)m_pDBAccess, szSql)){
			G_DB_DEBUG( (char *)"记录脚本信息失败[%s %s]", szSql, mysql_error((MYSQL *)m_pDBAccess)) ;
			return  ;
		}

		return ;		
	}
	catch(...)
	{
		G_DB_DEBUG( (char *)"插入交易记录异常[%s]", szSql) ;
		return ;
	}
}

int CDBOperation::RecordTCResult(ST_TranData* stpTranData) 
{
	char szSql[1024] = {0} ;
	char sz55areaData[512] = { 0 } ;
	BCDtoASCII(stpTranData->szICData_55, stpTranData->nICData_55_Len, sz55areaData) ;


	sprintf(szSql, "CALL RecordTCResult('%s', '%s', '%s', '%d', '%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s')",
		stpTranData->szTraceNumber_11, sz55areaData, stpTranData->szBatchNo, 0, stpTranData->szOutAccount_2,
		stpTranData->szAmount_4,stpTranData->szServicePointMode_22, stpTranData->szTerminalID_41, stpTranData->szMerchantID_42, 
		stpTranData->szTranCount,stpTranData->szICCardType,stpTranData->szTranRspCode, "000000000000", 
		stpTranData->szAuthCurrencyType, stpTranData->szTermReadAble, stpTranData->szAcquiringIDCode_32, stpTranData->szPROCConditionCode,
		stpTranData->szCardSequenceNumber_23 ,stpTranData->szBankDate_13, stpTranData->szBankTime_12) ;
G_DB_DEBUG("%d | %d | %s\n",pthread_self(), __LINE__, szSql);

	
	try
	{
		if(mysql_query_get_res((MYSQL *)m_pDBAccess, szSql)){
			G_DB_DEBUG( (char *)"记录TC信息失败[%s %s]", szSql, mysql_error((MYSQL *)m_pDBAccess)) ;
			return -1 ;
		}

		return 0;		
	}
	catch(...)
	{
		G_DB_DEBUG( (char *)"插入TC值异常[%s]",szSql) ;
		return -1;
	}
}
int CDBOperation::GetOriginalTranData(ST_TranData* stTranData)
{
	char szPlatTermNo[9] = { 0 };
	char szPlatMerchantNo[16] = { 0 };
	char szSql[512] = { 0 };
	MYSQL_RES *res; 
	MYSQL_ROW row;


	if(memcmp(stTranData->szTranType,"T104",4)==0 || memcmp(stTranData->szTranType,"T204",4)==0)
	{
		//退货传入交易参考号来查询。
		sprintf(szSql,"select TranType, CardSequnce,BankRetCode,PlatTermNo,PlatMerchantNO,TranAmt,OutAccount,ReferenceNumber,BatchNo ,Replace(BankTime,':','') as BankTime,PlatSerialNo from tab_support where datediff(Localdate,'%s')=0 and TermNo='%s' and ReferenceNumber='%s'", stTranData->szOriginalTranDate,stTranData->szTermNo,stTranData->szOriginalReferNumber);
	}
	else
	{
		//非退货传入流水号来查询。
		sprintf(szSql,"select TranType, BankRetCode,PlatTermNo,PlatMerchantNO,TranAmt,OutAccount,ReferenceNumber,BatchNo ,Replace(BankTime,':','') as BankTime,PlatSerialNo from tab_support where datediff(Localdate,'%s')=0 and TranType='T102' and TermNo='%s' and PlatSerialNo='%s'", stTranData->szLocalDate,stTranData->szTermNo,stTranData->szOriginalSerNo);
	}
G_DB_DEBUG("%d | %d | %s\n",pthread_self(), __LINE__, szSql);

	try
	{
		if(mysql_query_get_res((MYSQL *)m_pDBAccess, szSql, &res)){
			G_DB_DEBUG( (char *)"获取原交易信息失败[%s %s]", szSql, mysql_error((MYSQL *)m_pDBAccess)) ;
			memcpy(stTranData->szRetCode_39,"D5",2);
			return  -1;
		}

		if (!res) { 
			G_DB_DEBUG( (char *)"获取原交易信息失败[%s]",szSql);
			memcpy(stTranData->szRetCode_39,"D5",2);
			mysql_query_free_res((MYSQL *)m_pDBAccess, res);
			return -1;
		}

		if((row = mysql_fetch_row(res)) == NULL){
			G_DB_DEBUG( (char *)"获取原交易信息失败[%s |结果未找到]",szSql);
			memcpy(stTranData->szRetCode_39,"D5",2);
			mysql_query_free_res((MYSQL *)m_pDBAccess, res);
			return -1;
		}

		mysql_get_field_key_val(res, row, "BankRetCode", stTranData->szRetCode_39,sizeof(stTranData->szRetCode_39));
		if(memcmp(stTranData->szRetCode_39,"00",2)!=0)
		{
			G_DB_DEBUG( (char *)"获取原交易信息失败[%s]",szSql);
			mysql_query_free_res((MYSQL *)m_pDBAccess, res);
			return -1;
		}
		
		mysql_get_field_key_val(res, row, "PlatTermNo", szPlatTermNo,sizeof(szPlatTermNo));
		mysql_get_field_key_val(res, row, "PlatMerchantNO", szPlatMerchantNo,sizeof(szPlatMerchantNo));
		if(memcmp(stTranData->szTerminalID_41,szPlatTermNo,8)!=0 || memcmp(stTranData->szMerchantID_42,szPlatMerchantNo,15)!=0)
		{
			G_DB_DEBUG( (char *)"发生交易的终端和交易原始终端不一致[%s]",szSql);
			memcpy(stTranData->szRetCode_39,"D5",2);
			mysql_query_free_res((MYSQL *)m_pDBAccess, res);
			return -1;
		}
		mysql_get_field_key_val(res, row, "OutAccount", stTranData->szOutAccount_2,sizeof(stTranData->szOutAccount_2));
		mysql_get_field_key_val(res, row, "TranAmt",stTranData->szOriginalAmt,sizeof(stTranData->szOriginalAmt));
		mysql_get_field_key_val(res, row, "ReferenceNumber", stTranData->szOriginalReferNumber,sizeof(stTranData->szOriginalReferNumber));
		mysql_get_field_key_val(res, row, "BatchNo", stTranData->szOriginalBatchNo,sizeof(stTranData->szOriginalBatchNo));
		mysql_get_field_key_val(res, row, "BankTime", stTranData->szOriginalTranTime,sizeof(stTranData->szOriginalTranTime));
		mysql_get_field_key_val(res, row, "PlatSerialNo",stTranData->szOriginalSerNo,sizeof(stTranData->szOriginalSerNo));
		mysql_get_field_key_val(res, row, "TranType",stTranData->szOriginalTranType,sizeof(stTranData->szOriginalTranType));

		if(memcmp(stTranData->szTranType,"T104",4)==0)
		{
			mysql_get_field_key_val(res, row, "CardSequnce",stTranData->szCardSequenceNumber_23,sizeof(stTranData->szCardSequenceNumber_23));
		}
		mysql_query_free_res((MYSQL *)m_pDBAccess, res);
		return 0;
	}
	catch (...)
	{
		G_DB_DEBUG( (char *)"获取原交易信息失败[%s]",szSql);
		memcpy(stTranData->szRetCode_39,"D5",2);
		return -1;
	}
}

//记录冲正
int CDBOperation::RecordCancelInfo(ST_TranData* stpTranData)
{
	int nRet=0;
	char szData55[512] = { 0 } ;
	char szSql[512] = { 0 };
	BCDtoASCII(stpTranData->szICData_55, stpTranData->nICData_55_Len, szData55) ;

	sprintf(szSql,"CALL RecordCancelData( '%s','%s','%s','%s','%s','%s','%s','%s','%s','%d', '%s','%s','%s','%s','%s','%s')",
		stpTranData->szTermNo,stpTranData->szTraceNumber_11,stpTranData->szLocalDate,stpTranData->szOutAccount_2,stpTranData->szMerchantID_42,
		stpTranData->szTerminalID_41,stpTranData->szAmount_4,stpTranData->szBatchNo,stpTranData->szAuthCode_38, stpTranData->nIsICCardTrade, "98", 
		stpTranData->szTranType, stpTranData->szServicePointMode_22, stpTranData->szCardSequenceNumber_23, stpTranData->szExpiredDate_14, szData55);
G_DB_DEBUG("%d | %d | %s\n",pthread_self(), __LINE__, szSql);

	try
	{
		if(mysql_query_get_res((MYSQL *)m_pDBAccess, szSql)){
			G_DB_DEBUG( (char *)"记录冲正信息失败[%s %s]", szSql, mysql_error((MYSQL *)m_pDBAccess)) ;
			return  -1;
		}

		return 0;
	}
	catch (...)
	{
		G_DB_DEBUG( (char *)"记录冲正信息失败[%s]",szSql);
		return -1;
	}
}
int CDBOperation::GetCancelInfo(ST_TranData* stpTranData,ST_CancelData* stCancelData)
{
	char szSql[512] = { 0 };
	MYSQL_RES *res; 
	MYSQL_ROW row;

	sprintf(szSql,"select * from tab_cancel where termNo='%s' and IsCanceled!=1 and SendTimes!=3 and datediff(TransDate,now())=0",stpTranData->szTermNo);
G_DB_DEBUG("%d | %d | %s\n",pthread_self(), __LINE__, szSql);

	
	try
	{
		if(mysql_query_get_res((MYSQL *)m_pDBAccess, szSql, &res)){
			G_DB_DEBUG( (char *)"获取信息失败[%s %s]", szSql, mysql_error((MYSQL *)m_pDBAccess)) ;
			return  -1;
		}

		if (!res) { 
			G_DB_DEBUG( (char *)"获取信息失败[%s %s]", szSql, mysql_error((MYSQL *)m_pDBAccess)) ;
			mysql_query_free_res((MYSQL *)m_pDBAccess, res);
			return -1;
			
		}
		if((row = mysql_fetch_row(res)) == NULL){
			G_DB_DEBUG( (char *)"获取信息失败[%s |结果未找到]", szSql) ;
			mysql_query_free_res((MYSQL *)m_pDBAccess, res);
			return -1;
		}

		mysql_get_field_key_val(res, row, "TermNo",stCancelData->szTermNo,sizeof(stCancelData->szTermNo));
		mysql_get_field_key_val(res, row, "PlatSerialNo",stCancelData->szPlatSerialNo,sizeof(stCancelData->szPlatSerialNo));
		mysql_get_field_key_val(res, row, "TransDate",stCancelData->szTransDate,sizeof(stCancelData->szTransDate));
		mysql_get_field_key_val(res, row, "PlatMerchantNo",stCancelData->szPlatMerchantNo,sizeof(stCancelData->szPlatMerchantNo));
		mysql_get_field_key_val(res, row, "PlatTermNo",stCancelData->szPlatTermNo,sizeof(stCancelData->szPlatTermNo));

		mysql_get_field_key_val(res, row, "TradeMoney",stCancelData->szTradeMoney,sizeof(stCancelData->szTradeMoney));
		mysql_get_field_key_val(res, row, "BatchNo",stCancelData->szBatchNo,sizeof(stCancelData->szBatchNo));
		mysql_get_field_key_val(res, row, "AuthorCode",stCancelData->szAuthorCode,sizeof(stCancelData->szAuthorCode));
		mysql_get_field_key_val(res, row, "CardNo",stCancelData->szCardNo,sizeof(stCancelData->szCardNo));
		mysql_get_field_key_val(res, row, "CancelCode",stCancelData->szCancelCode,sizeof(stCancelData->szCancelCode));

		mysql_get_field_key_val(res, row, "TransType",stCancelData->szTransCode,sizeof(stCancelData->szTransCode));
		mysql_get_field_key_val(res, row, "ServicePointMode",stCancelData->szPinType,sizeof(stCancelData->szPinType));
		mysql_get_field_key_val(res, row, "CardSequenceNumber",stCancelData->CardSequenceNumber,sizeof(stCancelData->CardSequenceNumber));
		mysql_get_field_key_val(res, row, "Field55",stCancelData->szICData_55,sizeof(stCancelData->szICData_55));
		mysql_get_field_key_val(res, row, "IsICCardTrade",&stCancelData->nIsICTrade);

		mysql_get_field_key_val(res, row, "IsCanceled",&stCancelData->nIsCanceled);
		mysql_get_field_key_val(res, row, "CardExpDate",stCancelData->szCardExpDate,sizeof(stCancelData->szCardExpDate));

		mysql_query_free_res((MYSQL *)m_pDBAccess, res);
		return 0;
	}
	catch (...)
	{
		return -1;
	}
}

int CDBOperation::GetScriptInfo(ST_TranData* stpTranData)
{
	char szSql[512] = { 0 };
	MYSQL_RES *res; 
	MYSQL_ROW row;

	sprintf(szSql,"select * from tab_scriptinfo where termNo='%s' and IsNeedToSendScript=1 and SendTime!=3 and datediff(TransDate,now())=0",stpTranData->szTermNo);
G_DB_DEBUG("%d | %d | %s\n",pthread_self(), __LINE__, szSql);


	try
	{
		if(mysql_query_get_res((MYSQL *)m_pDBAccess, szSql, &res)){
			G_DB_DEBUG( (char *)"获取信息失败[%s %s]", szSql, mysql_error((MYSQL *)m_pDBAccess)) ;
			return  -1;
		}

		if (!res) { 
			G_DB_DEBUG( (char *)"获取信息失败[%s %s]", szSql, mysql_error((MYSQL *)m_pDBAccess)) ;
			mysql_query_free_res((MYSQL *)m_pDBAccess, res);
			return  -1;
			
		}
		if((row = mysql_fetch_row(res)) == NULL){
			mysql_query_free_res((MYSQL *)m_pDBAccess, res);
			G_DB_DEBUG( (char *)"获取信息失败[%s |结果未找到]", szSql) ;
			return  -1;
		}

		mysql_get_field_key_val(res, row, "CardNo",stpTranData->szOutAccount_2,sizeof(stpTranData->szOutAccount_2));
		mysql_get_field_key_val(res, row, "ProcessingCode",stpTranData->szProcessingCode_3,sizeof(stpTranData->szProcessingCode_3));
		mysql_get_field_key_val(res, row, "TradeMoney",stpTranData->szAmount_4,sizeof(stpTranData->szAmount_4));
		mysql_get_field_key_val(res, row, "SequenceNumber",stpTranData->szCardSequenceNumber_23,sizeof(stpTranData->szCardSequenceNumber_23));
		mysql_get_field_key_val(res, row, "AcquiringIDCode",stpTranData->szAcquiringIDCode_32,sizeof(stpTranData->szAcquiringIDCode_32));

		mysql_get_field_key_val(res, row, "PlatSerialNo",stpTranData->szReferenceNumber_37,sizeof(stpTranData->szReferenceNumber_37));
		mysql_get_field_key_val(res, row, "AuthorCode",stpTranData->szAuthCode_38,sizeof(stpTranData->szAuthCode_38));
		mysql_get_field_key_val(res, row, "TermNo",stpTranData->szTerminalID_41,sizeof(stpTranData->szTerminalID_41));
		mysql_get_field_key_val(res, row, "PlatMerchantNo",stpTranData->szMerchantID_42,sizeof(stpTranData->szMerchantID_42));
		mysql_get_field_key_val(res, row, "CurrencyCode",stpTranData->szCurrencyCode_49,sizeof(stpTranData->szCurrencyCode_49));

		mysql_get_field_key_val(res, row, "IsNeedToSendScript",&stpTranData->nIsNeedToSendScript);
		mysql_get_field_key_val(res, row, "BatchNo",stpTranData->szOriginalSerNo,sizeof(stpTranData->szOriginalSerNo));
		mysql_get_field_key_val(res, row, "PlatSerialNo",stpTranData->szOriginalSerNo,sizeof(stpTranData->szOriginalSerNo));
		mysql_get_field_key_val(res, row, "TransDate",stpTranData->szOriginalTranDate,sizeof(stpTranData->szOriginalTranDate));
		mysql_get_field_key_val(res, row, "PinInPutType",&stpTranData->nPinInPutType);

		mysql_query_free_res((MYSQL *)m_pDBAccess, res);		
		return 0;
	}
	catch (...)
	{
		return -1;
	}
}

int CDBOperation::UpdateCancelFlag(ST_CancelData* stCancelData)
{
	char szSql[512] = { 0 };
	char szCancelDay[20] = { 0 };
	char szCancelTime[20] = { 0 };
	char szCancleDate[20]={0};


	if(stCancelData->szTermNo[0]=='\0'  || stCancelData->szPlatSerialNo[0]=='\0'){
		G_DB_DEBUG( (char *)"更新冲正结果失败") ;
		return -1;
	}
	GetstrCurDate((char *)"-", szCancelDay);
	GetstrCurTime((char *)":", szCancelTime);
	sprintf(szCancleDate, "%s %s", szCancelDay, szCancelTime);

	sprintf(szSql, "UPDATE tab_cancel SET IsCanceled='%d', SendTimes=SendTimes+1, CancelTime='%s'  WHERE termNo='%s' AND PlatSerialNo='%s' AND DATEDIFF(TransDate,'%s')=0",
		stCancelData->nIsCanceled, szCancleDate, stCancelData->szTermNo,stCancelData->szPlatSerialNo,stCancelData->szTransDate);
G_DB_DEBUG("%d | %d | %s\n",pthread_self(), __LINE__, szSql);

	if(mysql_query((MYSQL *)m_pDBAccess, szSql)){
		G_DB_DEBUG( (char *)"更新冲正结果失败[%s %s]", szSql, mysql_error((MYSQL *)m_pDBAccess)) ;
		return -1;
	}

	sprintf(szSql,"UPDATE tab_support SET CancelFlag='%d' WHERE TermNo='%s' AND PlatSerialNo='%s' AND DATEDIFF(LocalDate,'%s')=0",
		stCancelData->nIsCanceled,  stCancelData->szTermNo,stCancelData->szPlatSerialNo,stCancelData->szTransDate);
G_DB_DEBUG("%d | %d | %s\n",pthread_self(), __LINE__, szSql);
	if(mysql_query((MYSQL *)m_pDBAccess, szSql)){
		G_DB_DEBUG( (char *)"更新冲正结果失败[%s %s]", szSql, mysql_error((MYSQL *)m_pDBAccess)) ;
		return -1;
	}



	mysql_commit((MYSQL *)m_pDBAccess);
	return 0;

}

int CDBOperation::UpdateScriptFlag(ST_TranData* stScriptData)
{
	char szSql[512] = { 0 } ;
	char szScriptDate[20] = { 0 } ;
	char szScriptDay[20] = { 0 } ;
	char szScriptTime[20] = { 0 } ;

	GetstrCurDate((char *)"-", szScriptDay);
	GetstrCurTime((char *)":", szScriptTime);

	sprintf(szScriptDate, "%s %s", szScriptDay, szScriptTime);

	sprintf(szSql, "UPDATE tab_scriptinfo SET IsNeedToSendScript='%s' ,SendTime=SendTime+1,SendDate='%s' WHERE termNo='%s' AND PlatSerialNo='%s'",
		stScriptData->nIsNeedToSendScript, szScriptDate, stScriptData->szTermNo, stScriptData->szOriginalSerNo);
G_DB_DEBUG("%d | %d | %s\n",pthread_self(), __LINE__, szSql);

	if(stScriptData->szTermNo[0] == '\0' || stScriptData->szOriginalSerNo[0] == '\0'  ){
		G_DB_DEBUG( (char *)"更新脚本结果失败[%s %s]", szSql) ;
		return -1;
	}
	
	if(mysql_query_get_res((MYSQL *)m_pDBAccess, szSql)){
		G_DB_DEBUG( (char *)"更新脚本结果失败[%s %s]", szSql, mysql_error((MYSQL *)m_pDBAccess)) ;
		return -1;
	}
	return 0;

}

/*
* 名称: GetICPKeyInfo
*
* 参数: 
*
* 返回值: >=0: 公钥条数
*		   -1: 没有
*	
* 说明: 获取公钥信息
*
*/
int CDBOperation::GetICPKeyInfo(ListICPKeyDataItem* pICPKeyDataList)
{
	int	nRetCode = 0 ;
	char	szSql[512] = { 0 } ;
	MYSQL_RES *res; 
	MYSQL_ROW row;


	sprintf( szSql, "select FULLVALUE from tab_icpkeyinfo");
G_DB_DEBUG("%d | %d | %s\n",pthread_self(), __LINE__, szSql);

	if(mysql_query_get_res((MYSQL *)m_pDBAccess, szSql, &res)){
		G_DB_DEBUG( (char *)"GetICPKeyInfo失败[%s %s]", szSql, mysql_error((MYSQL *)m_pDBAccess)) ;
		return -1;
	}	

	if (!res) { 
		G_DB_DEBUG( (char *)"GetICPKeyInfo失败[%s]",szSql);
		mysql_query_free_res((MYSQL *)m_pDBAccess, res);
		return -1;
	}

	nRetCode =  0 ;
	while ((row = mysql_fetch_row(res)) != NULL) { 
		ICPKeyDataItem* pICPkeyData = new ICPKeyDataItem ;
		mysql_get_field_key_val(res, row, "FULLVALUE",pICPkeyData->PKeyBuf, sizeof(pICPkeyData->PKeyBuf));
		pICPKeyDataList->push_back(pICPkeyData) ;
G_DB_DEBUG("%d | %d | %s\n",pthread_self(), __LINE__, pICPkeyData->PKeyBuf);
		nRetCode++ ;
	} 
	mysql_query_free_res((MYSQL *)m_pDBAccess, res);

	return nRetCode ;
	
}

void CDBOperation::RealeaseICPKeyItem(ListICPKeyDataItem* pListICPKeyDataItem)
{
	ListICPKeyDataItem::iterator it ;
	//释放每个元素
	for (it = pListICPKeyDataItem->begin(); it != pListICPKeyDataItem->end(); )
	{
		ListICPKeyDataItem::iterator tempIt = it++ ;
		if (*tempIt != NULL)
		{
			delete *tempIt ;
			*tempIt = NULL ;
		}
		pListICPKeyDataItem->erase(tempIt) ;
	}
}
/*
* 名称: GetICParmInfo
*
* 参数: 
*
* 返回值: >=0: 参数条数
*		   -1: 没有
*	
* 说明: 获取公钥信息
*
*/
int CDBOperation::GetICParamInfo(ListICParmDataItem* pICParmDataList)
{
	int	nRetCode = 0 ;
	char	szSql[512] = { 0 } ;
	MYSQL_RES *res; 
	MYSQL_ROW row;


	sprintf( szSql, "select FULLVALUE from tab_icparaminfo");
G_DB_DEBUG("%d | %d | %s\n",pthread_self(), __LINE__, szSql);

	if(mysql_query_get_res((MYSQL *)m_pDBAccess, szSql, &res)){
		G_DB_DEBUG( (char *)"GetICParamInfo失败[%s %s]", szSql, mysql_error((MYSQL *)m_pDBAccess)) ;
		return -1;
	}	

	if (!res) { 
		G_DB_DEBUG( (char *)"GetICParamInfo失败[%s]",szSql);
		mysql_query_free_res((MYSQL *)m_pDBAccess, res);
		return -1;
	}

	nRetCode =  0 ;
	while ((row = mysql_fetch_row(res)) != NULL) { 
		ICParmDataItem* pICParamData = new ICParmDataItem ;
		mysql_get_field_key_val(res, row, "FULLVALUE",pICParamData->PParmBuf, sizeof(pICParamData->PParmBuf));
		pICParmDataList->push_back(pICParamData) ;
G_DB_DEBUG("%d | %d | %s\n",pthread_self(), __LINE__, pICParamData->PParmBuf);
		nRetCode++ ;
	} 
	mysql_query_free_res((MYSQL *)m_pDBAccess, res);
	return nRetCode ;
}

void CDBOperation::RealeaseICParamItem(ListICParmDataItem* pListICParamDataItem)
{
	ListICParmDataItem::iterator it ;
	//释放每个元素
	for (it = pListICParamDataItem->begin(); it != pListICParamDataItem->end(); )
	{
		ListICParmDataItem::iterator tempIt = it++ ;
		if (*tempIt != NULL)
		{
			delete *tempIt ;
			*tempIt = NULL ;
		}
		pListICParamDataItem->erase(tempIt) ;
	}
}

//根据银行ID，获取银行名称
int CDBOperation::GetBankName(char* pOrganID, char *pBankName)
{
	int nRetCode = 0 ;
	char szSql[512] = { 0 } ;
	char szOraganID[5] = {0};
	MYSQL_RES *res; 
	MYSQL_ROW row;

	memcpy(szOraganID, pOrganID, 4);	//只取前4位
	
	sprintf(szSql,"select BankName from tab_bankorgan where BankOrganID='%s'",szOraganID);
G_DB_DEBUG("%d | %d | %s\n",pthread_self(), __LINE__, szSql);

	try
	{
		if(mysql_query_get_res((MYSQL *)m_pDBAccess, szSql, &res)){
			G_DB_DEBUG( (char *)"无银行卡信息[%s %s]", szSql, mysql_error((MYSQL *)m_pDBAccess)) ;
			return  -1;
		}

		if (!res) { 
			G_DB_DEBUG( (char *)"无银行卡信息[%s]",szSql);
			strcpy(pBankName, pOrganID);
			mysql_query_free_res((MYSQL *)m_pDBAccess, res);
			return -1;
		}
		if((row = mysql_fetch_row(res)) == NULL){
			G_DB_DEBUG( (char *)"无银行卡信息[%s |结果未找到]",szSql);
			strcpy(pBankName, pOrganID);
			mysql_query_free_res((MYSQL *)m_pDBAccess, res);
			return -1;
		}

		mysql_get_field_key_val(res, row, "BankName", pBankName, 128);
		mysql_query_free_res((MYSQL *)m_pDBAccess, res);
		
		return 0;
	}
	catch (...)
	{
		return -1;
	}
	return 0;
}

/*
* 名称: GetScrollInfo
*
* 参数: 
*	const char* pPsamID
*	char* pScrollInfo 
*
* 返回值: 0: 成功获取滚动条
*	 -1: 没有需要更新的滚动条
*	
* 说明: 获取滚动条信息
*
*/
int CDBOperation::GetScrollInfo( const char* pPsamID, char* pScrollInfo )
{
	int	nRetCode = 0 ;
	//long	lUpdateFlag = 0 ;
	char	szScrollInfo[128] = { 0 } ;
	char	szSql[256] = { 0 } ;
	MYSQL_RES *res; 
	MYSQL_ROW row;


	sprintf(szSql, "SELECT ScrollInfo FROM tab_term WHERE TermNo='%s' AND UpdateFlag=0", pPsamID);
G_DB_DEBUG("%d | %d | %s\n",pthread_self(), __LINE__, szSql);

	
	if(mysql_query((MYSQL *)m_pDBAccess, szSql)){
		G_DB_DEBUG( (char *)"GetScrollInfo失败[%s %s]", szSql, mysql_error((MYSQL *)m_pDBAccess)) ;
		mysql_rollback((MYSQL *)m_pDBAccess);
		return  -1;
	}
	
	res = mysql_use_result((MYSQL *)m_pDBAccess);
	if (!res) { 
		G_DB_DEBUG( (char *)"GetScrollInfo失败[%s]",szSql);
		mysql_query_free_res((MYSQL *)m_pDBAccess, res);
		mysql_commit((MYSQL *)m_pDBAccess);
		return -1;			
	}

	if((row = mysql_fetch_row(res)) == NULL){
		G_DB_DEBUG( (char *)"GetScrollInfo失败[%s |结果未找到]",szSql);
		mysql_query_free_res((MYSQL *)m_pDBAccess, res);
		mysql_commit((MYSQL *)m_pDBAccess);
		return -1;
	}
	strcpy(szScrollInfo, row[0]);
	mysql_query_free_res((MYSQL *)m_pDBAccess, res);
	
	if(strlen(szScrollInfo) < 64){
		sprintf(szSql, "UPDATE tab_term SET UpdateFlag = 1 WHERE TermNo='%s';", pPsamID);
G_DB_DEBUG("%d | %d | %s\n",pthread_self(), __LINE__, szSql);
		if(mysql_query((MYSQL *)m_pDBAccess, szSql)){
			G_DB_DEBUG( (char *)"GetScrollInfo失败[%s %s]", szSql, mysql_error((MYSQL *)m_pDBAccess)) ;
			mysql_rollback((MYSQL *)m_pDBAccess);
			return -1;
		}
		mysql_commit((MYSQL *)m_pDBAccess);
		return 0;
	}else{
		mysql_commit((MYSQL *)m_pDBAccess);
		return -1;
	}
	
	
}

int CDBOperation::GetLastTrade(ST_TranData* stTranData)
{
	char szSql[512] = { 0 };
	MYSQL_RES *res; 
	MYSQL_ROW row;

	sprintf(szSql,"select IssuerNo,AcquirerNo,BankRetCode,PlatTermNo,PlatMerchantNo,TranAmt,OutAccount,ReferenceNumber,BatchNo ,Replace(BankTime,':','') as BankTime,AuthorCode,PlatSerialNo,TranType,BankDate,AcquiringIDCode from tab_support where TermNo='%s' and BankRetCode='00' and CancelFlag=0 order by Bankdate desc,BankTime desc limit 0,1", stTranData->szTermNo);
G_DB_DEBUG("%d | %d | %s\n",pthread_self(), __LINE__, szSql);

	
	try
	{
		if(mysql_query_get_res((MYSQL *)m_pDBAccess, szSql, &res)){
			G_DB_DEBUG( (char *)"获取末笔信息失败[%s %s]", szSql, mysql_error((MYSQL *)m_pDBAccess)) ;
			memcpy(stTranData->szRetCode_39,"D5",2);
			return -1;
		}	

		if (!res) { 
			G_DB_DEBUG( (char *)"获取末笔信息失败[%s]",szSql);
			memcpy(stTranData->szRetCode_39,"D5",2);
			mysql_query_free_res((MYSQL *)m_pDBAccess, res);
			return -1;
		}
		if((row = mysql_fetch_row(res)) == NULL){
			G_DB_DEBUG( (char *)"获取末笔信息失败[%s |结果未找到]",szSql);
			memcpy(stTranData->szRetCode_39,"D5",2);
			mysql_query_free_res((MYSQL *)m_pDBAccess, res);
			return -1;
		}


		mysql_get_field_key_val(res, row, "OutAccount",stTranData->szOutAccount_2,sizeof(stTranData->szOutAccount_2));
		mysql_get_field_key_val(res, row, "TranAmt",stTranData->szAmount_4,sizeof(stTranData->szAmount_4));
		mysql_get_field_key_val(res, row, "ReferenceNumber",stTranData->szReferenceNumber_37,sizeof(stTranData->szReferenceNumber_37));
		mysql_get_field_key_val(res, row, "BatchNo",stTranData->szBatchNo,sizeof(stTranData->szBatchNo));
		mysql_get_field_key_val(res, row, "BankTime",stTranData->szBankTime_12,sizeof(stTranData->szBankTime_12));
		
		mysql_get_field_key_val(res, row, "BankDate",stTranData->szOriginalTranDate,sizeof(stTranData->szOriginalTranDate));
		mysql_get_field_key_val(res, row, "PlatSerialNo",stTranData->szTraceNumber_11,sizeof(stTranData->szOriginalSerNo));
		mysql_get_field_key_val(res, row, "TranType",stTranData->szTranType,sizeof(stTranData->szTranType));
		mysql_get_field_key_val(res, row, "AcquiringIDCode",stTranData->szAcquiringIDCode_32,sizeof(stTranData->szAcquiringIDCode_32));
		mysql_get_field_key_val(res, row, "PlatTermNo",stTranData->szTerminalID_41,sizeof(stTranData->szTerminalID_41));
		mysql_get_field_key_val(res, row, "PlatMerchantNo",stTranData->szMerchantID_42, sizeof(stTranData->szMerchantID_42));
		mysql_get_field_key_val(res, row, "AcquirerNo",stTranData->szAcquirerNo, sizeof(stTranData->szAcquirerNo));
		mysql_get_field_key_val(res, row, "IssuerNo",stTranData->szIssuerNo, sizeof(stTranData->szIssuerNo));

		mysql_query_free_res((MYSQL *)m_pDBAccess, res);
		return 0;
	}
	catch (...)
	{
		G_DB_DEBUG( (char *)"获取末笔信息失败[%s]",szSql);
		memcpy(stTranData->szRetCode_39,"D5",2);
		return -1;
	}
}

int CDBOperation::InsertICUnTouchUnLinkData(ICUnTouchUnLinkData* pICDataItem)
{
	char szTermDate[8+1] = { 0 } ;
	char szTermTime[8+1] = { 0 } ; 
	char szSerial[6+1] = { 0 } ;
	char szSql[1024] = { 0 };

	memcpy(szTermDate, pICDataItem->szReferenceNumber, 8) ;
	memcpy(szSerial, pICDataItem->szReferenceNumber+8, 6) ;
	sprintf(szSql, "insert into tab_icconsumetrade(TermNo, TradeMoney, TermDate, TermTime, TermSerNo, MerchantID, ICardData, OutAccount,CardSerno) VALUES('%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s')",
				pICDataItem->szTermID, pICDataItem->szMoney, szTermDate, "",szSerial, pICDataItem->szMerchantID, pICDataItem->sz55AreaBuff, 
				pICDataItem->szCardNum, pICDataItem->szICSerial) ;
G_DB_DEBUG("%d | %d | %s\n",pthread_self(), __LINE__, szSql);

	try
	{
		int	nRecordsAffected = 0 ;
		if (mysql_query_get_res((MYSQL *)m_pDBAccess, szSql) )
		{
			G_DB_DEBUG( (char *)"InsertParm失败[%s %s]", szSql, mysql_error((MYSQL *)m_pDBAccess)) ;
			return false ;
		}
		else
		{
			return true ;
		}
	}
	catch (...)
	{
		G_DB_DEBUG( (char *)"InsertParm fail[%s]",szSql) ;
		return -1;
	}

}


//通过银行卡号取出银行卡的发卡行名称
int CDBOperation::GetBankNameByCardNo(const char* pCardNo, char * pBankName) 
{
	char    szBankName[50] = { 0 } ;
	char	szSql[128] = { 0 } ;
	MYSQL_RES *res; 
	MYSQL_ROW row;

	sprintf(szSql, "select BankID,BankName,CardType,Len(CardBin) from tab_cardinfo where CardBin=left('%s', Len(CardBin))  order by Len(CardBin) desc limit 0,1", pCardNo) ;
G_DB_DEBUG("%d | %d | %s\n",pthread_self(), __LINE__, szSql);

	try
	{
		if(mysql_query_get_res((MYSQL *)m_pDBAccess, szSql, &res)){
			G_DB_DEBUG( (char *) "GetBankNameByBankNo 失败[%s %s]", szSql, mysql_error((MYSQL *)m_pDBAccess)) ;
			return -1;
		}	

		if (!res) { 
			G_DB_DEBUG( (char *) "GetBankNameByBankNo 失败[%s]",szSql) ;
			mysql_query_free_res((MYSQL *)m_pDBAccess, res);
			return -1;
			
		}
		if((row = mysql_fetch_row(res)) == NULL){
			G_DB_DEBUG( (char *) "GetBankNameByBankNo 失败[%s |结果未找到]",szSql) ;
			mysql_query_free_res((MYSQL *)m_pDBAccess, res);
			return -1;
		}


		mysql_get_field_key_val(res, row, "BankName",szBankName, sizeof(szBankName));
		mysql_query_free_res((MYSQL *)m_pDBAccess, res);
		if (strlen(szBankName) > 0)
		{
			memcpy(pBankName, szBankName, strlen(szBankName)) ;
		}
		else
		{
			G_DB_DEBUG( (char *)"GetBankNameByBankNo -- 取银行卡名称出错[%s]",szSql) ;
		}
		return 0 ;
	}
	catch(...)
	{
		G_DB_DEBUG( (char *)"GetBankNameByBankNo 发生异常[%s]",szSql) ;
		return -1 ;
	}
}

int CDBOperation::GetDayTrade(const char* pTermNo, const char* pTradeType, const char* pDate, long& lCount, long long& lSum) 
{
	char	szSql[256] = { 0 } ;
	MYSQL_RES *res; 
	MYSQL_ROW row;

	sprintf(szSql, "select count(ID) as TradeCount, Sum(TranAmt) as TradeSum from tab_support where TermNo='%s' and TranType='%s' and LocalDate='%s' and CancelFlag=0 and BankRetCode='00' ", pTermNo, pTradeType, pDate) ;
G_DB_DEBUG("%d | %d | %s\n",pthread_self(), __LINE__, szSql);

	try
	{
		if(mysql_query_get_res((MYSQL *)m_pDBAccess, szSql, &res)){
			LogTransMsg((char *)__FILE__, __LINE__, (char *) "GetDayTrade 失败[%s %s]", szSql, mysql_error((MYSQL *)m_pDBAccess)) ;
			return -1;
		}	

		if (!res) { 
			LogTransMsg((char *)__FILE__, __LINE__, (char *) "GetDayTrade 失败[%s]",szSql) ;
			//mysql_query_free_res((MYSQL *)m_pDBAccess, res);
			return -1;
		}
		if((row = mysql_fetch_row(res)) == NULL){
			LogTransMsg((char *)__FILE__, __LINE__, (char *) "GetDayTrade 失败[%s |结果未找到]",szSql) ;
			mysql_query_free_res((MYSQL *)m_pDBAccess, res);
			return -1;
		}

		lSum=0;
		mysql_get_field_key_val(res, row, "TradeCount",&lCount);
		if(lCount != 0)
			mysql_get_field_key_val(res, row, "TradeSum",&lSum);
		mysql_query_free_res((MYSQL *)m_pDBAccess, res);
		
		return 0 ;
	}
	catch(...)
	{
		LogTransMsg((char *)__FILE__, __LINE__, (char *)"GetDayTrade 发生异常[%s]",szSql) ;
		return -1 ;
	}
}


int CDBOperation::GetCardBindInfo(const char* pCardNo, char* pMobile, int& nRightFlag, int& nTryTimes, long long& lDaySum) 
{
	char	szSql[256] = { 0 } ;
	MYSQL_RES *res; 
	MYSQL_ROW row;

	sprintf(szSql, "select Mobile, DaySum, IsRight, ErrTimes from tab_cardbindmobile where CardNo='%s'", pCardNo) ;
G_DB_DEBUG("%d | %d | %s\n",pthread_self(), __LINE__, szSql);

	try
	{
		if(mysql_query_get_res((MYSQL *)m_pDBAccess, szSql, &res)){
			LogTransMsg((char *)__FILE__, __LINE__, (char *) "GetCardBindInfo 失败[%s %s]", szSql, mysql_error((MYSQL *)m_pDBAccess)) ;
			return -1;
		}	

		if (!res) { 
			LogTransMsg((char *)__FILE__, __LINE__, (char *) "GetCardBindInfo 失败[%s]",szSql) ;
			mysql_query_free_res((MYSQL *)m_pDBAccess, res);
			return -1;
		}
		if((row = mysql_fetch_row(res)) == NULL){
			LogTransMsg((char *)__FILE__, __LINE__, (char *) "GetCardBindInfo 失败[%s |结果未找到]",szSql) ;
			mysql_query_free_res((MYSQL *)m_pDBAccess, res);
			return -1;
		}

		mysql_get_field_key_val(res, row, "Mobile",pMobile, 11);
		mysql_get_field_key_val(res, row, "IsRight",&nRightFlag);
		mysql_get_field_key_val(res, row, "ErrTimes",&nTryTimes);
		mysql_get_field_key_val(res, row, "DaySum",&lDaySum);
		mysql_query_free_res((MYSQL *)m_pDBAccess, res);
		return 1 ;
	}
	catch(...)
	{
		LogTransMsg((char *)__FILE__, __LINE__, (char *)"GetCardBindInfo 发生异常[%s]",szSql) ;
		return -1 ;
	}
}


int CDBOperation::GetCreditBankList(st_bank_credit_t *bank_info, int *num)
{
	int	nRetCode = 0 ;
	char	szSql[512] = { 0 } ;
	MYSQL_RES *res; 
	MYSQL_ROW row;


	sprintf( szSql, "select SubTradeCode, BankNum, BankName, Memo from tab_creditcardbank");
G_DB_DEBUG("%d | %d | %s\n",pthread_self(), __LINE__, szSql);

	if(mysql_query_get_res((MYSQL *)m_pDBAccess, szSql, &res)){
		LogTransMsg((char *)__FILE__, __LINE__, (char *)" GetCreditBankList 失败[%s %s]", szSql, mysql_error((MYSQL *)m_pDBAccess)) ;
		return -1;
	}	

	if (!res) { 
		LogTransMsg((char *)__FILE__, __LINE__, (char *)"GetCreditBankList 失败[%s]",szSql);
		mysql_query_free_res((MYSQL *)m_pDBAccess, res);
		return -1;
	}

	nRetCode =  0 ;
	while ((row = mysql_fetch_row(res)) != NULL) { 
		
		mysql_get_field_key_val(res, row, "SubTradeCode",bank_info[nRetCode].subtradecode, 3);
		mysql_get_field_key_val(res, row, "BankNum",bank_info[nRetCode].banknum, 32);
		mysql_get_field_key_val(res, row, "BankName",bank_info[nRetCode].bankname, 32);
		mysql_get_field_key_val(res, row, "Memo",bank_info[nRetCode].memo, 256);
		nRetCode++ ;
	} 
	mysql_query_free_res((MYSQL *)m_pDBAccess, res);
	*num = nRetCode;
	return nRetCode ;

}
int CDBOperation::RecordMobileInfo(const char* pCardNo, const char* pMobile)
{
	char szDate[9] = {0};
	char szTime[7] = {0};
	char szSql[1024] = { 0 };
	GetstrCurDate( NULL,szDate);
	GetstrCurTime(NULL, szTime);
	
	sprintf(szSql,"insert into tab_cardbindmobile(CardNo, Mobile, DaySum, IsRight, ErrTimes, LastTime) VALUES('%s', '%s', 0, 0, 0, '%s%s')" ,
		pCardNo, pMobile, szDate, szTime);
G_DB_DEBUG("%d | %d | %s\n",pthread_self(), __LINE__, szSql);

	try
	{
		int	nRecordsAffected = 0 ;
		if (mysql_query_get_res((MYSQL *)m_pDBAccess, szSql) )
		{
			LogTransMsg((char *)__FILE__, __LINE__, (char *)"Tab_CardBindMobile失败[%s %s]", szSql, mysql_error((MYSQL *)m_pDBAccess)) ;
			return false ;
		}
		else
		{
			return true ;
		}
	}
	catch (...)
	{
		LogTransMsg((char *)__FILE__, __LINE__, (char *)"Tab_CardBindMobile fail[%s]",szSql) ;
		return -1;
	}
}

int CDBOperation::UpdateMobileErrTimes(const char* pCardNo)
{
	
	char szSql[512]={0};
	bool bRet=false;

	sprintf(szSql,"update tab_cardbindmobile set ErrTimes=ErrTimes+1 where CardNo='%s' ",pCardNo);
G_DB_DEBUG("%d | %d | %s\n",pthread_self(), __LINE__, szSql);

	if(mysql_query_get_res((MYSQL *)m_pDBAccess, szSql)){
		LogTransMsg((char *)__FILE__, __LINE__, (char *)"UpdateMobileErrTimes失败[%s %s]", szSql, mysql_error((MYSQL *)m_pDBAccess)) ;
		return -1;
	}

		
	return 0;
}

int CDBOperation::UpdateMobileErrTimes(const char* pCardNo, const char* pMoblie)
{
	
	char szSql[512]={0};
	bool bRet=false;

	sprintf(szSql,"update tab_cardbindmobile set Mobile='%s', ErrTimes=ErrTimes+1 where CardNo='%s' ", pMoblie, pCardNo);
G_DB_DEBUG("%d | %d | %s\n",pthread_self(), __LINE__, szSql);

	if(mysql_query_get_res((MYSQL *)m_pDBAccess, szSql)){
		LogTransMsg((char *)__FILE__, __LINE__, (char *)"UpdateMobileErrTimes失败[%s %s]", szSql, mysql_error((MYSQL *)m_pDBAccess)) ;
		return -1;
	}

	return 0;
}

int CDBOperation::UpdateCardBindMobile(const char* pCardNo, const char* pMoblie)
{
	char szSql[512]={0};
	bool bRet=false;

	sprintf(szSql,"update tab_cardbindmobile set Mobile='%s', IsRight=1 where CardNo='%s' ", pMoblie, pCardNo);
G_DB_DEBUG("%d | %d | %s\n",pthread_self(), __LINE__, szSql);


	if(mysql_query_get_res((MYSQL *)m_pDBAccess, szSql)){
		LogTransMsg((char *)__FILE__, __LINE__, (char *)"UpdateCardBindMobile失败[%s %s]", szSql, mysql_error((MYSQL *)m_pDBAccess)) ;
		return -1;
	}

	return 0;
}

int CDBOperation::UpdateMobileDaySum(const char* pCardNo, long long lMoney)
{
	
	char szSql[512]={0};
	bool bRet=false;

	sprintf(szSql,"update tab_cardbindmobile set DaySum=DaySum+%I64d where CardNo='%s' ", lMoney, pCardNo);
G_DB_DEBUG("%d | %d | %s\n",pthread_self(), __LINE__, szSql);

	if(mysql_query_get_res((MYSQL *)m_pDBAccess, szSql)){
		LogTransMsg((char *)__FILE__, __LINE__, (char *)"UpdateMobileErrTimes失败[%s %s]", szSql, mysql_error((MYSQL *)m_pDBAccess)) ;
		return -1;
	}


	return 0;
}


int CDBOperation::CheckNeedCardID(const char* pCardNo) 
{
	int nCheckHolderFlag = 0;
	char	szSql[256] = { 0 } ;
	MYSQL_RES *res; 
	MYSQL_ROW row;

	sprintf(szSql, "select CheckHolderFlag from tab_cardinfo where CardBin=left('%s', Len(CardBin)) and CardLen=%d order by CardBin desc limit 0,1", pCardNo, strlen(pCardNo)) ;
G_DB_DEBUG("%d | %d | %s\n",pthread_self(), __LINE__, szSql);

	try
	{
		if(mysql_query_get_res((MYSQL *)m_pDBAccess, szSql, &res)){
			LogTransMsg((char *)__FILE__, __LINE__, (char *) "CheckNeedCardID 失败[%s %s]", szSql, mysql_error((MYSQL *)m_pDBAccess)) ;
			return -1;
		}	

		if (!res) { 
			LogTransMsg((char *)__FILE__, __LINE__, (char *) "CheckNeedCardID 失败[%s]",szSql) ;
			mysql_query_free_res((MYSQL *)m_pDBAccess, res);
			return -1;
		}
		if((row = mysql_fetch_row(res)) == NULL){
			LogTransMsg((char *)__FILE__, __LINE__, (char *) "CheckNeedCardID 失败[%s |结果未找到]",szSql) ;
			mysql_query_free_res((MYSQL *)m_pDBAccess, res);
			return -1;
		}


		mysql_get_field_key_val(res, row, "CheckHolderFlag",&nCheckHolderFlag);
		mysql_query_free_res((MYSQL *)m_pDBAccess, res);
		
		return nCheckHolderFlag ;
	}
	catch(...)
	{
		LogTransMsg((char *)__FILE__, __LINE__, (char *)"CheckNeedCardID 发生异常[%s]",szSql) ;
		return -1 ;
	}
}
#endif

typedef struct mysql_res_{
	MYSQL_RES *res;
	int rowCount;
	MYSQL_FIELD *fields;
	int fieldsCount;
}mysqlRes;


int mysql_get_field_key_val(MYSQL_RES *res, MYSQL_ROW row, const char *fieldname, long long*val)
{
	if(res == NULL || row == NULL)return -1;
	int fieldcount = mysql_num_fields(res);
	int i;
	MYSQL_FIELD *field;
	for(i = 0; i < fieldcount; i++)
	{
		field = mysql_fetch_field_direct(res,i);
		if(field == NULL){
			return -1;
		}
		*val = 0L;
		if(strcmp(fieldname, field->name) == 0){
			if(row[i] == NULL)return -1;
			*val = atoll(row[i]);
			//strcpy(val, row[i]);
			return 0;
		}
	}
	return -1;
}

int mysql_get_field_key_val(MYSQL_RES *res, MYSQL_ROW row, const char *fieldname, long *val)
{
	if(res == NULL || row == NULL)return -1;
	int fieldcount = mysql_num_fields(res);
	int i;
	MYSQL_FIELD *field;
	for(i = 0; i < fieldcount; i++)
	{
		field = mysql_fetch_field_direct(res,i);
		if(field == NULL){
			return -1;
		}
		*val =0;
		if(strcmp(fieldname, field->name) == 0){
			if(row[i] == NULL)return -1;
			*val = atol(row[i]);
			//strcpy(val, row[i]);
			return 0;
		}
	}
	return -1;
}

int mysql_get_field_key_val(MYSQL_RES *res, MYSQL_ROW row, const char *fieldname, int *val)
{
	if(res == NULL || row == NULL)return -1;
	int fieldcount = mysql_num_fields(res);
	int i;
	MYSQL_FIELD *field;
	for(i = 0; i < fieldcount; i++)
	{
		field = mysql_fetch_field_direct(res,i);
		if(field == NULL){
			return -1;
		}
		*val = 0;
		if(strcmp(fieldname, field->name) == 0){
			if(row[i] == NULL)return -1;
			*val = atoi(row[i]);
			//strcpy(val, row[i]);
			return 0;
		}
	}
	return -1;
}
int mysql_get_field_key_val(MYSQL_RES *res, MYSQL_ROW row, const char *fieldname, char *val)
{
	if(res == NULL || row == NULL)return -1;
	int fieldcount = mysql_num_fields(res);
	int i;
	MYSQL_FIELD *field;
			
	for(i = 0; i < fieldcount; i++)
	{
		field = mysql_fetch_field_direct(res,i);
		if(field == NULL){
			return -1;
		}
		val[0]='\0';
		if(strcmp(fieldname, field->name) == 0){
			if(row[i] == NULL)return -1;
			//nBankIndex = atoi(row[index]);
			strcpy(val, row[i]);
			return 0;
		};
	}
	return -1;
}
int mysql_get_field_key_val(MYSQL_RES *res, MYSQL_ROW row, const char *fieldname, char *val, int val_len)
{
	if(res == NULL || row == NULL)return -1;
	int fieldcount = mysql_num_fields(res);
	int i;
	MYSQL_FIELD *field;
			
	for(i = 0; i < fieldcount; i++)
	{
		field = mysql_fetch_field_direct(res,i);
		if(field == NULL){
			return -1;
		}
		val[0]='\0';
		if(strcmp(fieldname, field->name) == 0){
			if(row[i] == NULL)return -1;
			//nBankIndex = atoi(row[index]);
			memcpy(val, row[i], val_len);
			return 0;
		};
	}
	return -1;
}
/*
*	入参:DB 数据指针
*			sql执行语句
*	出参:res返回结果集
*
*	返回值:-1　sql语句执行失败
*				>=0语句执行成功
*
**/
int mysql_query_get_res(MYSQL *DB, const  char *sql, int *res)
{
	int nRet;
/*
mysql_query()返回值为0时说明成功
否则失败
*/
	if(NULL == DB){
		//LogTransMsg((char *)__FILE__, __LINE__, (char *) "Failed to connect to mysql database.\n"); 
		return -1;
	}
	if(mysql_query(DB, sql)){
		mysql_rollback(DB);
		return -1;
	}
	//在多语句sql环境下，update数据不会立即执行
	mysql_commit(DB);
//	mysql_query(DB, "COMMIT");

	nRet = mysql_affected_rows(DB);
	
	*res=nRet;
	return 0;
}
/*
*	入参:DB 数据指针
*			sql执行语句
*	出参:res返回结果集
*
*	返回值:-1　sql语句执行失败
*				>=0语句执行成功
*
**/
int mysql_query_get_res(MYSQL *DB,const  char *sql)
{
	if(NULL == DB){
		//LogTransMsg((char *)__FILE__, __LINE__, (char *) "Failed to connect to mysql database.\n"); 
		return -1;
	}
	if(mysql_query(DB, sql)){
		mysql_rollback(DB);
		return -1;
	}
	mysql_commit(DB);
//	mysql_query(DB, "COMMIT");

	return 0;
	
}
/*
*	入参:DB 数据指针
*			sql执行语句
*	出参:res返回结果集
*
*	返回值:-1　sql语句执行失败
*				>=0语句执行成功
*
**/

int mysql_query_get_res(MYSQL *DB,const  char *sql, MYSQL_RES **res)
{
	if(NULL == DB){
		//LogTransMsg((char *)__FILE__, __LINE__, (char *) "Failed to connect to mysql database.\n"); 
		return -1;
	}
	int nRet;
	if(mysql_query(DB, sql)){
		mysql_rollback(DB);
		return -1;
	}
	mysql_commit(DB);
	//mysql_query(DB, "COMMIT");
	*res = mysql_store_result(DB);

	return 0;
}

int mysql_query_free_res(MYSQL *DB, MYSQL_RES *res)
{
	if(NULL == DB){
		return -1;
	}

	MYSQL_RES *new_res; 
	if(res)
		mysql_free_result(res);

	// 释放当前查询的所有结果集. 否则下次查询将会出错.
	while (!mysql_next_result(DB))
	{
		new_res = mysql_store_result(DB);
		if(new_res)
			mysql_free_result(new_res);
	}

	return 0;
}

int mysql_query_free_res(MYSQL *DB)
{
	if(NULL == DB){
		return -1;
	}

	MYSQL_RES *new_res; 

	// 释放当前查询的所有结果集. 否则下次查询将会出错.
	while (!mysql_next_result(DB))
	{
		new_res = mysql_store_result(DB);
		if(new_res)
			mysql_free_result(new_res);
	}

	return 0;
}

