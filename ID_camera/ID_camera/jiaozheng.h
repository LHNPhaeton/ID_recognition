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

#define showSteps         1//��ӡ������Ϣ
#define BoundWidth       60//��ͼƬ���ܵĿ������ɸѡ�������ȶ�Ϊ50.
#define LineMinLength     20//ɸѡ�����ȴ���20����
#define ImgWidth          428//�޸�����ͼƬ�Ĵ�С����Ϊ450
#define ImgHeight         270//�޸�����ͼƬ�Ĵ�С����Ϊ280
#define TopBottomAngle    10//20�� ���֤�����͵ײ��߽��ߺ�ˮƽ�ߵļнǷ�ΧΪ-20~20
#define LeftRightAngle    80////70�� ���֤���ұ߽��ߺ�ˮƽ�ߵļнǷ�ΧΪ-90~-70��70~90

using namespace std;
//using namespace cv;

//===========���㣬�Լ�������x��ĽǶȣ�����֮��ľ���
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

//================����ǶȺ͵�================/
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