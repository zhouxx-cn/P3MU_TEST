#!/bin/sh

# \brief   : 实现设备启动时候进行检测配置分区中文件是否是否一致
# \param   : 无
# \return  ：无

# 配置文件路径
CFGINFO_PATH="/mnt/mtd/config/custommode.ini"
CUSTOMROCFG_PATH="/mnt/mtd/config/customrocfg.flag"
CUSTOMABILITY_PATH="/mnt/mtd/config/customability.ini"
CUSTOMCFG_PATH="/mnt/mtd/config/customcfg.ini"
CUSTOMINFOPATH="/mnt/mtd/config/custominfo.ini"

# DEVINFO工具
DEVINFO_TOOL="/mnt/app/bin/DevinfoTool"

# devinfo分区中读出的文件路径
DEVINFO_PATH="/tmp/custommode.ini"

# 分解文件MD5记录文件路径
CFGMD5_PATH="/mnt/mtd/config/CfgRecordMD5.txt"

#custommode.flag 标志位
CFGINFO_FLAG="/mnt/mtd/config/.custommode.flag"

# 异常标志位
SPLITTING_FLAG="/mnt/mtd/config/.custommoderestore.flag"

# 开机读取devinfo分区信息
$DEVINFO_TOOL read $DEVINFO_PATH


# 检查文件是否存在
if [ -f "$DEVINFO_PATH" ] && [ -s "$DEVINFO_PATH" ]; then
    echo "The file has content"
	#计算文件的MD5值
	file1_md5=$(md5sum $CFGINFO_PATH | awk '{print $1}')
	file2_md5=$(md5sum $DEVINFO_PATH | awk '{print $1}')
	echo "$file1_md5 "
	echo "$file2_md5"
	if [ "$file1_md5" == "$file2_md5" ];then
		echo "coustmmode.ini file MD5 sample"
	else
		$DEVINFO_TOOL write $CFGINFO_PATH
		rm $CFGMD5_PATH
		echo -e  `md5sum $CFGINFO_PATH` >> $CFGMD5_PATH	
		echo -e  `md5sum $CUSTOMROCFG_PATH` >> $CFGMD5_PATH
		echo -e  `md5sum $CUSTOMCFG_PATH` >> $CFGMD5_PATH
		echo -e  `md5sum $CUSTOMABILITY_PATH` >> $CFGMD5_PATH
		echo -e  `md5sum $CUSTOMINFOPATH` >> $CFGMD5_PATH
		echo "devinfo and config is MD5 value no sample ,Write completed !!!"
		exit 0
	fi
    cat "$DEVINFO_PATH"
else
    $DEVINFO_TOOL write $CFGINFO_PATH
	rm $CFGMD5_PATH
	echo -e  `md5sum $CFGINFO_PATH` >> $CFGMD5_PATH	
	echo -e  `md5sum $CUSTOMROCFG_PATH` >> $CFGMD5_PATH
	echo -e  `md5sum $CUSTOMCFG_PATH` >> $CFGMD5_PATH
	echo -e  `md5sum $CUSTOMABILITY_PATH` >> $CFGMD5_PATH
	echo -e  `md5sum $CUSTOMINFOPATH` >> $CFGMD5_PATH
		
	echo "Write completed !!!"
    
	exit 0
fi

# 如果CfgRecordMD5.txt不存在则重新保存一份
if [ -f "$CFGMD5_PATH" ] && [ -s "$CFGMD5_PATH" ]; then
	echo "File $CFGMD5_PATH exists"
else
	touch  $CFGINFO_FLAG
	touch  $SPLITTING_FLAG

	#删临时文件
	rm $DEVINFO_PATH
	echo "File $CFGMD5_PATH does not exist;File $DEVINFO_PATH deleted"
	exit 0
fi

# 检查MD5值
check_md5() 
{

	#读取传入的文件路径
    filepath=$1
	
	#对CfgRecordMD5.txt逐行读取信息
    while read -r line; do
       
		md5_saved=$(echo "$line" | awk '{print $1}')
        path_saved=$(echo "$line" | awk '{print $2}')

		#判断是否是文本中记录的文件
        if [ "$path_saved" == "$filepath" ]; then
			
			#config实际使用的MD5值
			actual_md5=$(md5sum "$filepath" | awk '{print $1}')
            
			#判断实际MD5值和记录的MD5值是否一致
			if [ "$actual_md5" == "$md5_saved" ]; then
                echo "File $filepath MD5 value is normal: $actual_md5"
                #正常返回0
				return 0
            else
                if [ "$filepath" == "$CFGINFO_PATH" ]; then
                    cp $CFGINFO_PATH $DEVINFO_PATH
                    echo "Custom mode is broken"
                fi
                touch $SPLITTING_FLAG
                echo "File $filepath MD5 value is abnormal: $actual_md5"
                #异常返回1
				return 1
            fi
       
	   fi
    
	done < "$CFGMD5_PATH"
    
	return 2
}

CHECK_CONFIG_ARRAY="/mnt/mtd/config/custommode.ini \
					/mnt/mtd/config/customrocfg.flag \
					/mnt/mtd/config/customcfg.ini \
					/mnt/mtd/config/customability.ini \
					/mnt/mtd/config/custominfo.ini "
					
for File in $CHECK_CONFIG_ARRAY; do

    check_md5 "$File"
	
	#记录返回值
    match_found=$?
	
   #对返回值进行判断提示
   if [ $match_found -gt 0 ]; then
        echo "VERIFICATION FAILED"
		if [ $match_found -eq 1 ]; then
	       echo "$File file MD5 value abnormality"
		fi
		if [ $match_found -eq 2 ]; then
	       echo "The file does not exist : $File"
		fi
		break
   fi
   
done

# 删除临时文件
rm $DEVINFO_PATH
echo "File $DEVINFO_PATH deleted"