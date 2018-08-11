#����Դ�ļ���׺
PS=cpp

#���ñ�����
CC=g++

#������һ�е�ע�ʹ��룬��Ϊ�����Ѿ�֪��debug���makefile�����ִ�еģ��������ֱ��ȥdebugĿ¼��ִ��ָ����makefile�ļ����У�����������ע��
#DEBUG=$(shell ls -l | grep ^d | awk '{if($$9 == "debug") print $$9}')

#��ס��ǰ���̵ĸ�Ŀ¼·��
ROOT_DIR=$(shell pwd)

#Ŀ���ļ����ڵ�Ŀ¼
OBJS_DIR=$(ROOT_DIR)/obj
OBJS_LIB_DIR=$(OBJS_DIR)/lib

#bin�ļ����ڵ�Ŀ¼
BIN_DIR=$(ROOT_DIR)/bin

#���ñ��ؿ� ͷ�ļ�·��
LIB_DIR = $(ROOT_DIR)/lib

#���ñ��ؿ� makefile�ļ�·��
MAKEFILE_DIR = $(ROOT_DIR)/make


#����Լ���ȫ��ͷ�ļ�·��
INCLUDES =  $(ROOT_DIR)/src/include  $(ROOT_DIR)/src/include/PosPhoneProtocol
LIBS = pthread conf dl
CFLAGS = -g -fPIC  $(addprefix -L,$(LIB_DIR))  $(addprefix -I, $(INCLUDES)) $(addprefix -l,$(LIBS)) 

#���ÿ��ģ��.o����·��
CUR_MOD_OBJ_DIR=$(OBJS_DIR)
CUR_MOD_DIR=
MAKEFIE_LEVEL=2
export CUR_MOD_OBJ_DIR CUR_MOD_DIR MAKEFIE_LEVEL

##���������ж��Ƕ����ݿ⶯̬�ļ��Ĵ���
	ifeq ($(DB_TYPE),oracle)
ORA_INC=$(ORACLE_HOME)/precomp/public
ORA_LIBDIR=$(ORACLE_HOME)/lib $(ORACLE_HOME)/rdbms/lib $(ORACLE_HOME)/precomp/lib $(ORACLE_HOME)/sqlplus/lib $(ORACLE_HOME)/otrace/lib		
PROCFLAGS=  $(addprefix -L,$(ORA_LIBDIR))  $(addprefix -I,$(ORA_INC)) -lclntsh

#���� -fpic  ���relocation R_X86_64_32 against������
	else
#centos
PROCFLAGS=  -L/usr/lib64/mysql -lmysqlclient_r
	endif


#�����±�����������shell�У������൱�ڵ�������Ŀ¼�µ�makefile��
export PS CC OBJS_DIR BIN_DIR ROOT_DIR CFLAGS LIB_DIR OBJS_LIB_DIR ORA_INC MAKEFILE_DIR ORA_LIBDIR PROCFLAGS



