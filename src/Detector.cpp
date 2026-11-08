#include "Detector.hpp"
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/aruco.hpp>
using namespace std;
using namespace cv;
using namespace cv::aruco;
void Detector::detect(VideoCapture & cap)
{
     Mat img;
     Mat imgHSV;
     Mat mask_orange,mask_yellow,mask_red,mask_white,mask_blue,mask,mask_show;
     int mask_area;
     Mat kernel = getStructuringElement(MORPH_RECT, Size(3,3));
     while(true)
     {
         cap.read(img);
         if (img.empty())
         {
             break;
         }
         imshow("Image_raw",img);

         cvtColor(img,imgHSV,COLOR_BGR2HSV);
         
         inRange(imgHSV,Scalar(12,183,183),Scalar(16,250,209),mask_orange); //橙色面
         Moments m_orange = moments(mask_orange,true);
         int orange_area = m_orange.m00;
         
         inRange(imgHSV,Scalar(18,142,50),Scalar(99,255,233),mask_yellow); //黄色面
         Moments m_yellow = moments(mask_yellow,true);
         int yellow_area = m_yellow.m00;
         
         inRange(imgHSV,Scalar(23,141,80),Scalar(179,255,233),mask_red); //红色面
         Moments m_red = moments(mask_red,true);
         int red_area = m_red.m00;
         
         inRange(imgHSV,Scalar(20,31,216),Scalar(109,45,255),mask_white); //白色面
         Moments m_white = moments(mask_white,true);
         int white_area = m_white.m00;
         
         inRange(imgHSV,Scalar(81,104,56),Scalar(113,255,255),mask_blue); //蓝色面
         Moments m_blue = moments(mask_blue,true);
         int blue_area = m_blue.m00;
         
         mask = (orange_area > yellow_area) ? mask_orange : mask_yellow; //哪个面积大赋值给mask_area
         mask_area = (orange_area > yellow_area) ? orange_area : yellow_area;
         
         mask = (mask_area > red_area) ? mask : mask_red;
         mask_area = (mask_area > red_area) ? mask_area : red_area;
         
         mask = (mask_area > white_area) ? mask : mask_white;
         mask_area = (mask_area > white_area) ? mask_area : white_area;
         
         mask = (mask_area > blue_area) ? mask : mask_blue;
         mask_area = (mask_area > blue_area) ? mask_area : blue_area;

         if(mask_area == orange_area)               //通过面积判断现在是哪个面
         {
             inRange(imgHSV,Scalar(27,0,5),Scalar(139,255,76),mask_show);
         }
         
         if(mask_area == yellow_area)               //前一个面像素数少于一千时认定已翻过去，采用该面和下一面均符合条件的二值化范围
         {                                          //否则采用上个面与该面均符合条件的二值化区间
             if(orange_area <= 1000)
             {
                 inRange(imgHSV,Scalar(0,0,0),Scalar(179,69,39),mask_show);
                 morphologyEx(mask_show,mask_show,MORPH_GRADIENT,kernel);
             }
             inRange(imgHSV,Scalar(27,0,5),Scalar(139,255,76),mask_show);
         }
         
         if(mask_area == red_area)
         {
             if(yellow_area <= 1000)
             {
                 inRange(imgHSV,Scalar(0,0,0),Scalar(179,255,43),mask_show);
                 //morphologyEx(mask_show,mask_show,MORPH_GRADIENT,kernel);
                 dilate(mask_show,mask_show,kernel);
             }
             inRange(imgHSV,Scalar(0,0,0),Scalar(179,69,39),mask_show);
         }
         
         if(mask_area == white_area)
         {
             if(red_area <= 1000)
             {
                 inRange(imgHSV,Scalar(0,0,0),Scalar(179,255,29),mask_show);
                 //morphologyEx(mask_show,mask_show,MORPH_GRADIENT,kernel);
                 dilate(mask_show,mask_show,kernel);
             }
             inRange(imgHSV,Scalar(0,0,0),Scalar(179,255,43),mask_show);
         }
         
         if(mask_area == blue_area)
         {
             if(white_area <= 1000)
             {
                 inRange(imgHSV,Scalar(81,104,56),Scalar(113,255,255),mask_show);
                 morphologyEx(mask_show,mask_show,MORPH_CLOSE,kernel);
                 //dilate(mask_show,mask_show,kernel);
             }
             inRange(imgHSV,Scalar(0,0,0),Scalar(179,255,29),mask_show);
         }
         
         imshow("Image_binary",mask_show);

         waitKey(33);
     }
}
void Detector::detect(Mat &img)
{
     if (img.empty()) return;
     vector<vector<Point2f>> corners;
     vector<int> ids;
     Ptr<Dictionary> dict = getPredefinedDictionary(DICT_APRILTAG_36h11);
     detectMarkers(img, dict, corners, ids);
     if (!ids.empty())
     {
         drawDetectedMarkers(img, corners, ids);
     }
    imshow("result", img);
}