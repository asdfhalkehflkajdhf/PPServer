#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <dlfcn.h>

#ifdef __cplusplus
extern "C" {
#endif


typedef int (*PCall_func0)();
typedef int (*PCall_func1)(void *);
typedef int (*PCall_func2)(void *, void *);
typedef int (*PCall_func3)(void *, void *, void *);
typedef int (*PCall_func4)(void *, void *, void *, void *);



int dynamic_call_library_func0(char *libName, char *funcName) 
{ 
    void *handle;
	void *error;
	PCall_func0 selffunc=NULL;
	int ret;
	
	if(libName == NULL)return -9000;
	if(funcName == NULL)return -9001;
	
	//�򿪶�̬���ӿ�
	handle = dlopen(libName, RTLD_LAZY|RTLD_GLOBAL);
	if (!handle) {
		printf(" -9000 %s\n", dlerror());
		return -9000;
	}
	
	dlerror();
	//��ȡһ������
	selffunc = (PCall_func0)dlsym(handle, funcName);
	if ((error = dlerror()) != NULL)  {
		printf( " -9001 %s \n", (char *)error);
		return -9001;
	}
	
	ret = selffunc();
	//dlclose���ڹر�ָ������Ķ�̬���ӿ⣬ֻ�е��˶�̬���ӿ��ʹ�ü���Ϊ0ʱ,�Ż�������ϵͳж�ء�
	dlclose(handle);
    return ret;
}
int dynamic_call_library_func1(char *libName, char *funcName, void *argv1) 
{ 
    void *handle;
	void *error;
	PCall_func1 selffunc=NULL;
	int ret;
	
	if(libName == NULL)return -9000;
	if(funcName == NULL)return -9001;
	
	//�򿪶�̬���ӿ�
	handle = dlopen(libName, RTLD_LAZY);
	//handle = dlopen(libName, RTLD_NOW | RTLD_LOCAL | RTLD_DEEPBIND);
	if (!handle) {
		printf(" -9000 %s\n", dlerror());
		return -9000;
	}
	
	dlerror();
	//��ȡһ������
	selffunc = (PCall_func1)dlsym(handle, funcName);
	if ((error = dlerror()) != NULL)  {
		printf( " -9001 %s \n", (char *)error);
		return -9001;
	}
	
	ret = selffunc(argv1);
	//dlclose���ڹر�ָ������Ķ�̬���ӿ⣬ֻ�е��˶�̬���ӿ��ʹ�ü���Ϊ0ʱ,�Ż�������ϵͳж�ء�
	dlclose(handle);
    return ret;
}

int dynamic_call_library_func2(char *libName, char *funcName, void *argv1, void *argv2) 
{ 
    void *handle;
	void *error;
	PCall_func2 selffunc=NULL;
	int ret;
	
	if(libName == NULL)return -9000;
	if(funcName == NULL)return -9001;
	
	//�򿪶�̬���ӿ�
	handle = dlopen(libName, RTLD_LAZY|RTLD_GLOBAL);
	if (!handle) {
		printf(" -9000 %s\n", dlerror());
		return -9000;
	}
	
	dlerror();
	//��ȡһ������
	selffunc = (PCall_func2)dlsym(handle, funcName);
	if ((error = dlerror()) != NULL)  {
		printf( " -9001 %s \n", (char *)error);
		return -9001;
	}
	
	ret = selffunc(argv1, argv2);
	//dlclose���ڹر�ָ������Ķ�̬���ӿ⣬ֻ�е��˶�̬���ӿ��ʹ�ü���Ϊ0ʱ,�Ż�������ϵͳж�ء�
	dlclose(handle);
    return ret;
}

int dynamic_call_library_func3(char *libName, char *funcName, void *argv1, void *argv2, void *argv3) 
{ 
    void *handle;
	void *error;
	PCall_func3 selffunc=NULL;
	int ret;
	
	if(libName == NULL)return -9000;
	if(funcName == NULL)return -9001;
	
	//�򿪶�̬���ӿ�
	handle = dlopen(libName, RTLD_LAZY|RTLD_GLOBAL);
	if (!handle) {
		printf(" -9000 %s\n", dlerror());
		return -9000;
	}
	
	dlerror();
	//��ȡһ������
	selffunc = (PCall_func3)dlsym(handle, funcName);
	if ((error = dlerror()) != NULL)  {
		printf( " -9001 %s \n", (char *)error);
		return -9001;
	}
	
	ret = selffunc(argv1, argv2, argv3);
	//dlclose���ڹر�ָ������Ķ�̬���ӿ⣬ֻ�е��˶�̬���ӿ��ʹ�ü���Ϊ0ʱ,�Ż�������ϵͳж�ء�
	dlclose(handle);
    return ret;
}
int dynamic_call_library_func4(char *libName, char *funcName, void *argv1, void *argv2, void *argv3, void *argv4) 
{ 
    void *handle;
	void *error;
	PCall_func4 selffunc=NULL;
	int ret;
	
	if(libName == NULL)return -9000;
	if(funcName == NULL)return -9001;
	
	//�򿪶�̬���ӿ�
	handle = dlopen(libName, RTLD_LAZY|RTLD_GLOBAL);
	if (!handle) {
		printf(" -9000 %s\n", dlerror());
		return -9000;
	}
	
	dlerror();
	//��ȡһ������
	selffunc = (PCall_func4)dlsym(handle, funcName);
	if ((error = dlerror()) != NULL)  {
		printf( " -9001 %s \n", (char *)error);
		return -9001;
	}
	
	ret = selffunc(argv1, argv2, argv3, argv4);
	//dlclose���ڹر�ָ������Ķ�̬���ӿ⣬ֻ�е��˶�̬���ӿ��ʹ�ü���Ϊ0ʱ,�Ż�������ϵͳж�ء�
	dlclose(handle);
    return ret;
}


#ifdef __cplusplus
}
#endif
