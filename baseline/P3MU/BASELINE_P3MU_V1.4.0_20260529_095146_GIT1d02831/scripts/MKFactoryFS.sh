#!/bin/bash
set -e

echo "===== Make Factory Firmware ====="

check_param()
{
	if [[ ! -n $PLATFORM ]]; then
		$ECHO "*** Error: PLATFORM Is Empty!"
		exit 1
	fi
	if [[ ! -n $FLASH_TYPE ]]; then
		$ECHO "*** Error: FLASH_TYPE Is Empty!"
		exit 1
	fi
	if [[ ! -n $FLASH_SIZE ]]; then
		$ECHO "*** Error: FLASH_SIZE Is Empty!"
		exit 1
	fi
#	if [[ ! -n $VERSION ]]; then
#		$ECHO "*** Error: VERSION Is Empty!"
#		exit 1;
#	else
#		VERSION=`echo ${VERSION} | sed 's/ /-/g' | sed 's/://g' | sed 's/SYS/BSP/g'`
#		VERSION_EXT=${VERSION}_$(date "+%Y%m%d%H%M%S")
#	fi

	if [[ ! -n $OUT_DIR ]]; then
		$ECHO "*** Error: OUT_DIR Is Empty!"
	fi
    
	if [[ ! -n $OUT_DIR || ! -e $OUT_DIR ]]; then
		$ECHO "*** Error: OUT_DIR Is Empty Or Not Exist!"
		exit 1
	fi

	if [[ ! -n $FILE_NAME ]]; then
		$ECHO "*** Error: FILE_NAME Is Empty!"
		exit 1
	else
		for FILE in $FILE_NAME; do
			if [[ ! -n $FILE  || ! -e $FILE ]]; then
				$ECHO "*** Error: $FILE Is Not Exist!"
				exit 1
			fi
		done
	fi

 	if [[ ! -n $PART_SIZE ]]; then
		$ECHO "*** Error: PART_SIZE Is Empty!"
		exit 1
	else
		for SIZE in $PART_SIZE; do
			if [[ $SIZE -eq 0 ]]; then
				$ECHO "*** Error: part size Is $SIZE!"
				exit 1
			fi
		done
	fi

	if [[ ! -n $PART_OFSZ ]]; then
		$ECHO "*** Error: PART_OFSZ Is Empty!"
		exit 1
	fi

	FACTORY_NAME=factory.bin
}

make_factory()
{
	$ECHO "Making factory bin file begin ..."
	count=0;
	for FILE in $FILE_NAME; do
		count=$(($count+1));
		file_size=`wc -c < $FILE`;
		part_size=`echo $PART_SIZE | awk '{print $count}' count="$count"`;
		part_ofsz=`echo $PART_OFSZ | awk '{print $count}' count="$count"`;
		seek_count=$(($part_ofsz/65536));
		block_count=$(($part_size/65536));
		$ECHO "$FILE start ....";
		$ECHO "dd if=$FILE of=$FACTORY_NAME ibs=$file_size obs=65536 seek=$seek_count count=1";
		dd if=$FILE of=$FACTORY_NAME ibs=$file_size obs=65536 seek=$seek_count count=1;
		file_count=$(($file_size/65536));
		if [ $(($file_size%65536)) ];then
			file_count=$(($file_count+1));
		fi
		seek_count=$(($seek_count+$file_count));
		if [ $block_count -gt $file_count ];then
			block_count=$(($block_count-$file_count));
		else
			continue;
		fi
		$ECHO "tr '\000' '\377' < /dev/zero | dd of=$FACTORY_NAME bs=65536 count=$block_count seek=$seek_count iflag=fullblock;"
		tr '\000' '\377' < /dev/zero | dd of=$FACTORY_NAME bs=65536 count=$block_count seek=$seek_count iflag=fullblock;
	done
	echo "Making factory bin file done ..."
	chmod 0755 "$OUT_DIR/$FACTORY_NAME"
}

main()
{
	check_param;
	if [ $? != 0 ]; then
		exit 1;
	fi

	make_factory;
}

while getopts "p:t:b:v:o:n:s:f:" arg; do
	case $arg in
	p)
		PLATFORM=$OPTARG
		;;
	t)
		FLASH_TYPE=$OPTARG
		;;
	b)
		FLASH_SIZE=$OPTARG
		;;
	v)
		VERSION=$OPTARG
		;;
	o)
		OUT_DIR=$OPTARG
		;;
	n)
		FILE_NAME=$OPTARG
		;;
	s)
		PART_SIZE=$OPTARG
		;;
	f)
		PART_OFSZ=$OPTARG
		;;
	?)
		$ECHO "unkonw argument!!!"
		continue;
		;;
	esac
done

main;
