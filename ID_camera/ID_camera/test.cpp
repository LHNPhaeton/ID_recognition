#include "opencv/cv.h"    
#include "opencv/highgui.h"
#include "opencv/cvaux.h"
#include "opencv/cxcore.h"

#include "opencv2/opencv.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp" 
#include "opencv2/imgproc/imgproc.hpp" 
#include "lsd.h"  
#include "jiaozheng.h"
#include "id_card_font.h"
#include <iostream>
#include <string>
#include <cmath>
#include <Windows.h>  
#include <stdlib.h>
#include <stdio.h>
#include <time.h>  
#include <fstream>  
#include <tchar.h>  
#include <atlstr.h>
#include <map>


//using namespace cv;
using namespace std;

#define MAXFRAMENUM 1000

string imagePath = "..\\���֤ͼƬ\\";
//string imagePath = "..\\�߽����\\";
string savePath_final = "..\\������ͼƬ\\";
//string savePath = "..\\������ͼƬ\\";
string savePath_init = "..\\�߽�\\";
//string savePath = "..\\�����ͼƬ1\\";

extern vector<cv::Vec4i> lineT;
extern vector<cv::Vec4i> lineB;
extern vector<cv::Vec4i> lineL;
extern vector<cv::Vec4i> lineR;
extern IplImage* res_im;


