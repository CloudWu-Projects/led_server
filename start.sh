#!/bin/sh
export led_server_bin=./build/source/led_server
export cheyun_bin=./testScript/handle_CheYun.py
export SQLITE_WEB_PASSWORD=1qazxsw2
export db_path=./db/led_server.db
export db_port=12090
case $1 in
    start)
        echo "start"
        ## led server
        echo "--------------start led server----------"
        echo "--------$led_server_bin--------------"
        nohup $led_server_bin >/dev/null 2>&1 &
        items_pid= `ps -ef | grep led_server | grep -v grep | awk '{print $2}'`
        until [ -n "$items_pid" ]
            do
                items_pid=`ps -ef | grep led_server | grep -v grep | awk '{print $2}'`
                sleep 1
            done
        echo "--------------led server pid is $items_pid----------"
        echo "--------------led server start success----------"

        ## cheyun.py
        echo "--------------start handle_CheYun.py----------"
        nohup python $cheyun_bin >/dev/null 2>&1 &
        items_pid= `ps -ef | grep handle_CheYun.py | grep -v grep | awk '{print $2}'`
        until [ -n "$items_pid" ]
            do
                items_pid=`ps -ef | grep handle_CheYun.py | grep -v grep | awk '{print $2}'`
                sleep 1
            done

        echo "--------------handle_CheYun.py pid is $items_pid----------"
        echo "--------------handle_CheYun.py start success----------"

        

        ;;
    stop)
        echo "stop"
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