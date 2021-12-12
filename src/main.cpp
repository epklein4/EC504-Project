#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include "queue.h"
#include "bfs.h"
#include "pathTracer.h"
#include "utility.h"
#include "calcAdjMat.h"
#include "graph2file.h"
#include "calcEdges.h"
#include "edmondsKarp.h"

using namespace std;
using namespace cv;

#define PERFORMANCE_TEST 1

void CallBackFunc(int event, int x, int y, int flags, void *userdata);

//Global Variables
vector<vector<Point>> bgfgNull(2);
bool flag, success;
int sizeBG, sizeFG; //

int main()
{
    flag = false;
    sizeBG = 0;
    sizeFG = 0;

    int numBFS = 0; //we use these later
    int *bfsPtr = &numBFS;

    // read in image, image resolution
    //Mat srcIm = imread("resources/test1.jpg", IMREAD_GRAYSCALE); // read in image
    Mat srcIm = imread("./images/test1.jpg", IMREAD_GRAYSCALE);
    if (!srcIm.data)
    {
        cout << "Image not found" << endl;
        return -1;
    }
    int height = srcIm.size().height;
    int width = srcIm.size().width;
    cout << "Dimensions of input image: " << height << " x " << width << endl;
    //

    //change resolution based on user input
    int newDim, N;
    Mat resizedIm;
    int newHeight;
    int newWidth;
#if PERFORMANCE_TEST == 0
    cout << "The image will be squared. Input new width: ";
    cin >> newDim;
    N = newDim * newDim;
    resize(srcIm, resizedIm, Size(newDim, newDim));
    newHeight = resizedIm.size().height;
    newWidth = resizedIm.size().width;
    cout << "Dimensions of resized image: " << newHeight << " x " << newWidth << endl;
#endif
    //
#if PERFORMANCE_TEST == 1
    ofstream myfile;
    myfile.open("performance.txt");
    for (int pix = 40; pix <= 200; pix += 20)
    {
        numBFS = 0;
        bfsPtr = &numBFS;
        N = pix * pix;
        resize(srcIm, resizedIm, Size(pix, pix));
        newHeight = resizedIm.size().height;
        newWidth = resizedIm.size().width;
#endif

        //here we mange the mouse-clicking part
        //it's pretty delicate, be careful changing it
        /////////////////////////////////////////////////////

        // create (empty) window to display image
        namedWindow("Display pic");

        //give user instructions
        cout << "Left/Right click to add background/foreground pixels. " << endl;
        cout << "Press mouse wheel when finished." << endl;

        //call mouse handler to get source/sink pts from user
        setMouseCallback("Display pic", CallBackFunc, &bgfgNull);

        while (flag == false)
        {
            imshow("Display pic", resizedIm); // show image
            waitKey(1);
        }

        if (flag == true)
        {
            destroyAllWindows();
        }

#if PERFORMANCE_TEST == 1
        // Start timer
        auto start = chrono::high_resolution_clock::now();
#endif

        /////////////////////////////////////////////////////

        //assign source/sink info based on user input
        int nSource = sizeBG;
        int nSink = sizeFG;
        int sourceInd = 0;
        int sinkInd = N + 1;
        //

        //define V and E
        int V = N + 2;
        int E = nSource + nSink;
        //

        /*////////////////////////////////////////////////////
    ***CIRCLES SHOULD GO HERE***
    /////////////////////////////////////////////////////*/

        //Calculate NxN adjacency matrix and Nx1 intensity vector
        vector<Mat> Matrices;
        calcAdjMat(resizedIm, N, &E, Matrices);
        Mat adjMat = Matrices[0];
        Mat intensityMat = Matrices[1];
        Mat statsMat = Matrices[2];
        double meanInt, stdInt, varInt;
        meanInt = statsMat.at<double>(0, 0);
        stdInt = statsMat.at<double>(1, 0);
        varInt = statsMat.at<double>(2, 0);

        //instantiate some variables that we need for calculating edges
        int edgeList[(E + 1)];
        int edgeCap[(E + 1)];
        int FV[N + 2];
        vector<int> flow;
        vector<int> Cap;
        vector<int> Space;

        //calculate edges, edge capacity
        calcEdges(E, N, nSource, nSink, stdInt, adjMat, intensityMat,
                  edgeList, edgeCap, FV, flow, Cap, Space, bgfgNull);

        //put the graph info in a txt file
        graph2file((E + 1), nSource, nSink, sinkInd, V, E, height, width, N,
                   FV, edgeList, edgeCap, bgfgNull, adjMat);

        //run Edmonds-Karp
        int *segmentA;
        segmentA = edmondsKarp(V, E, success, sinkInd, bfsPtr, FV, edgeList, flow, Cap, Space);

#if PERFORMANCE_TEST == 0
        Mat newIm;
        newIm = Mat::zeros(newDim, newDim, CV_8UC1);

        for (int i = 1; i <= N; i++)
        {
            int index = i - 1;
            int x = index % newWidth;
            int y = (index - x) / newWidth;
            if (segmentA[i] != -1)
            {
                newIm.at<uchar>(y, x) = 255;
            }
            else
            {
                newIm.at<uchar>(y, x) = 0;
            }
        }

        namedWindow("newImg", WINDOW_AUTOSIZE);
        imshow("newImg", newIm);
        waitKey(0);
        destroyAllWindows();
#endif

#if PERFORMANCE_TEST == 1
        // End Timer and calculate time
        auto end = chrono::high_resolution_clock::now();
        auto duration = chrono::duration_cast<chrono::microseconds>(end - start);
        cout << "Time taken by Edmonds-Karp: " << duration.count() << " microseconds" << endl;
        if(pix != 40)
        {
            myfile << pix << " " << E << " " << duration.count() << endl;
        }
    }
    myfile.close();
#endif

    return 0;
}

void CallBackFunc(int event, int x, int y, int flags, void *userdata)
{
    if (event == EVENT_LBUTTONDOWN)
    {
        cout << "Left button of the mouse is clicked - position (" << x << ", " << y << ")" << endl;

        sizeBG++;
        bgfgNull[0].resize(sizeBG);

        (bgfgNull[0][sizeBG - 1]).x = x;
        (bgfgNull[0][sizeBG - 1]).y = y;

        cout << "Background pixel: (" << (bgfgNull[0][sizeBG - 1]).x << ", " << (bgfgNull[0][sizeBG - 1]).y << ")" << endl;
    }

    if (event == EVENT_RBUTTONDOWN)
    {
        cout << "Right button of the mouse is clicked - position (" << x << ", " << y << ")" << endl;
        sizeFG++;
        bgfgNull[1].resize(sizeFG);

        (bgfgNull[1][sizeFG - 1]).x = x;
        (bgfgNull[1][sizeFG - 1]).y = y;

        cout << "Foreground pixel: (" << (bgfgNull[1][sizeFG - 1]).x << ", " << (bgfgNull[1][sizeFG - 1]).y << ")" << endl;
    }

    if (event == EVENT_MBUTTONDOWN)
    {
        flag = true;
        cout << "Flag: " << flag << endl;
    }

    return;
}
