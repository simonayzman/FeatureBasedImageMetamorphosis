#!/bin/bash

if (($# < 2)) || (($# > 4)); then
	echo "usage ./mass_rename.sh <directory> <base name> [<new suffix>] [startNum]"
	exit
fi

directory=$1
baseName=$2
suffix=""
if (($# > 2)); then
	suffix=.$3
fi

numFiles=$(ls -1 $directory | wc -l)
counter=1
if (($# > 3)); then
	counter=$4
fi

if [ "$(ls -A "$directory")" ]; then

	for i in $directory*; do

		currentSuffix=${i##*.}

		prefix=""
	#	if (( counter < 100)); then
	#		prefix="0"
	#	fi
		if (( counter < 10)); then
			#prefix="00"
			prefix="0"
		fi
		#echo $i
		#echo "$directory$baseName"_"$prefix$counter$suffix"
		mv $i $directory$baseName"_"$prefix$counter$suffix
		counter=$((counter+1))
	done
else
    echo "$DIR is empty. Abort."
fi
