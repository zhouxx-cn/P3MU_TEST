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
DATA_CFG_PATH="/mnt/app/data/config"
ETC_READ_ONLY_PATH="/mnt/app/data/config/etc"

cfgComplete()
{
	if [ `tail $1 | sed ':label;N;s/\n//g;b label' | grep "\[end\]dvr=idvr9000"` ] \
		&& [ `head $1 | sed ':label;N;s/\n//g;b label' | grep "\[system\]dvr=idvr9000"` ]
	then
		echo "yes"
	fi
}

# 不会变的配置文件用这个比较 （比较备份区的和data分区的）
FixedFileComplete()
{
	#比较备份分区的文件和data分区的文件，是不是一样的
	if [cat $1 -eq cat $2]
	then
		echo "yes"
	fi
}

# 会被修改的配置文件，检查配置文件的键值是不是配对的
keyValueComplete()
{
	# 加载文件；获取配置；读取数据；配对数据格式
	if [ `head -n 1 $1 ` ]
	then
		echo "yes"
	fi
}


if [ ! -d $ETC_CFG_PATH/network/ ]
then
	echo "$ETC_CFG_PATH/network/ not exsit!"
	mkdir -p $ETC_CFG_PATH/network/
fi

# 如果CUSTOM下的配置存在，则Edvr.default连接到CUSTOM
if [ -e $DATA_CFG_PATH/config/Edvr.default ]
then
	if [ ! -e $APP_CFG_PATH/Edvr.default ]
	then
		echo "Not Exist, Recover Edvr.default from /custom/config/Edvr.default"
		cp $DATA_CFG_PATH/config/Edvr.default $APP_CFG_PATH/Edvr.default;
	fi
	
	if [ ! `cfgComplete "$APP_CFG_PATH/Edvr.default"` ]
	then
		echo "Not Complete, Recover Edvr.default from /custom/config/Edvr.default"
		cp $DATA_CFG_PATH/config/Edvr.default $APP_CFG_PATH/Edvr.default;
	fi
	
	if [ -h $APP_CFG_PATH/Edvr.default ]
	then
		rm -rf $APP_CFG_PATH/Edvr.default;
		cp $DATA_CFG_PATH/config/Edvr.default $APP_CFG_PATH/Edvr.default;
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


# Edvr.flag
if [ -e $DATA_CFG_PATH/config/Edvr.flag ]
then
	if [ ! -e $APP_CFG_PATH/Edvr.flag ]
	then
		echo "Not Exist, Recover Edvr.flag from /custom/config/Edvr.flag"
		cp $DATA_CFG_PATH/config/Edvr.flag $APP_CFG_PATH/Edvr.flag;
	fi
	# 文档异常 
	if [ `cat  $APP_CFG_PATH/Edvr.flag | grep "AlarmInCfg"` != "AlarmInCfg" ]
	then
		echo "Edvr.flag file abnormal!!"
		rm -rf $APP_CFG_PATH/Edvr.flag;
		cp $DATA_CFG_PATH/config/Edvr.flag $APP_CFG_PATH/Edvr.flag;
	fi
fi

# 手动申请did，服务器地址 p2pSer 
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
	else
		if [ -e $DATA_CFG_PATH/config/p2pSer.cfg-bak ]
		then
			cp $DATA_CFG_PATH/config/p2pSer.cfg-bak $APP_CFG_PATH/p2pSer.cfg-bak
			cp $DATA_CFG_PATH/config/p2pSer.cfg-bak $APP_CFG_PATH/p2pSer.cfg
		fi
	fi
fi

# SDCardCheckUseFsckfat.conf
if [ -e $DATA_CFG_PATH/config/SDCardCheckUseFsckfat.conf ]
then
	if [ ! -e $APP_CFG_PATH/SDCardCheckUseFsckfat.conf ]
	then
		echo "Not Exist, Recover SDCardCheckUseFsckfat from /custom/config/SDCardCheckUseFsckfat.conf"
		cp $DATA_CFG_PATH/config/SDCardCheckUseFsckfat.conf $APP_CFG_PATH/SDCardCheckUseFsckfat.conf;
	fi
	
	if [ `cat  $APP_CFG_PATH/SDCardCheckUseFsckfat.conf | grep "Config_BootNoBack_Choice"` != "Config_BootNoBack_Choice=1" ]
	then
		echo "SDCardCheckUseFsckfat.conf file abnormal!!"
		rm -rf $APP_CFG_PATH/SDCardCheckUseFsckfat.conf;
		cp $DATA_CFG_PATH/config/SDCardCheckUseFsckfat.conf $APP_CFG_PATH/SDCardCheckUseFsckfat.conf;
	fi
