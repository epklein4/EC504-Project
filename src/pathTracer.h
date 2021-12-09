#ifndef PATH_TRACE
#define PATH_TRACE

#include<iostream>
#include<iomanip>
#include <cstdlib>
#include <time.h>
#include <math.h>
#include <iostream>
#include <fstream>
#include <vector>
#include "queue.h"

using namespace std;

vector<vector<int>> backtrace(int node, int * parent) {
    cout << "---Starting backtrace---" << endl;
    vector<vector<int>> pL;
    vector<int> path;
    vector<int> pathLength;

	int iter2 = 1;
	int tempNode = node;

	while(parent[tempNode] != 0) {
        //cout << "parent of " << tempNode << " is " << parent[tempNode] << endl;
        //cout << "starting loop 1" << endl;
		tempNode = parent[tempNode];
		iter2++;
		}

    pathLength.push_back(iter2 + 1);

	int newNode = node;
	path.push_back(newNode);

	while(true) {
        cout << "starting loop 2" << endl;
        if (parent[newNode] == 0) {
            cout << "Parent of " << newNode << " is " << parent[newNode] << " << here it is!!!" << endl;
            break;
        }
	    cout << "Parent of " << newNode << " is " << parent[newNode] << endl;
		newNode = parent[newNode];
		path.push_back(newNode);
		}

    path.push_back(0);
    pL.push_back(path);
    pL.push_back(pathLength);

    return pL;
}

vector<int> edgePath(vector<int>& pathInfo, int * FV, int * edgeList) {
cout << "---Starting edgePath---" << endl;

vector<int> edgesOut;

for (int i = (pathInfo.size() - 1); i > 0 ; i--)
{
    //cout << "i = " << i << " > " << "0" << endl;
	int j = pathInfo[i];
	//cout << "j = " << j << endl;
	for (int k = FV[j]; k < FV[j+1]; k++)
		{
		    //cout << "k = " << k << " < " << FV[j+1] << endl;
			if (edgeList[k] == pathInfo[i-1])
			{
			    //cout << "Edge " << k << " == " << (pathInfo[i-1]) << endl;
				edgesOut.push_back(k);
				break;
			}

        }
}

return edgesOut;

}

#endif // PATH_TRACE
