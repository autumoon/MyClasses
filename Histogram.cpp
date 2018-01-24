#include "Histogram.h"

//����1Dֱ��ͼ  
cv::Mat Histogram1D::getHistogram(const cv::Mat &image){  
      
    cv::MatND hist;  
    //����ֱ��ͼ  ���������������͵Ķ�ͨ��ͼ��  
    cv::calcHist(&image,   
        1,  //���㵥��ͼ���ֱ��ͼ   
        channels,   //ͨ������  
        cv::Mat(),  //��ʹ��ͼ����Ϊ����  
        hist,   //���ص�ֱ��ͼ  
        1,  //����1D��ֱ��ͼ  
        histSize,   //�������  
        ranges  //����ֵ�ķ�Χ  
        );  
    return hist;  
}  
  
//����1Dֱ��ͼ��������һ��ͼ��  
cv::Mat Histogram1D::getHistogramImage(const cv::Mat &image){  
  
    //���ȼ���ֱ��ͼ  
    cv::MatND hist = getHistogram(image);  
    //��ȡ���ֵ����Сֵ  
    double maxVal = 0;  
    double minVal = 0;  
    cv::minMaxLoc(hist, &minVal, &maxVal, 0, 0);  
    //��ʾֱ��ͼ��ͼ��  
    cv::Mat histImg(histSize[0], histSize[0], CV_8U, cv::Scalar(255));  
    //������ߵ�Ϊnbins��90%  
    int hpt = static_cast<int>(0.9*histSize[0]);  
    //ÿ����Ŀ������һ����ֱ��  
    for (int h = 0; h < histSize[0]; h++)  
    {  
        float binVal = hist.at<float>(h);  
        int intensity = static_cast<int>(binVal*hpt / maxVal);  
        //����֮�����һ����  
        cv::line(histImg, cv::Point(h, histSize[0]), cv::Point(h, histSize[0] - intensity), cv::Scalar::all(0));  
    }  
    return histImg;  
}  
  
int ShowHist(){  
  
    //1.װ��ͼ��  
    cv::Mat srcImage = cv::imread("grayHorse.jpg");  
    if (!srcImage.data) return 0;  
    //2.histogram����  
    Histogram1D h;  
    /* 
    //3.����ֱ��ͼ 
    cv::MatND histo = h.getHistogram(srcImage); //�����histo������һ��ӵ��256����Ŀ��һά���� 
    //4.����ÿ����Ŀ 
    for (int  i = 0; i < 256; i++) 
    { 
        cout << "Value" << i << "=" << histo.at<float>(i) << endl; 
    } 
    */  
    //5.��ͼ�η�ʽ��ʾֱ��ͼ  
    //cv::namedWindow("Histogram");  
    //cv::imshow("Histogram", h.getHistogramImage(srcImage));  
      
    //6.ʹ��һ����ֵ��������ֵͼ����뱳����ǰ��  
    cv::Mat thresholded;  
    cv::threshold(srcImage, thresholded, 60, 255, cv::THRESH_BINARY);  
    cv::namedWindow("thresholded");  
    cv::imshow("thresholded", thresholded);  
    cv::waitKey();  
    system("pause");  
    return 0;  
}  