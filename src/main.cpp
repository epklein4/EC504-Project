#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>

using namespace std;
using namespace cv;

void CallBackFuncA(int event, int x, int y, int flags, void* userdata);
void CallBackFuncB(int event, int x, int y, int flags, void* userdata);

Point fg, bg;

int main()
{
    Mat srcIm = imread("resources/testimage.jpg", IMREAD_GRAYSCALE);// read in image

    if(!img.data)
    {
        std::cout << "Image not found" << std::endl;
        return -1;
    }

    //read image resolution
    int width = srcIm.size().width;
    int height = srcIm.size().height;

    cout << "Width : " << width << endl;
    cout << "Height: " << height << endl;

    //This would print all pixel values
    /*for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                cout << static_cast<int>(srcIm.at<uchar>(i,j)) << "  ";
                }
            cout << endl << endl;
        }
    */

    namedWindow( "Display horsey" );// create (empty) window to display image

    cout << "Select background: " << endl;
    setMouseCallback("Display horsey", CallBackFuncA, &bg);

    //cout << "Select foreground: " << endl;
    //setMouseCallback("Display horsey", CallBackFuncB, &fg);

    imshow( "Display horsey", srcIm );// show image
    waitKey(0);// wait for a keystroke in the window
    return 0;
}


//*/

void CallBackFuncA(int event, int x, int y, int flags, void* userdata)
{
    Point*bg = (Point*)userdata;
     if  ( event == EVENT_LBUTTONDOWN )
     {
          cout << "Left button of the mouse is clicked - position (" << x << ", " << y << ")" << endl;
          bg->x = x;
          bg->y = y;
          cout << "Background pixel: (" << bg->x << ", " << bg->y << ")" << endl;
          cout << "Select foreground: " << endl;
          setMouseCallback("Display horsey", CallBackFuncB, &fg);
     }

     return;
}

void CallBackFuncB(int event, int x, int y, int flags, void* userdata)
{
    Point*fg = (Point*)userdata;
     if  ( event == EVENT_LBUTTONDOWN )
     {
          cout << "Left button of the mouse is clicked - position (" << x << ", " << y << ")" << endl;
          fg->x = x;
          fg->y = y;
          cout << "Foreground pixel: (" << fg->x << ", " << fg->y << ")" << endl;
     }

     return;
}

//template CallBackFunc
/*void CallBackFuncB(int event, int x, int y, int flags, void* userdata)
{
    Point*bg = (Point*)userdata;
     if  ( event == EVENT_LBUTTONDOWN )
     {
          cout << "Left button of the mouse is clicked - position (" << x << ", " << y << ")" << endl;
          bg->x = x;
          bg->y = y;
     }
     else if  ( event == EVENT_RBUTTONDOWN )
     {
          //cout << "Right button of the mouse is clicked - position (" << x << ", " << y << ")" << endl;
          cout << *bg << endl;
     }
     else if  ( event == EVENT_MBUTTONDOWN )
     {
          cout << "Middle button of the mouse is clicked - position (" << x << ", " << y << ")" << endl;
     }
     else if ( event == EVENT_MOUSEMOVE )
     {
          cout << "Mouse move over the window - position (" << x << ", " << y << ")" << endl;

     }


     return;
}
*/
