#! /bin/ash

#ECHO_DBG="echo"
ECHO_DBG=true
DATA_CARD_VENDORS="HUAWEI|huawei|ZTE|zte"

MODEL_STR=`udevadm info -a -p $1 | grep "ATTRS{model}"`
MODEL_STORE=`echo ${MODEL_STR} | grep "Storage"`
#$ECHO_DBG "MODEL_STR ${MODEL_STR}"

produce_serial_file()
{
	udevadm info -a -p $1 | grep model | grep -v hiusb > /tmp/.$2
}

# param 1: prefix; param 2: current systemfs path.
fnd_idx()
{
	prefix=$1

	for i in `seq 0 9`; do
		if [ -d "/dev/$prefix$i" ]; then
			if [ -f "/tmp/.$prefix$i" ]; then
				path_ctx=`cat /tmp/.$prefix$i`
				if [ "$path_ctx" == "$MODEL_STR" ]; then
					return $i
				fi
			fi
		fi
	done

	for i in `seq 0 9`; do
		if [ ! -d "/dev/$prefix$i" ]; then
			produce_serial_file $2 $prefix$i
			return $i
		fi
	done

	return -1
}

VENDOR_STR=`udevadm info -a -p $1 | grep -E "${DATA_CARD_VENDORS}"`
#$ECHO_DBG "VENDOR_STR ${VENDOR_STR}"

if [ -z "$MODEL_STORE" ]; then
	if [ -z "$VENDOR_STR" ]; then
		#echo "CDDVD"
		fnd_idx cd $1
		echo "cd$?"
	else
		fnd_idx datacard $1
		echo "data_card$?"
	fi
else
	fnd_idx datacard $1
	echo "data_card$?"
fi
