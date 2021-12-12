#ifndef CALCEDGES
#define CALCEDGES

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <fstream>
#include <vector>
#include "utility.h"

using namespace std;
using namespace cv;

void calcEdges(int E, int N, int nSource, int nSink, int stdInt, Mat adjMat, Mat intensityMat,
               int * edgeList, int * edgeCap, int * FV, vector<int>& flow, vector<int>& Cap,
               vector<int>& Space, vector<vector<Point>>& bgfgNull) {

    vector<int> sinkIndList;

    int newWidth = sqrt(N);
    int edgeIndex = E;
    int edgeListIndexFront = 0;
    int edgeListIndexBack = E;

    edgeList[edgeListIndexBack] = -1;
    edgeListIndexBack--;

    FV[0] = 0;
    FV[1] = nSource;
    FV[N+1] = E;

    for (int i = 0; i < nSink; i++) {
        int x = (bgfgNull[1][i]).x;
        int y = (bgfgNull[1][i]).y;
        int index = (int) ((y * newWidth) + x);
        sinkIndList.push_back(index + 1);
        }

    sort(sinkIndList.begin(), sinkIndList.end());

    for (int i = 0; i < nSource; i++) {
        int x = (bgfgNull[0][i]).x;
        int y = (bgfgNull[0][i]).y;
        int index = (int) ((y * newWidth) + x);
        edgeList[edgeListIndexFront] = index + 1;
        edgeCap[i] = 101;
        edgeListIndexFront++;
        }

    int startPrev;
    int startCurrent;
    int numFound = 0;

    int * foundSink;

    vector<int>::iterator it;


    for (int y = (N-1); y >= 0; y--) {
        for (int x = (N-1); x >= 0; x--) {

		//Fill FV//
        	if (adjMat.at<int>(y, x) == 1) {
                numFound++;
                if (numFound == 1) {
                    startCurrent = y+1;
                }
                else {
                    startPrev = startCurrent;
                    startCurrent = y+1;
                    if (startPrev != startCurrent) {
                        FV[startPrev] = edgeListIndexBack + 1;
                    }
                }

                //calc capacity//
                int X1 = y % newWidth;
                int Y1 = (y - X1) / newWidth;
                int X2 = x % newWidth;
                int Y2 = (x - X2) / newWidth;
                int I1 = intensityMat.at<int>(y, 0);
                int I2 = intensityMat.at<int>(x, 0);
                double expTerm = ( -pow((I1 - I2),2) / (2 * stdInt) );
                int CAPACITY = (int) ((100 * exp(expTerm)) + 1);
                /////////////////

                it = find(sinkIndList.begin(), sinkIndList.end(), (y+1));

                if (it != sinkIndList.end()) {
                    edgeList[edgeListIndexBack] = (N+1);
                    edgeCap[edgeListIndexBack] = 101;
                    edgeListIndexBack--;
                    *it = -9999;
                    numFound++;
                    startPrev = startCurrent;
                    startCurrent = y+1;
                    if (startPrev != startCurrent) {
                        FV[startPrev] = edgeListIndexBack + 1;
                        }
                    }

                //fill in edgeList, edgeCap
                edgeList[edgeListIndexBack] = x + 1;
                edgeCap[edgeListIndexBack] = CAPACITY;
                edgeListIndexBack--;
                }
            }
        }

    for (int i = 0; i < (E + 1); i++)
        {
            Cap.push_back(edgeCap[i]);
            Space.push_back(edgeCap[i]);
            flow.push_back(0);
        }
}

#endif // CALCEDGES
