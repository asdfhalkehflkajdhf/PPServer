#!/bin/sh

#写入crontab的数据格式，启动进程自检验。还需要写个停止的脚本
#echo "#*/1 * * * *  /etc/%{name}/%{name}-cron.sh " >> ar/spool/cron/root

SERVICE_NAME=PPServer
SERVICE_CONF=conf.yaml

start() {
	cd bin
#		nohup ./$SERVICE_NAME ../etc/conf.yaml >>$LOG_DIR/$LOG_NAME 2>&1 &
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
checkproc() {
	ps -ef | grep -i $SERVICE_NAME | grep -v "grep"
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
    echo $"Usage: $0 {start|stop|restart|checkproc|help}"
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
  checkproc)
		checkproc
		;;
  help)
		helpShowInfo
        exit 2
        ;;

  * )
        helpShowInfo
        ;;
esac

exit $?