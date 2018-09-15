#!/usr/bin/env python
# coding: UTF-8

from __future__ import print_function
# Not implemented sys.argv by boost.python, initialize keras fail
import sys
if not hasattr(sys, 'argv'):
    sys.argv  = ['']
import keras
from keras.models import Sequential
from keras.models import load_model
from keras import backend as K
import math
import numpy as np
import os

MODEL_PATH = "bin\\data\\dnn\\model.h5"
BASE_LENGTH_HEAD_TO_THROAT = 66.0
IMG_ROW       = 16
IMG_COL       = 16
POINT_NUM     = 14
POSE_NUM      = 10
ARROWABLE_ERROR_LENGTH = 30.0

class Point():
    def __init__(self, _x, _y):
        self.x = _x;
        self.y = _y;

def create_point_list(skelton_pos_list):
    point_list = []
    for i in range(0, POINT_NUM):
        x_index = i * 2
        y_index = x_index + 1
        point_list.append(Point(skelton_pos_list[x_index], skelton_pos_list[y_index]))

    return point_list

def transform_to_head_coordinate(point_list):
    is_head = True
    head_x = 0.0
    head_y = 0.0
    transformed_point_list = []
    for point in point_list:
        if is_head == True:
            head_x = point.x
            head_y = point.y
            transformed_point = Point(0.0, 0.0)
            transformed_point_list.append(transformed_point)
            is_head = False
        else:
            x = point.x - head_x
            y = point.y - head_y
            transformed_point = Point(x, y)
            transformed_point_list.append(transformed_point)

    return transformed_point_list

def calc_resize_ratio(point_list):
    length_head_to_throat = math.sqrt((point_list[1].x ** 2) + (point_list[1].y ** 2))
    return BASE_LENGTH_HEAD_TO_THROAT / length_head_to_throat

def resize_skelton(point_list):
    resized_point_list = []
    resize_ratio = calc_resize_ratio(point_list)
    for point in point_list:
        x = point.x * resize_ratio
        y = point.y * resize_ratio
        resized_point_list.append(Point(x, y))

    return resized_point_list

def set_nparray(point_list):
    x_test = np.zeros((1, IMG_ROW, IMG_COL))
    for point in range(0, POINT_NUM):
        x_test[0, point, point]   = point_list[point].x
        x_test[0, point, point+1] = point_list[point].y

    x_test = x_test.reshape(x_test.shape[0], IMG_ROW, IMG_COL, 1)
    x_test = x_test.astype('float32')

    return x_test

def culc_pose_matching(point_list_left, point_list_right):
    matching_points = 0.0

    for point in range(0, POINT_NUM):
        x_left  = point_list_left[0, point, point]
        y_left  = point_list_left[0, point, point+1]
        x_right = point_list_right[0, point, point]
        y_right = point_list_right[0, point, point+1]

        x_error = abs(x_left - x_right)
        y_error = abs(y_left - y_right)

        if (x_error < ARROWABLE_ERROR_LENGTH) and (y_error < ARROWABLE_ERROR_LENGTH):
            matching_points += 1.0

    return matching_points / POINT_NUM

def convert_dataset(skelton_pos_list):
    point_list        = create_point_list(skelton_pos_list)
    transformeed_list = transform_to_head_coordinate(point_list)
    resized_list      = resize_skelton(transformeed_list)
    x_test            = set_nparray(resized_list)

    return x_test

def predict_pose(model, skelton_pos_list):
    current_point_array = convert_dataset(skelton_pos_list)

    proba_array = model.predict_proba(current_point_array, verbose=0)

    pose_id = proba_array[0].argmax()
    proba   = proba_array[0].max()

    return pose_id, proba, current_point_array

"""
    to call from cpp
"""
model = load_model(MODEL_PATH)
last_point_array = np.zeros((1, IMG_ROW, IMG_COL))

def predeict_pose_from_cpp(skelton_pos_list):
    global model
    global last_point_array

    pose_id, proba, current_point_array = predict_pose(model, skelton_pos_list)
    print("id, proba: ", pose_id, proba)

    last_pose_matching = culc_pose_matching(current_point_array, last_point_array)
    last_point_array   = current_point_array

    return (float(pose_id), float(proba), float(last_pose_matching))
