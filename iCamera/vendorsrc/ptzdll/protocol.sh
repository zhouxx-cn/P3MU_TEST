#!/bin/bash
for DIRENTRY in `ls`
    do  if [ -d $DIRENTRY ]; then
           echo "$DIRENTRY"
    fi
    done
