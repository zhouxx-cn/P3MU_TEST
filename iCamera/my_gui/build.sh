#! /bin/sh
#---------------------------------------------------------------------------
#          FILE: build.sh
#         USAGE: ./build.sh 
#        AUTHOR: zhangxj () 
#       CREATED: 2022-12-17 05:34:36 PM
#      REVISION: 1.0 
#---------------------------------------------------------------------------

SDK_PATH=../../zwsdk/


usage="USAGE: [C=ls] ./build.sh p3mu a|d|m|p|c  (All|DVR_SDK|my_gui|Pack|Debug)"
xt_ret() {
    [ "${1}" = "0" ] && return 0
    printf "\033[1;31m ${BASH_SOURCE[1]##*/}%-6s %s \033[0m\n" "|${BASH_LINENO[0]}|" "${@:2}"
    return 1
}

fn_main()
{
    CWD=${PWD}
    case $1 in
    p3mu)
        DEV_NUM=P3MU
        ;;
    *)
        echo "${usage}"; exit ;;
    esac

    cd $SDK_PATH && ./build.sh $1 $2
    xt_ret $? "make SDK Failed" || return $?
    cd $CWD

    case $2 in
    a)
        cd ../modules && ${C:-make clean } && make PRODUCT_TYPE=${DEV_NUM}
        xt_ret $? "make modules Failed" || return $?
        cd ../DVR_SDK && ${C:-make clean } && make PRODUCT_TYPE=${DEV_NUM}
        xt_ret $? "make DVR_SDK Failed" || return $?
        cd ../my_gui && make clean && make PRODUCT_TYPE=${DEV_NUM}
        xt_ret $? "make my_gui Failed" || return $?
        make install PRODUCT_TYPE=${DEV_NUM}
        ;; 
    d)
        cd ../DVR_SDK && ${C:-make clean} && make PRODUCT_TYPE=${DEV_NUM}
        xt_ret $? "make DVR_SDK Failed" || return $?
        cd ../my_gui && make clean && make PRODUCT_TYPE=${DEV_NUM}
        xt_ret $? "make my_gui Failed" || return $?
        make install PRODUCT_TYPE=${DEV_NUM}
        ;;
    m)
        make clean && make PRODUCT_TYPE=${DEV_NUM}
        xt_ret $? "make my_gui Failed" || return $?
        make install PRODUCT_TYPE=${DEV_NUM}
        ;;
    c)
        cd ../modules && ${C:-make clean }
        cd ../DVR_SDK && ${C:-make clean }
        cd ../my_gui && make clean
        ;;
    *)
        echo "${usage}"; exit ;;
    esac
}

fn_main $@

