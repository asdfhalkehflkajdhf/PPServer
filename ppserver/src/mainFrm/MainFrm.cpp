/*
* Copyright(c) 2010,  
* All rights 
*
* 文件名称:  MainFrm.cpp
* 摘    要:  
*
* 当前版本:  1.0
* 作    者:  牛永波，姜福来
* 完成日期:  2010年03月07日
*/

#include <netinet/in.h>
#include <netdb.h> //struct hostent

#include <sys/mman.h> 

#include <arpa/inet.h>

#include <sys/types.h>  
#include <sys/stat.h>  
#include <sys/socket.h>
#include <fcntl.h>  

#include <unistd.h>  
#include <stdio.h>  
#include <string.h>  
#include <stdlib.h>  
#include <errno.h>  
#include <locale.h>  

#include <time.h>
#include <pthread.h>
#include <string.h>
#include <signal.h>



#include "Config.h"
#include "MainFrm.h"
#include "log.h"
#include "multiThreadVariable.h"
#include "utils.h"
#include "encapsulate_queue.h"
#include "DBOperation.h"

CConfig *GConfig;
CLog *GLog ;



//char httpheader[40960];


queue_s g_webQueue;


#define bzero(p, s) memset(p, 0, s)  
#define USERAGENT "Wget/1.10.2"  
#define ACCEPT "*/*"  
#define ACCEPTLANGUAGE "zh-cn,zh;q=0.5"  
#define ACCEPTENCODING "gzip,deflate"  
#define ACCEPTCHARSET "gb2312,utf-8;q=0.7,*;q=0.7"  
#define KEEPALIVE "300"  
#define CONNECTION "keep-alive"  
#define CONTENTTYPE "application/x-www-form-urlencoded"  
  
#define MAXFILENAME 14  


