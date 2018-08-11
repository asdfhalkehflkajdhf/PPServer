
#生成目标(默认是空，即不生成目标，只生成.o文件)
#第一种，和当前目录名同名的可执行文件或库名
#DEST=$(notdir  $(shell pwd))

#第二种，生成指定可执行文件或库名文件
#DEST=test

#第三种，不生成可执行文件或库名文件，不定义DEST或是置空
DEST=PPserver


#生成DEST类型（如果有一个文件时可以使用*，两个文件时必须使用确定后缀）
DEST_TYPE=*				#bin或是lib


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


#当前模块编译后保存路径选择
ifeq (2,$(MAKELEVEL))
#当层数为２时表示当前为一个新模块的第一层，需要判断下是否为公共库
CUR_MOD_OBJ_DIR=$(OBJS_DIR)
ifeq ($(DEST), )
#@echo "need to set----------------1-"
else
#DEST 不为空，则使用
CUR_MOD_OBJ_DIR=$(OBJS_DIR)/$(DEST)
endif
endif


all: COMPILE_READY $(SUBDIRS) $(CUR_OBJS) ECHO

#调试信息
PROC:
	@echo "need to set-----------+++++++++++-----0-" $(MAKELEVEL) $(CUR_MOD_OBJ_DIR)
ifeq (2,$(MAKELEVEL))
#当层数为２时表示当前为一个新模块的第一层，需要判断下是否为公共库
	@echo "need to set-----------+++++++++++-----1-" $(CUR_MOD_OBJ_DIR)
ifeq ($(DEST), )
	@echo "need to set-----------+++++++++++-----2-" $(CUR_MOD_OBJ_DIR)
else
#DEST 不为空，则使用
	@echo "need to set-----------+++++++++++-----3-" $(CUR_MOD_OBJ_DIR)
endif

else
	@echo "need to set-----------+++++++++++-----4-"  $(MAKELEVEL)  $(CUR_MOD_OBJ_DIR)
endif

#编译前准备工作
COMPILE_READY:
ifeq (2,$(MAKELEVEL))
	@mkdir -p $(CUR_MOD_OBJ_DIR)
#	$(CC)  $(CFLAGS) $(USER_CFLAGS) -c $^ -o $(OBJS_DIR)/$(DEST)/$@
	@rm -rf $(CUR_MOD_OBJ_DIR)/makefile
	@cp ./makefile-$(DEST_TYPE) $(CUR_MOD_OBJ_DIR)/makefile
endif

#嵌套执行子目录下所有makefile
$(SUBDIRS): NULLREFER
	make -C $@

#编译当前目录的源文件
$(CUR_OBJS):%.o:%.$(PS)
#$@--目标文件，$^--所有依赖文件，$<--第一个依赖文件 -c 参数在生成.o时不需要连接动态库
	$(CC)  $(CFLAGS) $(USER_CFLAGS) -c $^ -o $(CUR_MOD_OBJ_DIR)/$@ 

#输出提示信息
ECHO:
	@echo "into dir:[`pwd`] exec make finish"

NULLREFER:
	