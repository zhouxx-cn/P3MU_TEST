#!/bin/sh

chmod 777 * -R

CURRENT=$(pwd)
TOPDIR=${CURRENT}/..

# 版型
BOARDTYPE="P3MU"

# SENSOR类型
SENSOR="GC20C3"

# 调试
DEBUG=false

# 设备型号
PRODUCTALIAS="P3MU"

# 用户
CUSTOMER="NORMAL"

# 语言包类型(默认中文)
LANG="CHINESE"

# 客户编号
CUSTOMNO=V001

while getopts ":b:s:d:p:c:l:v:" opt
do
    case $opt in
	    b)
            BOARDTYPE=${OPTARG}
        ;;
		s)
            SENSOR=${OPTARG}
        ;;
        d)
            DEBUG=${OPTARG}
        ;;
        p)
            PRODUCTALIAS=${OPTARG}
        ;;
        c)
            CUSTOMER=${OPTARG}
        ;;
        l)
            LANG=${OPTARG} 
        ;;
		v)
            CUSTOMNO=${OPTARG} 
        ;;
        ?)
            echo "Unknown Param"
        ;;
    esac
done

echo "BOARDTYPE = ${BOARDTYPE}"
echo "SENSOR = ${SENSOR}"
echo "DEBUG = ${DEBUG}"
echo "PRODUCTALIAS = ${PRODUCTALIAS}"
echo "CUSTOMER = ${CUSTOMER}"
echo "LANG = ${LANG}"
echo "CUSTOMNO = ${CUSTOMNO}"

