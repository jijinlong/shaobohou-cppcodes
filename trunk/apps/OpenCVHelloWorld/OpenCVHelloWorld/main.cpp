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
    LineSegment() {}
    LineSegment(const CvPoint &beg) : begPoint(beg) {}
    LineSegment(const CvPoint &beg, const CvPoint &end) : begPoint(beg), endPoint(end) {}

    CvPoint* GetNearestEndPoint(int x, int y)
    {
        long begDist2 = dist2(cvPoint(x, y), begPoint);
        long endDist2 = dist2(cvPoint(x, y), endPoint);

        return begDist2<endDist2 ? &begPoint : &endPoint;
    }

    void render(IplImage *temp, const CvScalar &col) const
    {
        cvLine(temp, begPoint, endPoint, col, 2);
        cvLine(temp, begPoint, begPoint, CV_RGB(0, 0, 0), 4);
        cvLine(temp, endPoint, endPoint, CV_RGB(0, 0, 0), 4);
    }

    CvPoint begPoint, endPoint;
};

// vanishing point
class VanishingPoint
{
public:
    VanishingPoint(const LineSegment &line)
    {
        lines.push_back(line);
    }

    void addLine(const LineSegment &line)
    {
        lines.push_back(line);
    }

    CvPoint* SelectPoint(int x, int y, int radius=10)
    {
        CvPoint *point = NULL;
        long bestDist2 = std::numeric_limits<long>::max();
        for(unsigned int i = 0; i < lines.size(); i++)
        {
            CvPoint *tempPoint = lines[i].GetNearestEndPoint(x, y);
            const long tempDist2 = dist2(cvPoint(x, y), *tempPoint);

            if(tempDist2 < bestDist2)
            {
                point = tempPoint;
                bestDist2 = tempDist2;
            }
        }

        if(bestDist2 <= radius*radius)
        {
            return point;
        }

        return NULL;
    }

    bool computeVanishingPoint(CvPoint &vpoint) const
    {
        assert(lines.size()> 1);

        const double &x1 = lines[0].begPoint.x; const double &y1 = lines[0].begPoint.y;
        const double &x2 = lines[0].endPoint.x; const double &y2 = lines[0].endPoint.y;
        const double &x3 = lines[1].begPoint.x; const double &y3 = lines[1].begPoint.y;
        const double &x4 = lines[1].endPoint.x; const double &y4 = lines[1].endPoint.y;

        double den =  (y4-y3)*(x2-x1) - (x4-x3)*(y2-y1);
        if(fabs(den) < 1e-6)
        {
            vpoint.x = static_cast<int>(((x2-x1)+(x4-x3))/2);
            vpoint.y = static_cast<int>(((y2-y1)+(y4-y3))/2);

            return true;
        }
        else
        {
            const double num1 = (x4-x3)*(y1-y3) - (y4-y3)*(x1-x3);
            //double num2 = (x2-x1)*(y1-y3) - (y2-y1)*(x1-x3);

            const double u1 = num1/den;
            //u2 = num2/den;

            vpoint.x = static_cast<int>(x1+u1*(x2-x1));
            vpoint.y = static_cast<int>(y1+u1*(y2-y1));

            return false;
        }
    }

    void render(IplImage *temp, const CvScalar &col) const
    {
        for(unsigned int i = 0; i < lines.size(); i++)
        {
            lines[i].render(temp, col);
        }

        if(lines.size() > 1)
        {
            CvPoint vpoint;
            const bool infinite = computeVanishingPoint(vpoint);

            if(!infinite)
            {
                cvLine(temp, vpoint, vpoint, col, 10);
            }
        }
    }

private:
    std::vector<LineSegment> lines;
};

// line segments
class Annotation
{
public:
    int width;
    int height;

    Annotation(): width(0), height(0), currLine(NULL), selectedPoint(NULL) {};
    Annotation(int w, int h) : width(w), height(h), currLine(NULL), selectedPoint(NULL) {};

    void BeginUpdate(int x, int y)
    {
        selectedPoint = SelectPoint(x, y);
        if(selectedPoint==NULL)
        {
            currLine = new LineSegment(cvPoint(x, y));
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
            if(currLine)
            {
                currLine->endPoint = cvPoint(x, y);
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
            if(currLine)
            {
                if(vanishings.size()<3 && !equal(currLine->begPoint, currLine->endPoint))
                {
                    vanishings.push_back(VanishingPoint(*currLine));
                    vanishings.back().addLine(LineSegment(cvPoint(width-currLine->begPoint.x, height-currLine->begPoint.y), cvPoint(width-currLine->endPoint.x, height-currLine->endPoint.y)));
                    std::cout << "ADDED LINE [" << currLine->begPoint.x << " " << currLine->begPoint.y << "] to [" << currLine->endPoint.x << " " << currLine->endPoint.y << "]" << std::endl;
                }

                delete currLine;
                currLine = NULL;
            }
        }
        else
        {
            selectedPoint->x = x;
            selectedPoint->y = y;
            selectedPoint = NULL;
        }
    }

    void render(IplImage *temp) const
    {
        if(vanishings.size()>0) vanishings[0].render(temp, CV_RGB(255, 0, 0));
        if(vanishings.size()>1) vanishings[1].render(temp, CV_RGB(0, 255, 0));
        if(vanishings.size()>2) vanishings[2].render(temp, CV_RGB(0, 0, 255));

        if(currLine)
        {
            if(vanishings.size()==0) currLine->render(temp, CV_RGB(255, 0, 0));
            if(vanishings.size()==1) currLine->render(temp, CV_RGB(0, 255, 0));
            if(vanishings.size()==2) currLine->render(temp, CV_RGB(0, 0, 255));
        }
    }

private:
    LineSegment *currLine;
    CvPoint *selectedPoint;
    std::vector<VanishingPoint> vanishings;


    CvPoint* SelectPoint(int x, int y, int radius=10)
    {
        CvPoint *point = NULL;
        long bestDist2 = std::numeric_limits<long>::max();
        for(unsigned int i = 0; i < vanishings.size(); i++)
        {
            CvPoint *tempPoint = vanishings[i].SelectPoint(x, y, radius);
            
            if(tempPoint)
            {
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

    annotation = Annotation(img->width, img->height);

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
