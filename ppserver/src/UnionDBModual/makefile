
#����Ŀ��(Ĭ���ǿգ���������Ŀ�ֻ꣬����.o�ļ�)
#��һ�֣��͵�ǰĿ¼��ͬ���Ŀ�ִ���ļ������
#DEST=$(notdir  $(shell pwd))

#�ڶ��֣�����ָ����ִ���ļ�������ļ�
#DEST=test

#�����֣������ɿ�ִ���ļ�������ļ���������DEST�����ÿ�
DEST=


#����DEST���ͣ������һ���ļ�ʱ����ʹ��*�������ļ�ʱ����ʹ��ȷ����׺��
DEST_TYPE=*				#bin����lib


#��ǰĿ¼�²���Ҫ�������Ŀ¼�б�
ELIMINATE_LIST= config lib include

#����˽�ͻ���������
USER_INCLUDES= 
USER_LIBS=
USER_LIBS_DIR=


#����Ϊʹ���߿����ñ���
##################################################################################

USER_CFLAGS = $(addprefix -L,$(USER_LIBS_DIR))  $(addprefix -I, $(USER_INCLUDES))  $(addprefix -l,$(USER_LIBS)) 


#��Ŀ¼��Makefileֱ�Ӷ�ȡ����Ŀ¼����
ALL_SUBDIRS=$(shell ls -l | grep ^d | awk '{print $$9}')

#ȥ��config lib include ������Ŀ¼
SUBDIRS=$(filter-out $(ELIMINATE_LIST) , $(ALL_SUBDIRS))
SUBDIRS=$(DB_TYPE)
#��ǰĿ¼������cpp �ļ���
CUR_SOURCE=${wildcard *.$(PS)}
#��ǰĿ¼������cpp �ļ��� .oĿ���ļ���
CUR_OBJS=${patsubst %.$(PS), %.o, $(CUR_SOURCE)}


#��ǰģ�����󱣴�·��ѡ��
ifeq (2,$(MAKELEVEL))
#������Ϊ��ʱ��ʾ��ǰΪһ����ģ��ĵ�һ�㣬��Ҫ�ж����Ƿ�Ϊ������
CUR_MOD_OBJ_DIR=$(OBJS_DIR)
ifeq ($(DEST), )
#@echo "need to set----------------1-"
else
#DEST ��Ϊ�գ���ʹ��
CUR_MOD_OBJ_DIR=$(OBJS_DIR)/$(DEST)
endif
endif


all: COMPILE_READY $(SUBDIRS) $(CUR_OBJS) ECHO

#������Ϣ
PROC:
	@echo "need to set-----------+++++++++++-----0-" $(MAKELEVEL) $(CUR_MOD_OBJ_DIR)
ifeq (2,$(MAKELEVEL))
#������Ϊ��ʱ��ʾ��ǰΪһ����ģ��ĵ�һ�㣬��Ҫ�ж����Ƿ�Ϊ������
	@echo "need to set-----------+++++++++++-----1-" $(CUR_MOD_OBJ_DIR)
ifeq ($(DEST), )
	@echo "need to set-----------+++++++++++-----2-" $(CUR_MOD_OBJ_DIR)
else
#DEST ��Ϊ�գ���ʹ��
	@echo "need to set-----------+++++++++++-----3-" $(CUR_MOD_OBJ_DIR)
endif

else
	@echo "need to set-----------+++++++++++-----4-"  $(MAKELEVEL)  $(CUR_MOD_OBJ_DIR)
endif

#����ǰ׼������
COMPILE_READY:
ifeq (2,$(MAKELEVEL))
ifneq ($(DEST), )
#@echo "need to set----------------1-"
	@mkdir -p $(CUR_MOD_OBJ_DIR)
#	$(CC)  $(CFLAGS) $(USER_CFLAGS) -c $^ -o $(OBJS_DIR)/$(DEST)/$@
	@rm -rf $(CUR_MOD_OBJ_DIR)/makefile
	@cp ./makefile-$(DEST_TYPE) $(CUR_MOD_OBJ_DIR)/makefile
endif
endif

#Ƕ��ִ����Ŀ¼������makefile
$(SUBDIRS): NULLREFER
	make -C $@

#���뵱ǰĿ¼��Դ�ļ�
$(CUR_OBJS):%.o:%.$(PS)
#$@--Ŀ���ļ���$^--���������ļ���$<--��һ�������ļ� -c ����������.oʱ����Ҫ���Ӷ�̬��
	$(CC)  $(CFLAGS) $(USER_CFLAGS) -c $^ -o $(CUR_MOD_OBJ_DIR)/$@ 

#�����ʾ��Ϣ
ECHO:
	@echo "into dir:[`pwd`] exec make finish"

NULLREFER:
	