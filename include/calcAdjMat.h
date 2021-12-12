#ifndef ADJ_MAT
#define ADJ_MAT

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <fstream>
#include <vector>

using namespace std;
using namespace cv;

//calculates adjMat for square matrix
//vector<Mat>
void calcAdjMat(Mat resizedIm, int N, int * edges, vector<Mat>& Matrices) {

    int E = (*edges);
    //vector<Mat> Matrices(2);

    //empty dimxdim adjacency matrix of itegers
    Mat adjMat;
    adjMat = Mat::zeros(N, N, CV_32SC1);

    //empty dimx1 intensity matrix of itegers
    Mat intensityMat;
    intensityMat = Mat::zeros(N, 1, CV_32SC1);

    Mat statsMat;
    statsMat = Mat::zeros(3, 1, CV_64FC1);

    int dim = sqrt(N);


    //fill adjacency matrix and intensity vector
    //it's a four-neighbor system
    for ( int y = 0; y < dim ; y++) {
        //cout << "starting loop" << endl;
        for (int x = 0; x < dim; x++) {
            //calculate pixel index
            int index;
            index = (int) ((y * dim) + x);

            //assign pixel intensity to intensity vector
            int pixelIntensity = (int) resizedIm.at<uchar>(y,x);
            intensityMat.at<int>(index, 0) = pixelIntensity;

            //loop to fill matrix and count edges
            int xL = x - 1;
            int xR = x + 1;
            int yU = y - 1;
            int yD = y + 1;

            int L = (int) ((y * dim) + xL);
            int R = (int) ((y * dim) + xR);
            int U = (int) ((yU * dim) + x);
            int D = (int) ((yD * dim) + x);

            if ((xL >= 0) && (xL < dim)) {
                adjMat.at<int>(index, L) = 1;
                E++;
                }

            if ((xR >= 0) && (xR < dim)) {
                adjMat.at<int>(index, R) = 1;
                E++;
                }

            if ((yU >= 0) && (yU < dim)) {
                adjMat.at<int>(index, U) = 1;
                E++;
                }

            if ((yD >= 0) && (yD < dim)) {
                adjMat.at<int>(index, D) = 1;
                E++;
                }
            }
        }
        *edges = E;
        Matrices.push_back(adjMat);
        Matrices.push_back(intensityMat);

        //Calc average intensity
        Scalar MeanInt, StdInt;
        double meanInt, stdInt, varInt;
        meanStdDev(intensityMat, MeanInt, StdInt);
        //meanInt = MeanInt[0];
        //stdInt = StdInt[0];
        //varInt = sqrt(stdInt);
        cout << "MEAN INTENSITY: " << meanInt << endl;

        statsMat.at<double>(0, 0) = MeanInt[0];
        statsMat.at<double>(1, 0) = StdInt[0];
        statsMat.at<double>(2, 0) = sqrt(StdInt[0]);
        Matrices.push_back(statsMat);

        return;
}

#endif // ADJ_MAT
