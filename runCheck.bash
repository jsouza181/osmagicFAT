#!/usr/bin/env bash

if [ $# -ne 1  ]; then
    echo -e "You need to provide one argument"
    exit 1
fi

./fsck.fat -t -v -V $1

exit 0
