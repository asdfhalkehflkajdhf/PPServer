#ifndef __MULTI_THREAD_VARIABLE_H__
#define __MULTI_THREAD_VARIABLE_H__

#ifdef __cplusplus 
extern "C" {
#endif


class pthread_m_V
{
private:
	static pthread_m_V* m_instance ;
	pthread_key_t  key;
public:
	pthread_m_V();
	pthread_m_V(void (*m_free)(void *));
	virtual ~pthread_m_V() ;
	
	static pthread_m_V* Instance();
	static pthread_m_V* Instance(void (*m_free)(void *));
	static void  Destory();
	
	void set_m(void *p);
	void *get_m();
};

//extern pthread_m_V PMVDBVar;
//extern pthread_m_V PMVSTTDVar;
extern pthread_key_t  PMVDBVar;
extern pthread_key_t  PMVSTTDVar;
extern pthread_key_t  PMVBUFFERVar;


#ifdef __cplusplus
}
#endif 

#endif

