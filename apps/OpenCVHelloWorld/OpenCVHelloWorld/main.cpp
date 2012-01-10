// OpenCV_Helloworld.cpp : Defines the entry point for the console application.
// Created for build/install tutorial, Microsoft Visual C++ 2010 Express and OpenCV 2.1.0

//#include "stdafx.h"

#include <Windows.h>

#include <cv.h>
#include <cxcore.h>
#include <highgui.h>

#include <iostream>


// see yu2008inferring and hedau2009recovering


// Point Functions
bool equal(const CvPoint &p1, const CvPoint &p2)
{
    return p1.x==p2.x  && p1.y==p2.y;
}

long dist2(const CvPoint &p1, const CvPoint &p2)
{
    const int dx = p1.x - p2.x;
    const int dy = p1.y - p2.y;
    return dx*dx + dy*dy;
}


// Line Class and Functions
class LineSegment
{
public:
    LineSegment(const CvPoint &beg) : finished(false), begPoint(beg) {}

    CvPoint* GetNearestEndPoint(int x, int y)
    {
        long begDist2 = dist2(cvPoint(x, y), begPoint);
        long endDist2 = dist2(cvPoint(x, y), endPoint);

        return begDist2<endDist2 ? &begPoint : &endPoint;
    }

    bool finished;
    CvPoint begPoint, endPoint;
};

//////class VanishingPoint
//////{
//////public:
//////
//////private:
//////    LineSegment lines1;
//////    LineSegment lines2;
//////};

// line segments
class Annotation
{
public:
    Annotation() : selectedPoint(NULL) {};

    CvPoint* SelectPoint(int x, int y, int radius=10)
    {
        CvPoint *point = NULL;
        long bestDist2 = std::numeric_limits<long>::max();
        for(unsigned int i = 0; i < lines.size(); i++)
        {
            if(lines[i].finished)
            {
                CvPoint *tempPoint = lines[i].GetNearestEndPoint(x, y);
                const long tempDist2 = dist2(cvPoint(x, y), *tempPoint);

                if(tempDist2 < bestDist2)
                {
                    point = tempPoint;
                    bestDist2 = tempDist2;
                }
            }
        }

        if(bestDist2 <= radius*radius)
        {
            return point;
        }

        return NULL;
    }

    void BeginUpdate(int x, int y)
    {
        selectedPoint = SelectPoint(x, y);
        if(selectedPoint==NULL)
        {
            lines.push_back(LineSegment(cvPoint(x, y)));
        }
        else
        {
            std::cout << "SELECTED POINT [" << selectedPoint->x << " " << selectedPoint->y << "]" << std::endl;
        }
    }

    void Update(int x, int y)
    {
        if(selectedPoint==NULL)
        {
            if(!lines.empty() && !lines.back().finished)
            {
                lines.back().endPoint = cvPoint(x, y);
            }
        }
        else
        {
            selectedPoint->x = x;
            selectedPoint->y = y;
        }
    }

    void EndUpdate(int x, int y)
    {
        if(selectedPoint==NULL)
        {
            if(!lines.empty())
            {
                if(!equal(lines.back().begPoint, cvPoint(x, y)))
                {
                    lines.back().endPoint = cvPoint(x, y);
                    lines.back().finished = true;
                    std::cout << "ADDED LINE [" << lines.back().begPoint.x << " " << lines.back().begPoint.y << "] to [" << lines.back().endPoint.x << " " << lines.back().endPoint.y << "]" << std::endl;
                }
                else
                {
                    lines.pop_back();
                }
            }
        }
        else
        {
            std::cout << "MOVED    POINT [" << selectedPoint->x << " " << selectedPoint->y << "] to [" << x << " " << y << "]" << std::endl;
            selectedPoint->x = x;
            selectedPoint->y = y;
            selectedPoint = NULL;
        }
    }

    void render(IplImage *temp) const
    {
        for(unsigned int i = 0; i < lines.size(); i++)
        {
            switch(i)
            {
            case 0:
            case 1:
                cvLine(temp, lines[i].begPoint, lines[i].endPoint, CV_RGB(255, 0, 0), 2);
                break;

            case 2:
            case 3:
                cvLine(temp, lines[i].begPoint, lines[i].endPoint, CV_RGB(0, 255, 0), 2);
                break;

            case 4:
            case 5:
                cvLine(temp, lines[i].begPoint, lines[i].endPoint, CV_RGB(0, 0, 255), 2);
                break;
            }
            cvLine(temp, lines[i].begPoint, lines[i].begPoint, CV_RGB(0, 0, 0), 4);
            cvLine(temp, lines[i].endPoint, lines[i].endPoint, CV_RGB(0, 0, 0), 4);
        }
    }

private:
    CvPoint *selectedPoint;
    std::vector<LineSegment> lines;
};

Annotation annotation;
bool renderUpdate = false;
void MouseCallback(int event, int x, int y, int flags, void* param)
{
    switch(event)
    {
    case CV_EVENT_MOUSEMOVE:
        annotation.Update(x, y);
        renderUpdate = true;
        break;

    case CV_EVENT_LBUTTONDOWN:
        annotation.BeginUpdate(x, y);
        break;

    case CV_EVENT_LBUTTONUP:
        annotation.EndUpdate(x, y);
        renderUpdate = true;
        break;
    }
}

//int g_switch_value = 0;
//void SwitchCallback(int position)
//{
//	const int bah = 0;
//}

//int _tmain(int argc, _TCHAR* argv[])
int main(int argc, char *argv[])
{
    std::string windowName = "Annotation";
    //IplImage *img = cvLoadImage("lena.png");
    IplImage *img = cvLoadImage("0000000004.jpg");
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
            annotation.render(temp);
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
