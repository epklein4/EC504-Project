#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <fstream>
#include <vector>

using namespace std;
using namespace cv;

typedef struct Queue
{
        int capacity;
        int size;
        int front;
        int rear;
        int *elements;
} Queue ;

Queue * createQueue(int maxElements);
void Enqueue(Queue *Q,int element);
int Dequeue(Queue *Q);
int * BFS(int *FirstVertex, int V, int *edgeList,int E, vector<int>& space);
void PrintAdjacencyListFormat( int *FirstVertex, int V, int *EdgeList,int E);
vector<vector<int>> backtrace(int node, int * parent);
vector<int> edgePath(vector<int>& pathInfo, int * FV, int * edgeList);

void CallBackFunc(int event, int x, int y, int flags, void* userdata);
void printMat(Mat * myMat);
void printMatDouble(Mat * myMat);
void printQ(Queue * Q);

vector<vector<Point>> bgfgNull(2);
bool bgSet, fgSet, flag;
int sizeBG, sizeFG;
bool success;

int main()
{
    bgSet = false;
    fgSet = false;
    flag = false;

    sizeBG = 0;
    sizeFG = 0;

    vector<Point> BG;
    vector<Point> FG;

    bgfgNull[0] = BG;
    bgfgNull[1] = FG;

    //Mat srcIm = imread("resources/test1.jpg", IMREAD_GRAYSCALE); // read in image
    Mat srcIm = imread("test1.jpg", IMREAD_GRAYSCALE); // read in image

    if(!srcIm.data) {
        cout << "Image not found" << endl;
        return -1;
        }

    //read image resolution
    int height = srcIm.size().height;
    int width = srcIm.size().width;
    cout << "Dimensions of input image: " << height << " x " << width << endl;

    int newDim;
    cout << "The image will be squared. Input new width: ";
    cin >> newDim;

    Mat resizedIm;
    resize(srcIm, resizedIm, Size(newDim, newDim));
    int newHeight = resizedIm.size().height;
    int newWidth = resizedIm.size().width;

    cout << "Dimensions of resized image: " << newHeight << " x " << newWidth << endl;

    /////////////////////////////////////////////////////
    namedWindow( "Display pic" );// create (empty) window to display image

    cout << "Left/Right click to add background/foreground pixels. " << endl;
    cout << "Press mouse wheel when finished." << endl;
    setMouseCallback("Display pic", CallBackFunc, &bgfgNull);

    while (flag == false) {
        imshow( "Display pic", resizedIm );// show image
        waitKey(1);
    }

    if (flag == true) {
        destroyAllWindows();
    }

    /////////////////////////////////////////////////////


    //int x = (bgfgNull[0][i]).x == bgfgNull[0][i]).x
    //int y = bgfgNull[1][i]).y

    //number of vertices (pixels)
    int N = newHeight * newWidth;
    int V = N + 2;

    int nSource = sizeBG;
    int nSink = sizeFG;
    int sourceInd = 0;
    int sinkInd = N + 1;

    //empty N by N adjacency matrix (4-connected)
    Mat adjMat;
    adjMat = Mat::zeros(N, N, CV_32SC1);

    Mat intensityMat;
    intensityMat = Mat::zeros(N, 1, CV_32SC1);

    int E = nSource + nSink;

    //Fill adjacency matrix and intensity vector
    for ( int y = 0; y < newHeight; y++) {
        for (int x = 0; x < newWidth; x++) {
            int index;
            index = (int) ((y * newWidth) + x);

            int pixelIntensity = (int) resizedIm.at<uchar>(y,x);
            intensityMat.at<int>(index, 0) = pixelIntensity;

            int xL = x - 1;
            int xR = x + 1;
            int yU = y - 1;
            int yD = y + 1;

            int L = (int) ((y * newWidth) + xL);
            int R = (int) ((y * newWidth) + xR);
            int U = (int) ((yU * newWidth) + x);
            int D = (int) ((yD * newWidth) + x);

            if ((xL >= 0) && (xL < newHeight)) {
                adjMat.at<int>(index, L) = 1;
                E++;
                }

            if ((xR >= 0) && (xR < newHeight)) {
                adjMat.at<int>(index, R) = 1;
                E++;
                }

            if ((yU >= 0) && (yU < newHeight)) {
                adjMat.at<int>(index, U) = 1;
                E++;
                }

            if ((yD >= 0) && (yD < newHeight)) {
                adjMat.at<int>(index, D) = 1;
                E++;
                }
            }
        }

    //Calc average intensity
    Scalar MeanInt, StdInt;
    double meanInt, stdInt, varInt;
    meanStdDev(intensityMat, MeanInt, StdInt);
    meanInt = MeanInt[0];
    stdInt = StdInt[0];
    varInt = sqrt(stdInt);
    cout << "MEAN INTENSITY: " << meanInt << endl;

    //create and fill Edges, edgeList
    int sizeEdgeList = E + 1;
    int edgeIndex = E;
    int edgeList[sizeEdgeList];
    int edgeCap[sizeEdgeList];
    vector<int> sinkIndList;
    int edgeListIndexFront = 0;
    int edgeListIndexBack = sizeEdgeList - 1;

    int * foundTemp = new int[N + 2];
    fill_n(foundTemp, (N+2), -1);

    edgeList[edgeListIndexBack] = -1;
    edgeListIndexBack--;

    int FV[N+2];
    FV[0] = 0;
    FV[1] = nSource;
    FV[101] = sizeEdgeList - 1;
    foundTemp[0] = 1;
    foundTemp[sinkInd] = 1;

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
        edgeCap[i] = 100;
        edgeListIndexFront++;
        }

    int startPrev;
    int startCurrent;
    int numFound = 0;

    int * foundSink;

    vector<int>::iterator it;

