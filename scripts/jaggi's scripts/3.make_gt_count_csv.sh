cat $1 |\
while read f; do
	echo "Processing counts for  - $f"
	f="${f##*/}"
	base="${f%.*}"
	# echo $base
	lfile="$base.txt"
	# echo $lfile
	chairCount=`grep -E "^56 " labels/$lfile | wc -l`
	personCount=`grep -E "^0 " labels/$lfile | wc -l`
	echo "$base.jpg,$personCount,$chairCount" >> gt_counts.csv
	echo "Done"
done