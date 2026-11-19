#!/bin/sh

echo "[CFG] Check Config Files..."

if [ ! "$(ls -A /mnt/mtd/config)" ]; then
    cp -raf /mnt/app/data/config/config/* /mnt/mtd/config/
    cp /mnt/app/data/config/config/.custommode.flag /mnt/mtd/config/
fi


/mnt/app/scripts/checkConfigure.sh

/mnt/app/scripts/checkPartitionFull.sh

/mnt/app/scripts/checkConfigFile.sh

echo "[CFG] Check Config Files Finished."
