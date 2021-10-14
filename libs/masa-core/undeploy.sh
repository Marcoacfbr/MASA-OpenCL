#!/bin/bash

#HOST=cuda-cic
PROJECT_DIR=~/cudalign/project
BUILD_DIR=${PROJECT_DIR}/build
BACKUP_DIR=${PROJECT_DIR}/backup

#rm src/cudalign

TAR_FILE=`ls -t $BACKUP_DIR/*.tar.gz | head -1`
echo $TAR_FILE

rm -r $BUILD_DIR
mkdir $BUILD_DIR
cd $BUILD_DIR
tar -xvzf $TAR_FILE
cd cudalign*
touch *
./configure
make $1
cp cudalign ../../..
exit


ssh -p443 ${HOST} "mkdir ${EXTRACT_DIR};cd ${EXTRACT_DIR};rm -r src;tar -xvzf ${TARGET_DIR}/${TAR_FILE}"
tar --exclude-vcs -cvzf ${TAR_FILE} src
scp -P443 ${TAR_FILE} ${HOST}:${TARGET_DIR}
ssh -p443 ${HOST} "mkdir ${EXTRACT_DIR};cd ${EXTRACT_DIR};rm -r src;tar -xvzf ${TARGET_DIR}/${TAR_FILE}"
