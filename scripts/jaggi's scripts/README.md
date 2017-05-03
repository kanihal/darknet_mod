## Helper scripts to  convert annotations o/p by vatic tool to yolo/darknet trainable format

1.simple_util_make_images_labels.sh <folder where images can be found[nested subfolders of images is also fine]> <vatic o/p annotations .txt file> 

usage:
`bash 1.simple_util_make_images_labels.sh /home/jkl/MLRD/classroom-annotated/1/frames_in /home/jkl/MLRD/classroom-annotated/1/1.txt`

It'll create images containing training images, labels folder corresponding labels and train.txt in the scripts directory.

you can feed train.txt for yolo darknet for training
 
