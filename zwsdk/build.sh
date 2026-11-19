#! /bin/sh

xt_ret() {
    [ "${1}" = "0" ] && return 0
    printf "\033[1;31m ${BASH_SOURCE[1]##*/}%-6s %s \033[0m\n" "|${BASH_LINENO[0]}|" "${@:2}"
    return 1
}

usage="\033[31m 
USAGE:       
./build.sh w85t|g85t|p4|p3mu|g43ta|p4u
\033[0m
"

fn_make()
{
    case $1 in
    w85t)
       DEV_NUM=W85T
       ;;
    g85t)
       DEV_NUM=G85T
       ;;
    p4)
       DEV_NUM=P4
       ;;
    p3mu)
       DEV_NUM=P3MU
       ;;
    g43ta)
       DEV_NUM=G43TA
       ;;
    p4u)
       DEV_NUM=P4U
       ;;	   
    g84f)
       DEV_NUM=G84F
       ;;
    w84f)
       DEV_NUM=W84F
       ;;	   
    g63f)
       DEV_NUM=G63F
       ;;
    w63f)
       DEV_NUM=W63F
       ;;       
      *)
    echo "${usage}"; exit ;;
    esac
    make clean && make PRODUCT_TYPE=${DEV_NUM}
    xt_ret $? "Make SDK Lib Failed..." || return $?
    make PRODUCT_TYPE=${DEV_NUM} install
}

function fn_main()
{
    fn_make $@
    xt_ret $? "Please Check Code !!!" || return $?
}

fn_main $@
