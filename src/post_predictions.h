#ifndef PREDICTIONS_H
#define PREDICTIONS_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <libgen.h>

#include "activation_layer.h"
#include "activations.h"
#include "assert.h"
#include "avgpool_layer.h"
#include "batchnorm_layer.h"
#include "blas.h"
#include "box.h"
#include "connected_layer.h"
#include "convolutional_layer.h"
#include "cost_layer.h"
#include "crnn_layer.h"
#include "crop_layer.h"
#include "detection_layer.h"
#include "dropout_layer.h"
#include "gru_layer.h"
#include "list.h"
#include "local_layer.h"
#include "maxpool_layer.h"
#include "normalization_layer.h"
#include "option_list.h"
#include "parser.h"
#include "region_layer.h"
#include "reorg_layer.h"
#include "rnn_layer.h"
#include "route_layer.h"
#include "shortcut_layer.h"
#include "softmax_layer.h"
#include "utils.h"


void save_detections_counts(char *image_id, int num, int width, int height, float thresh,
                     box *boxes, float **probs, char **names, int classes,int* counts);
void save_counts(char *image_id, int num, int width, int height, float thresh,
                     box *boxes, float **probs, char **names, int classes,int* counts);
float get_avg_prob(float **probs, int num, float thresh, int classes);
#endif // PREDICTIONS_H