static int g_main_debug_switch = 1;
#define G_MAIN_DEBUG(format,...) \
if (g_main_debug_switch ){                                    \
	printf(format, ##__VA_ARGS__);\
}

/************************************************************** 
功能：为 myanchor 分配 len 大小的内存 
***************************************************************/  
void GetMemory(char ** myanchor, int len)  
{  
	len = len>0?len:-len;
    if(!(*myanchor))  
        (*myanchor) = (char *)malloc(len + 1);  
    else{
		free((*myanchor));
		(*myanchor) = NULL;
        (*myanchor) = (char *)malloc(len + 1);  
	}
	if((*myanchor))
		memset((*myanchor), 0, len + 1); 
	else
		printf("malloc error [%d]\n", len);
}  

/************************************************************** 
功能：在字符串 s 里搜索 x 字符，并设置指针 d 指向该位置 
***************************************************************/  
void Rstrchr(char * s, int x, char ** d)  
{  
    int len = strlen(s) - 1;  
    while(len >= 0)  
    {  
        if(x == s[len]) {(*d) = s + len; return;}  
        len--;  
    }  
    (*d) = 0;  
} 

/************************************************************** 
功能：从字符串 src 中分析出网站地址和端口，并得到文件和目录 
***************************************************************/  
int GetHost(char * src, char * web, char * file, int * port, char * dir)  
{  
    char * pA, * pB, * pC;  
    int len;  
  
	char e[3] = "@/";
    *port = 0;  
    if(!(*src))  return -1;  
    pA = src;  
    if(!strncmp(pA, "http://", strlen("http://")))  pA = src+strlen("http://");  
    else return 1;  
    pB = strchr(pA, '/');  
    if(pB)  
    {  
        len = strlen(pA) - strlen(pB);  
        //GetMemory(web, len);  
        memcpy((web), pA, len);  
        if(*(pB+1))  
        {  
            Rstrchr(pB + 1, '/', &pC);  
            if(pC) len = strlen(pB + 1) - strlen(pC);  
            else len = 0;  
            if(len > 0)  
            {  
                //GetMemory(dir, len);  
                memcpy((dir), pB + 1, len);  
  
                if(pC + 1)  
                {  
                    len = strlen(pC + 1);  
                    //GetMemory(file, len);  
                    memcpy((file), pC + 1, len);  
                }  
                else  
                {  
                    len = 1;  
                    //GetMemory(file, len);  
                    memcpy((file), e, len);  
                }// else  
            }// if(len > 0)  
            else  
            {  
                len = 1;  
                //GetMemory(dir, len);  
                memcpy((dir), e + 1, len);  
  
                len = strlen(pB + 1);  
                //GetMemory(file, len);  
                memcpy((file), pB + 1, len);  
            }// else  
        }// if(*(pB+1))  
        else  
        {  
            len = 1;  
            //GetMemory(dir, len);  
            memcpy((dir), e + 1, len);  
  
            len = 1;  
            //GetMemory(file, len);  
            memcpy((file), e, len);  
        }// else  
    }// if(pB)  
    else  
    {  
        len = strlen(pA);  
        //GetMemory(web, len);  
        memcpy((web), pA, strlen(pA));  
        len = 1;  
        //GetMemory(dir, len);  
        memcpy((dir), e + 1, len);  
        len = 1;  
        //GetMemory(file, len);  
        memcpy((file), e, len);  
    }// else  
  
    pA = strchr((web), ':');  
    if(pA)  *port = atoi(pA + 1);  
    else *port = 80;  
  
    return 0;  
}  


int UrlFilter(char *src)
{

	int i;

	
    if(!src)        return 1;  

	for(i=0;i<GConfig->urlFilter.count; ++i){
		if(strstr(src, GConfig->urlFilter.str_array[i]))
			return 1;  
	}
	/*
    if(strstr(src, ".css"))        return 1;  
    if(strstr(src, ".xml"))        return 1;  
    if(strstr(src, ".ico"))        return 1;  
    if(strstr(src, ".jpg"))        return 1;  
    if(strstr(src, ".gif"))        return 1;  
    if(strstr(src, "javascript:"))        return 1;  
    if(strstr(src, "+"))        return 1;  
	*/
	return 0;
}
/************************************************************** 
功能：设置 HTTP 协议头内容的一些固定值 
***************************************************************/  
int SetLocalAgent(char * UserAgent, char * Accept, char * AcceptLanguage, char * AcceptEncoding, char * AcceptCharset, char * KeepAlive, char * Connection, char * ContentType)  
{  
    memcpy(UserAgent, USERAGENT, strlen(USERAGENT));  
    memcpy(Accept, ACCEPT, strlen(ACCEPT));  
    memcpy(AcceptLanguage, ACCEPTLANGUAGE, strlen(ACCEPTLANGUAGE));  
    memcpy(AcceptEncoding, ACCEPTENCODING, strlen(ACCEPTENCODING));  
    memcpy(AcceptCharset, ACCEPTCHARSET, strlen(ACCEPTCHARSET));  
    memcpy(KeepAlive, KEEPALIVE, strlen(KEEPALIVE));  
    memcpy(Connection, CONNECTION, strlen(CONNECTION));  
    memcpy(ContentType, CONTENTTYPE, strlen(CONTENTTYPE));  
    return 0;  
}  

int InitWebCrawler(char *url)
{
	pthread_spin_init(&g_webQueue.lock, PTHREAD_PROCESS_PRIVATE);
	g_webQueue.size=0;
	TAILQ_INIT(&g_webQueue.head);
	strcpy(g_webQueue.name, "webQueue");

    int WebPort;  
	int ret;
	int i;
    char WebHost[256] = {0};
	char PageAddress[256] = {0};
	char WebDir[256] = {0};  

	CLog *Log = CLog::Instance();

	char *rootUrl;
	if(url)
		rootUrl = url;
	else
		rootUrl = GConfig->rootUrl;

	//检测url
	ret = UrlFilter(rootUrl);
	if(ret) { Log->Logf("Get Root Host error from '%s'\n", rootUrl);exit(1);}
	//加入第一个web结点
	ret = GetHost(rootUrl, WebHost, PageAddress, &WebPort, WebDir); /* Get web page info */
	if(ret) { Log->Logf("Get Root Host error from '%s'\n", rootUrl);exit(1);}
	queue_node_add(&g_webQueue, WebHost, WebPort, WebDir, PageAddress);
    //free(WebHost); free(PageAddress);free(WebDir);  

	return 0;
	
}



  
/************************************************************** 
功能：连接一个网站服务器 
***************************************************************/  
int ConnectWeb(NodeQueue *NodeCurr)  
{ 
	struct sockaddr_in server_addr;  
	CLog *Log = CLog::Instance();
	int sockfd;


    /* connect to web server */  
    /* create a socket descriptor */  
    if((sockfd=socket(PF_INET,SOCK_STREAM,0))==-1)  
    {  
		G_MAIN_DEBUG("%d | %d | %s Socket Error\n", pthread_self(), __LINE__, __FILE__);
        Log->Logf("%d\tSocket Error:%s\a\n",__LINE__, strerror(errno));  
        return -1;  
    }  
  
    /* bind address */  
    bzero(&server_addr, sizeof(server_addr));  
    server_addr.sin_family = AF_INET;  
    server_addr.sin_port = htons(NodeCurr->port);  
//    server_addr.sin_addr = h_addr;

	//通过host解析出ip地址
	if(isIPValid(NodeCurr->host)){
		G_MAIN_DEBUG("%d | %d | %s is IP %s \n", pthread_self(), __LINE__, __FILE__, NodeCurr->host); 

		//server_addr.sin_addr = (struct in_addr)inet_addr(NodeCurr->host); 
		unsigned long p3;
		p3 =  inet_addr(NodeCurr->host);
		server_addr.sin_addr = *(struct in_addr *)&p3;
	}else{

		struct hostent *host_ent=NULL;
		if((host_ent=gethostbyname(NodeCurr->host))==NULL) /* get ip address by domain */  
		{  
			G_MAIN_DEBUG("%d | %d | %s tGethostname '%s' , %s\n", pthread_self(), __LINE__, __FILE__, NodeCurr->host, strerror(errno)); 
			Log->Logf("\tGethostname '%s' error, %s\n", NodeCurr->host, strerror(errno));  
			return -1;  
		}
		server_addr.sin_addr = *(struct in_addr *)host_ent->h_addr;
		G_MAIN_DEBUG("%d | %d | %s tConnect Error %08x %d\n", pthread_self(), __LINE__, __FILE__, host_ent->h_addr, NodeCurr->port);
	}
	/*****************************************************/


 
  G_MAIN_DEBUG("%d | %d | %s tConnect  %08x %d\n", pthread_self(), __LINE__, __FILE__, server_addr.sin_addr, NodeCurr->port);
    /* connect to the server */  
    if(connect(sockfd, (struct sockaddr *)(&server_addr), sizeof(struct sockaddr)) == -1)  
    {  
        Log->Logf("\tConnect Error:%s\a\n", strerror(errno));  
		close(sockfd);
        return -1;  
    }  
	return sockfd;
}  
  
/************************************************************** 
功能：向网站发送 HTTP 请求 
***************************************************************/  
void SendRequest(int sockfd, NodeQueue *NodeCurr)  
{  
    /* send my http-request to web server */  

	char *sendbuffer =(char *)pthread_getspecific(PMVBUFFERVar);

	
    char UserAgent[1024] = "",  
        Accept[1024] = "",  
        AcceptLanguage[1024] = "",  
        AcceptEncoding[1024] = "",  
        AcceptCharset[1024] = "",  
        KeepAlive[1024] = "",  
        Connection[1024] = "",  
        ContentType[1024] = "";  


	
	CLog *Log = CLog::Instance();
	int nbytes=0;
	int dsend = 0;
	int totalsend = 0;
    dsend = 0;totalsend = 0;  


	SetLocalAgent(UserAgent, Accept, AcceptLanguage, AcceptEncoding, AcceptCharset, KeepAlive, Connection, ContentType); /* Get client browser information */  
  
	if(strcmp(NodeCurr->dir, "/")){	
		//printf("GET /%s/%s HTTP/1.0\r\nHost: %s\r\nUser-Agent: %s\r\nAccept: %s\r\nConnection: %s\r\n\r\n", NodeCurr->dir, strcmp(NodeCurr->page, "@")?NodeCurr->page:"", NodeCurr->host, UserAgent, Accept, Connection);  
		sprintf(sendbuffer, "GET /%s/%s HTTP/1.0\r\nHost: %s\r\nUser-Agent: %s\r\nAccept: %s\r\nConnection: %s\r\n\r\n", NodeCurr->dir, strcmp(NodeCurr->page, "@")?NodeCurr->page:"", NodeCurr->host, UserAgent, Accept, Connection);  
	}else{  
		//printf("GET %s%s HTTP/1.0\r\nHost: %s\r\nUser-Agent: %s\r\nAccept: %s\r\nConnection: %s\r\n\r\n", NodeCurr->dir, strcmp(NodeCurr->page, "@")?NodeCurr->page:"", NodeCurr->host, UserAgent, Accept, Connection);	
		sprintf(sendbuffer, "GET %s%s HTTP/1.0\r\nHost: %s\r\nUser-Agent: %s\r\nAccept: %s\r\nConnection: %s\r\n\r\n", NodeCurr->dir, strcmp(NodeCurr->page, "@")?NodeCurr->page:"", NodeCurr->host, UserAgent, Accept, Connection);	
	}

  
	fprintf(stdout, "\tRequest is:\n%s", sendbuffer);  
	

	
    nbytes=strlen(sendbuffer);  
    while(totalsend < nbytes)  
    {  
        dsend = write(sockfd, sendbuffer + totalsend, nbytes - totalsend);  
        if(dsend==-1)  {Log->Logf("\tsend error!%s\n", strerror(errno));exit(0);}  
        totalsend+=dsend;  
        Log->Logf("\n%d \tRequest%d bytes send OK!\n", pthread_self(),totalsend);  
    }  
}  
  
/************************************************************** 
功能：接收网站的 HTTP 返回 
***************************************************************/  
void ReceiveResponse(int sockfd, NodeQueue *NodeCurr)  
{  
	CLog *Log = CLog::Instance();
	char buffer[1024] = {0};
	char fileName[512];
    /* get response from web server */  
	int nbytes=0;
    fd_set writefds;  
    struct timeval tival;  
    int retry = 0;  
    FILE * localfp = NULL;  
  
	int ret;
    int i=0;
	int j = 0;  
    __ReCeive:  
    FD_ZERO(&writefds);  
    tival.tv_sec = 10;  
    tival.tv_usec = 0;  
    if(sockfd > 0) FD_SET(sockfd, &writefds);  
    else {
		Log->Logf( "\n\tError, socket is negative!\n");
		return;
	}  
  
    ret = select(sockfd + 1, &writefds, NULL, NULL, &tival);  
    if(ret ==0 )  
    {  
        if(retry++ < 10) goto __ReCeive;  
    }  
    if(ret <= 0) {
		Log->Logf( "\n\tError while receiving!\n");
		return;
	}
  
    if(FD_ISSET(sockfd, &writefds))  
    {  
        memset(buffer, 0, 1024);  
       // memset(STD->httpheader, 0, 1024);  
		sprintf(fileName, "%s/%s", GConfig->OUTPUTDIR, NodeCurr->file);
G_MAIN_DEBUG("%d | %d | %s output %s\n", pthread_self(), __LINE__, __FILE__, fileName);
        if((localfp = fopen(fileName, "w")) == NULL) {
			Log->Logf( "create file '%s' error\n", fileName);
			return;
		}
        /* receive data from web server */  
        while((nbytes=read(sockfd,buffer,1024)))  
        {  
                /* 获取 HTTP 消息体 */  
				fwrite(buffer, 1, nbytes, localfp);
        }//while  
     
        //新建标记文件
        
		sprintf(fileName, "%s/%s.%s", GConfig->OUTPUTDIR, NodeCurr->file, GConfig->OUTPUTDIR_FINISH_FLAG);
        create_file(fileName);
        fclose(localfp);  
    }//if(FD_ISSET(sockfd, &writefds))  
    close(sockfd); /* because HTTP protocol do something one connection, so I can close it after receiving */  

}  
  
/************************************************************** 
功能：执行一次 HTTP 请求 
***************************************************************/  
void DoOnce(NodeQueue *NodeCurr)  
{  
	int sockfd;
	/* send and receive */  
	sockfd = ConnectWeb(NodeCurr); /* connect to the web server */  
	if(sockfd<0){
		return;
	}
G_MAIN_DEBUG("%d | %d | %s sockfd %d\n", pthread_self(), __LINE__, __FILE__, sockfd);

	/* send a requestdata */  
	SendRequest(sockfd, NodeCurr);  

	/* receive a response message from web server */  
	ReceiveResponse(sockfd, NodeCurr);  
  
}  
/************************************************************** 
功能：从 src 中找出前面的字母、数字等内含，即一个网页地址中主机名后面的部分 
***************************************************************/  
void GetAfterPosWithSlash(char * src, char ** d)  
{  
    char * x;  
    if(src)  
    x = src;  
    else {*d = 0; return ;}  
  
    while(x)  
    {  
        if(*x >= 'a' && *x <= 'z') {x++; continue;}  
        else if(*x >= 'A' && *x <= 'Z') {x++; continue;}  
        else if(*x >= '0' && *x <= '9') {x++; continue;}  
        else if(*x == '.' || *x == '-' || *x == '_' || *x == '=') {x++; continue;}  
        else if(*x == ':' || *x == '/' || *x == '?' || *x == '&') {x++; continue;}  
        else {break;}  
    }  
    if(x) *d = x;  
    else *d = 0;  
}  

/************************************************************** 
功能：从 src 中分析出网页链接，并加入到当前节点的子节点上 
***************************************************************/  
void GetLink(NodeQueue * NodeCurr, char * src)  
{  
    char * pa, * pb, * pc;  
    char * myanchor = 0;  
    int len = 0;  
	int ret;

    pa = src;  
	G_MAIN_DEBUG("%d | %d | %s GetLink  start\n", pthread_self(), __LINE__, __FILE__);
	
	CDBOperation *SDB =(CDBOperation *)pthread_getspecific(PMVDBVar);

	
	
    do  
    {  
		if( memcmp(pa+9, "302", 3) == 0){
			pb = strstr(pa, "Location: ");
            pc = strstr(pb+10, "\r\n");  
            len = pc  - pb - 10;  
            GetMemory(&myanchor, len);  
            memcpy(myanchor, pb+10, len);  
		}else if((pb = strstr(pa, "href='")))  
        {  
            pc = strchr(pb + 6, '\'');  
            len = strlen(pb + 6) - strlen(pc);  
            GetMemory(&myanchor, len);  
            memcpy(myanchor, pb + 6, len);  
        }  
        else if((pb = strstr(pa, "href=\"")))  
        {  
            pc = strchr(pb + 6, '"');  
            len = strlen(pb + 6) - strlen(pc);  
            GetMemory(&myanchor, len);  
            memcpy(myanchor, pb + 6, len);  
        }  
        else if((pb = strstr(pa, "href=")))  
        {  
            GetAfterPosWithSlash(pb + 5, &pc);  
            len = strlen(pb + 5) - strlen(pc);  
            GetMemory(&myanchor, len);  
            memcpy(myanchor, pb + 5, len);  
        }  
        else {goto __returnLink ;}  

		if(pc + 1)  
			pa = pc + 1;
		
  //添加一个结点

		if(strlen(myanchor) > 0){
			CLog *Log = CLog::Instance();
			int WebPort;	
			char WebHost[256] = {0}, PageAddress[256] = {0}, WebDir[256] = {0};  
			  
			//检测url
			ret = UrlFilter(myanchor);
			if(ret) { Log->Logf("GetHost UrlFilter '%s'\n", myanchor);continue;}
			//加入第一个web结点
			ret = GetHost(myanchor, WebHost, PageAddress, &WebPort, WebDir); /* Get web page info */

			G_MAIN_DEBUG("%d | %d | %s GetHost	[%s]\n", pthread_self(), __LINE__, __FILE__, myanchor);

			ret = SDB->isNeedToHandle(WebDir,WebHost,PageAddress);
			G_MAIN_DEBUG("%d | %d | %s isNeedToHandle	[%d]\n", pthread_self(), __LINE__, __FILE__, ret);
			if( !ret ){
				Log->Logf("GetHost '%s' is not analyze\n", myanchor);
				continue;
			}
			
			queue_node_add(&g_webQueue, WebHost, WebPort, WebDir, PageAddress);
			//free(WebHost); free(PageAddress);free(WebDir);	
			SDB->CrawlerRecord(WebDir,WebHost,PageAddress);
		}


			

    }while(pa);  
__returnLink:  
    return;  
}  
  

/************************************************************** 
功能：分析网页 
***************************************************************/  
void AnalyzePage(NodeQueue * node)  
{  
	
	char * mapped_mem;  
	char fileName[512];

    int fd;  
    int flength = 0;  
	
	sprintf(fileName, "%s/%s", GConfig->OUTPUTDIR, node->file);
    fd = open(fileName, O_RDONLY);  
    if(fd == -1){
		G_MAIN_DEBUG("%d | %d | %s AnalyzePage [%s] open error\n", pthread_self(), __LINE__, __FILE__, fileName);
		goto __AnalyzeDone;
    }
    flength =get_file_size(node->file);  
	
	G_MAIN_DEBUG("%d | %d | %s AnalyzePage [%d][%s] start\n", pthread_self(), __LINE__, __FILE__, flength, fileName);
    mapped_mem = (char *)mmap(NULL, flength+1, PROT_READ, MAP_SHARED, fd, 0);  
    //GetEmail(node, mapped_mem);  
    GetLink(node, mapped_mem);  
    close(fd);  
    munmap(mapped_mem, flength);  
__AnalyzeDone:  
    close(fd);  
    //node->IsHandled = 1;  
    //remove(node->file);  
}  
  

/************************************************************** 
功能：处理单个节点信息 
***************************************************************/  
void HandOneNode(NodeQueue * node)  
{  
	CLog *Log = CLog::Instance();

  
    NodeQueue * NodeCurr = node;  
	
	G_MAIN_DEBUG("%d | %d | %s host %s\n", pthread_self(), __LINE__, __FILE__, NodeCurr->host);
	



    DoOnce(NodeCurr);  
   // fprintf(stdout, "\n\tThe following is the response header:\n%s", STD->httpheader);  
    AnalyzePage(NodeCurr);  
}  

static volatile int g_upload_run_signal=NEWIUP_OK;
static int get_upload_run_signal(void)
{
	return g_upload_run_signal;
}
void set_upload_run_signal(int signal)
{
	g_upload_run_signal=signal;
	return ;
}


/************************************************************** 
功能：处理节点信息 线程
***************************************************************/  
void *HandleNodePthread(void * var)  
{

	NodeQueue *node = NULL;
	CLog *Log = CLog::Instance();

	//生存时间
	int survival_time = (int )time(NULL);
	
	char *sendbuff = (char *)malloc(40960);

	pthread_setspecific(PMVBUFFERVar, (void *)sendbuff);
	//pthread_setspecific(PMVSTTDVar, (void *)STD);

	CDBOperation *tDB = new CDBOperation;
	pthread_setspecific(PMVDBVar, (void *)tDB);


	while(get_upload_run_signal() == NEWIUP_OK){

		node = queue_node_pop(&g_webQueue);
		if (node == NULL){
			//结束时间
			int termination_time;
			sleep (QUEUE_EMPTY_SLEEP);
			termination_time = (int )time(NULL) - survival_time;
			//三分钟没有动作线程结束
			if( termination_time > 0 && termination_time > GConfig->timeOut )
				break;
			continue;
		}
		survival_time = (int )time(NULL);

		G_MAIN_DEBUG("%d | %d | %s GET a node %s\n", pthread_self(), __LINE__, __FILE__, node->host);

		
		HandOneNode(node); 
        if(!strcmp(node->dir, "/"))  
            Log->Logf("\t%s:%d%s%s => %s %d\n", node->host, node->port, node->dir, strcmp(node->page, "@")?node->page:"", node->file);  
        else  
            Log->Logf("\t%s:%d/%s/%s => %s %d\n", node->host, node->port, node->dir, strcmp(node->page, "@")?node->page:"", node->file);  
		//不管是否成功都释放
		queue_node_free(node);

	}



	delete tDB;
	pthread_setspecific(PMVDBVar, NULL);
	free(sendbuff);
	pthread_setspecific(PMVSTTDVar, NULL);

	return NULL;
}  

static pthread_t *pthread_id;

void pthread_start()
{
	int i;
	int num = GConfig->nMaxConnNum;
	pthread_id = (pthread_t *)malloc(sizeof(pthread_id) * num);
	if(!pthread_id)exit(1);

	for(i=0;i<num; ++i){
		pthread_create(&pthread_id[i], NULL, HandleNodePthread, NULL);
	}
	return ;
}
void pthread_end()
{
	int i;
	int num = GConfig->nMaxConnNum;
	if(!pthread_id)return;

	for(i=0;i<num; ++i){
		pthread_join(pthread_id[i], NULL);
	}
	return ;
}


int queue_exit(void)
{
	NodeQueue *node = NULL;

		
        while ((node = TAILQ_FIRST(&g_webQueue.head)) != NULL)
        {
            TAILQ_REMOVE(&g_webQueue.head, node, next);
			printf("%s\n", node->host);
			queue_node_free(node);
        }
		


    return 0;    
}
void signal_func(int signal)
{
	set_upload_run_signal(NEWIUP_FAIL);
	//exit(1);
}
void signal_init()
{
    signal(SIGINT, signal_func);
    //signal(SIGUSR1, set_monitor_sche_debug_switch);
#if 0
    signal(SIGTERM, signal_func);
    signal(SIGHUP, signal_func);
    signal(SIGQUIT, signal_func);
    signal(SIGILL, signal_func);
    signal(SIGTRAP, signal_func);
    signal(SIGBUS, signal_func);
    signal(SIGSEGV, signal_func);
    signal(SIGIOT, signal_func);
    signal(SIGFPE, signal_func);
    //signal(SIGKILL, signal_func);
    signal(SIGPIPE, signal_func);
    //signal(SIGSTOP, signal_func);
    signal(SIGTTOU, signal_func);
    signal(SIGTTIN, signal_func);
    signal(SIGXFSZ, signal_func);
    signal(SIGPROF, signal_func);
    signal(SIGVTALRM, signal_func);
#endif
}


int main(int argv, char ** args)
{

	if(argv >3){
		printf("%s [<config>] [rootUrl]\n", args[0]);
		return 0;
	}

	if(argv == 1){
		GConfig =  CConfig::Instance((char *)"./conf.yaml");
	}else{
		GConfig =  CConfig::Instance(args[1]);
	}

	if(GConfig->CConfigInitResault){
		printf("%s config file [%s] read is error\n", args[0], args[1]);
		GConfig->Destory();
		return 0;
	}


	GLog = CLog::Instance(GConfig->LOGDIR);

	/*install signals*/
	pthread_key_create(&PMVDBVar, NULL);
	pthread_key_create(&PMVBUFFERVar, NULL);
	pthread_key_create(&PMVSTTDVar, NULL);

	signal_init();

	if( argv == 3 )
		InitWebCrawler(args[2]);
	else
		InitWebCrawler(NULL);
		
	printf("init ok\n");

	pthread_start();

	pthread_end();
	

	GConfig->Destory();
	pthread_key_delete(PMVDBVar);
	pthread_key_delete(PMVBUFFERVar);
	pthread_key_delete(PMVSTTDVar);

	queue_exit();
	
	return 0;
}



