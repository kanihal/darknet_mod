if [[ -z $1 || -z $2 ]]; then
	echo "usage : 1.simple_util_make_images_labels.sh <folder where images can be found> <vatic o/p annotations .txt file> "
	exit 1
fi

echo "Processing"
if [ ! -d "images" ]; then 
	mkdir images
fi
if [ ! -d "original" ]; then 
	mkdir original
fi
if [ ! -d "labels" ]; then 
	mkdir labels
fi
find $1 -type f -name '*.jpg' -o -name '*.jpeg' -o -name '*.png'  -o -name '*.bmp'|\
	while read fname; do
		cp $fname "images/${fname##*/}"
	done
awk -f convert_wh.awk $2 #saves labels in folder -original 
for l in `ls original`
do
	cp "./original/$l" "./labels/$l.txt"
done
# awk -f ../convert_wh_labels.awk -v pre="$f_" "$f.txt" 
for f in `ls images/`; do	
		readlink -f "images/$f" >> train.txt
done
echo "Done"