#!/bin/bash
path=$1
count=0
$ERR_LOG = "/tmp/error_file"
loop () {
	for file in "$1"/* 
	do
		count=$(($count+1))
		if [ -d "$file" ]
		then 
			loop $file $count
		else
			ls -ogh --time-style=+"" $file 2>> ERR_LOG
		fi	
	done
}
loop $path $count
echo "count watched file = $count"

cat $ERR_LOG
rm $ERR_LOG


sizeAllFiles= $(($sizeAllFiles +$(stat -c%s $file)))


