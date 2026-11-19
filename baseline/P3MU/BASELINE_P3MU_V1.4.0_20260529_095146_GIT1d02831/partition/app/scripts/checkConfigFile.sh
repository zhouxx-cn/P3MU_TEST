#!/bin/sh

#该脚本必须放到APP，CFG分区做完链接后调用
#功能：
#1.检测目录是否存在
#2.检测关键文件是否存在

CFG_PATH="/mnt/mtd"
APP_CFG_PATH="$CFG_PATH/config"
ETC_CFG_PATH="$CFG_PATH/etc"
HTTP_CFG_PATH="$CFG_PATH/http"
CUSTOM_DIR="/mnt/app/custom"

cfgComplete()
{
	if [ `tail $1 | sed ':label;N;s/\n//g;b label' | grep "\[end\]dvr=idvr9000"` ] \
		&& [ `head $1 | sed ':label;N;s/\n//g;b label' | grep "\[system\]dvr=idvr9000"` ]
	then
		echo "yes"
	fi
}

if [ ! -d $ETC_CFG_PATH/network/ ]
then
	echo "$ETC_CFG_PATH/network/ not exsit!"
	mkdir -p $ETC_CFG_PATH/network/
fi

#如果CUSTOM下的配置存在，则Edvr.default连接到CUSTOM
if [ -e $CUSTOM_DIR/config/Edvr.default ]
then
	if [ ! -e $APP_CFG_PATH/Edvr.default ]
	then
		echo "Not Exist, Recover Edvr.default from /custom/config/Edvr.default"
		cp $CUSTOM_DIR/config/Edvr.default $APP_CFG_PATH/Edvr.default;
	fi
	
	if [ ! `cfgComplete "$APP_CFG_PATH/Edvr.default"` ]
	then
		echo "Not Complete, Recover Edvr.default from /custom/config/Edvr.default"
		cp $CUSTOM_DIR/config/Edvr.default $APP_CFG_PATH/Edvr.default;
	fi
	
	if [ -h $APP_CFG_PATH/Edvr.default ]
	then
		rm -rf $APP_CFG_PATH/Edvr.default;
		cp $CUSTOM_DIR/config/Edvr.default $APP_CFG_PATH/Edvr.default;
	fi
fi

if [ ! -e $APP_CFG_PATH/Edvr.cfg ]
then
	if [ -e $APP_CFG_PATH/Edvr.cfg-bak ]
	then
		echo "Not Exist, Recover Edvr.cfg from Edvr.cfg-bak"
		cp $APP_CFG_PATH/Edvr.cfg-bak $APP_CFG_PATH/Edvr.cfg
	else
		echo "Not Exist, Recover Edvr.cfg and Edvr.cfg-bak from Edvr.default"
		cp $APP_CFG_PATH/Edvr.default $APP_CFG_PATH/Edvr.cfg
		cp $APP_CFG_PATH/Edvr.default $APP_CFG_PATH/Edvr.cfg-bak
	fi
else
	if [ ! `cfgComplete "$APP_CFG_PATH/Edvr.cfg"` ]
	then
		if [ -e $APP_CFG_PATH/Edvr.cfg-bak ] \
			&& [ `cfgComplete "$APP_CFG_PATH/Edvr.cfg-bak"` ]
		then
			echo "Not Complete, Recover Edvr.cfg from Edvr.cfg-bak"
			cp $APP_CFG_PATH/Edvr.cfg-bak $APP_CFG_PATH/Edvr.cfg
		else
			echo "Not Complete, Recover Edvr.cfg and Edvr.cfg-bak from Edvr.default"
			cp $APP_CFG_PATH/Edvr.default $APP_CFG_PATH/Edvr.cfg
			cp $APP_CFG_PATH/Edvr.default $APP_CFG_PATH/Edvr.cfg-bak
		fi
	fi
fi

if [ ! -e $APP_CFG_PATH/Edvr.cfg-bak ] || [ ! `cfgComplete "$APP_CFG_PATH/Edvr.cfg-bak"` ]
then
	echo "Not-Exist or Not-Complete, Recover Edvr.cfg-bak from Edvr.cfg"
	cp $APP_CFG_PATH/Edvr.cfg $APP_CFG_PATH/Edvr.cfg-bak
fi

if [ `ls -l $APP_CFG_PATH/Edvr.cfg | awk '{print $5}'` -lt `ls -l $APP_CFG_PATH/Edvr.default | awk '{print $5}'` ]
then
	echo "Size too Small, Recover Edvr.cfg from Edvr.default"
	cp $APP_CFG_PATH/Edvr.default $APP_CFG_PATH/Edvr.cfg
