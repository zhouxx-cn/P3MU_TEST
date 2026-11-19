#!/bin/sh

#功能：
#1.检测分区是否满

CFG_PATH="/mnt/mtd"
APP_CFG_PATH="$CFG_PATH/config"
ETC_CFG_PATH="$CFG_PATH/etc"
HTTP_CFG_PATH="$CFG_PATH/http"
CUSTOM_DIR="/mnt/app/custom"
TESTFILE="$CFG_PATH/.TestFile"

#开始检测分区是否满
echo "Start check partition full!"

# 检查data是否满
data_used=`df -h | grep data | awk '{print $5}' | tr -d "[a-z][A-Z]%"`
if [ $data_used -gt 90 ]; then
	echo "[XRS][LOG] Data Partition Full, Remove Log..."
	rm -rf /mnt/mtd/log/*
fi

integer=1
while [ $integer -le 3 ]
do  
    /bin/touch "$TESTFILE";

	if [ -e "$TESTFILE" ]
	then
        echo "partition $CFG_PATH not full!";
        rm "$TESTFILE";
		break;
	else
		if [ $integer = 1 ]
		then
			echo "partition $CFG_PATH is full";
			echo "Delete log.db"
			rm -rf $APP_CFG_PATH/log.db*;
			echo "Delete Custom Audio Files..."
			rm -rf $APP_CFG_PATH/motionCustomAlarm.g711u
			rm -rf $APP_CFG_PATH/carCustomAlarm.g711u
			rm -rf $APP_CFG_PATH/fireCustomAlarm.g711u
		elif [ $integer = 2 ]
		then
			echo "partition $CFG_PATH is full";
			echo "Delete boa.conf-bak"
			rm -rf $CFG_PATH/etc/conf.d/boa/boa.conf-bak
		elif [ $integer = 3 ]
		then
			echo "partition $CFG_PATH is full";
			echo "Delete pppoe.conf-bak"
			rm -rf $CFG_PATH/etc/ppp/pppoe.conf-bak
		fi
	fi
	integer=$(($integer+1))
done

echo "End check partition full!"
