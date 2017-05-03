#include "network.h"
#include "detection_layer.h"
#include "region_layer.h"
#include "cost_layer.h"
#include "utils.h"
#include "parser.h"
#include "box.h"
#include "image.h"
#include "demo.h"
#include <sys/time.h>
#include "post_predictions.h"
#define FRAMES 3

//#ifdef OPENCV
#include "opencv2/highgui/highgui_c.h"
#include "opencv2/imgproc/imgproc_c.h"
image get_image_from_stream(CvCapture *cap);
image ipl_to_image(IplImage* src);
IplImage* image_to_Ipl(image img, int w, int h, int depth, int c, int step);

static char **demo_names;
static image **demo_alphabet;
static int demo_classes;

static float **probs;
static box *boxes;
static int *counts;

static network net;
static image in   ;
static image in_s ;
static image det  ;
static image det_s;
static image disp = {0};
static CvCapture * cap;
static CvVideoWriter* writer;

static float fps = 0;
static float demo_thresh = 0;
static float demo_hier_thresh = .5;

static float *predictions[FRAMES];
static int demo_index = 0;
static image images[FRAMES];
static float *avg;

char* file;
char* out_vid_file;

static int w, h, depth, c, step= 0;

void *fetch_in_thread(void *ptr)
{
    //    in = get_image_from_stream(cap);
    //    if(!in.data){
    //        cvReleaseVideoWriter(&writer);
    //        error("Stream closed.");
    //    }
    IplImage* frame = cvQueryFrame(cap);
    if (!frame) {
        error("Stream closed.");
    }
    if(step == 0)
    {
        w = frame->width;
        h = frame->height;
        c = frame->nChannels;
        depth= frame->depth;
        step = frame->widthStep;
    }

    in = ipl_to_image(frame);
    rgbgr_image(in);

    in_s = resize_image(in, net.w, net.h);
    return 0;
}

void *detect_in_thread(void *ptr)
{
    float nms = .4;

    layer l = net.layers[net.n-1];
    float *X = det_s.data;
    float *prediction = network_predict(net, X);

    memcpy(predictions[demo_index], prediction, l.outputs*sizeof(float));
    mean_arrays(predictions, FRAMES, l.outputs, avg);
    l.output = avg;

    free_image(det_s);
    if(l.type == DETECTION){
        get_detection_boxes(l, 1, 1, demo_thresh, probs, boxes, 0);
    } else if (l.type == REGION){
        get_region_boxes(l, 1, 1, demo_thresh, probs, boxes, 0, 0, demo_hier_thresh);
    } else {
        error("Last layer must produce detections\n");
    }
    if (nms > 0) do_nms(boxes, probs, l.w*l.h*l.n, l.classes, nms);
    printf("\033[2J");
    printf("\033[1;1H");
    printf("\nFPS:%.1f\n",fps);
    printf("Objects:\n\n");

    images[demo_index] = det;
    det = images[(demo_index + FRAMES/2 + 1)%FRAMES];
    demo_index = (demo_index + 1)%FRAMES;

    draw_detections(det, l.w*l.h*l.n, demo_thresh, boxes, probs, demo_names, demo_alphabet, demo_classes);
    printf("counts classes=%d",demo_classes);
    save_detections_counts(file, l.w*l.h*l.n, in.w, in.h, demo_thresh, boxes, probs, demo_names, demo_classes,counts);

    //jaggi
    IplImage* outputIpl= image_to_Ipl(det, w, h, depth, c, step);
    //    cv::Mat outputMat = cv::cvarrToMat(outputIpl, true);
    /*
     cvNamedWindow("image", CV_WINDOW_AUTOSIZE);
     cvShowImage("image", outputIpl);
     cvWaitKey(1);
    */
    int person_count;
    int chair_count;
    int j,k;
    person_count=0;
    printf("finding person count...");
    for(j=0;j<demo_classes;j++){
        if(strcmp(demo_names[j],"person")==0){
            person_count=counts[j];
            break;
        }
    }
    for(k=0;k<demo_classes;k++){
        if(strcmp(demo_names[k],"chair")==0){
            chair_count=counts[k];
            break;
        }
    }
    printf("Done");
    char buffer[30];

    CvFont font;
    cvInitFont(&font, CV_FONT_HERSHEY_SIMPLEX,0.5,0.5,0,2,8);
    printf("writing label to image\n");

    sprintf(buffer, "       -> %s  : %d",demo_names[k],chair_count);
    cvPutText(outputIpl, buffer,cvPoint(20, 40), &font, CV_RGB(150,250,50));

    sprintf(buffer, "counts -> %s : %d",demo_names[j],person_count);
    cvPutText(outputIpl, buffer,cvPoint(20, 20), &font, CV_RGB(150,250,50));

    printf("written label\n");
    cvWriteFrame(writer,outputIpl);

    cvShowImage("Demo", outputIpl);
    cvReleaseImage(&outputIpl);
    return 0;
}

double get_wall_time()
{
    struct timeval time;
    if (gettimeofday(&time,NULL)){
        return 0;
    }
    return (double)time.tv_sec + (double)time.tv_usec * .000001;
}

