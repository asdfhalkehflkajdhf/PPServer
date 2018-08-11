

#if !defined(_DBOPERATION_H__INCLUDED_)
#define _DBOPERATION_H__INCLUDED_


//#include "ICPKeyParmInfo.h"
//#include "SelfTypeDefine.h"
//#include "TranDataDeclare.h"


//������־����ö�ٶ���
enum{PLAT_LOG=1, THIR_LOG=2, TERM_LOG=3};
//tab_creditcardbank ���ݽṹ
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
	* + ����: UpdateBankWorkKey
	* +
	* + ����: 
	* +
	* + ����ֵ: 0���ɹ�
	* +
	* + ˵��: �������й�����Կ
	* +
	* + ����: ����
	* +
	* + ����: 2013-11-09
    ***********************************************************************************************/
	int	UpdateBankWorkKey(ST_TranData* stpTranData) ;	
	int UpdateBankWorkKey(char* pMerchantID, char* pTermID, char* pPinKey, char* pMacKey, char* pBatchNo);

	int GetBankWorkKey(char* pMerchantID, char* pTermID, int& nBankIndex, char* pPinKey, char* pMacKey, char* pTmkKey);
	
	/**************************************************************************************************
	* + ����: UpdateTermWorkKey
	* +
	* + ����: 
	* +
	* + ����ֵ: 0���ɹ�
	* +
	* + ˵��: �����ն�����Կ
	* +
	* + ����: ����
	* +
	* + ����: 2013-11-09
    ***********************************************************************************************/
	int	UpdateTermWorkKey(ST_TranData* stpTranData) ;	

	/**************************************************************************************************
	* + ����: GetTermInfo
	* +
	* + ����: 
	* +
	* + ����ֵ: 0���ɹ� ������ʧ��
	* +
	* + ˵��: ��ȡ�ն���Ϣ
	* +
	* + ����: ����
	* +
	* + ����: 2013-11-09
    ***********************************************************************************************/
	int GetTermInfo(ST_TranData* stpTranData);

	/**************************************************************************************************
	* + ����: GetBankSerialNo
	* +
	* + ����: 
	* +
	* + ����ֵ: 0���ɹ� ������ʧ��
	* +
	* + ˵��: ��ȡƽ̨��ˮ��
	* +
	* + ����: ����
	* +
	* + ����: 2013-11-09
    ***********************************************************************************************/

	int GetBankSerialNo(ST_TranData* stTranData);
	int GetBankSerialNo(const char* pPlatMerchantID, const char* pPlatTermNo, char* pSerialNo, char* pBatchNo);
	/**************************************************************************************************
	* + ����: GetOriginalTranData
	* +
	* + ����: 
	* +
	* + ����ֵ: 0���ɹ� ������ʧ��
	* +
	* + ˵��: ��ȡԭʼ������Ϣ
	* +
	* + ����: ����
	* +
	* + ����: 2013-11-09
    ***********************************************************************************************/
	int GetOriginalTranData(ST_TranData* stTranData);

	/**************************************************************************************************
	* + ����: GetErrInfo
	* +
	* + ����: 
	* +
	* + ����ֵ: ��
	* +
	* + ˵��: ��ȡ�������ʵ��������Ϣ
	* +
	* + ����: ����
	* +
	* + ����: 2013-11-09
    ***********************************************************************************************/
	void GetErrInfo(const char* pErrCode,char* pErrInfo);

	/**************************************************************************************************
	* + ����: GetErrShowInfo
	* +
	* + ����: 
	* +
	* + ����ֵ: ��
	* +
	* + ˵��: ��ȡ���������ʾ��Ϣ
	* +
	* + ����: ����
	* +
	* + ����: 2013-11-09
    ***********************************************************************************************/
	void GetErrShowInfo(const char* pErrCode,char* pErrShowInfo);

	/**************************************************************************************************
	* + ����: RecordTranData
	* +
	* + ����: 
	* +
	* + ����ֵ: ��
	* +
	* + ˵��: ����
	* +
	* + ����: ����
	* +
	* + ����: 2013-11-09
    ***********************************************************************************************/
	void RecordTranData(ST_TranData* stpTranData);


	/**************************************************************************************************
	* + ����: RecordCancelInfo
	* +
	* + ����: 
	* +
	* + ����ֵ: 0���ɹ�
	* +
	* + ˵��: ��¼������Ϣ
	* +
	* + ����: ����
	* +
	* + ����: 2013-11-09
    ***********************************************************************************************/

	int RecordCancelInfo(ST_TranData* stpTranData);
	/**************************************************************************************************
	* + ����: RecordScriptData
	* +
	* + ����: 
	* +
	* + ����ֵ: 0���ɹ�
	* +
	* + ˵��: ��¼�ű���Ϣ
	* +
	* + ����: ����
	* +
	* + ����: 2013-11-29
    ***********************************************************************************************/

	void RecordScriptData(ST_TranData* stpTranData) ;
	
	/**************************************************************************************************
	* + ����: RecordTCResult
	* +
	* + ����: 
	* +
	* + ����ֵ: 0���ɹ�
	* +
	* + ˵��: ��ȡTcֵ��Ϣ
	* +
	* + ����: ����
	* +
	* + ����: 2013-11-09
    ***********************************************************************************************/
	int RecordTCResult(ST_TranData* stpTranData) ;

	/**************************************************************************************************
	* + ����: GetCancelInfo
	* +
	* + ����: 
	* +
	* + ����ֵ: 0���ɹ�
	* +
	* + ˵��: ��ȡ������Ϣ
	* +
	* + ����: ����
	* +
	* + ����: 2013-11-09
    ***********************************************************************************************/

	int GetCancelInfo(ST_TranData* stpTranData,ST_CancelData* stCancelData);
	int GetCreditBankList(st_bank_credit_t *bank_info, int *num);

	/**************************************************************************************************
	* + ����: GetScriptInfo
	* +
	* + ����: 
	* +
	* + ����ֵ: 0���ɹ�
	* +
	* + ˵��: ��ȡ�ű�ִ�н����Ϣ
	* +
	* + ����: ����
	* +
	* + ����: 2013-11-28
    ***********************************************************************************************/
	int GetScriptInfo(ST_TranData* stpTranData);

	/**************************************************************************************************
	* + ����: UpdateCancelFlag
	* +
	* + ����: 
	* +
	* + ����ֵ: 0���ɹ�
	* +
	* + ˵��: ���³�����־
	* +
	* + ����: ����
	* +
	* + ����: 2013-11-09
    ***********************************************************************************************/
	int UpdateCancelFlag(ST_CancelData* stCancelData);
	int UpdateScriptFlag(ST_TranData* stScriptData) ;

 	int GetICParamInfo(ListICParmDataItem* pICParmDataList);
 	int GetICPKeyInfo(ListICPKeyDataItem* pICPKeyDataList);

	void RealeaseICPKeyItem(ListICPKeyDataItem* pListICPKeyDataItem);
	void RealeaseICParamItem(ListICParmDataItem* pListICParamDataItem);
	/**************************************************************************************************
	* + ����: InsertPKey
	* +
	* + ����: 
	* +
	* + ����ֵ: 0���ɹ�
	* +
	* + ˵��: ���� ������Կ
	* +
	* + ����: ����
	* +
	* + ����: 2013-11-09
    ***********************************************************************************************/
	int InsertPKey(ICPKeyDataItem* pICPKeyDataItem);
	/**************************************************************************************************
	* + ����: InsertParm
	* +
	* + ����: 
	* +
	* + ����ֵ: 0���ɹ�
	* +
	* + ˵��: ���� ����
	* +
	* + ����: ����
	* +
	* + ����: 2013-11-09
    ***********************************************************************************************/
	int InsertParm(ICParmDataItem* pICParamItem) ;
	/**************************************************************************************************
	* + ����: GetBankName
	* +
	* + ����: 
	* +
	* + ����ֵ: 0���ɹ�
	* +
	* + ˵��: ��ȡ������
	* +
	* + ����: ����
	* +
	* + ����: 2013-11-09
    ***********************************************************************************************/
	int GetBankName(char* szOrganID, char *pBankName);

	/*
	* ����: GetScrollInfo
	*
	* ����: 
	*	const char* pPsamID
	*	char* pScrollInfo 
	*
	* ����ֵ: 0: �ɹ���ȡ������
	*	 -1: û����Ҫ���µĹ�����
	*	
	* ˵��: ��ȡ��������Ϣ
	*
	*/
	int GetScrollInfo( const char* pPsamID, char* pScrollInfo ) ;
		
	/*
	* ����: GetLastTrade
	*
	* ����: 
	*	ST_TranData* stTranData 
	*
	*	
	* ˵��: ��ȡĩ�ʽ��ײ�ѯ
	*
	*/
	int GetLastTrade(ST_TranData* stTranData) ;

	/*
	* ����: InsertICUnTouchUnLinkData
	*
	* ����: 
	*	ListICUnTouchUnLinkDataItem* pListICData
	*
	*	
	* ˵��: ���ѻ��������ݱ��浽���ݿ�
	*
	*/
	int InsertICUnTouchUnLinkData(ICUnTouchUnLinkData* PICDataItem) ;
	
	/*
	* ˵���� ͨ�����Ż�ȡ���п�����
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