fi

# NtpServer.txt
if [ -e $DATA_CFG_PATH/config/NtpServer.txt ]
then
	if [ ! -e $APP_CFG_PATH/NtpServer.txt ]
	then
		echo "Not Exist, Recover NtpServer.txt from /custom/config/NtpServer.txt"
		cp $DATA_CFG_PATH/config/NtpServer.txt $APP_CFG_PATH/NtpServer.txt;
	fi
	if [ `cat  $APP_CFG_PATH/NtpServer.txt | grep "time.windows.com"` != "time.windows.com" ]
	then
		echo "NtpServer.txt file abnormal!!"
		rm -rf $APP_CFG_PATH/NtpServer.txt;
		cp $DATA_CFG_PATH/config/NtpServer.txt $APP_CFG_PATH/NtpServer.txt;
	fi
fi

# DdnsServer.txt
if [ -e $DATA_CFG_PATH/config/DdnsServer.txt ]
then
	if [ ! -e $APP_CFG_PATH/DdnsServer.txt ]
	then
		echo "Not Exist, Recover DdnsServer.txt from /custom/config/DdnsServer.txt"
		cp $DATA_CFG_PATH/config/DdnsServer.txt $APP_CFG_PATH/DdnsServer.txt;
	fi
	if [[ `cat  $APP_CFG_PATH/DdnsServer.txt | grep "justlinux,www.justlinux.com"` != "justlinux,www.justlinux.com" ]]
	then
		echo "Not Exist, DdnsServer.txt file abnormal!!"
		rm -rf $APP_CFG_PATH/DdnsServer.txt;
		cp $DATA_CFG_PATH/config/DdnsServer.txt $APP_CFG_PATH/DdnsServer.txt;
	fi
fi

# hostapd.conf
if [ -e $DATA_CFG_PATH/config/hostapd.conf ]
then
	if [ ! -e $APP_CFG_PATH/hostapd.conf ]
	then
		echo "Not Exist, Recover hostapd.conf from /custom/config/hostapd.conf"
		cp $DATA_CFG_PATH/config/hostapd.conf $APP_CFG_PATH/hostapd.conf;
	fi
	# 文档异常 
	if [ `cat  $APP_CFG_PATH/hostapd.conf | grep "driver=nl80211"` != "driver=nl80211" ]
	then
		echo "hostapd.conf file abnormal!!"
		rm -rf $APP_CFG_PATH/hostapd.conf;
		cp $DATA_CFG_PATH/config/hostapd.conf $APP_CFG_PATH/hostapd.conf;
	fi
fi

# host.conf
if [ -e $DATA_CFG_PATH/config/host.conf ]
then
	if [ ! -e $APP_CFG_PATH/host.conf ]
	then
		echo "Not Exist, Recover host.conf from /custom/config/host.conf"
		cp $DATA_CFG_PATH/config/host.conf $APP_CFG_PATH/host.conf;
	fi
	# 文档异常 
	if [ `cat  $APP_CFG_PATH/host.conf | grep "domain_0"` != "domain_0" ]
	then
		echo "host.conf file abnormal!!"
		rm -rf $APP_CFG_PATH/host.conf;
		cp $DATA_CFG_PATH/config/host.conf $APP_CFG_PATH/host.conf;
	fi
fi

# httpd.conf
if [ -e $DATA_CFG_PATH/config/httpd.conf ]
then
	if [ ! -e $APP_CFG_PATH/httpd.conf ]
	then
		echo "Not Exist, Recover httpd.conf from /custom/config/httpd.conf"
		cp $DATA_CFG_PATH/config/httpd.conf $APP_CFG_PATH/httpd.conf;
	fi
	# 文档异常 
	if [ `cat  $APP_CFG_PATH/httpd.conf | grep "80"` == "" ]
	then
		echo "httpd.conf file abnormal!!"
		rm -rf $APP_CFG_PATH/httpd.conf;
		cp $DATA_CFG_PATH/config/httpd.conf $APP_CFG_PATH/httpd.conf;
	fi
fi

