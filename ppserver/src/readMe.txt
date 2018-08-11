当前文件夹下每一个文件都是一个模块

情况一、添加一个公共模块
	新建testUtil文件夹，
	复制make目录下　makefile-src 文件到testUtil文件夹改为makefile 。


情况二、添加一个公共模块，有私有系统头文件和库
	新建testUtil文件夹，
	复制make目录下　makefile-src 文件到testUtil文件夹改为makefile 。
	
#配置私和环境变量，
USER_INCLUDES= 
USER_LIBS=
USER_LIBS_DIR=
	
	注意：最好不要用，没试过，还有就是makefile-src只编译不连接
	
情况三、添加一个生成可执行程序
	新建testBin文件夹，
	复制make目录下　makefile-src 文件到testBin文件夹改为makefile 。
	
#必选项，最后会在bin目录下生成同名可执行文件
DEST=

#配置私和环境变量，
USER_INCLUDES= 
USER_LIBS=
USER_LIBS_DIR=

	复制make目录下　makefile-bin 文件到testBin文件夹，不改名
如果有需要可以
#配置私和环境变量，
USER_INCLUDES= 
USER_LIBS=
USER_LIBS_DIR=

情况四、添加一个生成库文件，参考情况三



注意：每个模块中不能有同名文件，
		所有公共模块中不能有同名文件，
		
		