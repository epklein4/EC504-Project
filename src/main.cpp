#include <opencv2/opencv.hpp>
#include <iostream>

int main()
{
    cv::Mat img = cv::imread("resources/testimage.jpg", cv::IMREAD_COLOR);

    if(!img.data)
    {
        std::cout << "Image not found" << std::endl;
        return -1;
    }

    cv::namedWindow("testimage", cv::WINDOW_AUTOSIZE);
    cv::imshow("testimage", img);
    cv::waitKey(0);

    return 0;
}