# tutk_config.ini
if [ -e $DATA_CFG_PATH/config/tutk_config.ini ]
then
	if [ ! -e $APP_CFG_PATH/tutk_config.ini ]
	then
		echo "Not Exist, Recover tutk_config.ini from /custom/config/tutk_config.ini"
		cp $DATA_CFG_PATH/config/tutk_config.ini $APP_CFG_PATH/tutk_config.ini;
	fi
	# 文档异常 
	if [ `cat  $APP_CFG_PATH/tutk_config.ini | grep "dvr=idvr9000"` != "dvr=idvr9000" ]
	then
		echo "tutk_config.ini file abnormal!!"
		rm -rf $APP_CFG_PATH/tutk_config.ini;
		cp $DATA_CFG_PATH/config/tutk_config.ini $APP_CFG_PATH/tutk_config.ini;
	fi
fi



# tutkuid.conf
if [ -e $DATA_CFG_PATH/config/tutkuid.conf ]
then
	if [ ! -e $APP_CFG_PATH/tutkuid.conf ]
	then
		echo "Not Exist, Recover tutkuid.conf from /custom/config/tutkuid.conf"
		cp $DATA_CFG_PATH/config/tutkuid.conf $APP_CFG_PATH/tutkuid.conf;
	fi
	# 文档异常 
	if [ `cat  $APP_CFG_PATH/tutkuid.conf | grep "1"` == "" ]
	then
		echo "tutkuid.conf file abnormal!!"
		rm -rf $APP_CFG_PATH/tutkuid.conf;
		cp $DATA_CFG_PATH/config/tutkuid.conf $APP_CFG_PATH/tutkuid.conf;
	fi
fi


#config.ini
if [ -e $DATA_CFG_PATH/config/config.ini ]
then
	if [ ! -e $APP_CFG_PATH/config.ini ]
	then
		echo "Not Exist, Recover config.ini from /custom/config/config.ini"
		cp $DATA_CFG_PATH/config/config.ini $APP_CFG_PATH/config.ini;
	fi
	# 文档异常 
	if [ `cat  $APP_CFG_PATH/config.ini | grep "idvr9000"` != "idvr9000" ]
	then
		echo "config.ini file abnormal!!"
		rm -rf $APP_CFG_PATH/config.ini;
		cp $DATA_CFG_PATH/config/config.ini $APP_CFG_PATH/config.ini;
	fi
fi




# custommode.ini
if [ -e $DATA_CFG_PATH/config/custommode.ini ];
then
	if [ ! -e $APP_CFG_PATH/custommode.ini ]
	then
		echo "Not Exist, Recover custommode.ini from /custom/config/custommode.ini"
		cp $DATA_CFG_PATH/config/custommode.ini $APP_CFG_PATH/custommode.ini;
	fi
	if [ ! `cfgComplete "$APP_CFG_PATH/custommode.ini"` ]
	then
		echo "custommode.ini file abnormal!!"
		rm -rf $APP_CFG_PATH/custommode.ini;
		cp $DATA_CFG_PATH/config/custommode.ini $APP_CFG_PATH/custommode.ini;
	fi
fi

# custominfo.ini
if [ -e $DATA_CFG_PATH/config/custominfo.ini ];
then
	if [ ! -e $APP_CFG_PATH/custominfo.ini ]
	then
		echo "Not Exist, Recover custominfo.ini from /custom/config/custominfo.ini"
		cp $DATA_CFG_PATH/config/custominfo.ini $APP_CFG_PATH/custominfo.ini;
	fi
	if [ ! `cfgComplete "$APP_CFG_PATH/custominfo.ini"` ]
	then
		echo "custominfo.ini file abnormal!!"
		rm -rf $APP_CFG_PATH/custominfo.ini;
		cp $DATA_CFG_PATH/config/custominfo.ini $APP_CFG_PATH/custominfo.ini;
	fi
fi
# custominfo.ini-bak
if [ ! -e $APP_CFG_PATH/custominfo.ini-bak ] || [ ! `cfgComplete "$APP_CFG_PATH/custominfo.ini-bak"` ]
then
	echo "Not-Exist or Not-Complete, Recover custominfo.ini-bak from custominfo.ini"
	cp $APP_CFG_PATH/custominfo.ini $APP_CFG_PATH/custominfo.ini-bak
fi

# customability.ini
if [ -e $DATA_CFG_PATH/config/customability.ini ];
then
	if [ ! -e $APP_CFG_PATH/customability.ini ]
	then
		echo "Not Exist, Recover customability.ini from /custom/config/customability.ini"
		cp $DATA_CFG_PATH/config/customability.ini $APP_CFG_PATH/customability.ini;
	fi
	if [ ! `cfgComplete "$APP_CFG_PATH/customability.ini"` ]
	then
		echo "customability.ini file abnormal!!"
		rm -rf $APP_CFG_PATH/customability.ini;
		cp $DATA_CFG_PATH/config/customability.ini $APP_CFG_PATH/customability.ini;
	fi
