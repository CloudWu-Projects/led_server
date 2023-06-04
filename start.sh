#!/bin/sh
case $1 in
    start)
        echo "start"
        ## led server
        echo "--------------start led server----------"
        echo "--------$led_server_bin--------------"
        nohup supervise ./ledserver/ >/dev/null 2>&1 &
        items_pid= `ps -ef |grep ledserver|grep -v grep|awk '{print $2}'`
        until [ -n "$items_pid" ]
            do
                items_pid=`ps -ef |grep ledserver|grep -v grep|awk '{print $2}'`
                sleep 1
            done
        echo "--------------led server pid is $items_pid----------"
        echo "--------------led server start success----------"

        ## cheyun.py
        echo "--------------start handle_CheYun.py----------"
        nohup supervise ./py/ >/dev/null 2>&1 &
        items_pid= `ps -ef | grep ./py | grep -v grep | awk '{print $2}'`
        until [ -n "$items_pid" ]
            do
                items_pid=`ps -ef | grep ./py | grep -v grep | awk '{print $2}'`
                sleep 1
            done

        echo "--------------handle_CheYun.py pid is $items_pid----------"
        echo "--------------handle_CheYun.py start success----------"

        

        ;;
    stop)
        echo "stop"
        P_ID= `ps -ef | grep ledserver | grep -v grep | awk '{print $2}'`
        if [ ""=="$P_ID" ]; then
            echo "====supervise ledserver not exists"
        else
            kill -9 $P_ID
            echo "====supervise ledserver stop success"
        fi
        P_ID= `ps -ef | grep ./py | grep -v grep | awk '{print $2}'`
        if [ ""=="$P_ID" ]; then
            echo "====supervise ./py not exists"
        else
            kill -9 $P_ID
            echo "====supervise ./py stop success"
        fi

        P_ID= `ps -ef | grep led_server | grep -v grep | awk '{print $2}'`
        if [ ""=="$P_ID" ]; then
            echo "====led_server not exists"
        else
            kill -9 $P_ID
            echo "====led_server stop success"
        fi

        P_ID= `ps -ef | grep handle_CheYun.py | grep -v grep | awk '{print $2}'`
        if [ ""=="$P_ID" ]; then
            echo "====handle_CheYun.py not exists"
        else
            kill -9 $P_ID
            echo "====handle_CheYun.py stop success"   
        fi

       
        ;;
    *)
        echo "Usage: $0 {start|stop}"
        exit 1

        ;;
esac