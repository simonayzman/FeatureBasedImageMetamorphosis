#!/bin/bash

if (($# != 1)); then
	echo "usage ./jpg2png.sh <directory>"
	exit
fi

newDir="jpg/"
directory=$1

$(mkdir $directory$newDir)

for i in $directory*.jpg; do 
	currentFile=${i##*/}
	echo $currentFile
	echo $i
	convert $i ${i/jpg/png}
	mv $i $directory$newDir$currentFile
done

$(cd ..)

