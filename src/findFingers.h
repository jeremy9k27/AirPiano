#ifndef FINDFINGERS_H
#define FINDFINGERS_H

#include <vector>
#include <opencv2/opencv.hpp>

using namespace cv;

std::vector<cv::Point> findFingersHelper(const Mat& inputImage);

#endif