ofstream adjMatOut("adjMat.txt");

    for (int y = (N-1); y >= 0; y--) {
        for (int x = (N-1); x >= 0; x--) {
            adjMatOut << adjMat.at<int>(y, x) << "  ";
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
                        foundTemp[startPrev] = 1;
                    }
                }

                //calc cost, step 1
                int X1 = y % newWidth;
                int Y1 = (y - X1) / newWidth;
                int X2 = x % newWidth;
                int Y2 = (x - X2) / newWidth;

                int I1 = intensityMat.at<int>(y, 0);
                int I2 = intensityMat.at<int>(x, 0);
                double expTerm = ( -pow((I1 - I2),2) / (2 * stdInt) );

                int CAPACITY = (int) (100 * exp(expTerm));

                it = find(sinkIndList.begin(), sinkIndList.end(), (y+1));

                if (it != sinkIndList.end()) {
                    edgeList[edgeListIndexBack] = 101;
                    edgeCap[edgeListIndexBack] = 100;
                    edgeListIndexBack--;
                    *it = -9999;
                    numFound++;
                    startPrev = startCurrent;
                    startCurrent = y+1;
                    if (startPrev != startCurrent) {
                        FV[startPrev] = edgeListIndexBack + 1;
                        foundTemp[startPrev] = 1;
                        }
                    }

                edgeList[edgeListIndexBack] = x + 1;
                edgeCap[edgeListIndexBack] = CAPACITY;
                edgeListIndexBack--;
                }
            }
            adjMatOut << endl;
        }

adjMatOut.close();

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

ofstream FirstVertex("FirstVertex.txt");
    for (int i = 0; i <= sinkInd; i++)
        {
            cout << i << ": " << FV[i] << endl;
            FirstVertex << i << ": " << FV[i] << endl;
        }
FirstVertex.close();

ofstream EdgeList("EdgeList.txt");
        for (int i = 0; i < sizeEdgeList; i++)
        {
            cout << i << ": " << edgeList[i] << ", cap: " << edgeCap[i] << endl;
            EdgeList << i << ": " << edgeList[i] << ", cap: " << edgeCap[i] << endl;
        }
EdgeList.close();

vector<int> flow;
vector<int> Cap;
vector<int> Space;

for (int i = 0; i < sizeEdgeList; i++)
    {
        Cap.push_back(edgeCap[i]);
        Space.push_back(edgeCap[i]);
        flow.push_back(0);
    }

vector<int> tempFlow;
vector<int> tempCap;
vector<int> tempSpace;
vector<vector<int>> pathInfo;
vector<int> edgesOut;
int minSpace;
int * parentOut;

///////////////////while possible,
while (true) {
        cout << "debug 1" << endl;
parentOut = BFS(FV, V, edgeList, E, Space);
if (success == false) {
    break;
}

pathInfo = backtrace(sinkInd, parentOut);
edgesOut = edgePath(pathInfo[0], FV, edgeList);

for(int i=0; i < edgesOut.size(); i++) {
    cout << "Edge: " << edgesOut[i] << ", ";
    tempFlow.push_back(flow[edgesOut[i]]);
    cout << "Flow: " << flow[edgesOut[i]] << ", ";
    tempCap.push_back(Cap[edgesOut[i]]);
    cout << "Cap: " << Cap[edgesOut[i]] << ", ";
    tempSpace.push_back(Space[edgesOut[i]]);
    cout << "Space: " << Space[edgesOut[i]] << endl << endl;
    }

minSpace =*min_element(tempSpace.begin(), tempSpace.end());

for(int i=0; i < edgesOut.size(); i++) {
    cout << "Update -- Edge: " << edgesOut[i] << ", ";
    flow[edgesOut[i]] = flow[edgesOut[i]] + minSpace;
    cout << "Flow: " << flow[edgesOut[i]] << ", ";
    cout << "Cap: " << Cap[edgesOut[i]] << ", ";
    Space[edgesOut[i]] = Space[edgesOut[i]] - minSpace;
    cout << "Space: " << Space[edgesOut[i]] << endl << endl;
    }

tempFlow.clear();
tempCap.clear();
tempSpace.clear();
pathInfo.clear();
edgesOut.clear();
}
/////////////////////

