

#if !defined(_DBOPERATION_H__INCLUDED_)
#define _DBOPERATION_H__INCLUDED_


//#include "ICPKeyParmInfo.h"
//#include "SelfTypeDefine.h"
//#include "TranDataDeclare.h"


//错误日志类型枚举定义
enum{PLAT_LOG=1, THIR_LOG=2, TERM_LOG=3};
//tab_creditcardbank 数据结构
typedef struct st_bank_credit_{
	char subtradecode[6];
	char bankname[32];
	char banknum[32];
	char memo[256];
}st_bank_credit_t;

class CDBOperation  
{
public:
	CDBOperation() ;
	~CDBOperation() ;
private:
	static CDBOperation* m_instance ;
	int survival_time;

public:
	static CDBOperation* Instance();
	void  Destory();


	//int GetErrorMsgFromCode( char* pErrorCode, char* pErrorMsg ) ;

	bool OpenDB() ;
	bool OpenDB( const char *host, const char *user, const char *passwd, const char *db, unsigned int port, const char *unix_socket);

	void CloseDB();

	int isNeedToHandle(char *dir, char *host, char *page);
	int CrawlerRecord(char *dir, char *host, char *page);

#if 0

	/**************************************************************************************************
	* + 名称: UpdateBankWorkKey
	* +
	* + 参数: 
	* +
	* + 返回值: 0：成功
	* +
	* + 说明: 更新银行工作密钥
	* +
	* + 作者: 王月
	* +
	* + 日期: 2013-11-09
    ***********************************************************************************************/
	int	UpdateBankWorkKey(ST_TranData* stpTranData) ;	
	int UpdateBankWorkKey(char* pMerchantID, char* pTermID, char* pPinKey, char* pMacKey, char* pBatchNo);

	int GetBankWorkKey(char* pMerchantID, char* pTermID, int& nBankIndex, char* pPinKey, char* pMacKey, char* pTmkKey);
	
	/**************************************************************************************************
	* + 名称: UpdateTermWorkKey
	* +
	* + 参数: 
	* +
	* + 返回值: 0：成功
	* +
	* + 说明: 更新终端作密钥
	* +
	* + 作者: 王月
	* +
	* + 日期: 2013-11-09
    ***********************************************************************************************/
	int	UpdateTermWorkKey(ST_TranData* stpTranData) ;	

	/**************************************************************************************************
	* + 名称: GetTermInfo
	* +
	* + 参数: 
	* +
	* + 返回值: 0：成功 其他：失败
	* +
	* + 说明: 获取终端信息
	* +
	* + 作者: 王月
	* +
	* + 日期: 2013-11-09
    ***********************************************************************************************/
	int GetTermInfo(ST_TranData* stpTranData);

	/**************************************************************************************************
	* + 名称: GetBankSerialNo
	* +
	* + 参数: 
	* +
	* + 返回值: 0：成功 其他：失败
	* +
	* + 说明: 获取平台流水号
	* +
	* + 作者: 王月
	* +
	* + 日期: 2013-11-09
    ***********************************************************************************************/

	int GetBankSerialNo(ST_TranData* stTranData);
	int GetBankSerialNo(const char* pPlatMerchantID, const char* pPlatTermNo, char* pSerialNo, char* pBatchNo);
	/**************************************************************************************************
	* + 名称: GetOriginalTranData
	* +
	* + 参数: 
	* +
	* + 返回值: 0：成功 其他：失败
	* +
	* + 说明: 获取原始交易信息
	* +
	* + 作者: 王月
	* +
	* + 日期: 2013-11-09
    ***********************************************************************************************/
	int GetOriginalTranData(ST_TranData* stTranData);

	/**************************************************************************************************
	* + 名称: GetErrInfo
	* +
	* + 参数: 
	* +
	* + 返回值: 无
	* +
	* + 说明: 获取错误码的实际意义信息
	* +
	* + 作者: 王月
	* +
	* + 日期: 2013-11-09
    ***********************************************************************************************/
	void GetErrInfo(const char* pErrCode,char* pErrInfo);

	/**************************************************************************************************
	* + 名称: GetErrShowInfo
	* +
	* + 参数: 
	* +
	* + 返回值: 无
	* +
	* + 说明: 获取错误码的显示信息
	* +
	* + 作者: 王月
	* +
	* + 日期: 2013-11-09
    ***********************************************************************************************/
	void GetErrShowInfo(const char* pErrCode,char* pErrShowInfo);

	/**************************************************************************************************
	* + 名称: RecordTranData
	* +
	* + 参数: 
	* +
	* + 返回值: 无
	* +
	* + 说明: 记账
	* +
	* + 作者: 王月
	* +
	* + 日期: 2013-11-09
    ***********************************************************************************************/
	void RecordTranData(ST_TranData* stpTranData);


	/**************************************************************************************************
	* + 名称: RecordCancelInfo
	* +
	* + 参数: 
	* +
	* + 返回值: 0：成功
	* +
	* + 说明: 记录冲正信息
	* +
	* + 作者: 王月
	* +
	* + 日期: 2013-11-09
    ***********************************************************************************************/

