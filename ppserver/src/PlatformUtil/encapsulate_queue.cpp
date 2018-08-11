#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/inotify.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <pthread.h>

#include "utils.h"

#include "queue.h"
#include "encapsulate_queue.h"


int FileNumber = 0;  


NodeQueue *queue_node_pop(queue_s *queue)
{

    pthread_spin_lock(&queue->lock);    
    NodeQueue *ptr = TAILQ_FIRST(&queue->head);
    if (ptr != NULL)
    {
        TAILQ_REMOVE(&queue->head, ptr, next);    
        queue->size--;   
    }
    pthread_spin_unlock(&queue->lock);    

    return ptr;
}

int queue_node_push(queue_s *queue, NodeQueue *node)
{
	if(queue == NULL || node == NULL){
		return 0;
	}
    pthread_spin_lock(&queue->lock);    
    TAILQ_INSERT_TAIL(&queue->head, node, next);
    //TAILQ_INSERT_HEAD(&queue->head, node, next);
    queue->size++;   
    pthread_spin_unlock(&queue->lock);    

    return 0;
}

void queue_node_free(NodeQueue *node)
{
	if(node == NULL){
		return;
	}
	/*
	if(node->copy == NEWIUP_OK){
		__sync_fetch_and_sub(node->retain_count, 1);
		free(node);
		return;
	}
    if (node->monitorfile!= NULL)
    {
        free(node->monitorfile);
    }
	if(node->src != NULL)
	{
		free(node->src);
	}
	if(node->src_com!= NULL)
	{
		free(node->src_com);
	}
*/
    free(node);

    return ;
}

NodeQueue * queue_node_add(queue_s *queue, char *host, int   port, char *dir, char *page)
{
	NodeQueue *node = NULL;
	if(host == NULL || dir == NULL || page == NULL)
	{
        return NULL;
	}
    node = (NodeQueue *)malloc(sizeof(NodeQueue));
    if (node == NULL)
    {
        return NULL;
    }

	strcpy(node->host, host);
	strcpy(node->dir, dir);
	strcpy(node->page, page);
	sprintf(node->file, "file_%06d.txt", FileNumber++);
	
    node->port= port;
	
    queue_node_push(queue, node);

    return node;
}

NodeQueue * queue_node_copy(NodeQueue * oldnode)
{
	NodeQueue *node = NULL;
	if(oldnode == NULL)
	{
        return NULL;
	}
    node = (NodeQueue *)malloc(sizeof(NodeQueue));
    if (node == NULL)
    {
        return NULL;
    }
	memcpy(node, oldnode, sizeof(NodeQueue));
	//__sync_fetch_and_add(node->retain_count, 1);
    return node;
}
#if 0
/*
get node  Reference number
*/
int queue_node_is_retain(NodeQueue *node)
{
	//return __sync_fetch_and_add(node->retain_count, 0);
	return 0 ;
}



char *get_wd_path_by_wd(Wdqueue *head, int wd)
{
    Wd_node *node = NULL;
    TAILQ_FOREACH(node, &head->head, next)
    {
        if (node->wd == wd)
        {
            return node->path;
        }
    }

    return NULL;
}
char *get_wd_subpath_by_wd(Wdqueue *head, int wd)
{
    Wd_node *node = NULL;
    TAILQ_FOREACH(node, &head->head, next)
    {
        if (node->wd == wd)
        {
            return node->subpath;
        }
    }

    return NULL;
}

//return wd:sucess -1:fail
int get_wd_wd_by_path(Wdqueue *head, char *path)
{
    Wd_node *node = NULL;
    TAILQ_FOREACH(node, &head->head, next)
    {
        if (strcmp(node->path, path) == 0)
        {
            return node->wd;
        }
    }

    return NEWIUP_FAIL;
}

//return wd:sucess -1:fail
Wd_node *get_wd_by_wd(Wdqueue *head,int wd)
{
    Wd_node *node = NULL;
    TAILQ_FOREACH(node, &head->head, next)
    {
        if (node->wd == wd)
        {
            return node;
        }
    }

    return NULL;
}

int show_monitor_dir(int index, char *name, Wdqueue *head)
{
    Wd_node *node = NULL;
    TAILQ_FOREACH(node, &head->head, next)
    {
        logging(LOG_INFO, "%s gid %d| %s wd:%d path:%s subpath:%s\n",
			__func__, index, name, node->wd,node->path, node->subpath);
    }
    return 0;
}

//return 0:sucess -1:fail(exit)
int del_monitor_dir(int fd, int index, char *name, Wdqueue *head, int wd, char *path)    
{

    Wd_node *node = NULL;
    TAILQ_FOREACH(node, &head->head, next)
    {
        if (strcmp(node->path, path) == 0)
        {
            break;
        }
    }
    if (node != NULL)
    {
        TAILQ_REMOVE(&head->head, node, next);
		head->size--;
        inotify_rm_watch(fd, node->wd);
		logging(LOG_INFO, "%s OK: gid:%d gname:%s wd:%d path:%s\n",
			__func__,
			index,
			name, 
			node->wd,
			node->path);		
    }

    return NEWIUP_OK;
}

//return 0:sucess -1:fail(exit)
int add_monitor_dir(int fd, int index, char *name,  unsigned int mask, Wdqueue *head, char *path, Wd_node * fnode)
{
    int wd = inotify_add_watch(fd, path, mask);
	if (wd < 0)
	{
        logging(LOG_ERROR, "add monitor dir [%s] error1 --%s|%d\n", path, __func__, __LINE__);
		exit(-1);
	}

    Wd_node * node= (Wd_node *)malloc(sizeof(Wd_node));
    if (node == NULL)
    {
        logging(LOG_ERROR, "add monitor dir [%s] error2 --%s|%d\n", path, __func__, __LINE__);
		exit(-1);
    }
    node->wd = wd;
	
	strcpy(node->path, path);
	if(fnode == NULL){
		node->suboffset = strlen(path)+1;
		node->subpath = node->path+node->suboffset;
	}else{
		node->suboffset = fnode->suboffset;
		node->subpath = node->path+node->suboffset;
	}
	head->size++;
    TAILQ_INSERT_TAIL(&head->head, node, next);
	logging(LOG_INFO, "%s OK: gid:%d gname:%s wd:%d path:%s subpath:%s\n",
		__func__,
		index,
		name, 
		wd,	node->path, node->subpath);

    DIR *pdir = opendir(node->path);
    if (pdir == NULL)
    {
        logging(LOG_ERROR, "open monitor dir:%s error --%s|%d\n", node->path, __func__, __LINE__);
        exit (-1);
    }
    char fullpath[WDQUEUE_PATH_LEN]={0};
    struct dirent *pdirent = NULL; 

    while ((pdirent = readdir(pdir)) != NULL)
    {
        if (pdirent->d_name[0] == '.')
        {
            continue;
        }

        snprintf(fullpath, sizeof(fullpath), "%s/%s", node->path, pdirent->d_name);
        if ( !is_dir(fullpath) )
        {
            add_monitor_dir(fd, index, name, mask, head, fullpath, node);
			
        }
    }

    closedir(pdir);


    return NEWIUP_OK;
}

#endif

