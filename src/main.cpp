#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <fstream>
#include <vector>
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

void CallBackFunc(int event, int x, int y, int flags, void* userdata);

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
    int * bfsPtr = &numBFS;

    // read in image, image resolution
    //Mat srcIm = imread("resources/test1.jpg", IMREAD_GRAYSCALE); // read in image
    Mat srcIm = imread("test1.jpg", IMREAD_GRAYSCALE);
    if(!srcIm.data) {
        cout << "Image not found" << endl;
        return -1;
        }
    int height = srcIm.size().height;
    int width = srcIm.size().width;
    cout << "Dimensions of input image: " << height << " x " << width << endl;
    //

    //change resolution based on user input
    int newDim, N;
    cout << "The image will be squared. Input new width: ";
    cin >> newDim;
    N = newDim * newDim;
    Mat resizedIm;
    resize(srcIm, resizedIm, Size(newDim, newDim));
    int newHeight = resizedIm.size().height;
    int newWidth = resizedIm.size().width;
    cout << "Dimensions of resized image: " << newHeight << " x " << newWidth << endl;
    //

    //here we mange the mouse-clicking part
    //it's pretty delicate, be careful changing it
    /////////////////////////////////////////////////////

    // create (empty) window to display image
    namedWindow( "Display pic" );

    //give user instructions
    cout << "Left/Right click to add background/foreground pixels. " << endl;
    cout << "Press mouse wheel when finished." << endl;

    //call mouse handler to get source/sink pts from user
    setMouseCallback("Display pic", CallBackFunc, &bgfgNull);

    while (flag == false) {
        imshow( "Display pic", resizedIm );// show image
        waitKey(1);
    }

    if (flag == true) {
        destroyAllWindows();
    }

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
    int edgeList[(E+1)];
    int edgeCap[(E+1)];
    int FV[N+2];
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
    edmondsKarp(V, E, success, sinkInd, bfsPtr, FV, edgeList, flow, Cap, Space);

return 0;

}


void CallBackFunc(int event, int x, int y, int flags, void* userdata) {
     if  ( event == EVENT_LBUTTONDOWN ) {
        cout << "Left button of the mouse is clicked - position (" << x << ", " << y << ")" << endl;

        sizeBG++;
        bgfgNull[0].resize(sizeBG);

        (bgfgNull[0][sizeBG - 1]).x = x;
        (bgfgNull[0][sizeBG - 1]).y = y;

        cout << "Background pixel: (" << (bgfgNull[0][sizeBG - 1]).x << ", " << (bgfgNull[0][sizeBG - 1]).y << ")" << endl;
        }

    if  ( event == EVENT_RBUTTONDOWN ) {
        cout << "Right button of the mouse is clicked - position (" << x << ", " << y << ")" << endl;
        sizeFG++;
        bgfgNull[1].resize(sizeFG);

        (bgfgNull[1][sizeFG - 1]).x = x;
        (bgfgNull[1][sizeFG - 1]).y = y;

        cout << "Foreground pixel: (" << (bgfgNull[1][sizeFG - 1]).x << ", " << (bgfgNull[1][sizeFG - 1]).y << ")" << endl;
        }

    if (  event == EVENT_MBUTTONDOWN  ) {
        flag = true;
        cout << "Flag: " << flag << endl;
        }

     return;
    }
