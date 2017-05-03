echo "Deleting"
if [ -d "images" ]; then 
	rm -rf images
fi
if [ -d "labels" ]; then 
	rm -rf labels
fi
for f in `ls`
do
	if [[ -d $f ]]; then
		cd $f
		if [ -d "images" ]; then 
			rm -rf images
		fi
		if [ -d "original" ]; then 
			rm -rf original
		fi
		cd ..
	fi
done
echo "Done"