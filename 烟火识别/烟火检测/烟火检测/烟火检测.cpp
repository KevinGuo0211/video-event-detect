// 烟火检测.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <iostream>
#include <cv.h>
#include <cxcore.h>
#include <highgui.h>
#include "atlstr.h"
#include<cmath>
using namespace std;


int _tmain(int argc, _TCHAR* argv[])
{
	char* filePath = "G:\\2000 2004年全国特大火灾案例选编05.AVI";
	CvCapture* capture = cvCaptureFromAVI(filePath);
	if(!capture)
		return 0;
	int height = (int)cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT);
	int width = (int)cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH);
	IplImage *frame=0;
	IplImage *pf1=0 , *pf2=0 , *pf3=0 ;
	pf1 = cvCreateImage(cvSize(width, height),8,1);
	pf2 = cvCreateImage(cvSize(width, height),8,1);
	pf3 = cvCreateImage(cvSize(width, height),8,1);

	CvMat *pframemat1 = 0, *pframemat2 = 0,*pframemat3 = 0,*pframemat4 = 0,*pframemat5 = 0,*pframemat6 = 0;
	pframemat1 = cvCreateMat(height, width, CV_32FC1);
	pframemat2 = cvCreateMat(height, width, CV_32FC1);
	pframemat3 = cvCreateMat(height, width, CV_32FC1);
	pframemat4 = cvCreateMat(height, width, CV_32FC1);
	pframemat5 = cvCreateMat(height, width, CV_32FC1);
	pframemat6 = cvCreateMat(height, width, CV_32FC1);

	int num = 1;
	while(true)
	{
		//运动物体检测部分
		frame = cvQueryFrame(capture);
		if(!frame)
			break;
		if(num%3 == 1)
		{
			cvCvtColor(frame, pf1, CV_BGR2GRAY);
			//pframemat1 = cvCreateMat(height, width, CV_32FC1);
			cvConvert(pf1, pframemat1);
		}
		if(num%3 == 2)
		{
			cvCvtColor(frame, pf2, CV_BGR2GRAY);
			//pframemat2 = cvCreateMat(height, width, CV_32FC1);
			cvConvert(pf2, pframemat2);
		}
		if(num%3 == 0)
		{
			cvCvtColor(frame, pf3, CV_BGR2GRAY);
			//pframemat3 = cvCreateMat(height, width, CV_32FC1);
			cvConvert(pf3, pframemat3);
		}

		cvSub(pframemat1, pframemat2, pframemat4, 0);
		cvSub(pframemat3, pframemat2, pframemat5, 0);
		cvMul(pframemat4, pframemat5, pframemat6, 1);

	    //二值化前景图
	    cvThreshold(pframemat6, pframemat6, 60, 255.0, CV_THRESH_BINARY);//令图像只有黑白二值
	    //进行形态学滤波，去掉噪音  
	    cvErode(pframemat6, pframemat6, 0, 1);

		int num1 = cvCountNonZero(pframemat6);

		//烟火检测部分
		IplImage *colorlaplace=NULL;
		if(num == 1)
			colorlaplace = cvCreateImage(cvGetSize(frame),frame->depth,frame->nChannels);//创建副本
		else 
		{
			colorlaplace = cvCreateImage(cvGetSize(frame),frame->depth,frame->nChannels);//创建副本
			cvCopyImage(frame,colorlaplace);//当前帧复制到副本
		}
		int snum1 = 0;
		int snum2 = 0;
		for(int i = 0; i < height; i++)
		{
			for(int j = 0; j < width; j++)
			{
				double b = ((uchar*)(colorlaplace->imageData+i*colorlaplace->widthStep))[j*colorlaplace->nChannels+0];
				double sj = ((uchar*)(colorlaplace->imageData+i*colorlaplace->widthStep))[j*colorlaplace->nChannels+1];
				double r = ((uchar*)(colorlaplace->imageData+i*colorlaplace->widthStep))[j*colorlaplace->nChannels+2];
				double r1 = r/(r+b+sj);
				double j1 = sj/(r+b+sj);
				double r2 = sj/(r+1);
				double j2 = b/(sj+1);
				double b2 = b/(r+1);
				//烟雾像素识别
				if((0.3043<r1&&r1<0.3353)&&(0.3187<j1&&j1<0.3373)&&(r1<j1))
				{
					snum1++;

					((uchar*)(colorlaplace->imageData+i*colorlaplace->widthStep))[j*colorlaplace->nChannels+0] = 255;
					((uchar*)(colorlaplace->imageData+i*colorlaplace->widthStep))[j*colorlaplace->nChannels+1] = 255;
					((uchar*)(colorlaplace->imageData+i*colorlaplace->widthStep))[j*colorlaplace->nChannels+2] = 255;
				}
				//火焰像素识别
				/*
				if((0.25<r2&&r2<0.65)&&(0.2<j2&&j2<0.6)&&(0.05<b2&&b2<0.45)&&(r>200)&&(b<100)&&(sj<200))
				{
					snum2++;

					((uchar*)(colorlaplace->imageData+i*colorlaplace->widthStep))[j*colorlaplace->nChannels+0] = 0;
					((uchar*)(colorlaplace->imageData+i*colorlaplace->widthStep))[j*colorlaplace->nChannels+1] = 0;
					((uchar*)(colorlaplace->imageData+i*colorlaplace->widthStep))[j*colorlaplace->nChannels+2] = 255;
				}
				*/

			}
		}
		num1 = cvCountNonZero(pframemat6);
		if((3<=num1&&num1<=25&&snum1>10)||(3<=num1&&num1<=25&&snum2>10))
		{
			cout<<"发现烟火"<<endl;
			//system("pause");
		}
		num++;

		cvShowImage("原视频", frame);
		cvShowImage("差视频", pframemat6);
		cvShowImage("烟火视频", colorlaplace);
	    if( cvWaitKey(5) >= 0 )
	        break;
	}
	system("pause");
}

