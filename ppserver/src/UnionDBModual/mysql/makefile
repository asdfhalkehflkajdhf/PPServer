
#����Ŀ��(Ĭ���ǿգ���������Ŀ�ֻ꣬����.o�ļ�)
#��һ�֣��͵�ǰĿ¼��ͬ���Ŀ�ִ���ļ������
#DEST=$(notdir  $(shell pwd))

#�ڶ��֣�����ָ����ִ���ļ�������ļ�
#DEST=test

#�����֣������ɿ�ִ���ļ�������ļ���������DEST�����ÿ�
DEST=


#����DEST���ͣ������һ���ļ�ʱ����ʹ��*�������ļ�ʱ����ʹ��ȷ����׺��				#bin����lib
DEST_TYPE=*


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

#��ǰĿ¼������cpp �ļ���
CUR_SOURCE=${wildcard *.$(PS)}
#��ǰĿ¼������cpp �ļ��� .oĿ���ļ���
CUR_OBJS=${patsubst %.$(PS), %.o, $(CUR_SOURCE)}


all:$(SUBDIRS) $(CUR_OBJS) ECHO
$(SUBDIRS): NULLREFER
	make -C $@
$(CUR_OBJS):%.o:%.$(PS)
ifeq ($(DEST), )
#$@--Ŀ���ļ���$^--���������ļ���$<--��һ�������ļ� -c ����������.oʱ����Ҫ���Ӷ�̬��
	$(CC)  $(CFLAGS) $(USER_CFLAGS) -c $^ -o $(OBJS_DIR)/$@ 
else
	@mkdir -p $(OBJS_DIR)/$(DEST)
	$(CC)  $(CFLAGS) $(USER_CFLAGS) -c $^ -o $(OBJS_DIR)/$(DEST)/$@
	@rm -rf $(OBJS_DIR)/$(DEST)/makefile
	@cp ./makefile-$(DEST_TYPE) $(OBJS_DIR)/$(DEST)/makefile
endif

#�����ʾ��Ϣ
ECHO:
	@echo "into dir:[`pwd`] exec make finish"

NULLREFER:
	