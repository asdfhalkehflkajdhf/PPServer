#设置源文件后缀
PS=cpp

#设置编译器
CC=g++

#无需下一行的注释代码，因为我们已经知道debug里的makefile是最后执行的，所以最后直接去debug目录下执行指定的makefile文件就行，具体下面有注释
#DEBUG=$(shell ls -l | grep ^d | awk '{if($$9 == "debug") print $$9}')

#记住当前工程的根目录路径
ROOT_DIR=$(shell pwd)

#目标文件所在的目录
OBJS_DIR=$(ROOT_DIR)/obj
OBJS_LIB_DIR=$(OBJS_DIR)/lib

#bin文件所在的目录
BIN_DIR=$(ROOT_DIR)/bin

#设置本地库 头文件路径
LIB_DIR = $(ROOT_DIR)/lib

#设置本地库 makefile文件路径
MAKEFILE_DIR = $(ROOT_DIR)/make


#添加自己的全局头文件路径
INCLUDES =  $(ROOT_DIR)/src/include  $(ROOT_DIR)/src/include/PosPhoneProtocol
LIBS = pthread conf dl
CFLAGS = -g -fPIC  $(addprefix -L,$(LIB_DIR))  $(addprefix -I, $(INCLUDES)) $(addprefix -l,$(LIBS)) 

#添加每个模块.o生成路径
CUR_MOD_OBJ_DIR=$(OBJS_DIR)
CUR_MOD_DIR=
MAKEFIE_LEVEL=2
export CUR_MOD_OBJ_DIR CUR_MOD_DIR MAKEFIE_LEVEL

##以下条件判断是对数据库动态文件的处理
	ifeq ($(DB_TYPE),oracle)
ORA_INC=$(ORACLE_HOME)/precomp/public
ORA_LIBDIR=$(ORACLE_HOME)/lib $(ORACLE_HOME)/rdbms/lib $(ORACLE_HOME)/precomp/lib $(ORACLE_HOME)/sqlplus/lib $(ORACLE_HOME)/otrace/lib		
PROCFLAGS=  $(addprefix -L,$(ORA_LIBDIR))  $(addprefix -I,$(ORA_INC)) -lclntsh

#不加 -fpic  会出relocation R_X86_64_32 against　错误
	else
#centos
PROCFLAGS=  -L/usr/lib64/mysql -lmysqlclient_r
	endif


#将以下变量导出到子shell中，本次相当于导出到子目录下的makefile中
export PS CC OBJS_DIR BIN_DIR ROOT_DIR CFLAGS LIB_DIR OBJS_LIB_DIR ORA_INC MAKEFILE_DIR ORA_LIBDIR PROCFLAGS



