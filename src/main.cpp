#include "Detector.hpp"
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/aruco.hpp>
using namespace std;
using namespace cv;
int main()
{
      Detector d;
      VideoCapture cap("resource/1763c611fdc7753801aebabb52c1f8c5.mp4");
      if (!cap.isOpened()) 
      { 
           cout << "视频打不开" << endl;
           return -1; 
      }
      VideoCapture cam(0);
      if (!cam.isOpened())
      {
           cout << "摄像头打开失败" << endl;
           return -1;
      }
      Mat src;
      Mat mask;
      while (cam.read(src))
      {
           if (src.empty())
           {
                break;
           }
           d.detect(src);
           if (waitKey(1) == 27)
           {
                break;
           }
      }
      destroyAllWindows();
      d.detect(cap);
      return 0;
}