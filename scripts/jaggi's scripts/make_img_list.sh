for f in `ls imgs/`; do
	readlink -f "imgs/$f" >> imglist.txt
done
	