void demo(char *cfgfile, char *weightfile, float thresh, int cam_index, const char *filename, char **names, int classes, int frame_skip, char *prefix, float hier_thresh)
{
    //skip = frame_skip;
    image **alphabet = load_alphabet();
    int delay = frame_skip;
    demo_names = names;
    demo_alphabet = alphabet;
    demo_classes = classes;
    demo_thresh = thresh;
    demo_hier_thresh = hier_thresh;
    printf("Demo\n");
    net = parse_network_cfg(cfgfile);
    if(weightfile){
        load_weights(&net, weightfile);
    }
    set_batch_network(&net, 1);

    srand(2222222);

    if(filename){
        printf("video file: %s\n", filename);//file name can be rtsp stream , from the command line
        cap = cvCaptureFromFile(filename);
        if(!cap)
            error("Couldn't read video file.\n");
        file=filename;
        //writer=cvCreateVideoWriter("predicitons.mp4",-1,30,cvSize(600,600),1);
        out_vid_file = malloc(strlen("pred_") +strlen(basename(filename))+ strlen("_.avi"));
        strcpy(out_vid_file, "pred_");
        strcat(out_vid_file, basename(filename));
        strcat(out_vid_file, "_.avi");

        CvSize size = cvSize((int)cvGetCaptureProperty(cap,CV_CAP_PROP_FRAME_WIDTH), (int)cvGetCaptureProperty(cap,CV_CAP_PROP_FRAME_HEIGHT));
        writer = cvCreateVideoWriter(out_vid_file, CV_FOURCC('D','I','V','X'), cvGetCaptureProperty(cap,CV_CAP_PROP_FPS),size, 1);
        if(!writer)
            error("Couldn't open writer.\n");
    }else{
        cap = cvCaptureFromCAM(cam_index);
        if(!cap)
            error("Couldn't connect to webcam.\n");
    }



    layer l = net.layers[net.n-1];
    int j;

    avg = (float *) calloc(l.outputs, sizeof(float));
    for(j = 0; j < FRAMES; ++j) predictions[j] = (float *) calloc(l.outputs, sizeof(float));
    for(j = 0; j < FRAMES; ++j) images[j] = make_image(1,1,3);

    boxes = (box *)calloc(l.w*l.h*l.n, sizeof(box));
    counts = (int *) calloc(demo_classes,sizeof(int));
    if (!counts)
        printf("counts malloc %d\n",demo_classes);

    probs = (float **)calloc(l.w*l.h*l.n, sizeof(float *));
    for(j = 0; j < l.w*l.h*l.n; ++j) probs[j] = (float *)calloc(l.classes, sizeof(float));

    pthread_t fetch_thread;
    pthread_t detect_thread;

    fetch_in_thread(0);
    det = in;
    det_s = in_s;

    fetch_in_thread(0);
    detect_in_thread(0);
    disp = det;
    det = in;
    det_s = in_s;

    for(j = 0; j < FRAMES/2; ++j){
        fetch_in_thread(0);
        detect_in_thread(0);
        disp = det;
        det = in;
        det_s = in_s;
    }

    int count = 0;
    if(!prefix){
        cvNamedWindow("Demo", CV_WINDOW_NORMAL);
        cvMoveWindow("Demo", 0, 0);
        //        cvResizeWindow("Demo", 1352, 1013);
    }

    double before = get_wall_time();

    while(1){
        ++count;
        if(1){
            if(pthread_create(&fetch_thread, 0, fetch_in_thread, 0)) error("Thread creation failed");
            if(pthread_create(&detect_thread, 0, detect_in_thread, 0)) error("Thread creation failed");

            if(!prefix){
                //                printf("wii");//hitting this
                //                printf("trying to write to video\n");
                //                IplImage *img = cvCreateImage(cvSize(disp.w,disp.h), IPL_DEPTH_8U, disp.c);
                //                IplImage *buffer = img;
                //                img = cvCreateImage(cvSize(600, 600), buffer->depth, buffer->nChannels);
                //                cvResize(buffer, img, CV_INTER_LINEAR);
                //                cvWriteFrame(writer,img);
                //                printf("written to video\n");


                //                show_image(disp, "Demo");
                int c = cvWaitKey(1);
                if (c == 10){
                    if(frame_skip == 0) frame_skip = 60;
                    else if(frame_skip == 4) frame_skip = 0;
                    else if(frame_skip == 60) frame_skip = 4;
                    else frame_skip = 0;
                }
            }else{
                //                printf("wie");
                char buff[256];
                sprintf(buff, "%s_%08d", prefix, count);
                save_image(disp, buff);
            }

            pthread_join(fetch_thread, 0);
            pthread_join(detect_thread, 0);

            if(delay == 0){
                free_image(disp);
                disp  = det;
            }
            det   = in;
            det_s = in_s;
        }else {
            //            printf("we");
            fetch_in_thread(0);
            det   = in;
            det_s = in_s;
            detect_in_thread(0);
            if(delay == 0) {
                free_image(disp);
                disp = det;
            }

            show_image(disp, "Demo");
            cvWaitKey(1);
        }
        --delay;
        if(delay < 0){
            delay = frame_skip;

            double after = get_wall_time();
            float curr = 1./(after - before);
            fps = curr;
            before = after;
        }
    }

}
//#else
//void demo(char *cfgfile, char *weightfile, float thresh, int cam_index, const char *filename, char **names, int classes, int frame_skip, char *prefix, float hier_thresh)
//{
//    fprintf(stderr, "Demo needs OpenCV for webcam images.\n");
//}
//#endif


/* References
    videowriter reference - https://github.com/Guanghan/darknet/blob/master/src/yolo_kernels.cu
*/

