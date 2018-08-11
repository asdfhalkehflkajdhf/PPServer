#include <pthread.h>  
#include <stdio.h>  
#include <unistd.h>  
#include <stdlib.h> 
#include "multiThreadVariable.h"

using  namespace  std;

pthread_m_V* pthread_m_V::m_instance =NULL;
pthread_m_V::pthread_m_V()
{
	pthread_key_create(&key,NULL);
}
pthread_m_V::pthread_m_V(void (*m_free)(void *))
{
	pthread_key_create(&key,m_free);
}

pthread_m_V::~pthread_m_V()
{
	pthread_key_delete(key);
}
pthread_m_V* pthread_m_V::Instance()
{
	if( NULL == m_instance )
	{
		m_instance = new pthread_m_V();
	}
	
	return m_instance ;
}

pthread_m_V* pthread_m_V::Instance(void (*m_free)(void *))
{
	if( NULL == m_instance )
	{
		m_instance = new pthread_m_V(m_free);
	}
	
	return m_instance ;
}
void  pthread_m_V::Destory()
{
	if(NULL != m_instance)
	{
		delete m_instance ;
		m_instance = NULL ;
	}
}

void pthread_m_V::set_m(void *p)
{
	pthread_setspecific(key,p);
}
void *pthread_m_V::get_m()
{
	return pthread_getspecific(key);
}


/*定义多线程变量存放到公共文件中*/
//pthread_m_V PMVDBVar;
//pthread_m_V PMVSTTDVar;

pthread_key_t  PMVDBVar;
pthread_key_t  PMVSTTDVar;
pthread_key_t  PMVBUFFERVar;

