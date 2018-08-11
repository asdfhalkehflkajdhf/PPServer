#include $(ORACLE_HOME)/precomp/lib/env_precomp.mk

#子目录的Makefile直接读取其子目录就行
SUBDIRS=$(shell ls -l | grep ^d | awk '{print $$9}')
#以下同根目录下的makefile的相同代码的解释
PS=cpp

CUR_PC_SOURCE=${wildcard *.pc}
CUR_OBJS=${patsubst %.pc, %.o, $(CUR_PC_SOURCE)}

#在生成.o时不需要连接动态库
#LIBS = pthread kfpubs
#DEST=$(notdir  $(shell pwd))


#code=cpp 这个参数表明我们通过 proc 预编译命令产生的代码文件是符合 C++ 规范的,如果您是在标准C环境中使用, 那么您可以指定 code=ansi_c  ? 
#cpp_suffix=cc 这个参数让 proc 命令产生的文件以 .cc 作为文件扩展名 
#proc -lclntsh parse=no code=cpp cpp_suffix=cpp iname=*.pc




all:$(SUBDIRS) CREATE_CPP $(CUR_OBJS)
$(SUBDIRS):ECHO
	@echo "A"
	make -C $@
CREATE_CPP:
	@echo "B"
	pwd
	#./build.sh
	@rm *.cpp *.lis
	#proc -lclntsh parse=no  THREADS=YES CPOOL=YES CMAX=500 CMIN=100 CINCR=1 code=cpp cpp_suffix=cpp iname=DBOperation.pc 
	proc -lclntsh parse=no THREADS=YES  code=cpp cpp_suffix=cpp iname=DBOperation.pc 
	#$(foreach i, $(CUR_PC_SOURCE), $(shell proc -lclntsh parse=no code=cpp cpp_suffix=cpp iname=$(i)))
	@echo "BB"
$(CUR_OBJS):%.o:%.$(PS)
	@echo "C"
	$(CC)  $(CFLAGS) $(addprefix -L,$(ORA_LIBDIR))  $(addprefix -I,$(ORA_INC)) -lclntsh -c $^ -o $(OBJS_DIR)/$@

ECHO:
	@echo $(SUBDIRS)
