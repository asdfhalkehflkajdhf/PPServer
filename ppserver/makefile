include ./makefile.mk

#debug�ļ������makefile�ļ���Ҫ���ִ�У�����������Ҫִ�е���Ŀ¼Ҫ�ų�debug�ļ��У�����ʹ��awk�ų���debug�ļ��У���ȡʣ�µ��ļ���
#SUBDIRS=$(shell ls -l | grep ^d | awk '{if($$9 != "debug") print $$9}')
#SUBDIRS = 	src/DlApi \
#			src/mainFrm \
#			src/PublicModule \
#			src/PlatformUtil \
#			src/PosPhoneProtocol \
#			src/RModule \
#			src/Sample \
#			src/TransDLL 

#��һ��Ĭ�ϵĸ�Ŀ¼��src
SUBDIRS = src

#��ǰĿ¼������cpp �ļ���
#CUR_SOURCE=${wildcard *.$(PS)}
#��ǰĿ¼������cpp���ļ��ġ�.oĿ���ļ���
#CUR_OBJS=${patsubst %.$(PS), %.o, $(CUR_SOURCE)}


#ע�������˳����Ҫ��ִ��SUBDIRS��������DEBUG, ����CUR_OBJSΪ���������Ŀ���ǲ���ִ�е�
all:$(SUBDIRS) $(CUR_OBJS) DEBUG ECHO

#�ݹ�ִ����Ŀ¼�µ�makefile�ļ������ǵݹ�ִ�еĹؼ�
$(SUBDIRS): CREATE_DIR
#�жϡ�TARGET_DIR�������Ƿ�Ϊ��
ifeq ($(TARGET_DIR), )
#$@--Ŀ���ļ���$^--���������ļ���$<--��һ�������ļ�
	make -C $@
else
	make -C $(TARGET_DIR)

endif

DEBUG:
#ֱ��ȥdebugĿ¼��ִ��makefile�ļ�
#	rm ./obj/makefile
	@\cp ./make/makefile-src ./obj/makefile
	make -C obj
#

#�����ʾ��Ϣ
ECHO:
	@echo "into dir:[`pwd`] exec make finish"

#����debug��Ŀ¼
CREATE_DIR:
	@mkdir -p $(OBJS_DIR)
	@mkdir -p $(OBJS_DIR)/lib
	@mkdir -p $(BIN_DIR)
	
#��c�ļ�����Ϊo�ļ���������ָ������Ŀ���ļ���Ŀ¼�м�OBJS_DIR
$(CUR_OBJS):%.o:%.$(PS)
	$(CC) -c $^ -o $(ROOT_DIR)/$(OBJS_DIR)/$@ $(CFLAGS) 
CLEAN:
	@rm -rf $(OBJS_DIR)/*
	@rm -rf $(BIN_DIR)/*