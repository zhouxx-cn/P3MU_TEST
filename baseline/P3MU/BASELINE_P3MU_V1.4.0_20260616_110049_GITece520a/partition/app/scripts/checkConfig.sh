#!/bin/sh

echo "[CFG] Check Config Files..."

if [ ! "$(ls -A /mnt/mtd/config)" ]; then
    cp -raf /mnt/app/data/config/config/* /mnt/mtd/config/
fi

/mnt/app/scripts/checkPartitionFull.sh

/mnt/app/scripts/checkConfigFile.sh

echo "[CFG] Check Config Files Finished."