if [ ${DEBUG} = "true" ]; then
$ECHO  "debug mode"
	#删除英文的音频文件
    #rm -rfv $CURRENT/app/custom/speech/english/*
	# 还原调试命令
    #svn revert --recursive $CURRENT/app/bin/ImageToolServer-T41
    #svn revert --recursive $CURRENT/usr/sbin/impdbg
    #svn revert --recursive $CURRENT/usr/sbin/logcat
	#nfs ko还原
    #svn revert --recursive $CURRENT/usr/ko/lockd.ko
    #svn revert --recursive $CURRENT/usr/ko/nfs.ko
    #svn revert --recursive $CURRENT/usr/ko/nfsv3.ko
    #svn revert --recursive $CURRENT/usr/ko/sunrpc.ko
	touch $CURRENT/data/config/en_nfs
	touch $CURRENT/data/config/en_coredump
	cp -rav $CURRENT/nfs_ko/*.ko $CURRENT/usr/ko/
	cp -rav $CURRENT/nfs_ko/com_over_telnet $CURRENT/app/bin/
	#touch $CURRENT/data/config/en_debug_mode
else
$ECHO  "release mode"
	# 发布版本
	#rm -rfv $CURRENT/app/bin/ImageToolServer-T41
	#rm -rfv $CURRENT/usr/sbin/impdbg
	#rm -rfv $CURRENT/usr/sbin/logcat
	#nfs ko删除
	#rm -rfv $CURRENT/usr/ko/lockd.ko
	#rm -rfv $CURRENT/usr/ko/nfs.ko
	#rm -rfv $CURRENT/usr/ko/nfsv3.ko
	#rm -rfv $CURRENT/usr/ko/sunrpc.ko
	#还原音频文件
    #svn revert --recursive $CURRENT/app/custom/speech/english/
	#chmod 777 $CURRENT/app/custom/speech/english/*
	#rm -rfv $CURRENT/data/config/en_nfs
	#rm -rfv $CURRENT/data/config/en_debug_mode
	rm $CURRENT/data/config/en_nfs
fi

#先删除所有的文件
rm -rf $CURRENT/app/data/config/config/custommode.ini
rm -rf $CURRENT/app/device/DeviceType.txt
rm -rf $CURRENT/tar/config/custommode.ini
rm -rf $CURRENT/usr/devlist
rm -rf $CURRENT/env.txt
rm -rf $CURRENT/app/data/sensor/*
rm -rf $CURRENT/app/data/config/config/Edvr.cfg
rm -rf $CURRENT/app/data/config/config/Edvr.cfg-bak
rm -rf $CURRENT/app/data/config/config/Edvr.default
rm -rf $CURRENT/app/custom/config/Edvr.default
rm -rf $CURRENT/app/data/config/config/customability.ini
rm -rf $CURRENT/app/custom/info/custominfo.ini
rm -rf $CURRENT/app/data/config/config/customcfg.ini  
rm -rf $CURRENT/app/device/capacity/DeviceCap.xml
rm -rf $CURRENT/app/device/DeviceVersion.ini
rm -rf $CURRENT/app/bin/myWatchDogProcess

#拷贝一份默认PTZCap
cp -rav $CURRENT/custom_made/${PRODUCTALIAS}/NORMAL/PTZCap.xml $CURRENT/app/device/capacity/PTZCap.xml

#从定制文件夹拷贝
cp -rav $CURRENT/custom_made/${PRODUCTALIAS}/${CUSTOMER}/custommode.ini $CURRENT/app/data/config/config/custommode.ini
cp -rav $CURRENT/custom_made/${PRODUCTALIAS}/${CUSTOMER}/scene $CURRENT/app/data/sensor/
cp -rav $CURRENT/custom_made/${PRODUCTALIAS}/${CUSTOMER}/Edvr.cfg $CURRENT/app/data/config/config/Edvr.cfg
cp -rav $CURRENT/custom_made/${PRODUCTALIAS}/${CUSTOMER}/customability.ini $CURRENT/app/data/config/config/customability.ini
cp -rav $CURRENT/custom_made/${PRODUCTALIAS}/${CUSTOMER}/custominfo.ini $CURRENT/app/custom/info/custominfo.ini
cp -rav $CURRENT/custom_made/${PRODUCTALIAS}/${CUSTOMER}/customcfg.ini  $CURRENT/app/data/config/config/customcfg.ini  
cp -rav $CURRENT/custom_made/${PRODUCTALIAS}/${CUSTOMER}/DeviceCap.xml $CURRENT/app/device/capacity/DeviceCap.xml
cp -rav $CURRENT/custom_made/${PRODUCTALIAS}/${CUSTOMER}/PTZCap.xml $CURRENT/app/device/capacity/PTZCap.xml
cp -rav $CURRENT/custom_made/${PRODUCTALIAS}/${CUSTOMER}/DeviceVersion.ini $CURRENT/app/device/DeviceVersion.ini

cp -rav $CURRENT/custom_made/${PRODUCTALIAS}/devlist $CURRENT/usr/devlist
if [ -f $CURRENT/custom_made/${PRODUCTALIAS}/${CUSTOMER}/env.txt ]
then
    #存在定制env.txt则复制。
    echo "use Custom env.txt"
    cp -af $CURRENT/custom_made/${PRODUCTALIAS}/${CUSTOMER}/env.txt $CURRENT/env.txt
else                                                                                                  
    echo "no use Custom env.txt"
    cp -rav $CURRENT/custom_made/${PRODUCTALIAS}/env.txt $CURRENT/env.txt
fi
cp -rav $CURRENT/custom_made/${PRODUCTALIAS}/DeviceType.txt $CURRENT/app/device/DeviceType.txt
cp -rav $CURRENT/custom_made/${PRODUCTALIAS}/myWatchDogProcess $CURRENT/app/bin/myWatchDogProcess

rm $CURRENT/tar/config/customability.ini
rm $CURRENT/tar/config/custominfo.ini
rm $CURRENT/tar/config/customcfg.ini    

#修改版本号
$TOPDIR/tools/pc/iniopt $CURRENT/app/custom/info/custominfo.ini set CloudUpgrade:CustomNo ${CUSTOMNO}
$TOPDIR/tools/pc/iniopt $CURRENT/app/custom/info/custominfo.ini clr Device:DeviceModel

# 语音定制
#1中文  2英语  3越南  4印尼  5西班牙
#6葡萄牙  7韩语  8泰语  9日语  10德语
#11意大利语  12土耳其语  13波兰语  14法语
#15捷克语  16希腊语  17波斯语  18希伯来语
#19阿拉伯语  20俄语
rm -rf  $CURRENT/data/speech/*

if [ $LANG = "CHINESE" ]
then
    #替换语音包
	cp -af $CURRENT/custom_made/${PRODUCTALIAS}/${CUSTOMER}/chinese/* $CURRENT/data/speech/
elif [ $LANG = "ENGLISH" ]
then
    #替换语音包
	cp -af $CURRENT/custom_made/${PRODUCTALIAS}/${CUSTOMER}/english/* $CURRENT/data/speech/
elif [ $LANG = "VIETNAMESE" ]
then
    #替换语音包
	cp -af $CURRENT/custom_made/${PRODUCTALIAS}/${CUSTOMER}/vietnamese/* $CURRENT/data/speech/
elif [ $LANG = "INDONESIAN" ]
then
    #替换语音包
	cp -af $CURRENT/custom_made/${PRODUCTALIAS}/${CUSTOMER}/indonesian/* $CURRENT/data/speech/
elif [ $LANG = "SPANISH" ]
then
    #替换语音包
	cp -af $CURRENT/custom_made/${PRODUCTALIAS}/${CUSTOMER}/spanish/* $CURRENT/data/speech/
elif [ $LANG = "PORTUGUESE" ]
then
    #替换语音包
	cp -af $CURRENT/custom_made/${PRODUCTALIAS}/${CUSTOMER}/portuguese/* $CURRENT/data/speech/
elif [ $LANG = "THAI" ]
then
	#替换语音包
	cp -af $CURRENT/custom_made/${PRODUCTALIAS}/${CUSTOMER}/thai/* $CURRENT/data/speech/
elif [ $LANG = "PERSIAN" ]
then
    #替换语音包
	cp -af $CURRENT/custom_made/${PRODUCTALIAS}/${CUSTOMER}/persian/* $CURRENT/data/speech/
elif [ $LANG = "TURKISH" ]
then
    #替换语音包
    cp -af $CURRENT/custom_made/${PRODUCTALIAS}/${CUSTOMER}/turkish/* $CURRENT/data/speech/

elif [ $LANG = "JAPANESE" ]
then
    #替换语音包
    cp -af $CURRENT/custom_made/${PRODUCTALIAS}/${CUSTOMER}/japanese/* $CURRENT/data/speech/
else
    #替换语音包
    cp -af $CURRENT/custom_made/${PRODUCTALIAS}/NORMAL/chinese/* $CURRENT/data/speech/
fi

rm -rfv $CURRENT/web/http/Config.html
rm -rfv $CURRENT/web/http/Protocol.html
rm -rfv $CURRENT/web/http/download
rm -rfv $CURRENT/web/http/DeviceCapTable.ini
echo "StreamTypeCount=2" > $CURRENT/web/http/DeviceCapTable.ini
cp -rav $CURRENT/app/data/config/config/Edvr.cfg $CURRENT/app/data/config/config/Edvr.cfg-bak
cp -rav $CURRENT/app/data/config/config/Edvr.cfg $CURRENT/app/data/config/config/Edvr.default
cp -rav $CURRENT/app/data/config/config/Edvr.cfg $CURRENT/app/custom/config/Edvr.default
#cp -rav $CURRENT/app/data/config/config/Edvr.cfg $CURRENT/tar/config/Edvr.default
cp -rav $CURRENT/app/data/config/etc/conf.d/boa/boa.conf $CURRENT/temp/data/config/etc/conf.d/boa/boa.conf
cp -rav $CURRENT/app/data/config/etc/conf.d/boa/boa.conf $CURRENT/data/config/etc/conf.d/boa/boa.conf
cp -rav $CURRENT/app/data/config/etc/conf.d/boa/boa.conf $CURRENT/app/data/config/etc/conf.d/boa/boa.conf-bak
cp -rav $CURRENT/app/data/config/etc/conf.d/boa/boa.conf $CURRENT/temp/data/config/etc/conf.d/boa/boa.conf-bak
cp -rav $CURRENT/app/data/config/etc/conf.d/boa/boa.conf $CURRENT/data/config/etc/conf.d/boa/boa.conf-bak
cp -rav $CURRENT/app/data/config/etc/conf.d/boa/mime.types $CURRENT/app/data/config/etc/conf.d/boa/mime.types
cp -rav $CURRENT/app/data/config/etc/conf.d/boa/mime.types $CURRENT/temp/data/config/etc/conf.d/boa/mime.types
cp -rav $CURRENT/app/data/config/etc/conf.d/boa/mime.types $CURRENT/data/config/etc/conf.d/boa/mime.types
cp -rav $CURRENT/app/data/config/etc/conf.d/boa/boa.conf $CURRENT/tar/etc/conf.d/boa/boa.conf
cp -rav $CURRENT/app/data/config/etc/conf.d/boa/boa.conf $CURRENT/tar/etc/conf.d/boa/boa.conf-bak
echo "BOARD: $1 SENSOR: $2"

#文件修改权限
chmod 777 -R $CURRENT/*