fi

#手动申请did，服务器地址
if [ -e $APP_CFG_PATH/p2pSer.cfg-bak ]
then
	if [ "p2pSer.cfg-bak" = `ls -lt $APP_CFG_PATH | grep p2p | head -n 1 | awk '{printf $9}'` ] \
		|| [ "2021-01-01" = `stat $APP_CFG_PATH/p2pSer.cfg-bak | grep Modify | awk '{print $2}'` ]
	then
		cp $APP_CFG_PATH/p2pSer.cfg-bak $APP_CFG_PATH/p2pSer.cfg
	fi
else
	if [ -e $APP_CFG_PATH/p2pSer.cfg ]
	then
		cp $APP_CFG_PATH/p2pSer.cfg $APP_CFG_PATH/p2pSer.cfg-bak
	fi	
fi

#if [ ! -L $1/etc/udhcpd.conf ]
#then
#	if [ -e $1/etc/udhcpd.conf ]
#	then
#		rm -rf $1/etc/udhcpd.conf
#	fi
#
#	ln -s /custom/config/udhcpd.conf $1/etc/udhcpd.conf
#fi

if [ ! -e $ETC_CFG_PATH/network/interfaces ]
then
	echo "recover $ETC_CFG_PATH/network/interfaces"
	echo "auto lo" >> $ETC_CFG_PATH/network/interfaces
	echo "iface lo inet loopback" >> $ETC_CFG_PATH/network/interfaces
	echo "" >> $ETC_CFG_PATH/network/interfaces
	echo "auto eth0" >> $ETC_CFG_PATH/network/interfaces
	echo "iface eth0 inet static" >> $ETC_CFG_PATH/network/interfaces
	echo "address 192.168.1.11" >> $ETC_CFG_PATH/network/interfaces
	echo "netmask 255.255.255.0" >> $ETC_CFG_PATH/network/interfaces
	echo "gateway 192.168.1.1" >> $ETC_CFG_PATH/network/interfaces
	echo "" >> $ETC_CFG_PATH/network/interfaces
	echo "auto wlan0" >> $ETC_CFG_PATH/network/interfaces
	echo "iface wlan0 inet static" >> $ETC_CFG_PATH/network/interfaces
	echo "address 192.168.2.11" >> $ETC_CFG_PATH/network/interfaces
	echo "netmask 255.255.255.0" >> $ETC_CFG_PATH/network/interfaces
	echo "gateway 192.168.2.1" >> $ETC_CFG_PATH/network/interfaces
fi

if [ ! -e $ETC_CFG_PATH/resolv.conf ]
then
	echo "recover $ETC_CFG_PATH/resolv.conf"
	if [ -e /app/root/edvr/ConfigBak/etc/resolv.conf ]
	then
		cp /app/root/edvr/ConfigBak/etc/resolv.conf $ETC_CFG_PATH/resolv.conf
	else
		echo "nameserver  114.114.114.114" >> $ETC_CFG_PATH/resolv.conf
		echo "nameserver  8.8.8.8" >> $ETC_CFG_PATH/resolv.conf
	fi
fi

if [ ! -e $ETC_CFG_PATH/resolv_wlan.conf ]
then
	echo "recover $ETC_CFG_PATH/resolv_wlan.conf"
	if [ -e /app/root/edvr/ConfigBak/etc/resolv_wlan.conf ]
	then
		cp /app/root/edvr/ConfigBak/etc/resolv_wlan.conf $ETC_CFG_PATH/resolv_wlan.conf
	else
		echo "nameserver  nameserver 192.168.2.1" >> $ETC_CFG_PATH/resolv_wlan.conf
		echo "nameserver 1.0.0.0" >> $ETC_CFG_PATH/resolv.conf
	fi
fi

if [ ! -e $ETC_CFG_PATH/wifi.conf ]
then
	echo "recover $ETC_CFG_PATH/wifi.conf"
	echo "" >> $ETC_CFG_PATH/wifi.conf
	echo "ctrl_interface=/var/run/wpa_supplicant" >> $ETC_CFG_PATH/wifi.conf
	echo "" >> $ETC_CFG_PATH/wifi.conf
fi

#Config分区文件缺失会从ConfigBak复制，已存在的不会覆盖
#awk 'BEGIN { cmd="cp -ri /app/root/edvr/ConfigBak/*  /config/ "; print "n" |cmd; }' 2> /dev/null 
