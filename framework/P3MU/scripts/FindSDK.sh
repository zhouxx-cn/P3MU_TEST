#! /bin/bash

#
# SDK looking up script, called from Makefile.param
# 

# Search in these directories 
SDK_SEARCH_PATH="/home/syh/work/xrs/AX/SDK"
# Only go down for SEARCH_DEPTH subdir
SEARCH_DEPTH=3

# $1 sdk full path
check_if_is_a_valid_sdk_dir()
{
    if [[ -e $1/sdk.unpack ]] || [[ -e $1/sdk_unpack.sh ]] || [[ -e $1/envsetup.sh ]] ; then
        return 0
    fi

    #echo "Invalid SDK In [$1]"
    return 1
}

temp_dir=""
# Search sdk in ${SDK_SEARCH_PATH}
# $1 sdk dir name like Hi3531_SDK_V1.0.7.1
find_sdk_by_dir_name()
{
    for dir in ${SDK_SEARCH_PATH}; do
        temp_dir=`find -L ${dir} -maxdepth ${SEARCH_DEPTH} -type d -name $1 2>/dev/null`
        #echo "Find ${temp_dir} In ${dir} For $1"

        check_if_is_a_valid_sdk_dir ${temp_dir}
        if [[ "$?" -eq 0 ]]; then
            echo ${temp_dir}
            return 0
        fi
    done

    temp_dir=""
    return 1
}

#echo "Find SDK Dirtect [$1]"
find_sdk_by_dir_name $1
#echo "temp_dir=${temp_dir}"

# vim:et:sw=4:ts=4

