#!/bin/sh



SERVICE_NAME=PPServer
SERVICE_CONF=conf.yaml

start() {
	cd bin
	nohup ./$SERVICE_NAME ../etc/conf.yaml >>$LOG_DIR/$LOG_NAME 2>&1 &
	./$SERVICE_NAME ../etc/conf.yaml &
	cd ..
	echo -n $"Starting $SERVICE_NAME services"
	echo 
	ps aux | grep $SERVICE_NAME
}

stop() {
	EXE_NUM=`ps -ef | grep -i '$SERVICE_NAME' | grep -v "grep" | wc -l`
	if [ $EXE_NUM -ne 0 ]; then
		killall $SERVICE_NAME
	fi
	echo -n $"Shutting down $SERVICE_NAME services"
	echo 
}

restart() {
	stop
	
	EXE_NUM=`ps -ef | grep -i '$SERVICE_NAME' | grep -v "grep" | wc -l`
	while [ $EXE_NUM -ne 0 ]
	do
		sleep 1;
		EXE_NUM=`ps -ef | grep -i '$SERVICE_NAME' | grep -v "grep" | wc -l`
	done
	
	start
}

helpShowInfo() {
    echo $"Usage: $0 {makefile [dbtype]|makefile2 subdir|clean|start|stop|restart|help}"
}

##############################################################################################

ERROR_OUT=../error.txt

makefile() {


	if [ -z "$1" ];
	then
		DB_TYPE=mysql
	else
		if [ "$1" != "mysql" -a "$1" != "oracle" ];
		then
			echo "Parameter error [mysql or oracle]"
			exit
		fi
		
		if [ "$1" = "mysql" ]; then
			DB_TYPE=mysql
		fi
		
		if [ "$1" = "oracle" ]; then 
			ORACLE_BASE=/home/oracle/app
			ORACLE_HOME=$ORACLE_BASE/oracle/product/11.2.0/dbhome_1
			ORACLE_SID=orcl
			PATH=$PATH:$ORACLE_HOME/bin
			LD_LIBRARY_PATH=$ORACLE_HOME/lib:/usr/lib

			DB_TYPE=oracle

			export ORACLE_BASE ORACLE_HOME ORACLE_SID PATH LD_LIBRARY_PATH
		fi
		
	fi
	export DB_TYPE

	rm -rf ./bin/PPServer 
	#make 编译错误信息输出到文件
	make 2>$ERROR_OUT
	echo 
	if [ ! -s $ERROR_OUT ]; then
		echo "make ok"
	else
		echo "make fail"
		echo "ouput error info to [$ERROR_OUT]"
	fi

}
#表示两个参数，第一个参数为　指定目录，第二个参数为数据库类型
makefile2() {

	if [ -z "$1" ];
	then
		echo "input subdir!"
		exit 
	else
		TARGET_DIR=$1
		
		DB_TYPE=mysql
		
		if [ "$2" = "oracle" ]; then 
			ORACLE_BASE=/home/oracle/app
			ORACLE_HOME=$ORACLE_BASE/oracle/product/11.2.0/dbhome_1
			ORACLE_SID=orcl
			PATH=$PATH:$ORACLE_HOME/bin
			LD_LIBRARY_PATH=$ORACLE_HOME/lib:/usr/lib

			DB_TYPE=oracle

			export ORACLE_BASE ORACLE_HOME ORACLE_SID PATH LD_LIBRARY_PATH
		fi
		
	fi
	export DB_TYPE
	export TARGET_DIR
	
	make 2>$ERROR_OUT
	echo 
	if [ ! -s $ERROR_OUT ]; then
		echo "make ok"
	else
		echo "make fail"
		echo "make -C $1 2>$ERROR_OUT"
		echo "ouput error info to [$ERROR_OUT]"
	fi

}
clean() {
	make CLEAN
}



case "$1" in
  start)
        start
        ;;
  stop)
        stop
        ;;
  restart)
        restart
        ;;
  help)
		helpShowInfo
        exit 2
        ;;
  makefile)
        makefile $2
        ;;
  makefile2)
        makefile2 $2
        ;;
  clean)
        clean
        ;;
  *)
        helpShowInfo
        ;;
esac

exit $?