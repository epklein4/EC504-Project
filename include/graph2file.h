#ifndef GRAPH2FILE
#define GRAPH2FILE

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

using namespace std;
using namespace cv;

void graph2file(int sizeEdgeList, int nSource, int nSink, int sinkInd, int V, int E, int height, int width, int N,
                int * FV, int * edgeList, int * edgeCap, vector<vector<Point>>& bgfgNull, Mat adjMat) {
int newHeight = sqrt(N);
int newWidth = newHeight;

ofstream myFile("metadata.txt");
    myFile << "G(" << V << "," << E << ")" << endl;
    myFile << "Number of source nodes: " << nSource << endl;
    myFile << "Source node indices (y,x): ";
    for (int i = 0; i < nSource; i++) {
        int x = (bgfgNull[0][i]).x;
        int y = (bgfgNull[0][i]).y;
        myFile << "(" << y << ", " << x << ")  ";
        }
    myFile << endl << "Number of sink nodes: " << nSink << endl;
    myFile << "Sink node indices (y,x): ";
    for (int i = 0; i < nSource; i++) {
        int x = (bgfgNull[1][i]).x;
        int y = (bgfgNull[1][i]).y;
        myFile << "(" << y << ", " << x << ")  ";
        }
    myFile << endl << "Original resolution: " << height << "x" << width << endl;
    myFile << "Squared resolution: " << newHeight << "x" << newWidth << endl;
myFile.close();

ofstream adjMatOut("adjMat.txt");
    for (int y = (N-1); y >= 0; y--) {
        for (int x = (N-1); x >= 0; x--) {
            adjMatOut << adjMat.at<int>(y, x) << "  ";
            }
            adjMatOut << endl;
        }
adjMatOut.close();

ofstream FirstVertex("FirstVertex.txt");
    for (int i = 0; i <= sinkInd; i++)
        {
            //cout << i << ": " << FV[i] << endl;
            FirstVertex << i << ": " << FV[i] << endl;
        }
FirstVertex.close();

ofstream EdgeList("EdgeList.txt");
        for (int i = 0; i < sizeEdgeList; i++)
        {
            //cout << i << ": " << edgeList[i] << ", cap: " << edgeCap[i] << endl;
            EdgeList << i << ": " << edgeList[i] << ", cap: " << edgeCap[i] << endl;
        }
EdgeList.close();
return;
}

#endif
