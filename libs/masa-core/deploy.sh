#!/bin/bash

#HOST=cuda-cic
HOST=$1
PORT=$2
TARGET_PROJECT_DIR=~/cudalign

if test -z $HOST
then
	echo './deploy.sh [hostname] [port]'
	exit
fi
if test -z $PORT
then
	PORT=22
fi

#rm src/cudalign

make dist

SRC_TAR_FILE=`ls -t *.tar.gz | head -1`
#echo $SRC_TAR_FILE

#BACKUP_FILE=cudalign.`date +"%Y%m%d%H%M%S"`.tar.gz
#echo $BACKUP_FILE

#cp ${SRC_TAR_FILE} ${SRC_BACKUP_DIR}/${BACKUP_FILE}
#scp -P443 ${SRC_TAR_FILE} ${HOST}:${TARGET_BACKUP_DIR}/${BACKUP_FILE}
scp ${SRC_TAR_FILE} ${HOST}:${TARGET_PROJECT_DIR}/${SRC_TAR_FILE}

exit


ssh -p443 ${HOST} "mkdir ${EXTRACT_DIR};cd ${EXTRACT_DIR};rm -r src;tar -xvzf ${TARGET_DIR}/${TAR_FILE}"
tar --exclude-vcs -cvzf ${TAR_FILE} src
scp -P443 ${TAR_FILE} ${HOST}:${TARGET_DIR}
ssh -p443 ${HOST} "mkdir ${EXTRACT_DIR};cd ${EXTRACT_DIR};rm -r src;tar -xvzf ${TARGET_DIR}/${TAR_FILE}"
