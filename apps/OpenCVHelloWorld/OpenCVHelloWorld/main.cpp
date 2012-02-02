// OpenCV_Helloworld.cpp : Defines the entry point for the console application.
// Created for build/install tutorial, Microsoft Visual C++ 2010 Express and OpenCV 2.1.0

//#include "stdafx.h"

#include "AnnotationTool.h"
#include "BoxRoomView.h"
#include "VanishingPoint.h"
#include "Geometry.h"
#include "Selectable.h"

#include <Windows.h>

#include <cxcore.h>
#include <highgui.h>


// see yu2008inferring and hedau2009recovering


bool renderUpdate = false;
AnnotationTool *annotation = NULL;
void MouseCallback(int event, int x, int y, int flags, void* param)
{
    if(annotation)
    {
        switch(event)
        {
        case CV_EVENT_MOUSEMOVE:
            annotation->Update(x, y);
            renderUpdate = true;
            break;

        case CV_EVENT_LBUTTONDOWN:
            annotation->BeginUpdate(x, y);
            break;

        case CV_EVENT_LBUTTONUP:
            annotation->EndUpdate(x, y);
            renderUpdate = true;
            break;
        }
    }
}

//int g_switch_value = 0;
//void SwitchCallback(int position)
//{
//	const int bah = 0;
//}

// Returns an empty string if dialog is canceled
std::string openfilename(char *filter = "All Files (*.*)\0*.*\0", HWND owner = NULL) 
{
    OPENFILENAME ofn;
    char fileName[MAX_PATH] = "";
    ZeroMemory(&ofn, sizeof(ofn));

    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = owner;
    ofn.lpstrFilter = filter;
    ofn.lpstrFile = fileName;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
    ofn.lpstrDefExt = "";

    std::string fileNameStr;

    if ( GetOpenFileName(&ofn) )
        fileNameStr = fileName;

    return fileNameStr;
}

//int _tmain(int argc, _TCHAR* argv[])
int main(int argc, char *argv[])
{
    // call open file dialog
    std::string imageName = openfilename();

    std::string windowName = "Annotation";
    //std::string imageName = "0000000004.jpg";
    IplImage *img = cvLoadImage(imageName.c_str());
    IplImage *temp = cvCloneImage(img);


    // load annotation from file if exists
    annotation = new AnnotationTool(img->width, img->height);
    std::ifstream fin(imageName+".dat");
    if(fin.is_open())
    {
        annotation->load(fin);
        renderUpdate = true;
    }

    cvNamedWindow(windowName.c_str(), 1);
    cvShowImage(windowName.c_str(), img);

    cvSetMouseCallback(windowName.c_str(), MouseCallback);
    //cvCreateTrackbar("Switch", windowName.c_str(), &g_switch_value, 1, switch_callback);

    while(true)
    {
        if(renderUpdate)
        {
            cvCopyImage(img, temp);
            annotation->render(temp);
            cvShowImage(windowName.c_str(), temp);

            renderUpdate = false;
        }

        if(cvWaitKey(15)==27) 
            break;
    }

    //        cvWaitKey();
    cvDestroyWindow(windowName.c_str());
    cvReleaseImage(&img);

    // save annotation to file
    std::ofstream fout(imageName+".dat");
    annotation->save(fout);
    fout.close();

    delete annotation;

    return 0;
}
