#include "opencv.hpp"

class Histogram1D{  
  
private:  
    int histSize[1];    //项的数量  
    float hranges[2];   //像素的最小及最大值  
    const float *ranges[1];  
    int channels[1];    //仅用到一个通道  
public:  
    Histogram1D(){  
        //准备1D直方图的参数  
        histSize[0] = 256;  
        hranges[0] = 0.0;  
        hranges[1] = 255.0;  
        ranges[0] = hranges;  
        channels[0] = 0;    //默认情况下，我们考察0号通道  
    }  
  
    cv::Mat getHistogram(const cv::Mat &image);  
    cv::Mat getHistogramImage(const cv::Mat &image);  
};
