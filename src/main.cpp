#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <queue>

using namespace std;
using namespace cv;

void CallBackFunc(int event, int x, int y, int flags, void *userdata);
void printMat(Mat *myMat);
void printMatDouble(Mat *myMat);

int maxFlow(int V, int source, int sink, Mat adjMat);
bool BFS(int source, int sink, int V);

vector<vector<int>> residual;
vector<int> parent;

vector<vector<Point>> bgfgNull(2);
bool bgSet, fgSet, flag;
int sizeBG, sizeFG;

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

     Mat srcIm = imread("resources/test1.jpg", IMREAD_GRAYSCALE); // read in image

     if (!srcIm.data)
     {
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
     namedWindow("Display pic"); // create (empty) window to display image

     cout << "Left/Right click to add background/foreground pixels. " << endl;
     cout << "Press mouse wheel when finished." << endl;
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

     /////////////////////////////////////////////////////

     //number of vertices (pixels)
     int N = newHeight * newWidth;
     int V = N + 2;

     //empty N by N adjacency matrix (4-connected)
     Mat adjMat;
     adjMat = Mat::zeros(N, N, CV_32SC1);

     Mat intensityMat;
     intensityMat = Mat::zeros(N, 1, CV_32SC1);

     int E = 0;

     for (int y = 0; y < newHeight; y++)
     {
          for (int x = 0; x < newWidth; x++)
          {
               int index;
               index = (int)((y * newWidth) + x);

               int pixelIntensity = (int)resizedIm.at<uchar>(y, x);
               intensityMat.at<int>(index, 0) = pixelIntensity;

               int xL = x - 1;
               int xR = x + 1;
               int yU = y - 1;
               int yD = y + 1;

               int L = (int)((y * newWidth) + xL);
               int R = (int)((y * newWidth) + xR);
               int U = (int)((yU * newWidth) + x);
               int D = (int)((yD * newWidth) + x);

               if ((xL > 0) && (xL < newHeight))
               {
                    adjMat.at<int>(index, L) = 1;
                    E++;
               }

               if ((xR > 0) && (xR < newHeight))
               {
                    adjMat.at<int>(index, R) = 1;
                    E++;
               }

               if ((yU > 0) && (yU < newHeight))
               {
                    adjMat.at<int>(index, U) = 1;
                    E++;
               }

               if ((yD > 0) && (yD < newHeight))
               {
                    adjMat.at<int>(index, D) = 1;
                    E++;
               }
          }
     }

     Scalar MeanInt, StdInt;
     double meanInt, stdInt, varInt;
     meanStdDev(intensityMat, MeanInt, StdInt);
     meanInt = MeanInt[0];
     stdInt = StdInt[0];
     varInt = sqrt(stdInt);

     cout << "MEAN INTENSITY: " << meanInt << endl;

     E = 2 * E;

     Mat Edges;
     Edges = Mat::zeros(E, 4, CV_32SC1);

     int edgeIndex = 0;

     for (int y = 0; y < N; y++)
     {
          for (int x = 0; x < N; x++)
          {
               if (adjMat.at<int>(y, x) == 1)
               {

                    //calc cost, step 1
                    int X1 = y % newWidth;
                    int Y1 = (y - X1) / newWidth;
                    int X2 = x % newWidth;
                    int Y2 = (x - X2) / newWidth;

                    int I1 = intensityMat.at<int>(y, 0);
                    int I2 = intensityMat.at<int>(x, 0);
                    double expTerm = (-pow((I1 - I2), 2) / (2 * stdInt));

                    int CAPACITY = (int)(100 * exp(expTerm));

                    for (int k = 0; k < 2; k++)
                    {
                         Edges.at<int>(edgeIndex, 0) = edgeIndex;

                         if (k == 0)
                         {
                              Edges.at<int>(edgeIndex, 1) = y; //startnode
                              Edges.at<int>(edgeIndex, 2) = x; //endnode
                         }
                         else
                         {
                              Edges.at<int>(edgeIndex, 1) = x; //startnode
                              Edges.at<int>(edgeIndex, 2) = y; //endnode
                         }

                         Edges.at<int>(edgeIndex, 3) = CAPACITY;
                         edgeIndex++;
                    }
               }
          }
     }

     //add source/sink edge info
     Mat altEdges;
     altEdges = Mat::zeros((sizeBG + sizeFG), 4, CV_32SC1);

     Mat newEdges;
     vconcat(Edges, altEdges, newEdges);

     //add 2 rows and 2 columns to adj mat for source/sink
     Mat newRows = Mat::zeros(2, N, CV_32SC1);
     Mat newCols = Mat::zeros(V, 2, CV_32SC1);

     Mat adjMatTemp, newAdj;

     vconcat(adjMat, newRows, adjMatTemp);
     hconcat(adjMatTemp, newCols, newAdj);

     int nSource = sizeBG;
     int nSink = sizeFG;

     int sourceInd = N;
     int sinkInd = N + 1;

     cout << "nSource = " << nSource << endl;
     cout << "nSink = " << nSink << endl;
     cout << "sourceInd = " << sourceInd << endl;
     cout << "sinkInd = " << sinkInd << endl;

     int edg = E;

     for (int i = 0; i < nSource; i++)
     {
          int x = (bgfgNull[0][i]).x;
          int y = (bgfgNull[0][i]).y;

          int pixInd = (int)((y * newWidth) + x);
          newAdj.at<int>(sourceInd, pixInd) = 1;

          newEdges.at<int>(edg, 0) = edg;
          newEdges.at<int>(edg, 1) = sourceInd; //startnode
          newEdges.at<int>(edg, 2) = pixInd;    //endnode
          newEdges.at<double>(edg, 3) = 1;

          edg++;
     }

     for (int i = 0; i < nSink; i++)
     {
          int x = (bgfgNull[1][i]).x;
          int y = (bgfgNull[1][i]).y;

          int pixInd = (int)((y * newWidth) + x);
          newAdj.at<int>(sinkInd, pixInd) = 1;

          newEdges.at<int>(edg, 0) = edg;
          newEdges.at<int>(edg, 1) = pixInd;  //startnode
          newEdges.at<int>(edg, 2) = sinkInd; //endnode
          newEdges.at<double>(edg, 3) = 1;

          edg++;
     }

     E = E + nSource + nSink;

     printMat(&newAdj);
     printMat(&newEdges);

     ofstream myFile("metadata.txt");
     myFile << "G(" << V << "," << E << ")" << endl;
     myFile << "Number of source nodes: " << nSource << endl;
     myFile << "Source node indices (y,x): ";

     for (int i = 0; i < nSource; i++)
     {
          int x = (bgfgNull[0][i]).x;
          int y = (bgfgNull[0][i]).y;
          myFile << "(" << y << ", " << x << ")  ";
     }

     myFile << endl
            << "Number of sink nodes: " << nSink << endl;
     myFile << "Sink node indices (y,x): ";

     for (int i = 0; i < nSource; i++)
     {
          int x = (bgfgNull[1][i]).x;
          int y = (bgfgNull[1][i]).y;
          myFile << "(" << y << ", " << x << ")  ";
     }

     myFile << endl
            << "Original resolution: " << height << "x" << width << endl;
     myFile << "Squared resolution: " << newHeight << "x" << newWidth << endl;

     myFile.close();

     ofstream adjMatOut("adjMat.txt");

     int tempY = newAdj.size().height;
     int tempX = newAdj.size().width;

     for (int y = 0; y < tempY; y++)
     {
          for (int x = 0; x < tempX; x++)
          {
               adjMatOut << newAdj.at<int>(y, x) << "  ";
          }
          adjMatOut << endl;
     }

     adjMatOut.close();

     ofstream edgesOut("Edges.txt");

     tempY = newEdges.size().height;
     tempX = newEdges.size().width;

     edgesOut << left << setw(13) << "edgeNumber"
              << left << setw(13) << "startNode"
              << left << setw(13) << "endNode"
              << left << setw(13) << "Capacity" << endl;

     for (int y = 0; y < tempY; y++)
     {
          for (int x = 0; x < tempX; x++)
          {
               edgesOut << left << setw(11) << newEdges.at<int>(y, x) << "  ";
          }
          edgesOut << endl;
     }

     edgesOut.close();

     //maxFlow(N, sourceInd, sinkInd, adjMat);

     return 0;
}

