#!/bin/sh

#增加新定制步骤：
#1、各定制间的配置差异添加到custommade/*中
#2、修改build.sh/build_4g_ota.sh文件中的 usage，严格对应客户编号等信息
#3、修改build.sh/build_4g_ota.sh文件中的 fn_custom 函数

usage="USAGE: ./build.sh 序号
序号      客户编号      定制语言              客户名称
0         V001          中文|CHINESE          标准版本|NORMAL"

CUSTOM=P3MU
CURRENT=$(pwd)
CUSTOMER=NORMAL
SPEECH_LANGUAGE=CHINESE
CUSTOM_INFO_CUSTOMNO=V001
BOARDTYPE_PATH=$CURRENT/custom_made/$CUSTOM/devlist
VERSION_PATH=$CURRENT/custom_made/$CUSTOM/$CUSTOMER/DeviceVersion.ini

#--------------------------------------------------------------------
xt_ret() {
    [ "${1}" = "0" ] && return 0
    printf "${BASH_SOURCE[1]##*/}%-6s %s\n" "|${BASH_LINENO[0]}|" "${@:2}"
    return 1
}

fn_custom()
{
    #add custom
    case $1 in
    0)
    CUSTOMER=NORMAL; CUSTOM_INFO_CUSTOMNO=V001; SPEECH_LANGUAGE=CHINESE
    ;;
    *)
    echo "${usage}"; exit ;;
    esac
    VERSION_PATH=$CURRENT/custom_made/$CUSTOM/$CUSTOMER/DeviceVersion.ini
    #xt_ret $? "CUSTOM Failed" || return $?
}

fn_make()
{
    echo "CUSTOMER=${CUSTOMER},speechlanguage=${SPEECH_LANGUAGE},customno=${CUSTOM_INFO_CUSTOMNO}"
    make clean BOARD_TYPE=P3MU SENSORS=GC20C3  debug=false custom=${CUSTOM} customer=${CUSTOMER} speechlanguage=${SPEECH_LANGUAGE} customno=${CUSTOM_INFO_CUSTOMNO};
    make BOARD_TYPE=P3MU SENSORS=GC20C3  debug=false custom=${CUSTOM} customer=${CUSTOMER} speechlanguage=${SPEECH_LANGUAGE} customno=${CUSTOM_INFO_CUSTOMNO}
    xt_ret $? "MAKE Failed" || return $?
}

fn_pack()
{
    BOARDTYPE=$(cat $BOARDTYPE_PATH | sed 's/TYPE_//' | sed 's/\//_/g')
    major_version=$(grep -m 1 'Major_version' $VERSION_PATH | cut -d '=' -f 2)
    minor_version=$(grep -m 1 'Minor_version' $VERSION_PATH | cut -d '=' -f 2)
    reversion=$(grep -m 1 'Reversion' $VERSION_PATH | cut -d '=' -f 2)
    version="$major_version.$minor_version.$reversion"
    PACK_TIME=$(date "+%Y%m%d%H%M%S")                                                                                             
    OUTPUT_DIR=${CURRENT}/output/${BOARDTYPE}_${version}_${PACK_TIME}_${CUSTOMER}_${SPEECH_LANGUAGE}_${CUSTOM_INFO_CUSTOMNO}
    echo $OUTPUT_DIR
    mkdir -p $OUTPUT_DIR
    upgrade_name=UPDATE_${BOARDTYPE}_${version}_${PACK_TIME}_${CUSTOMER}_${SPEECH_LANGUAGE}_${CUSTOM_INFO_CUSTOMNO}.bin
    restore_name=ipcUpgrade_${BOARDTYPE}_${version}_${PACK_TIME}_${CUSTOMER}_${SPEECH_LANGUAGE}_${CUSTOM_INFO_CUSTOMNO}.bin
    factory_name=rootfs_${BOARDTYPE}_${version}_${PACK_TIME}_${CUSTOMER}_${SPEECH_LANGUAGE}_${CUSTOM_INFO_CUSTOMNO}.bin
    cp -ravf ${CURRENT}/upgrade.bin $OUTPUT_DIR/$upgrade_name
    cp -ravf ${CURRENT}/restore.bin $OUTPUT_DIR/$restore_name
    cp -ravf ${CURRENT}/factory.bin $OUTPUT_DIR/$factory_name
    echo $restore_name > $OUTPUT_DIR/upg.txt

    md5sum ./app/bin/xrscam | awk '{print $1," xrscam"}' > $OUTPUT_DIR/version.txt    
    cd $OUTPUT_DIR
    md5sum $factory_name >> $OUTPUT_DIR/version.txt
    md5sum $restore_name >> $OUTPUT_DIR/version.txt
    md5sum $upgrade_name >> $OUTPUT_DIR/version.txt
    chmod 777 -R ./*
    cd -
    xt_ret $? "PACK Failed" || return $?
}

function fn_main()
{
    fn_custom $@
    xt_ret $? "Make fn_custom Failed" || return $?
    fn_make $@
    xt_ret $? "Make fn_make Failed" || return $?
    fn_pack $@
    xt_ret $? "Make fn_release Failed" || return $?
}

fn_main $@
