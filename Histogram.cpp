#include "Histogram.h"

//计算1D直方图  
cv::Mat Histogram1D::getHistogram(const cv::Mat &image){  
      
    cv::MatND hist;  
    //计算直方图  计算任意像素类型的多通道图像  
    cv::calcHist(&image,   
        1,  //计算单张图像的直方图   
        channels,   //通道数量  
        cv::Mat(),  //不使用图像作为掩码  
        hist,   //返回的直方图  
        1,  //这是1D的直方图  
        histSize,   //项的数量  
        ranges  //像素值的范围  
        );  
    return hist;  
}  
  
//计算1D直方图，并返回一幅图像  
cv::Mat Histogram1D::getHistogramImage(const cv::Mat &image){  
  
    //首先计算直方图  
    cv::MatND hist = getHistogram(image);  
    //获取最大值和最小值  
    double maxVal = 0;  
    double minVal = 0;  
    cv::minMaxLoc(hist, &minVal, &maxVal, 0, 0);  
    //显示直方图的图像  
    cv::Mat histImg(histSize[0], histSize[0], CV_8U, cv::Scalar(255));  
    //设置最高点为nbins的90%  
    int hpt = static_cast<int>(0.9*histSize[0]);  
    //每个条目都绘制一条垂直线  
    for (int h = 0; h < histSize[0]; h++)  
    {  
        float binVal = hist.at<float>(h);  
        int intensity = static_cast<int>(binVal*hpt / maxVal);  
        //两点之间绘制一条线  
        cv::line(histImg, cv::Point(h, histSize[0]), cv::Point(h, histSize[0] - intensity), cv::Scalar::all(0));  
    }  
    return histImg;  
}  
  
int ShowHist(){  
  
    //1.装载图像  
    cv::Mat srcImage = cv::imread("grayHorse.jpg");  
    if (!srcImage.data) return 0;  
    //2.histogram对象  
    Histogram1D h;  
    /* 
    //3.计算直方图 
    cv::MatND histo = h.getHistogram(srcImage); //这里的histo对象是一个拥有256个条目的一维数组 
    //4.遍历每个条目 
    for (int  i = 0; i < 256; i++) 
    { 
        cout << "Value" << i << "=" << histo.at<float>(i) << endl; 
    } 
    */  
    //5.以图形方式显示直方图  
    //cv::namedWindow("Histogram");  
    //cv::imshow("Histogram", h.getHistogramImage(srcImage));  
      
    //6.使用一个阈值来创建二值图像分离背景和前景  
    cv::Mat thresholded;  
    cv::threshold(srcImage, thresholded, 60, 255, cv::THRESH_BINARY);  
    cv::namedWindow("thresholded");  
    cv::imshow("thresholded", thresholded);  
    cv::waitKey();  
    system("pause");  
    return 0;  
}  