fi
# customability.ini-bak
if [ ! -e $APP_CFG_PATH/customability.ini-bak ] || [ ! `cfgComplete "$APP_CFG_PATH/customability.ini-bak"` ]
then
	echo "Not-Exist or Not-Complete, Recover customability.ini-bak from customability.ini"
	cp $APP_CFG_PATH/customability.ini $APP_CFG_PATH/customability.ini-bak
fi

# customcfg.ini
if [ -e $DATA_CFG_PATH/config/customcfg.ini ];
then
	if [ ! -e $APP_CFG_PATH/customcfg.ini ]
	then
		echo "Not Exist, Recover customcfg.ini from /custom/config/customcfg.ini"
		cp $DATA_CFG_PATH/config/customcfg.ini $APP_CFG_PATH/customcfg.ini;
	fi
	if [ ! `cfgComplete "$APP_CFG_PATH/customcfg.ini"` ]
	then
		echo "customability.ini file abnormal!!"
		rm -rf $APP_CFG_PATH/customcfg.ini;
		cp $DATA_CFG_PATH/config/customcfg.ini $APP_CFG_PATH/customcfg.ini;
	fi
fi
# customcfg.ini-bak
if [ ! -e $APP_CFG_PATH/customcfg.ini-bak ] || [ ! `cfgComplete "$APP_CFG_PATH/customcfg.ini-bak"` ]
then
	echo "Not-Exist or Not-Complete, Recover customcfg.ini-bak from customcfg.ini"
	cp $APP_CFG_PATH/customcfg.ini $APP_CFG_PATH/customcfg.ini-bak
fi

# udhcpd.conf
if [ -e $DATA_CFG_PATH/config/udhcpd.conf ];
then
	if [ ! -e $APP_CFG_PATH/udhcpd.conf ]
	then
		echo "Not Exist, Recover udhcpd.conf"
		cp $DATA_CFG_PATH/config/udhcpd.conf $APP_CFG_PATH/udhcpd.conf;
	fi
	if [ ! `cfgComplete "$APP_CFG_PATH/udhcpd.conf"` ]
	then
		echo "udhcpd.conf file abnormal!!"
		rm -rf $APP_CFG_PATH/udhcpd.conf;
		cp $DATA_CFG_PATH/config/udhcpd.conf $APP_CFG_PATH/udhcpd.conf;
	fi
fi

# udhcpd.conf
if [ -e $DATA_CFG_PATH/config/udhcpd.conf ];
then
	if [ ! -e $APP_CFG_PATH/udhcpd.conf ]
	then
		echo "Not Exist, Recover udhcpd.conf"
		cp $DATA_CFG_PATH/config/udhcpd.conf $APP_CFG_PATH/udhcpd.conf;
	fi
	if [ ! `cfgComplete "$APP_CFG_PATH/udhcpd.conf"` ]
	then
		echo "udhcpd.conf file abnormal!!"
		rm -rf $APP_CFG_PATH/udhcpd.conf;
		cp $DATA_CFG_PATH/config/udhcpd.conf $APP_CFG_PATH/udhcpd.conf;
	fi
fi

#interfaces
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
		echo "nameserver  192.168.2.1" >> $ETC_CFG_PATH/resolv_wlan.conf
		echo "nameserver  1.0.0.0" >> $ETC_CFG_PATH/resolv.conf
	fi
fi

if [ ! -e $ETC_CFG_PATH/wifi.conf ]
then
	echo "recover $ETC_CFG_PATH/wifi.conf"
	echo "" >> $ETC_CFG_PATH/wifi.conf
	echo "ctrl_interface=/var/run/wpa_supplicant" >> $ETC_CFG_PATH/wifi.conf
	echo "" >> $ETC_CFG_PATH/wifi.conf
fi

if [ ! -e $ETC_CFG_PATH/udhcpc/default.script ]
then
	echo "recover $ETC_CFG_PATH/udhcpc/default.script"
	cp $ETC_READ_ONLY_PATH/udhcpc/default.script $ETC_CFG_PATH/udhcpc/default.script
fi

#Config分区文件缺失会从ConfigBak复制，已存在的不会覆盖
#awk 'BEGIN { cmd="cp -ri /app/root/edvr/ConfigBak/*  /config/ "; print "n" |cmd; }' 2> /dev/null 
#awk 'BEGIN { cmd="cp -ri /mnt/app/data/config/config/*  /config/ "; print "n" |cmd; }' 2> /dev/null 





