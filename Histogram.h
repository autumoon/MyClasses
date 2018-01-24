#include "opencv.hpp"

class Histogram1D{  
  
private:  
    int histSize[1];    //�������  
    float hranges[2];   //���ص���С�����ֵ  
    const float *ranges[1];  
    int channels[1];    //���õ�һ��ͨ��  
public:  
    Histogram1D(){  
        //׼��1Dֱ��ͼ�Ĳ���  
        histSize[0] = 256;  
        hranges[0] = 0.0;  
        hranges[1] = 255.0;  
        ranges[0] = hranges;  
        channels[0] = 0;    //Ĭ������£����ǿ���0��ͨ��  
    }  
  
    cv::Mat getHistogram(const cv::Mat &image);  
    cv::Mat getHistogramImage(const cv::Mat &image);  
};
