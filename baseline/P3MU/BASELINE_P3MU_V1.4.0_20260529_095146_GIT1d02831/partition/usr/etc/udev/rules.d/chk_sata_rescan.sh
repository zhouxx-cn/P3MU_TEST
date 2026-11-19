#!/bin/sh

loop_time=60
rescan_count=0
sata_found_max=0

scsi_host_rescan()
{
	scsi_host_tbl=`ls /sys/class/scsi_host/`

	for dir in ${scsi_host_tbl}; do
		echo "- - -" > /sys/class/scsi_host/${dir}/scan
		echo "rescan scsi $dir done"
	done
	let rescan_count+=1
}

main()
{
	sata_spec_cnt=`cat /tmp/hisilinux.rules | grep sata | grep -v esata | wc -l`

	if [ $sata_spec_cnt -lt 4 ]; then
		exit
	fi
	
	while true; do
		sleep $loop_time

		sata_found=`ls /dev/ | grep sata | grep -v esata | wc -l`
		if [ $sata_found_max -lt $sata_found ]; then
			sata_found_max=$sata_found
		fi
		
		if [[ $sata_found -eq 0  &&  $sata_found_max -gt 0 ]]; then
			echo "All the sata down, rescan all!!"
			if [[ $rescan_count -gt 4 && $sata_spec_cnt -gt 8 ]]; then
				echo `date`": sata max found $sata_found_max disappear!" > /mnt/mtd/log/sata_fail.log
				reboot; sleep 30; killall daemon;
			else
				scsi_host_rescan
			fi
		else
			if [ $sata_found -lt $sata_spec_cnt ]; then
				if [ $rescan_count -lt 1 ]; then
					echo "sata found $sata_found, support $sata_spec_cnt"
					scsi_host_rescan
				elif [[ $rescan_count -lt 3 && $sata_found -lt $sata_found_max ]]; then
					echo "sata found $sata_found, sata_found_max $sata_found_max"
					scsi_host_rescan
				fi
			fi
		fi
	done
}

main
