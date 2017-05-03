BEGIN{
	personIndex=0
	chairIndex=56
	wp=720
	hp=405
	dw = 1./wp
	dh = 1./hp
}
{
	if ($7==0 && $8==0){
		xmin=$2
		xmax=$4
		ymin=$3
		ymax=$5
		
		x = (xmin + xmax)/2.0
		y = (ymin + ymax)/2.0
		w = xmax - xmin
		h = ymax - ymin
		
		x = x*dw
		w = w*dw
		y = y*dh
		h = h*dh

		if ($10~"person"){
             #print personIndex" "$2" "$3" "$4" "$5>> $6.txt
             printf("%d %.16f %.16f %.16f %.16f\n",personIndex,x,y,w,h) >> "./original/"$6.txt
        }
        else if($10~"chair"){	
        	#print chairIndex" "$2" "$3" "$4" "$5>> $6.txt
			printf("%d %.16f %.16f %.16f %.16f\n",chairIndex,x,y,w,h) >> "./original/"$6.txt
		}
	}

}
