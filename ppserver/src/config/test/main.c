#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <pthread.h>

#include "../conf.h"
#include "../conf-yaml-loader.h"
#include "../utils.h"


typedef struct global_config_t
{
	int affinity;
	int max_upload_num;
	int compress_buff_size;
	long upload_timeout;
	long connect_timeout;
	char log_dir_path[256];
}global_config_s;


global_config_s global_config={-1,10000,200,300,30,{0}};

/**
 * \brief Get a ConfNodeValue by node.
 *
 */
int ConfGetNodeValue(ConfNode *node, char *name, char **value)
{
	if(node == NULL || name == NULL)
	{
		return -1;
	}
	if(strcmp(node->name, name)==0)
	{
		*value = node->val;
		return 0;
	}
	return -1;
}

int ConfNodeNameCheck(char *name)
{
	ConfNode *child1 = NULL;
	ConfNode *child2 = NULL;
	//ConfNode *child3 = NULL;
    ConfNode *root = ConfGetRootNode();
	int num=0;

	TAILQ_FOREACH(child1, &root->head, next) 
	{		
		TAILQ_FOREACH(child2, &child1->head, next) 
		{
			if(strcmp(child2->val, name) == 0){
				num++;
				if(num>=2){
					return -1;
				}
			} 
			//printf("\t%s:%s\n",child2->name, child2->val);
		}
	}
	return 0;
}



int show_config_info()
{
    printf("-- %s\n", __func__);
	ConfNode *child1 = NULL;
	ConfNode *child2 = NULL;
	ConfNode *child3 = NULL;
    ConfNode *root = ConfGetRootNode();

	TAILQ_FOREACH(child1, &root->head, next) 
	{
		printf("%s:%s\n",child1->name, child1->val);
		
		TAILQ_FOREACH(child2, &child1->head, next) 
		{
			printf("\t%s:%s\n",child2->name, child2->val);
			TAILQ_FOREACH(child3, &child2->head, next) 
			{
				printf("\t\t%s:%s\n",child3->name, child3->val);
				
			}
		}
	}
	return 0;
}
int global_cfg_init()
{

    ConfNode *child = NULL;
    ConfNode *node = NULL;
    char *str = NULL;

    node = ConfGetNode("global-config");
    TAILQ_FOREACH(child, &node->head, next) 
    {
		
		printf("%s %s\n", child->name, child->val);
		
/*         if (ConfGetNodeValue(child, "affinity", &str) == 0)
        {
        	if(strcmp(str, "no") == 0){
				global_config.affinity = -1;
			}else{
				global_config.affinity = atoi(str);
			}
			continue;
        }
        if (ConfGetNodeValue(child, "max-upload", &str) == 0)
        {
			global_config.max_upload_num= atoi(str);
			continue;
        }
		if (ConfGetNodeValue(child, "log-dir-path", &str) == 0)
        {
			strncpy(global_config.log_dir_path, str, 256);
			continue;
        }
        if (ConfGetNodeValue(child, "upload-timeout", &str) == 0)
        {
			global_config.upload_timeout= atoi(str);
			continue;
        }

		if (ConfGetNodeValue(child, "connect-timeout", &str) == 0)
		{
			global_config.connect_timeout= atoi(str);
			continue;
		}
		if (ConfGetNodeValue(child, "compress-buff-size", &str) == 0)
		{
			global_config.compress_buff_size= atoi(str);
			continue;
		}
		printf( "--%s [%s] error\n",__func__, child->name);
		exit(1); */
    }

/* 	if (is_dir(global_config.log_dir_path) != 0)
	{
		if (recursive_make_dir(global_config.log_dir_path, DIR_MODE) != 0)
		{
			printf( "--%s create direcotry [%s] error\n",__func__, global_config.log_dir_path);
			return 1;
		}
		printf( "create log direcotry: [%s]\n", global_config.log_dir_path);
	}
    printf( "affinity             =%d\n", global_config.affinity);
    printf( "max_upload_num       =%d\n", global_config.max_upload_num);
    printf( "compress_buff_size   =%d\n", global_config.compress_buff_size);
    printf( "connect_timeout      =%d\n", global_config.connect_timeout);
    printf( "upload_timeout       =%d\n", global_config.upload_timeout);
    printf( "log_dir_path         =%s\n", global_config.log_dir_path); */

    return 0;
	
}
int ThreadGetCPUAffinity(pthread_t pid, int cpuid)
{
	int ret;
	if(cpuid == -1){
		return 0;
	}
    cpu_set_t cs;

    CPU_ZERO(&cs);
    CPU_SET(cpuid, &cs);

	ret = pthread_getaffinity_np(pid, sizeof(cpu_set_t), &cs);
    if (ret != 0){
        //printf("pthread_getaffinity_np fail\n");
	}
	int j;
	for (j = 0; j < CPU_SETSIZE; j++)
        if (CPU_ISSET(j, &cs)){
            //printf("CPU %d\n", j);
		}
	return ret;
}

int ThreadSetCPUAffinity(pthread_t pid, int cpuid)
{
	int ret;
	if(cpuid == -1){
		return 0;
	}
    cpu_set_t cs;

    CPU_ZERO(&cs);
    CPU_SET(cpuid, &cs);

    ret =  pthread_setaffinity_np(pid, sizeof(cpu_set_t), &cs);
	
	return ret;
}

void signal_func(int signal)
{
	printf("exit signal id [%d].\n", signal);
	exit(1);
}
void signal_init()
{
    signal(SIGINT, signal_func);
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
int cfg_init()
{
	int ret;
	ret = global_cfg_init();
	if(ret != 0)
	{
		return 1;
	}
	return 0;
}


void newiup_exit()
{

}

int main(int argc, char **argv)
{
	int ret = 0;

	if( argc != 2 ) 
    {
		printf( "Usage: newiup config-file\n");
        exit(15);  
	}


	ConfInit();
	if (ConfYamlLoadFile(argv[1]) != 0) 
    {
		printf( "Invalid format of configuration files, please check\n");
		printf( "NOTE:¡¡configuration file Don't make the 'TAB' and the '_'\n");
		printf( "     or Check whether the [monitor-group.servers-backup:] configuration is begin with the '|'\n");
		return 1;
	}
	//show_config_info();
	ret = cfg_init();
	if(ret != 0)
	{
		goto init_fail;
	}
	
	if (ThreadSetCPUAffinity(pthread_self(), global_config.affinity) != 0){
		printf("could not set CPU affinity to %d\n", global_config.affinity);
		goto init_fail;
	}
	printf("set CPU affinity to %d\n", global_config.affinity);

	signal_init();


init_fail:
	newiup_exit();
	return 0;
} 
