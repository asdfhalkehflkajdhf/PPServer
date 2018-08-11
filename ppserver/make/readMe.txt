第一步：
	使用时把当前目录下 makefile-src文件复制到每个需要编译的文件夹下。改为 makefile
	当需要生成bin或是lib时，把 makefile-bin 或 makefile-lib 复制到相应该文件夹,只能是复制到每个模块文件夹的第一层

第二步：
	对 makefile 文件做相应私有配置
	
注意：
	makefile-bin 或 makefile-lib 每个文件夹下只能有一个，只能是复制到每个模块文件夹的第一层。生成目标文件一模块文件名相同。
	每个模块不要有公共代码，只和当前模块代码相关，
	
	如果模块文件夹的第一层没有　makefile-bin 或 makefile-lib 　则这个模块为公共的。会编译进每个程序。
	
	
	如果修改了src目录层级，必须修改　makefile-src　中　MAKELEVEL　的比较参数，改变２为相应数字。参数当前src目录为２层级。
	
	makefile-test为测试文件，没用。