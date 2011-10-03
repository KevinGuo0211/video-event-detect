// FireSmoke_Detect_DLL.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "cv.h"
#include "highgui.h"
#include<fstream>

double max(double a,double b,double c)
{
	double temp=max(a,b);
	return max(temp,c);
}

double min(double a,double b,double c)
{
	double temp=min(a,b);
	return min(temp,c);
}

void insert(int &max, int &mid, int &mix, int a)
{
	if(a<=mix)
		return;
	if(a>mix&&a<=mid)
	{
		mix = a;
		return;
	}
	if(a>mid&&a<=max)
	{
		mix = mid;
		mid = a;
		return;
	}
	if(a>max)
	{
		mix = mid;
		mid = max;
		max = a;
		return;
	}
}

 extern "C" __declspec(dllexport) int fireDetect(char* filePath)
 {  
	 CvCapture* capture = cvCaptureFromAVI(filePath);
	
	 if(!capture)
		return 0;
	 int height = (int)cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT);
	 int width = (int)cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH);
	 int fps = (int)cvGetCaptureProperty(capture, CV_CAP_PROP_FPS);
	 IplImage *frame=0;
	 IplImage *pf1=0 , *pf2=0 , *pf3=0, *pFrImg=0 ;
	 pf1 = cvCreateImage(cvSize(width, height),8,1);
	 pf2 = cvCreateImage(cvSize(width, height),8,1);
	 pf3 = cvCreateImage(cvSize(width, height),8,1);
	 pFrImg = cvCreateImage(cvSize(width, height),  IPL_DEPTH_8U,1);

	 CvMat *pframemat1 = 0, *pframemat2 = 0,*pframemat3 = 0,*pframemat4 = 0,*pframemat5 = 0,*pframemat6 = 0;
	 pframemat1 = cvCreateMat(height, width, CV_32FC1);
	 pframemat2 = cvCreateMat(height, width, CV_32FC1);
	 pframemat3 = cvCreateMat(height, width, CV_32FC1);
	 pframemat4 = cvCreateMat(height, width, CV_32FC1);
	 pframemat5 = cvCreateMat(height, width, CV_32FC1);
	 pframemat6 = cvCreateMat(height, width, CV_32FC1);

	 int a = 0;
	 double B=0.0,G=0.0,R=0.0,Y=0.0,Cr=0.0,Cb=0.0,V,H,S;
	 double i,j,k;
	 //double r1,j1,r2,j2,b2;
	 bool isShowFire = false;

	 ofstream ofs;
	 ofs.open("Area.txt", ostream::trunc);
	 ofs.close();
	 ofs.clear();
	 ofs.open("Width.txt", ostream::trunc);
	 ofs.close();
	 ofs.clear();
	 ofs.open("Height.txt", ostream::trunc);
	 ofs.close();
	 ofs.clear();
	 ofs.open("Smoke.txt", ostream::trunc);
	 ofs.close();
	 ofs.clear();

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
		 cvConvert(pframemat6, pFrImg);
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
		
		 int fireArea = 0;
		 int smokeArea = 0;
		
		 for (int y=0; y< pFrImg->height; y++)
	     {
		    for (int x=0; x< pFrImg->width; x++)
		    {
				//颜色计算
				a = ((uchar*)(pFrImg->imageData + y*pFrImg->widthStep))[x];
			    B = ((uchar*)(colorlaplace->imageData + y*colorlaplace->widthStep))[x*colorlaplace->nChannels];
			    G = ((uchar*)(colorlaplace->imageData + y*colorlaplace->widthStep))[x*colorlaplace->nChannels+1];
			    R = ((uchar*)(colorlaplace->imageData + y*colorlaplace->widthStep))[x*colorlaplace->nChannels+2];
			    i=(R+G+B)/3;
			    j=(R-B)/2;
			    k=(2*G-R-B)/4;
			 
			    V=max(R,G,B);
			    double X=V-min(R,G,B);
			    if(V==0)
			    {
				    S=0;
			    }
			    else 
			    {
				    S=X*255/V;
			    }
			    if(V==R)
				    H=(G - B)*60/S;
			    if(V==G)
				    H= 120+(B - R)*60/S;
			    if(V==B)
				    H= 240+(B - R)*60/S;
			    if(H<0)  
				    H=H+360;
			    V=V/255.0;
			    S=S/255.0;

				//如果是运动区域的像素
			    if (a==0)
			    {
					bool isFirePix = false;
					//火焰检测
				    if((R>G)&&(G>B)&&(R>200)&&(G<235)&&(B<225))
				    {
					    if((0<H<63)&&(V>0.80)&&(S>0.35))
					    {
						     ((uchar*)(colorlaplace->imageData + y*colorlaplace->widthStep))[x*colorlaplace->nChannels] = 0;  //B
						     ((uchar*)(colorlaplace->imageData + y*colorlaplace->widthStep))[x*colorlaplace->nChannels+1] = 0;  //G
						     ((uchar*)(colorlaplace->imageData + y*colorlaplace->widthStep))[x*colorlaplace->nChannels+2] = 255;  //R	

							 ((uchar*)(pFrImg->imageData + y*pFrImg->widthStep))[x*pFrImg->nChannels] = 255;  //B
						     ((uchar*)(pFrImg->imageData + y*pFrImg->widthStep))[x*pFrImg->nChannels+1] = 255;  //G
							 ((uchar*)(pFrImg->imageData + y*pFrImg->widthStep))[x*pFrImg->nChannels+2] = 255;  //R	
							 fireArea++;
							 isFirePix = true;
					    }
				    }

					if(!isFirePix)
					{
						//烟雾检测
						int max_ = (int)max((double)R, (double)B, (double)G);
						int min_ = (int)min((double)R, (double)B, (double)G);
						bool condition1 = max_-15<=min_+15;
						double I = (R + G + B)/sqrt(3.0);
						bool condition2 = (I >= 150) && (I <= 220);
						bool condition3 = (I >= 80) && (I <= 150);
				    
						if(condition1 && (condition2 || condition3))
						{
				   			((uchar*)(pFrImg->imageData + y*pFrImg->widthStep))[x*pFrImg->nChannels] = 255;  //B
							((uchar*)(pFrImg->imageData + y*pFrImg->widthStep))[x*pFrImg->nChannels+1] = 255;  //G
							((uchar*)(pFrImg->imageData + y*pFrImg->widthStep))[x*pFrImg->nChannels+2] = 255;  //R	
							fireArea++;
						}else
						{
							((uchar*)(pFrImg->imageData + y*pFrImg->widthStep))[x*pFrImg->nChannels] = 0;  //B
							((uchar*)(pFrImg->imageData + y*pFrImg->widthStep))[x*pFrImg->nChannels+1] = 0;  //G
							((uchar*)(pFrImg->imageData + y*pFrImg->widthStep))[x*pFrImg->nChannels+2] = 0;  //R	
						}
					}
			    }else
			    {
				     ((uchar*)(pFrImg->imageData + y*pFrImg->widthStep))[x*pFrImg->nChannels] = 0;  //B
				     ((uchar*)(pFrImg->imageData + y*pFrImg->widthStep))[x*pFrImg->nChannels+1] = 0;  //G
					 ((uchar*)(pFrImg->imageData + y*pFrImg->widthStep))[x*pFrImg->nChannels+2] = 0;  //R	
			    }
		    }
		 }
		
		 cvShowImage("烟火二值图", pFrImg);
		
		 //轮廓检测
		 if(num%fps == 0)
		 {
			 CvMemStorage*stor = cvCreateMemStorage(0);
			 CvSeq*cont = cvCreateSeq(CV_SEQ_ELTYPE_POINT, sizeof(CvSeq), sizeof(CvPoint), stor);
			 cvFindContours(pFrImg, stor, &cont, sizeof(CvContour), CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE, cvPoint(0,0));
			 int maxArea = 3, midArea = 2, mixArea = 1;
			 int maxWidth = 3, midWidth = 2, mixWidth = 1;
			 int maxHeight = 3, midHeight = 2, mixHeight = 1;
			 int totalArea = 0;
			 for(;cont;cont = cont->h_next)
			 {
				 CvRect r = ((CvContour*)cont)->rect;
				 if(r.height * r.width>=10)
				 {
					 insert(maxArea, midArea, mixArea, r.height*r.width);
					 insert(maxWidth, midWidth, mixWidth, r.width);
					 insert(maxHeight, midHeight, mixHeight, r.height);

					 cvRectangle(colorlaplace, cvPoint(r.x, r.y),
					 cvPoint(r.x+r.width, r.y+r.height), CV_RGB(255,0,0), 1, CV_AA,0);

					 totalArea += r.height * r.width;
				 }
			 }
			 cvReleaseMemStorage(&stor);

			 //写入文件
			
			 int areaInt = (int)(10000.0*totalArea/(width*height));
		     float areaFloat = (float)areaInt / 100;

			 ofs.open("Area.txt", ostream::app);
			 ofs<<areaFloat<<",";
			 ofs.close();
			 ofs.clear();

			 ofs.open("Width.txt", ostream::app);
			 ofs<<maxWidth<<",";
			 ofs.close();
			 ofs.clear();

			 ofs.open("Height.txt", ostream::app);
			 ofs<<maxHeight<<",";
			 ofs.close();
			 ofs.clear();

			 ofs.open("Smoke.txt", ostream::app);
			 ofs<<smokeArea<<",";
			 ofs.close();
			 ofs.clear();

		 }
			
		 num++;
		
		 cvShowImage("原视频", frame);
		 /*
		 FILE*f = fopen("isShowFire.txt", "r");
		 int temp;
		 fscanf(f, "%d", &temp);
		 if(temp)
			 isShowFire = true;
		 else
			 isShowFire = false;
		 if(isShowFire)
		 */
			 //cvShowImage("烟火二值图", pframemat6);
		 //cvShowImage("烟火视频", colorlaplace);
		
	     if( cvWaitKey(5) >= 0 )
	         break;
	 }
	 //释放内存
	 cvReleaseMat(&pframemat1);
	 cvReleaseMat(&pframemat2);
	 cvReleaseMat(&pframemat3);
	 cvReleaseMat(&pframemat4);
	 cvReleaseMat(&pframemat5);
	 cvReleaseMat(&pframemat6);

	 cvReleaseImage(&pf1);
	 cvReleaseImage(&pf2);
	 cvReleaseImage(&pf3);
	 cvReleaseImage(&pFrImg);
	 cvReleaseImage(&frame);

	 cvDestroyWindow("烟火二值图");
	 cvDestroyWindow("原视频");
     return 0;
 }


