#!/bin/sh

if [ $# -ne 2 ]; then
	echo "Usage: $0 detect_app interval_time(Seconds)";
	exit;
fi

APP_NAME=$1
INV_TIME=$2

# Check if $APP_NAME is exist or not 
ID=`ps | grep -v grep | grep -v $0 | grep $APP_NAME`;
if [ ! -n "$ID" ] ; then
	echo "no this process: $APP_NAME";
	exit;
fi

print_in_console()
{
	i=1

	echo -e "\nAPP_NAME: $APP_NAME";
	echo "Time Inv: $INV_TIME Second(s)";
	echo -e "COUNT\tPID\tMEM\tIDLE\tVmData\tCloseWait\tSockets\tThreads";

	while true
	do
		ID=`ps | grep -m 1 $APP_NAME | awk  '{print $1}'`;
		SOCK_CNT=`ls -al /proc/$ID/fd | grep -c "socket"`;
		THREAD_CNT=`cat /proc/$ID/status | grep "Threads" | awk '{print $2}'`;
		CLOSEWAIT_CNT=`netstat -an | grep -c "CLOSE_WAIT"`;
		VMDATA_VAL=`cat /proc/$ID/status | grep "VmData" | awk '{print $2}'`;
		CPU_RATE=`top -n1 | grep "CPU:" | grep -v "grep" | awk '{print $2}'`;
		CPU_IDLE=`top -n1 | grep "CPU:" | grep -v "grep" | awk '{print $8}'`;
		echo -e "$i\t$ID\t$CPU_RATE\t$CPU_IDLE\t$VMDATA_VAL\t\t$CLOSEWAIT_CNT\t\t$SOCK_CNT\t\t$THREAD_CNT";
		sleep $INV_TIME;
		i=$(($i+1));
	done
}

load_in_file()
{
	j=1;
	FILE_NAME=$3

	echo -e "\t\t\t===Process Infomation Table===\t\t\t" > $FILE_NAME;
	echo "App Name: $APP_NAME"  >> $FILE_NAME;
	echo "Time Inv: $INV_TIME" >> $FILE_NAME;
	echo -e "COUNT\tPID\tCPU_RATE CPU_IDLE VmData(KB) CloseWaits Sockets Threads" >> $FILE_NAME;

	while true
	do
		ID=`ps | grep -m 1 $APP_NAME | awk  '{print $1}'`;
		SOCK_CNT=`ls -al /proc/$ID/fd | grep -c "socket"`;
		THREAD_CNT=`cat /proc/$ID/status | grep "Threads" | awk '{print $2}'`;
		CLOSEWAIT_CNT=`netstat -an | grep -c "CLOSE_WAIT"`;
		VMDATA_VAL=`cat /proc/$ID/status | grep "VmData" | awk '{print $2}'`;
		CPU_RATE=`top -n1 | grep "CPU:" | grep -v "grep" | awk '{print $2}'`;
		CPU_IDLE=`top -n1 | grep "CPU:" | grep -v "grep" | awk '{print $8}'`;
		echo -e "$j\t$ID\t$CPU_RATE\t$CPU_IDLE\t$VMDATA_VAL\t$CLOSEWAIT_CNT\t$SOCK_CNT\t$THREAD_CNT" >> $FILE_NAME;
		j=$(($j+1));
	done
}


print_in_console;
#load_in_file;

exit 0;
