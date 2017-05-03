dirlist=`ls`
if [ ! -d "images" ]; then
	mkdir images
else
	echo "directory images exists" 
fi

if [ ! -d "labels" ]; then
	mkdir labels
else
	echo "directory labels exists" 
fi

sep="_"
for f in $dirlist
do
	if [[ -d $f ]]; then
		cd $f
		echo "Processing $f"
		if [ ! -d "images" ]; then 
			mkdir images
		fi
		if [ ! -d "original" ]; then 
			mkdir original
		fi
		find frames_in/ -type f -name '*.jpg' |\
		while read fname; do
			cp $fname "../images/$f$sep${fname##*/}"
			cp $fname "images/$f$sep${fname##*/}"
		done
		awk -f ../convert_wh.awk "$f.txt" #saves labels in folder -original 
		for l in `ls original`
		do
			cp "./original/$l" "../labels/$f$sep$l.txt"
			mv "./original/$l" "./original/$f$sep$l.txt"
		done
		# awk -f ../convert_wh_labels.awk -v pre="$f_" "$f.txt" 
		cd ..
		echo "Done"
	fi
done