//IplImage *jiaozheng(IplImage *res_im, string filename)
bool jiaozheng(IplImage *res_im, string filename)
{
	/*
	//============���������û����ȫ========//
	if ((lineT.size() == 0) || (lineB.size() == 0) || (lineL.size() == 0) || (lineR.size() == 0))
	{
		//return res_im;
		return false;
	}*/

	cv::Vec4i lineTLongest = chooseLongest(lineT);
	cv::Vec4i lineBLongest = chooseLongest(lineB);
	cv::Vec4i lineLLongest = chooseLongest(lineL);
	cv::Vec4i lineRLongest = chooseLongest(lineR);

	lineT.clear();
	lineB.clear();
	lineL.clear();
	lineR.clear();

	vector<cv::Vec4i> linesAll;
	linesAll.push_back(lineTLongest);
	linesAll.push_back(lineBLongest);
	linesAll.push_back(lineLLongest);
	linesAll.push_back(lineRLongest);

	cout << "linesAll.size: " << linesAll.size() << endl;
	//needed for visualization only//�������ߵ������ӳ���ȫ��
	for (unsigned int i = 0; i<linesAll.size(); i++)
	{
		cv::Vec4i v = linesAll[i];
		linesAll[i][0] = 0;
		linesAll[i][1] = ((float)v[1] - v[3]) / (v[0] - v[2])* -v[0] + v[1];
		linesAll[i][2] = res_im->width;
		linesAll[i][3] = ((float)v[1] - v[3]) / (v[0] - v[2])*(res_im->width - v[2]) + v[3];
		if (((v[0] - v[2])) == 0)  {
			linesAll[i][0] = v[0];
			linesAll[i][1] = 0;
			linesAll[i][2] = v[2];
			linesAll[i][3] = res_im->height;
		}	

		if (showSteps)
		{
			//cout << linesAll[i][0] << " " << linesAll[i][1] << " " << linesAll[i][2] << " " << linesAll[i][3] << endl;
		}

	}

	//Draw Lines
	if (showSteps)
	{
		for (unsigned int i = 0; i<linesAll.size(); i++)
		{
			cv::Vec4i v = linesAll[i];
			//cvLine(res_im, cvPoint(v[0], v[1]), cvPoint(v[2], v[3]), CV_RGB(0, 255, 255), 1, CV_AA);//�����ӳ��ı߽���
		}			
	}

	std::vector<ptAndAngle> corners;//�ߵĽ���洢
	for (unsigned int i = 0; i<linesAll.size() - 1; i++)
	{
		for (unsigned int j = i + 1; j<linesAll.size(); j++)
		{
			//===========�ߵĽ���=============/
			ptAndAngle pt = computeIntersect(linesAll[i], linesAll[j]);//����ֱ�ߵļнǷ�Χ��70~110�Ȼ�-110~-70��֮�䣬�����ڣ��ý�����Ч
			if ((pt.ptf.x >= -10 && pt.ptf.y >= -10)
				&& (pt.ptf.x<res_im->width + 10) && (pt.ptf.y<res_im->height + 10))//pt.ptf.x<res_im->width + 20) && (pt.ptf.y<res_im->height + 20
			{
				corners.push_back(pt);
			}
		}
	}

	vector<cv::Vec4i>(linesAll).swap(linesAll);

	
	if (showSteps){
		for (unsigned int i = 0; i<corners.size(); i++)
		{
			//cout << corners[i].ptf.x << " " << corners[i].ptf.y << endl;
			//cvCircle(res_im, corners[i].ptf, 3, CV_RGB(255, 255, 0), 2);
		}
	}

	std::vector<cv::Point2f> cornersPt;
	for (unsigned int i = 0; i<corners.size(); i++)
	{
		cornersPt.push_back(corners[i].ptf);
	}

	std::vector<cv::Point2f> approx;
	cv::approxPolyDP(cv::Mat(cornersPt), approx, cv::arcLength(cv::Mat(cornersPt), true)*0.02, false);
	if (approx.size() != 4)
	{
		std::cout << "The object is not quadrilateral���ı��Σ�!" << std::endl;
		return false;
	}

	if (showSteps){  //=============�����ı�����ϵĶ�����==================/
		for (unsigned int i = 0; i<approx.size(); i++){
			//cvCircle(res_im, approx[i], 3, CV_RGB(0, 0, 255), 3);
			printf("[ %f\t %f ]\n", approx[i].x, approx[i].y);
		}
	}

	std::vector<cv::Point2f>(approx).swap(approx);

	//���ĵ�
	cv::Point2f center(0, 0);
	//get mass center
	for (unsigned int i = 0; i < corners.size(); i++)
	{
		center += cornersPt[i];
	}
	center *= (1. / corners.size());

	std::vector<ptAndAngle>(corners).swap(corners);

	//if (showSteps)
		//cvCircle(res_im, center, 3, CV_RGB(255, 255, 0), 6);

	//=============���ĸ����㰴��˳������=============/
	sortCorners(cornersPt, center);
	std::vector<cv::Point2f>(cornersPt).swap(cornersPt);

	//============���==============/
	cv::Mat quad = cv::Mat::zeros(res_im->height, res_im->width, CV_8UC3);

	//corners of the destination image
	std::vector<cv::Point2f> quad_pts;
	quad_pts.push_back(cv::Point2f(0, 0));
	quad_pts.push_back(cv::Point2f(quad.cols, 0));//(220,0)
	quad_pts.push_back(cv::Point2f(quad.cols, quad.rows));//(220,300)
	quad_pts.push_back(cv::Point2f(0, quad.rows));

	std::vector<cv::Point2f>(quad_pts).swap(quad_pts);

	// Get transformation matrix
	cv::Mat transmtx = cv::getPerspectiveTransform(cornersPt, quad_pts); //��Դ����ϵ���ѻ���ģ���Ŀ������ϵ��ת������

	//Mat src(res_im);
	cv::Mat src = cv::cvarrToMat(res_im);
	// Apply perspective transformation͸��ת��
	cv::warpPerspective(src, quad, transmtx, quad.size());

	IplImage *dst = cvCreateImage(cvSize(quad.cols, quad.rows), 8, res_im->nChannels);
	uchar *dataDst = (uchar *)(dst->imageData);

	for (size_t nrow = 0; nrow < quad.rows; nrow++)
	{
		for (size_t ncol = 0; ncol < quad.cols; ncol++)
		{
			cv::Vec3i bgr = quad.at<cv::Vec3b>(nrow, ncol);//��Vec3b 
			for (size_t k = 0; k<quad.channels(); k++){
				dataDst[nrow*(dst->widthStep) + ncol*(dst->nChannels) + k] = bgr.val[k];
			}
		}
	}

	if (showSteps)
	{
		/*
		cvNamedWindow("Image", 1);
		cvShowImage("Image", res_im);
		cvNamedWindow("dst", 1);
		cvShowImage("dst", dst);

		cvWaitKey(0);

		cvDestroyWindow("Image");
		cvDestroyWindow("dst");*/
	}

	cvReleaseImage(&res_im);

	cv::Mat IDImg = cv::cvarrToMat(dst, true);
	cv::imwrite(savePath_init + filename, IDImg);
	cout << "*************�ҵ��ĸ��߽磡��������������" << endl << endl;

	cv::Mat IDImgGray;
	cvtColor(IDImg, IDImgGray, CV_BGR2GRAY);
	bool flag = get_ID_info(IDImgGray);
	//cout << "***********************" << endl;

	if (flag)
	{
		cv::imwrite(savePath_final + filename, IDImg);
		//return true;
	}
	cvReleaseImage(&dst);
	//return dst;
	return flag;
}

