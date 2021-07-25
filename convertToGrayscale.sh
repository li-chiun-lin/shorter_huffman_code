#! /bin/bash

find dataset | grep -e "\.tif" | while read fn; do
	#echo $fn;
	if tiffdump $fn | grep SamplesPer | grep -e "<3" ; then
		echo $fn;
		tiff2bw $fn $fn.tmp
		mv $fn.tmp $fn
	fi
done
