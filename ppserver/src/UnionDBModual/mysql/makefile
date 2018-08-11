
#生成目标(默认是空，即不生成目标，只生成.o文件)
#第一种，和当前目录名同名的可执行文件或库名
#DEST=$(notdir  $(shell pwd))

#第二种，生成指定可执行文件或库名文件
#DEST=test

#第三种，不生成可执行文件或库名文件，不定义DEST或是置空
DEST=


#生成DEST类型（如果有一个文件时可以使用*，两个文件时必须使用确定后缀）				#bin或是lib
DEST_TYPE=*


#当前目录下不需要编译的子目录列表
ELIMINATE_LIST= config lib include

#配置私和环境变量，
USER_INCLUDES= 
USER_LIBS=
USER_LIBS_DIR=


#以上为使用者可配置变量
##################################################################################

USER_CFLAGS = $(addprefix -L,$(USER_LIBS_DIR))  $(addprefix -I, $(USER_INCLUDES))  $(addprefix -l,$(USER_LIBS)) 


#子目录的Makefile直接读取其子目录就行
ALL_SUBDIRS=$(shell ls -l | grep ^d | awk '{print $$9}')

#去除config lib include 三个子目录
SUBDIRS=$(filter-out $(ELIMINATE_LIST) , $(ALL_SUBDIRS))

#当前目录下所以cpp 文件名
CUR_SOURCE=${wildcard *.$(PS)}
#当前目录下所有cpp 文件的 .o目标文件名
CUR_OBJS=${patsubst %.$(PS), %.o, $(CUR_SOURCE)}


all:$(SUBDIRS) $(CUR_OBJS) ECHO
$(SUBDIRS): NULLREFER
	make -C $@
$(CUR_OBJS):%.o:%.$(PS)
ifeq ($(DEST), )
#$@--目标文件，$^--所有依赖文件，$<--第一个依赖文件 -c 参数在生成.o时不需要连接动态库
	$(CC)  $(CFLAGS) $(USER_CFLAGS) -c $^ -o $(OBJS_DIR)/$@ 
else
	@mkdir -p $(OBJS_DIR)/$(DEST)
	$(CC)  $(CFLAGS) $(USER_CFLAGS) -c $^ -o $(OBJS_DIR)/$(DEST)/$@
	@rm -rf $(OBJS_DIR)/$(DEST)/makefile
	@cp ./makefile-$(DEST_TYPE) $(OBJS_DIR)/$(DEST)/makefile
endif

#输出提示信息
ECHO:
	@echo "into dir:[`pwd`] exec make finish"

NULLREFER:
	