/*
void processImg()
{
	Directory dir;
	bool addPath = false;//true; 
	//��ȡĿ¼�µ������ļ���
	string suffix = "*.jpg";//��׺
	vector<string> filenames = dir.GetListFiles(imagePath, suffix, addPath);
	string ImgName;
	int i;
	for (i = 0; i < filenames.size(); i++)
	{
		cout << "ͼ�� " << i + 1 << ":  " << filenames[i] << endl;
		ImgName = imagePath + filenames[i];
		const char *pstrImageName = ImgName.c_str();
		IplImage *pSrcImage = cvLoadImage(pstrImageName, CV_LOAD_IMAGE_UNCHANGED);
	
		char *vif = vifLine(pSrcImage);
		//  cout<<vif<<endl;
		IplImage * dstImg = jiaozheng(res_im, filenames[i]);
		//cvNamedWindow("dst", 1);
		//cvShowImage("dst", dstImg);

		cvReleaseImage(&pSrcImage);
		cvReleaseImage(&dstImg);
		free(vif);
		//cvWaitKey(0);
	}
}*/


bool processCapImg(IplImage * pSrcImage, string filename)
{
	int vif = vifLine(pSrcImage);
	//  cout<<vif<<endl;
	if (vif == 0)
	{
		return jiaozheng(res_im, filename);
	}

	return false;
}


int main()
{
	/**/
	IplImage *pFrame = NULL;
	//IplImage *pGrayImg = NULL;

	CvCapture *pCapture = cvCreateCameraCapture(0); //��ȡ����ͷ  
	cout << "��������ͷ�ɹ�..." << endl;
	cv::waitKey(100);
	int nFrmNum = 0;

	if (!pCapture)
	{
		cout << "��������ͷʧ��..." << endl;
		return 0;
	}
	//create window
	cvNamedWindow("Video", 1);

	int currentFrame = 1;
	bool stopflag = false;
	while (pFrame = cvQueryFrame(pCapture))    //��֡��ȡ
	{
		//pGrayImg = cvCreateImage(cvGetSize(pFrame), IPL_DEPTH_8U, 1);
		//cvCvtColor(pFrame, pGrayImg, CV_BGR2GRAY);

		cvShowImage("Video", pFrame);
		//string ImgName = savePath + to_string(currentFrame) + ".jpg";
		string ImgName = to_string(currentFrame) + ".jpg";
		currentFrame++;

		stopflag = processCapImg(pFrame, ImgName);

		if (stopflag)
		{
			cv::waitKey(0);
		}

		//��֡��ļ��ʱ��
		int c = cvWaitKey(50);//��esc�˳�
		if ((char)c == 27)
		{
			cvDestroyWindow("Video");
			cvReleaseImage(&pFrame);
			return 0;
		}

	}

	//system("pause");
	return 0;


	/*
	VideoCapture camera(0);
	waitKey(100);

	int currentFrame = 1;
	//����һ���������ƶ�ȡ��Ƶѭ�������ı���  
	bool stopFlag = false;

	Mat camerFrame, grayFrame;
	while (!stopFlag)
	{
		Mat camerFrame;
		camera >> camerFrame;
		//cvtColor(camerFrame, grayFrame, CV_BGR2GRAY);
		if (camerFrame.empty())
		{
			std::cerr << "�޷���ȡ����ͷͼ��" << std::endl;
			getchar();
			return 0;
		}

		imshow("ԭͼ", camerFrame);
		string ImgName = savePath + to_string(currentFrame) + ".jpg";
		currentFrame++;
		imwrite(ImgName, camerFrame);
		
		//��֡��ļ��ʱ��
		int c = waitKey(50);
		
		//����ESC���ߵ���ָ���Ľ���֡���˳���ȡ��Ƶ  
		if ((char)c == 27 || currentFrame > MAXFRAMENUM)
		{
			stopFlag = true;
		}

		//���°������ͣ���ڵ�ǰ֡���ȴ���һ�ΰ���  
		if (c >= 0)
		{
			waitKey(0);
		}
	}*/

	/*
	clock_t start, finish;
	start = clock();
	processImg();
	finish = clock();
	cout << endl << "�����ʱ��   " << (double)(finish - start) / CLOCKS_PER_SEC << endl;*/


}