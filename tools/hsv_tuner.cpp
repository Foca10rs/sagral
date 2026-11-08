// HSV 调参工具（支持逐帧切换）
// 编译：g++ hsv_tuner.cpp -o hsv_tuner $(pkg-config --cflags --libs opencv4)
// 用法：./hsv_tuner [视频/图片路径] [起始帧]
// 按键：a / ← 上一帧    d / → 下一帧    s 保存当前掩码    ESC / q 退出
#include <opencv2/opencv.hpp>
#include <iostream>
using namespace std;
using namespace cv;

Mat g_src, g_hsv, g_mask, g_result;
VideoCapture g_cap;
bool   g_isVideo = false;
int    g_frame = 0, g_total = 0;

int lowH = 0,   highH = 179;
int lowS = 0,   highS = 255;
int lowV = 0,   highV = 255;

void on_track(int, void*)
{
    if (g_src.empty()) return;

    int lH = min(lowH, highH), hH = max(lowH, highH);
    int lS = min(lowS, highS), hS = max(lowS, highS);
    int lV = min(lowV, highV), hV = max(lowV, highV);

    inRange(g_hsv, Scalar(lH, lS, lV), Scalar(hH, hS, hV), g_mask);

    g_result = Mat::zeros(g_src.size(), g_src.type());
    g_src.copyTo(g_result, g_mask);

    char buf[160];
    snprintf(buf, sizeof(buf), "H:%d-%d S:%d-%d V:%d-%d  frame:%d/%d",
             lH, hH, lS, hS, lV, hV, g_frame, g_total);

    Mat showMask;
    cvtColor(g_mask, showMask, COLOR_GRAY2BGR);
    putText(showMask, buf, Point(6, 24), FONT_HERSHEY_SIMPLEX, 0.55, Scalar(0, 0, 255), 2);
    putText(g_result, buf, Point(6, 24), FONT_HERSHEY_SIMPLEX, 0.55, Scalar(0, 255, 0), 2);

    imshow("Image_binary", showMask);
    imshow("result", g_result);
    imshow("Image_raw", g_src);
}

// 跳到指定帧并重新处理
bool loadFrame(int idx)
{
    if (!g_isVideo) return false;
    if (idx < 0) idx = 0;
    if (idx >= g_total) idx = g_total - 1;

    g_cap.set(CAP_PROP_POS_FRAMES, idx);
    Mat f;
    g_cap >> f;
    if (f.empty()) return false;

    g_frame = idx;
    g_src = f;
    if (g_src.rows > 800 || g_src.cols > 1000) {
        double s = min(800.0 / g_src.rows, 1000.0 / g_src.cols);
        resize(g_src, g_src, Size(), s, s, INTER_AREA);
    }
    cvtColor(g_src, g_hsv, COLOR_BGR2HSV);
    on_track(0, 0);
    return true;
}

int main(int argc, char** argv)
{
    string path = (argc > 1) ? argv[1]
                : "/home/sagral/C++/第二次小作业/resource/1763c611fdc7753801aebabb52c1f8c5.mp4";
    int startFrame = (argc > 2) ? atoi(argv[2]) : 0;

    bool isVideo = path.size() > 4 &&
        (path.substr(path.size()-4) == ".mp4" || path.substr(path.size()-4) == ".avi");

    if (isVideo) {
        g_cap.open(path);
        if (!g_cap.isOpened()) { cout << "打不开视频: " << path << endl; return -1; }
        g_isVideo = true;
        g_total = (int)g_cap.get(CAP_PROP_FRAME_COUNT);
        printf("视频共 %d 帧\n", g_total);
        if (!loadFrame(startFrame)) { cout << "读不到帧" << endl; return -1; }
    } else {
        g_src = imread(path);
        if (g_src.empty()) { cout << "读不到图片: " << path << endl; return -1; }
        if (g_src.rows > 800 || g_src.cols > 1000) {
            double s = min(800.0 / g_src.rows, 1000.0 / g_src.cols);
            resize(g_src, g_src, Size(), s, s, INTER_AREA);
        }
        cvtColor(g_src, g_hsv, COLOR_BGR2HSV);
    }

    namedWindow("result", WINDOW_NORMAL);
    namedWindow("Image_binary", WINDOW_NORMAL);
    namedWindow("Image_raw", WINDOW_NORMAL);

    createTrackbar("Low  H", "result", &lowH,  179, on_track);
    createTrackbar("High H", "result", &highH, 179, on_track);
    createTrackbar("Low  S", "result", &lowS,  255, on_track);
    createTrackbar("High S", "result", &highS, 255, on_track);
    createTrackbar("Low  V", "result", &lowV,  255, on_track);
    createTrackbar("High V", "result", &highV, 255, on_track);

    on_track(0, 0);

    cout << "a/左: 上一帧   d/右: 下一帧   s: 保存掩码   ESC/q: 退出" << endl;

    while (true) {
        int k = waitKey(20) & 0xFF;
        if (k == 27 || k == 'q') break;
        else if (k == 'a' || k == 81) loadFrame(g_frame - 1);        // 左/上
        else if (k == 'd' || k == 83) loadFrame(g_frame + 1);        // 右/下
        else if (k == 's') {
            string fn = "mask_" + to_string(g_frame) + ".png";
            imwrite(fn, g_mask);
            cout << "已保存 " << fn << endl;
        }
    }
    return 0;
}
