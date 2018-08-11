include ./makefile.mk

#debug文件夹里的makefile文件需要最后执行，所以这里需要执行的子目录要排除debug文件夹，这里使用awk排除了debug文件夹，读取剩下的文件夹
#SUBDIRS=$(shell ls -l | grep ^d | awk '{if($$9 != "debug") print $$9}')
#SUBDIRS = 	src/DlApi \
#			src/mainFrm \
#			src/PublicModule \
#			src/PlatformUtil \
#			src/PosPhoneProtocol \
#			src/RModule \
#			src/Sample \
#			src/TransDLL 

#第一个默认的根目录是src
SUBDIRS = src

#当前目录下所以cpp 文件名
#CUR_SOURCE=${wildcard *.$(PS)}
#当前目录下所有cpp　文件的　.o目标文件名
#CUR_OBJS=${patsubst %.$(PS), %.o, $(CUR_SOURCE)}


#注意这里的顺序，需要先执行SUBDIRS最后才能是DEBUG, 其中CUR_OBJS为空所以这个目标是不会执行的
all:$(SUBDIRS) $(CUR_OBJS) DEBUG ECHO

#递归执行子目录下的makefile文件，这是递归执行的关键
$(SUBDIRS): CREATE_DIR
#判断　TARGET_DIR　变量是否为空
ifeq ($(TARGET_DIR), )
#$@--目标文件，$^--所有依赖文件，$<--第一个依赖文件
	make -C $@
else
	make -C $(TARGET_DIR)

endif

DEBUG:
#直接去debug目录下执行makefile文件
#	rm ./obj/makefile
	@\cp ./make/makefile-src ./obj/makefile
	make -C obj
#

#输出提示信息
ECHO:
	@echo "into dir:[`pwd`] exec make finish"

#创建debug子目录
CREATE_DIR:
	@mkdir -p $(OBJS_DIR)
	@mkdir -p $(OBJS_DIR)/lib
	@mkdir -p $(BIN_DIR)
	
#将c文件编译为o文件，并放在指定放置目标文件的目录中即OBJS_DIR
$(CUR_OBJS):%.o:%.$(PS)
	$(CC) -c $^ -o $(ROOT_DIR)/$(OBJS_DIR)/$@ $(CFLAGS) 
CLEAN:
	@rm -rf $(OBJS_DIR)/*
	@rm -rf $(BIN_DIR)/*