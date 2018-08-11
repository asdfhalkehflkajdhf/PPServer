#ifndef _DL_API_H
#define _DL_API_H

#ifdef __cplusplus
extern "C" {
#endif

/*
ʹ��g++�����.so���У�����ǰ������� exter "C" 
*/

int dynamic_call_library_func0(char *libName, char *funcName) ;
int dynamic_call_library_func1(char *libName, char *funcName, void *argv1) ;
int dynamic_call_library_func2(char *libName, char *funcName, void *argv1, void *argv2) ;
int dynamic_call_library_func3(char *libName, char *funcName, void *argv1, void *argv2, void *argv3) ;
int dynamic_call_library_func4(char *libName, char *funcName, void *argv1, void *argv2, void *argv3, void *argv4) ;



#ifdef __cplusplus
}
#endif

#endif