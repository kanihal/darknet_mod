import os
from os import walk, getcwd
from PIL import Image

def convert(size, box):
    dw = 1./size[0]
    dh = 1./size[1]
    x = (box[0] + box[1])/2.0
    y = (box[2] + box[3])/2.0
    w = box[1] - box[0]
    h = box[3] - box[2]
    x = x*dw
    w = w*dw
    y = y*dh
    h = h*dh
    return (x,y,w,h)
    
    
"""-------------------------------------------------------------------""" 

""" Configure Paths"""
debug=1
in_path = "original/"
out_path = "labels/"

wd = getcwd()
train_list = open('%s/train.txt'%wd, 'w')

""" Get input text file list """
file_list = []
for (dirpath, dirnames, filenames) in walk(in_path):
    file_list.extend(filenames)
    break
if(debug):
    print(file_list)

txt_path = in_path + file_list[0]
txt_file = open(txt_path, "r")
lines = txt_file.read().split('\n')

""" Process """
for file in file_list:
    # file =  open("Labels/stop_sign/001", "r")
    
    """ Open input text files """
    txt_path = in_path + file
    if debug:
        print("Input:" + txt_path)
    txt_file = open(txt_path, "r")
    lines = txt_file.read().split('\n')   #No need ->for ubuntu, use "\r\n" instead of "\n"
    
    """ Open output text files """
    txt_out_path = out_path + file+".txt"
    print("Output:" + txt_out_path)
    txt_outfile = open(txt_out_path, "w")
    
    
    """ Convert the data to YOLO format """
    is_object = False
    for line in lines:
        #print('lenth of line is: ')
        #print(len(line))
        #print('\n')
        if(len(line) > 0 ): #for non blank lines
            is_object=True
            if debug:
                print(line + "\n")
            elems = line.split(' ')
            print(elems)
            cls_id = elems[0]
            xmin = elems[1]
            xmax = elems[3]
            ymin = elems[2]
            ymax = elems[4]
            #
            img_path = str('%s/images/%s.jpg'%(wd, file))
            #t = magic.from_file(img_path)
            #wh= re.search('(\d+) x (\d+)', t).groups()
            im=Image.open(img_path)
            w= int(im.size[0])
            h= int(im.size[1])
            #w = int(xmax) - int(xmin)
            #h = int(ymax) - int(ymin)
            # print(xmin)
            b = (float(xmin), float(xmax), float(ymin), float(ymax))
            bb = convert((w,h), b)
            if debug:
                print(bb)
            txt_outfile.write(str(cls_id) + " " + " ".join([str(a) for a in bb]) + '\n')
    
    """ Save those images with bb into list"""
    if(is_object):
        train_list.write(img_path+"\n")
                
train_list.close()      
