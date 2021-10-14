#!/bin/bash
trap 'continue' SIGINT

if [ -z "$1" ]; then
    echo "usage: $0 [masa-library repository]"
    exit
fi

PREFIX=libs/masa-core
BRANCH=master
if [ -n "$2" ]; then
	BRANCH=$2
fi

git checkout -b $BRANCH -- $PREFIX
git clean -fdx $PREFIX



if git stash | grep -q 'No local changes to save' ;  then
	CHANGED=false
	echo 'No local changes to save'
else
	CHANGED=true
	echo 'Local changes saved to stash'
fi
#git stash
#git subtree pull --prefix $PREFIX $1 master --squash
git subtree pull --prefix $PREFIX $1 $BRANCH --squash
#git stash apply
if [ $CHANGED == true ]; then
	git stash apply
	echo 'Local changes restored from stash'
fi

trap SIGINT



