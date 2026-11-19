#!/bin/sh
INSTALL=cp
#将所有库安装
LIBDIR=
INLCUDE_H=
len=`echo $LIBDIR|wc -c`
if [ $len -eq 1 ];then
	echo "没有制定库安装路径"
else
	$INSTALL ./lib/*so $DIR
fi
	len=`echo $LIBDIR|wc -c`
if [ $len -eq 1 ];then
	echo "没有头文件安装路径"
else
	$INSTALL $INLCUDE_H ./include/
fi
