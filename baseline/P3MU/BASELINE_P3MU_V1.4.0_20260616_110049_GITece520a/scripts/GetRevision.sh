#!/bin/bash

REVISION_STR=NOREVISION
GITPREFIX=GIT
SVNPREFIX=SVN
VERSION=000000

if [ -e ".git" ]; then
    VERSION=`git rev-list HEAD -n 1 | cut -c 1-7`
    REVISION_STR=$GITPREFIX$VERSION
elif [ -e ".svn" ]; then
    VERSION=`svn info --non-interactive 2>/dev/null | grep '^Last Changed Rev' | awk '{print $4}'`
    REVISION_STR=$SVNPREFIX$VERSION
fi

echo $REVISION_STR

