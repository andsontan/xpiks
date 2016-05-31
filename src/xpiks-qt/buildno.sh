#!/bin/sh
if git log -n 1 > /dev/null 2>&1 ; then
    git log -n 1 --pretty=format:"%h" > BUILDNO
else 
    if [ ! -f BUILDNO ]; then
	od -An -N8 -tx8 </dev/urandom | tr -d ' ' >BUILDNO
    fi
fi
cat BUILDNO
