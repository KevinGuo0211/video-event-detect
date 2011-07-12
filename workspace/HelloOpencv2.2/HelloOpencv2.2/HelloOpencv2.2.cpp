// HelloOpencv2.2.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"


#include "cv.h"
#include <cxcore.h>
#include <highgui.h>


int _tmain(int argc, _TCHAR* argv[])
{
        IplImage *img = cvLoadImage("1.jpg");
        cvNamedWindow("Image:",1);
        cvShowImage("Image:",img);

        cvWaitKey();
        cvDestroyWindow("Image:");
        cvReleaseImage(&img);

        return 0;

}
