#ifndef UTIL
#define UTIL

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include<iostream>
#include<iomanip>
#include <cstdlib>
#include <time.h>
#include <math.h>
#include <iostream>
#include <fstream>
#include <vector>

using namespace std;
using namespace cv;

void printMat(Mat * myMat) {
    int Y = myMat->size().height;
    int X = myMat->size().width;

    for (int y = 0; y < Y; y++) {
        for (int x = 0; x < X; x++) {
            cout << myMat->at<int>(y, x) << "  ";
            }
        cout << endl << "--NEW ROW--" << endl;;
        }

    cout << endl << "--FINISHED PRINTING--" << endl;

    return;
}

void printMatDouble(Mat * myMat) {
    int Y = myMat->size().height;
    int X = myMat->size().width;

    for (int y = 0; y < Y; y++) {
        for (int x = 0; x < X; x++) {
            cout << myMat->at<double>(y, x) << "  ";
            }
        cout << endl << "--NEW ROW--" << endl;;
        }

    cout << endl << "--FINISHED PRINTING--" << endl;

    return;
}



#endif
