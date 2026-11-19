#! /bin/ash
###############################################################################
# 01. 2019-04-02 v1.0.0 完成基本功能
# 1). 
###############################################################################

ECHO_DBG=true
#ECHO_DBG=echo
RM="rm -rf"

DEV_NAME=
ATA_MAP_FILE=/tmp/ata_map
DEVINFO_DEV=/proc/debugs/devinfo
RULES_FILE=/etc/udev/rules.d/hisilinux.rules
EN_AHCI_RESCAN_FILE="/mnt/mtd/en_ahci_rescan"

ATA_MEG_SATA_01_ARRAY="total=1 1.00=0=sata"
SATA_MEG_SATA_01_ARRAY="0:0:0:0"

ATA_MEG_SATA_02_ARRAY="total=2 1.00=0=sata 2.00=1=sata"
SATA_MEG_SATA_02_ARRAY="0:0:0:0 1:0:0:0"

###############################################################################
# get_dev_infos function
###############################################################################
get_dev_infos()
{
	DEV_NAME=`cat $DEVINFO_DEV | sed -n "s/=/\n/p" | sed -n '2p'`
	$ECHO_DBG "DEV_NAME: $DEV_NAME"
}

###############################################################################
# remove_ata_map
###############################################################################
remove_ata_map()
{
	$RM $ATA_MAP_FILE
}

###############################################################################
# remove_udev_rules function
###############################################################################
remove_udev_rules()
{
	$RM $RULES_FILE
	$RM /dev/sata
	$RM /dev/sata*/
	$RM /dev/usb
	$RM /dev/usb*/
	$RM /dev/esata
	$RM /dev/esata*/
	$RM /dev/cd
	$RM /dev/cd*/
	$RM /dev/iscsi
	$RM /dev/iscsi*/
}

###############################################################################
# restart_udev function
###############################################################################
restart_udev()
{
	killall udevd
	sleep 3	# wait 3s for udevd down
	source /etc/profile
	udevd --daemon
}

##############################################################################
# create_cd_usb_rules funtion : 
# 1. create cd and usb udev action rules
###############################################################################
create_cd_usb_rules()
{
	echo "DRIVERS==\"usb-storage\", SUBSYSTEM==\"block\", KERNEL==\"sr*\", PROGRAM==\"/usr/etc/udev/rules.d/div_dvd_and_3g_card.sh \$devpath\", RESULT==\"cd*\",  NAME=\"%c/disk\"" >> $RULES_FILE
	echo "DRIVERS==\"usb-storage\", SUBSYSTEM==\"block\", KERNEL==\"sd*\", PROGRAM==\"/usr/etc/udev/rules.d/distinguish_usb.sh     \$devpath\", RESULT==\"usb*\", NAME=\"%c/disk%n\"" >> $RULES_FILE

	#echo "KERNEL==\"event*\", SUBSYSTEM==\"input\", ATTRS{idVendor}==\"0525\", ATTRS{idProduct}==\"a4ac\", NAME=\"usb_kb\"" >> $RULES_FILE
	echo "KERNEL==\"event*\" SUBSYSTEM==\"input\",  ATTRS{idVendor}==\"0525\", ATTRS{idProduct}==\"a4ac\", ENV{USB_HID_TYPE}=\"0525:a4ac\"" >> $RULES_FILE
	echo "ENV{USB_HID_TYPE}=\"0525:a4ac\", DRIVERS==\"usbhid\" ATTRS{bInterfaceProtocol}==\"01\" NAME=\"usb_kb\"" >> $RULES_FILE
	$ECHO_DBG "create cd usb action udev rules"
}

##############################################################################
# create_sd_rules funtion : 
# 1. create sd udev action rules
###############################################################################
create_sd_rules()
{
	case $DEV_NAME in
	MEG_EYE_D3X_*)
		echo "" >> $RULES_FILE
		echo "# sd device " >> $RULES_FILE

		echo "DRIVERS==\"mmcblk\", SUBSYSTEM==\"block\", KERNEL==\"mmcblk0\", ATTRS{type}==\"SD\", SYMLINK=\"tf0/disk\"" >> $RULES_FILE
		echo "DRIVERS==\"mmcblk\", SUBSYSTEM==\"block\", KERNEL==\"mmcblk0p*\", ATTRS{type}==\"SD\", SYMLINK=\"tf0/disk%n\"" >> $RULES_FILE
		$ECHO_DBG "create sd action udev rules"
		;;
	*)
		;;
	esac
}

###############################################################################
# create_sata_rules funtion : 
# 1. create sata action udev rules
###############################################################################
create_ata_map()
{
	for i in $1; do
		echo "$i" >> $ATA_MAP_FILE
	done
	$ECHO_DBG "create ata map "$1
}

###############################################################################
# create_sata_rules funtion : 
# 1. create sata action udev rules
###############################################################################
create_sata_rules()
{
	k=0
	for i in $1; do
		echo "SUBSYSTEMS==\"scsi\", KERNELS==\"$i\", KERNEL==\"sd*\", SUBSYSTEM==\"block\", ATTRS{vendor}==\"ATA*\", NAME=\"sata${k}/disk%n\"" >> $RULES_FILE
		let k+=1
	done
	$ECHO_DBG "create sata action udev rules"
}

###############################################################################
# create_esata_rules funtion : 
# 1. create esata action udev rules
###############################################################################
create_esata_rules()
{
	k=0
	for i in $1; do
		echo "SUBSYSTEMS==\"scsi\", KERNELS==\"$i\", KERNEL==\"sd*\", SUBSYSTEM==\"block\", ATTRS{vendor}==\"ATA*\", NAME=\"esata${k}/disk%n\"" >> $RULES_FILE
		let k+=1
	done
	$ECHO_DBG "create esata action udev rules"
}

###############################################################################
# create_sata_esata_rules funtion : 
# 1. create sata and esata action udev rules
###############################################################################
create_sata_esata_rules()
{
	case $DEV_NAME in
	MEG_EYE_W4K_E75* | MEG_EYE_D3X_* | MEG_EYE_C2X_*)
		create_ata_map "$ATA_MEG_SATA_01_ARRAY"
		create_sata_rules "$SATA_MEG_SATA_01_ARRAY"
		;;
	*)
		i=1
		while [ $i -le 3 ]; do
			echo "MVSS UDEV rule, unsupported DEV_NAME: "$DEV_NAME > /dev/kmsg
			sleep 1
			let i+=1
		done
		;;
	esac
}

create_udev_rules()
{
	echo "" >> $RULES_FILE
	echo "# scsi device " >> $RULES_FILE

	create_cd_usb_rules "$CD_USB_ARRAY"
	create_sata_esata_rules
	create_sd_rules

	chmod a+rwx $RULES_FILE
	$RM $INFO_FILE

	restart_udev
}

###############################################################################
# main_loop funtion
###############################################################################
main_loop()
{
	remove_ata_map
	remove_udev_rules

	get_dev_infos
	create_udev_rules
	if [ -f $EN_AHCI_RESCAN_FILE ]; then
		echo "Enable ahci rescan script!"
		/usr/etc/udev/rules.d/chk_sata_rescan.sh &
	fi
}

###############################################################################
# main funtion
###############################################################################
main_loop