	int RecordCancelInfo(ST_TranData* stpTranData);
	/**************************************************************************************************
	* + 名称: RecordScriptData
	* +
	* + 参数: 
	* +
	* + 返回值: 0：成功
	* +
	* + 说明: 记录脚本信息
	* +
	* + 作者: 王月
	* +
	* + 日期: 2013-11-29
    ***********************************************************************************************/

	void RecordScriptData(ST_TranData* stpTranData) ;
	
	/**************************************************************************************************
	* + 名称: RecordTCResult
	* +
	* + 参数: 
	* +
	* + 返回值: 0：成功
	* +
	* + 说明: 获取Tc值信息
	* +
	* + 作者: 王月
	* +
	* + 日期: 2013-11-09
    ***********************************************************************************************/
	int RecordTCResult(ST_TranData* stpTranData) ;

	/**************************************************************************************************
	* + 名称: GetCancelInfo
	* +
	* + 参数: 
	* +
	* + 返回值: 0：成功
	* +
	* + 说明: 获取冲正信息
	* +
	* + 作者: 王月
	* +
	* + 日期: 2013-11-09
    ***********************************************************************************************/

	int GetCancelInfo(ST_TranData* stpTranData,ST_CancelData* stCancelData);
	int GetCreditBankList(st_bank_credit_t *bank_info, int *num);

	/**************************************************************************************************
	* + 名称: GetScriptInfo
	* +
	* + 参数: 
	* +
	* + 返回值: 0：成功
	* +
	* + 说明: 获取脚本执行结果信息
	* +
	* + 作者: 王月
	* +
	* + 日期: 2013-11-28
    ***********************************************************************************************/
	int GetScriptInfo(ST_TranData* stpTranData);

	/**************************************************************************************************
	* + 名称: UpdateCancelFlag
	* +
	* + 参数: 
	* +
	* + 返回值: 0：成功
	* +
	* + 说明: 更新冲正标志
	* +
	* + 作者: 王月
	* +
	* + 日期: 2013-11-09
    ***********************************************************************************************/
	int UpdateCancelFlag(ST_CancelData* stCancelData);
	int UpdateScriptFlag(ST_TranData* stScriptData) ;

 	int GetICParamInfo(ListICParmDataItem* pICParmDataList);
 	int GetICPKeyInfo(ListICPKeyDataItem* pICPKeyDataList);

	void RealeaseICPKeyItem(ListICPKeyDataItem* pListICPKeyDataItem);
	void RealeaseICParamItem(ListICParmDataItem* pListICParamDataItem);
	/**************************************************************************************************
	* + 名称: InsertPKey
	* +
	* + 参数: 
	* +
	* + 返回值: 0：成功
	* +
	* + 说明: 插入 公开密钥
	* +
	* + 作者: 王月
	* +
	* + 日期: 2013-11-09
    ***********************************************************************************************/
	int InsertPKey(ICPKeyDataItem* pICPKeyDataItem);
	/**************************************************************************************************
	* + 名称: InsertParm
	* +
	* + 参数: 
	* +
	* + 返回值: 0：成功
	* +
	* + 说明: 插入 参数
	* +
	* + 作者: 王月
	* +
	* + 日期: 2013-11-09
    ***********************************************************************************************/
	int InsertParm(ICParmDataItem* pICParamItem) ;
	/**************************************************************************************************
	* + 名称: GetBankName
	* +
	* + 参数: 
	* +
	* + 返回值: 0：成功
	* +
	* + 说明: 获取发卡行
	* +
	* + 作者: 王月
	* +
	* + 日期: 2013-11-09
    ***********************************************************************************************/
	int GetBankName(char* szOrganID, char *pBankName);

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
	int GetScrollInfo( const char* pPsamID, char* pScrollInfo ) ;
		
	/*
	* 名称: GetLastTrade
	*
	* 参数: 
	*	ST_TranData* stTranData 
	*
	*	
	* 说明: 获取末笔交易查询
	*
	*/
	int GetLastTrade(ST_TranData* stTranData) ;

	/*
	* 名称: InsertICUnTouchUnLinkData
	*
	* 参数: 
	*	ListICUnTouchUnLinkDataItem* pListICData
	*
	*	
	* 说明: 将脱机交易数据保存到数据库
	*
	*/
	int InsertICUnTouchUnLinkData(ICUnTouchUnLinkData* PICDataItem) ;
	
	/*
	* 说明： 通过卡号获取银行卡名称
	*/
	int GetBankNameByCardNo(const char* pCardNo, char * pBankName) ;

	int GetDayTrade(const char* pTermNo, const char* pTradeType, const char* pDate, long& lCount, long long int& lSum) ;

	int GetCardBindInfo(const char* pCardNo, char* pMobile, int& nRightFlag, int& nTryTimes, long long int& lDaySum);

	int RecordMobileInfo(const char* pCardNo, const char* pMobile);

	int UpdateMobileErrTimes(const char* pCardNo);

	int UpdateMobileErrTimes(const char* pCardNo, const char* pMoblie);

	int UpdateCardBindMobile(const char* pCardNo, const char* pMoblie);

	int UpdateMobileDaySum(const char* pCardNo, long long int lMoney);

	int CheckNeedCardID(const char* pCardNo);
#endif

private:

	void* m_pDBAccess ;
};



#endif // !defined(_DBOPERATION_H__INCLUDED_)
