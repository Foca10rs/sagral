#pragma once
#include <opencv2/opencv.hpp>
class Detector
{
     public:
         void detect(cv::VideoCapture & cap);
         void detect(cv::Mat &image);
};