cout << "debug 2" << endl;

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
        bgSet = true;
        }

    if  ( event == EVENT_RBUTTONDOWN ) {
        cout << "Right button of the mouse is clicked - position (" << x << ", " << y << ")" << endl;
        sizeFG++;
        bgfgNull[1].resize(sizeFG);

        (bgfgNull[1][sizeFG - 1]).x = x;
        (bgfgNull[1][sizeFG - 1]).y = y;

        cout << "Foreground pixel: (" << (bgfgNull[1][sizeFG - 1]).x << ", " << (bgfgNull[1][sizeFG - 1]).y << ")" << endl;
        fgSet = true;
        }

    if (  event == EVENT_MBUTTONDOWN  ) {
        flag = true;
        cout << "Flag: " << flag << endl;
        }

     return;
    }

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

int * BFS(int * FirstVertex, int V, int * EdgeList, int E, vector<int>& space) {

    success = false;

    Queue * Q = createQueue(V);
    int source = 0;

    int * Found = new int[V];
    fill_n(Found, V, -1);

    Enqueue(Q, source); //push into quene and set found.
    Found[source] = 1;

    int node;
    static int CountDequeue = 0;

    int * parent = new int[V];
    fill_n(parent, V, -1);

    while((Q->size > 0) && (node != 101)) {
        node = Dequeue(Q);
        if (node == 101) {
            success = true;
            break;
        }
        CountDequeue++;

        //Put children in Q
        //cout << "FV from " << FirstVertex[node] << " to " << FirstVertex[node + 1] << endl;
        for(int i = FirstVertex[node]; i < FirstVertex[node +1]; i++)
            {
            if((Found[EdgeList[i]] == -1) && (space[i] > 0))
                {
                //cout << " Found new, " << EdgeList[i] << " at " << i << endl;
                Enqueue(Q,EdgeList[i]);
                Found[EdgeList[i]] = 1;
                parent[EdgeList[i]] = node;
                }
            }
    }
    return parent;
}


Queue * createQueue(int maxElements)
{

        /* Create a Queue */
        Queue *Q;
        Q = (Queue*)malloc(sizeof(Queue));

        /* Initialise its properties */
        Q->elements = (int *)malloc(sizeof(int)*maxElements);
        Q->size = 0;
        Q->capacity = maxElements;
        Q->front = -1;
        Q->rear = 0;
        return Q;
}

void printQ(Queue * Q)
{
    cout << "Queue:  ";
    for (int i = 0; i < Q->size; i++)
    {
        cout << Q->elements[i] << "  ";
    }
    cout << endl;
}

void Enqueue(Queue *Q,int element)
{
        /* If the Queue is full, we cannot push an element into it as there is no space for it.*/
        if(Q->size == Q->capacity)
        {
                printf("Queue is Full\n");
        }
        else
        {
                for (int i = Q->size; i > 0; i--) {
                    Q->elements[i] = Q->elements[i - 1];
                }
                Q->size++;
                Q->front = Q->front + 1;
                Q->elements[Q->rear] = element;
        }
        return;
}

int  Dequeue(Queue *Q)
{
    int  tempElem = -1;
    int element;

    /* If Queue size is zero then it is empty. So we cannot pop */
    if(Q->size==0) {
        cout << "Queue is Empty" << endl;
        return tempElem;
        }
    else {
        element = Q->elements[Q->front];
        Q->size--;
        Q->front--;
        return element;
    }
}

vector<vector<int>> backtrace(int node, int * parent) {
    vector<vector<int>> pL;
    vector<int> path;
    vector<int> pathLength;

	int iter2 = 1;
	int tempNode = node;

	cout << "bt debug 2" << endl;

	while(parent[tempNode] != 0) {
		tempNode = parent[tempNode];
		iter2++;
		}

    pathLength.push_back(iter2 + 1);

	int newNode = node;
	path.push_back(newNode);

	while(true) {
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




