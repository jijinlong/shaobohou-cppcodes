// OpenCV_Helloworld.cpp : Defines the entry point for the console application.
// Created for build/install tutorial, Microsoft Visual C++ 2010 Express and OpenCV 2.1.0

//#include "stdafx.h"

#include <Windows.h>

#include <cv.h>
#include <cxcore.h>
#include <highgui.h>

#include <iostream>


class LineSegment
{
public:
    LineSegment(const CvPoint &beg) : finished(false), lineBeg(beg) {}

    bool finished;
    CvPoint lineBeg;
    CvPoint lineEnd;
};

// line segments
bool renderUpdate = false;
std::vector<LineSegment> lines;

void MouseCallback(int event, int x, int y, int flags, void* param)
{
    switch(event)
    {
    case CV_EVENT_MOUSEMOVE: 
        if(!lines.empty() && !lines.back().finished)
        {
            lines.back().lineEnd = cvPoint(x, y);
            renderUpdate = true;
        }
        break;

    case CV_EVENT_LBUTTONDOWN:
        lines.push_back(LineSegment(cvPoint(x, y)));
        break;

    case CV_EVENT_LBUTTONUP:
        if(!lines.empty())
        {
            if(lines.back().lineBeg.x!=x || lines.back().lineBeg.y!=y)
            {
                lines.back().lineEnd = cvPoint(x, y);
                lines.back().finished = true;
                renderUpdate = true;
                std::cout << "ADDED LINE [" << lines.back().lineBeg.x << " " << lines.back().lineBeg.y << "] to [" << lines.back().lineEnd.x << " " << lines.back().lineEnd.y << "]" << std::endl;
            }
            else
            {
                lines.pop_back();
            }
        }
        break;
    }
}

//void switch_callback(int position)
//{
//	const int bah = 0;
//}

//int g_switch_value = 0;

//int _tmain(int argc, _TCHAR* argv[])
int main(int argc, char *argv[])
{
    std::string windowName = "Annotation";
    IplImage *img = cvLoadImage("lena.png");
    IplImage *temp = cvCloneImage(img);

    cvNamedWindow(windowName.c_str(), 1);
    cvShowImage(windowName.c_str(), img);

    cvSetMouseCallback(windowName.c_str(), MouseCallback);
    //cvCreateTrackbar("Switch", windowName.c_str(), &g_switch_value, 1, switch_callback);

    while(true)
    {
        if(renderUpdate)
        {
            cvCopyImage(img, temp);
            for(unsigned int i = 0; i < lines.size(); i++)
            {
                cvLine(temp, lines[i].lineBeg, lines[i].lineEnd, CV_RGB(255, 0, 0), 5);
            }
            cvShowImage(windowName.c_str(), temp);

            renderUpdate = false;
        }

        if(cvWaitKey(15)==27) 
            break;
    }

    //        cvWaitKey();
    cvDestroyWindow(windowName.c_str());
    cvReleaseImage(&img);

    return 0;
}