// Compute max flow algorithm
int maxFlow(int V, int source, int sink, Mat adjMat)
{
     int max = 0;
     int u, v;

     residual.reserve(adjMat.rows);
     for(int i = 0; i < adjMat.rows; i++)
     {
          residual[i].reserve(adjMat.cols);
          for(int j = 0; j < adjMat.cols; j++)
               residual[i].emplace_back(adjMat.at<int>(i,j));
     }
cout << "HERE\n";

     parent.reserve(V);

     while(BFS(source, sink, V))
     {
          int flow = INT_MAX;
          for(int v = sink; v != source; v = parent[v])
          {
               u = parent[v];
               residual[u][v] -= flow;
               residual[v][u] += flow;
          }

          max += flow;
     }

     cout << "Max Flow: " << max << endl;
     return max;
}

bool BFS(int source, int sink, int V)
{
     vector<bool> visited;
     visited.reserve(V);
     std::generate(visited.begin(), visited.end(), [](){ return 0; });

     queue<int> q;
     q.push(source);
     visited[source] = true;
     parent[source] = -1;

     while(!q.empty())
     {
          int u = q.front();
          q.pop();

          for(int v = 0; v < V; v++)
          {
               if(v == sink)
               {
                    parent[v] = u;
                    return true;
               }
               
               q.push(v);
               parent[v] = u;
               visited[v] = true;
          }
     }
     return false;
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
          bgSet = true;
     }

     if (event == EVENT_RBUTTONDOWN)
     {
          cout << "Right button of the mouse is clicked - position (" << x << ", " << y << ")" << endl;
          sizeFG++;
          bgfgNull[1].resize(sizeFG);

          (bgfgNull[1][sizeFG - 1]).x = x;
          (bgfgNull[1][sizeFG - 1]).y = y;

          cout << "Foreground pixel: (" << (bgfgNull[1][sizeFG - 1]).x << ", " << (bgfgNull[1][sizeFG - 1]).y << ")" << endl;
          fgSet = true;
     }

     if (event == EVENT_MBUTTONDOWN)
     {
          flag = true;
          cout << "Flag: " << flag << endl;
     }

     return;
}

void printMat(Mat *myMat)
{
     int Y = myMat->size().height;
     int X = myMat->size().width;

     for (int y = 0; y < Y; y++)
     {
          for (int x = 0; x < X; x++)
          {
               cout << myMat->at<int>(y, x) << "  ";
          }
          cout << endl
               << "--NEW ROW--" << endl;
          ;
     }

     cout << endl
          << "--FINISHED PRINTING--" << endl;

     return;
}

void printMatDouble(Mat *myMat)
{
     int Y = myMat->size().height;
     int X = myMat->size().width;

     for (int y = 0; y < Y; y++)
     {
          for (int x = 0; x < X; x++)
          {
               cout << myMat->at<double>(y, x) << "  ";
          }
          cout << endl
               << "--NEW ROW--" << endl;
          ;
     }

     cout << endl
          << "--FINISHED PRINTING--" << endl;

     return;
}
