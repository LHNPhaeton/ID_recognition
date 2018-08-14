#ifndef _JIAOZHENG_H
#define _JIAOZHENG_H

#include "opencv/cv.h"
#include "opencv/cxcore.h"
#include "opencv/highgui.h"

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <cmath>
#include <stdio.h>
#include <time.h>
#include <iostream>
#include <vector>
#include <stdlib.h>
#include <time.h> 
#include <io.h>
#include <math.h>
#include "lsd.h"

#define showSteps         1//打印过程信息
#define BoundWidth       60//在图片四周的宽度内做筛选，这个宽度定为50.
#define LineMinLength     20//筛选出长度大于20的线
#define ImgWidth          428//修改输入图片的大小，宽为450
#define ImgHeight         270//修改输入图片的大小，高为280
#define TopBottomAngle    10//20， 身份证顶部和底部边界线和水平线的夹角范围为-20~20
#define LeftRightAngle    80////70， 身份证左右边界线和水平线的夹角范围为-90~-70或70~90

using namespace std;
//using namespace cv;

//===========两点，以及两点与x轴的角度，两点之间的距离
struct linePtAngle
{
	CvPoint startPt;
	CvPoint endPt;
	double angle;
	float lineLength;
};

struct pPoint
{
	double x;
	double y;
};

/*
l.a = l.p1.y - l.p2.y;
l.b = l.p2.x - l.p1.x;
l.c = l.p1.x*l.p2.y - l.p1.y*l.p2.x;
*/
struct Line
{
	pPoint p1, p2;
	double a, b, c;
};

//================定义角度和点================/
struct ptAndAngle
{
	double Angle;
	cv::Point2f ptf;
};

bool InRectYesOrNo(CvPoint pt, CvRect rect);
float lenghtOf2P(CvPoint pt1, CvPoint pt2);
vector<cv::Vec4i> chooseLine(IplImage* src, vector<linePtAngle> lineTop);
vector<cv::Vec4i> chooseLineLR(IplImage* src, vector<linePtAngle> lineTop);
void GetLinePara(Line &l);
pPoint getCrossPoint(Line &l1, Line &l2);
double angle(pPoint o, pPoint s, pPoint e);
ptAndAngle computeIntersect(cv::Vec4i a, cv::Vec4i b);
void GetMaxAndMin(double *arr, int n, double &max, double &min);
cv::Vec4i chooseLongest(vector<cv::Vec4i> lineT);
void sortCorners(std::vector<cv::Point2f>& corners, cv::Point2f center);
int vifLine(IplImage * m_im);
IplImage * jiaoZheng(IplImage * m_im);

#endif