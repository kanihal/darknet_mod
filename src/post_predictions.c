#include "post_predictions.h"

void save_detections_counts(char *image_id, int num, int width, int height, float thresh,
                            box *boxes, float **probs, char **names, int classes,int* counts)
{
    FILE *fp;
    //    FILE *fpc;
    /*static int *p = NULL;
    if(!p) p = (int *)malloc(sizeof(int));*/
    char *pred_filename=NULL;
    //    char *count_filename=NULL;
    int j;
    int i;
    printf("classes=%d\n",classes);
    printf("intializing counts=0 ... ");
    for (j=0;j<classes;++j){
        counts[j]=0;
    }
    printf("Done\n");

    pred_filename = calloc(strlen("pred_") +strlen(basename(image_id))+ strlen("_.csv"),sizeof(char));
    strcpy(pred_filename, "pred_");
    strcat(pred_filename, basename(image_id));
    strcat(pred_filename, "_.csv");
    printf("malloc fp open...");
    if ((fp = fopen(pred_filename, "a")) == NULL) {
        printf("File open error: %s\n", pred_filename);
        exit(1);
    }
    printf("Done\n");
    //    printf("malloc fpc open...");
    //    count_filename = calloc(strlen("counts_") +strlen(basename(image_id))+ strlen("_.csv"),sizeof(char));
    //    printf("cpy\n");
    //    strcpy(count_filename, "counts_");
    //    printf("cat\n");
    //    strcat(count_filename, basename(image_id));
    //    printf("cat\n");
    //    strcat(count_filename, "_.csv");
    //    printf("done\n");
    //    if ((fpc = fopen(count_filename, "a")) == NULL) {
    //        printf("File open error: %s\n", count_filename);
    //        exit(1);
    //    }
    //    printf("Done\n");

    fprintf(fp, "image_id,img_width,img_height,thresh,box_left,box_right,\
            box_top,box_bottom,class_prob,class_name,class_id\n");


            printf("num=%d\n",num);
            printf("classes=%d\n",classes);

    for (i = 0; i < num; ++i) {
        int class = max_index(probs[i], classes);
        float prob = probs[i][class];

        if (! (prob > thresh)) {
            continue;
        }

        box b = boxes[i];

        int left  = (b.x - b.w / 2.) * width;
        int right = (b.x + b.w / 2.) * width;
        int top   = (b.y - b.h / 2.) * height;
        int bot   = (b.y + b.h / 2.) * height;

        if (left < 0) left = 0;
        if (right > width - 1) right = width - 1;
        if (top < 0) top = 0;
        if (bot > height - 1) bot = height - 1;
        counts[class]+=1;
        fprintf(fp, "%s,%d,%d,%f,%d,%d,%d,%d,%f,%s,%d\n",
                image_id, width, height, thresh, left, right, top, bot, prob, names[class],class);
    }
    fprintf(fp, " , , , , , , , , , , \n");
    fprintf(fp, "image_id,class,count\n");
    for (j=0;j<classes;++j){
        fprintf(fp, " %s,%s,%d\n",image_id,names[j],counts[j]);
    }
    fprintf(fp, " , , \n");
    fprintf(fp, " , , , , , , , , , , ,############################################\n");
    fclose(fp);
    //    fclose(fpc);
    if(!pred_filename)
        free(pred_filename);
    //    if(!count_filename)
    //        free(count_filename);
    printf("successfully returned from save_detections_counts\n");

}









void save_counts(char *image_id, int num, int width, int height, float thresh,
                 box *boxes, float **probs, char **names, int classes,int* counts)
{
    FILE *fp;
    //    FILE *fpc;
    /*static int *p = NULL;
    if(!p) p = (int *)malloc(sizeof(int));*/
    char *pred_filename=NULL;
    //    char *count_filename=NULL;
    int j;
    int i;
    printf("classes=%d\n",classes);
    printf("intializing counts=0 ... ");
    for (j=0;j<classes;++j){
        counts[j]=0;
    }
    printf("Done\n");
    pred_filename = calloc(strlen(basename(image_id)),sizeof(char));
    strcat(pred_filename, basename(image_id));


    printf("malloc fp open...");
    if ((fp = fopen("counts.csv", "a")) == NULL) {
        printf("File open error: counts.csv\n");
        exit(1);
    }
    printf("Done\n");
    //    printf("malloc fpc open...");
    //    count_filename = calloc(strlen("counts_") +strlen(basename(image_id))+ strlen("_.csv"),sizeof(char));
    //    printf("cpy\n");
    //    strcpy(count_filename, "counts_");
    //    printf("cat\n");
    //    strcat(count_filename, basename(image_id));
    //    printf("cat\n");
    //    strcat(count_filename, "_.csv");
    //    printf("done\n");
    //    if ((fpc = fopen(count_filename, "a")) == NULL) {
    //        printf("File open error: %s\n", count_filename);
    //        exit(1);
    //    }
    //    printf("Done\n");

    //    fprintf(fp, "image_id,img_width,img_height,thresh,box_left,box_right,
    //            box_top,box_bottom,class_prob,class_name,class_id\n");


    printf("num=%d\n",num);
    printf("classes=%d\n",classes);

    for (i = 0; i < num; ++i) {
        int class = max_index(probs[i], classes);
        float prob = probs[i][class];

        if (! (prob > thresh)) {
            continue;
        }

        box b = boxes[i];

        int left  = (b.x - b.w / 2.) * width;
        int right = (b.x + b.w / 2.) * width;
        int top   = (b.y - b.h / 2.) * height;
        int bot   = (b.y + b.h / 2.) * height;

        if (left < 0) left = 0;
        if (right > width - 1) right = width - 1;
        if (top < 0) top = 0;
        if (bot > height - 1) bot = height - 1;
        counts[class]+=1;
        //        fprintf(fp, "%s,%d,%d,%f,%d,%d,%d,%d,%f,%s,%d\n",
        //                image_id, width, height, thresh, left, right, top, bot, prob, names[class],class);
    }
    //    fprintf(fp, " , , , , , , , , , , \n");
    //    fprintf(fp, "image_id,class,count\n");
    //    for (j=0;j<classes;++j){
    //        fprintf(fp, " %s,%s,%d\n",image_id,names[j],counts[j]);
    //    }
    //    fprintf(fp, " , , \n");
    //    fprintf(fp, " , , , , , , , , , , ,############################################\n");
    fprintf(fp, "%s,%d,%d\n",pred_filename,counts[0],counts[56]);//0 for person and 56 for chair
    fclose(fp);
    //    fclose(fpc);
    if(!pred_filename)
        free(pred_filename);
    //    if(!count_filename)
    //        free(count_filename);
    printf("successfully returned from save_detections_counts\n");

}

float get_avg_prob(float **probs, int num, float thresh, int classes){


    float p=0;
    int count=0;
    int i;
    for (i = 0; i < num; ++i) {
        int class = max_index(probs[i], classes);
        float prob = probs[i][class];
        //        printf("p=%f\n",prob);
        //        printf("c=%d\n",count);
        if (prob > thresh) {
            p+=prob;
            count++;
            //printf("--prob=%f\n",prob);
            //printf("--count=%d\n",count);
        }

    }

    if (count==0){
        return 0;
    }else{
        //        printf("p/c=%f\n",p/count);
        return (p/count);
    }
}
