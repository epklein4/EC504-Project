#ifndef EK
#define EK

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

using namespace std;
using namespace cv;

void edmondsKarp(int V, int E, bool success, int sinkInd, int * bfsPtr, int * FV, int * edgeList,
                 vector<int>& flow, vector<int>& Cap, vector<int>& Space) {

	vector<int> tempFlow;
	vector<int> tempCap;
	vector<int> tempSpace;
	vector<vector<int>> pathInfo;
	vector<int> edgesOut;
	int minSpace;
	int * parentOut;

	//while possible, run BFS and augment paths (edmonds-karp)//
	while (true) {
        success = false;
		parentOut = BFS(FV, V, edgeList, E, Space, &success);
		if (success == true) {
    			(*bfsPtr)++;
    			cout << "---BFS " << (*bfsPtr) << " successful---" << endl;
			}
        else {
            (*bfsPtr)++;
            cout << "---BFS " << (*bfsPtr) << " failed---" << endl;
            break;
            }
		pathInfo = backtrace(sinkInd, parentOut);
		edgesOut = edgePath(pathInfo[0], FV, edgeList);
		for(int i=0; i < edgesOut.size(); i++) {
            //cout << "Edge: " << edgesOut[i] << ", ";
            tempFlow.push_back(flow[edgesOut[i]]);
            //cout << "Flow: " << flow[edgesOut[i]] << ", ";
            tempCap.push_back(Cap[edgesOut[i]]);
            //cout << "Cap: " << Cap[edgesOut[i]] << ", ";
            tempSpace.push_back(Space[edgesOut[i]]);
            //cout << "Space: " << Space[edgesOut[i]] << endl << endl;
            }

		minSpace =*min_element(tempSpace.begin(), tempSpace.end());

		for(int i=0; i < edgesOut.size(); i++) {
            //cout << "Update -- Edge: " << edgesOut[i] << ", ";
            flow[edgesOut[i]] = flow[edgesOut[i]] + minSpace;
            //cout << "Flow: " << flow[edgesOut[i]] << ", ";
            //cout << "Cap: " << Cap[edgesOut[i]] << ", ";
            Space[edgesOut[i]] = Space[edgesOut[i]] - minSpace;
            //cout << "Space: " << Space[edgesOut[i]] << endl << endl;
            }

		tempFlow.clear();
		tempCap.clear();
		tempSpace.clear();
		pathInfo.clear();
		edgesOut.clear();
		}
		return;
}

#endif
