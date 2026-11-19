#! /bin/ash

ATTR_SIZE_STR=`udevadm info --query=all --attribute-walk --path=$1 | grep "ATTR{size}"`
ATTRS_SERIAL=`udevadm info --query=all --attribute-walk --path=$1 | grep "ATTRS{serial}" | grep -v hiusb`

produce_serial_file()
{
	udevadm info -a -p $1 | grep serial | grep -v hiusb > /tmp/.$2
}

# param 1: prefix; param 2: current systemfs path.
fnd_idx()
{
	prefix=$1

	for i in `seq 0 9`; do
		if [ -d "/dev/$prefix$i" ]; then
			if [ -f "/tmp/.$prefix$i" ]; then
				path_ctx=`cat /tmp/.$prefix$i`
				if [ "$path_ctx" == "$ATTRS_SERIAL" ]; then
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

if [ "$ATTR_SIZE_STR" == "ATTR{size}==\"0\"" ]; then
	hexdump -n 512 /dev/`echo ${1##/*/}`
	ATTR_SIZE_STR=`udevadm info --query=all --attribute-walk --path=$1 | grep "ATTR{size}"`
	if [ "$ATTR_SIZE_STR" == "ATTR{size}==\"0\"" ]; then
		echo "NOTUSBDISK"
	else
		#echo "USBDISK"
		fnd_idx usb $1
		echo "usb$?"
	fi
else
	#echo "USBDISK"
	fnd_idx usb $1
	echo "usb$?"
fi
