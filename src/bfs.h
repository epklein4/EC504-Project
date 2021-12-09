#ifndef BFS_
#define BFS_

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

int * BFS(int * FirstVertex, int V, int * EdgeList, int E, vector<int>& space, bool * success);

int * BFS(int * FirstVertex, int V, int * EdgeList, int E, vector<int>& space, bool * success) {
    //cout << "---Starting BFS---" << endl;

    Queue * Q = createQueue(V);
    int source = 0;

    int * Found = new int[V];
    fill_n(Found, V, -1);

    Enqueue(Q, source); //push source into quene and set found.
    Found[source] = 1;

    int node;
    static int CountDequeue = 0;

    int * parent = new int[V + 1];
    fill_n(parent, V, -1);

    while(true) {
        if (Q->size == 0) {
            cout << "---Oops! Q is empty. Terminating BFS.---" << endl;
            return parent;
        }
        node = Dequeue(Q);
        printQ(Q);
        cout << "node = " << node << ", Found: " << Found[node] << endl;;
        if (node == (V-1)) {
            parent[V] = 1; //flagging successful run
            *success = true;
            //cout << "---Found Sink, terminating BFS.---" << endl;
            return parent;
        }
        CountDequeue++;
        cout << "FV from " << FirstVertex[node] << " to " << FirstVertex[node + 1] << endl;
        for(int i = FirstVertex[node]; i < FirstVertex[node +1]; i++)
            {
            cout << "Looking from: " << FirstVertex[node] << " to " << FirstVertex[node +1] << endl;
            cout << "Found " << EdgeList[i] << ": " << Found[EdgeList[i]] << ", Space: " << space[i] << endl;
            if((Found[EdgeList[i]] == -1) && (space[i] > 0))
                {
                cout << " Found new, " << EdgeList[i] << " at " << i << endl;
                Enqueue(Q,EdgeList[i]);
                Found[EdgeList[i]] = 1;
                parent[EdgeList[i]] = node;
                cout << "Parent of " << EdgeList[i] << " is " << node << endl;
                }
            }
    }
    *success = false;
    return parent;
}

#endif // BFS
