
#password custom
if [ -e /config/etc/user.db ]
then
	echo "user.db exsit"
else
	cp /custom/usrpswd/* /config/etc/
fi

