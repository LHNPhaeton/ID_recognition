#include "jiaozheng.h"  

//===================判断点在矩形内===================,这里把贴边的去掉
bool InRectYesOrNo(CvPoint pt, CvRect rect)
{
	if (((pt.x >rect.x) && (pt.x <(rect.x + rect.width))) && ((pt.y >rect.y) && (pt.y <(rect.y + rect.height))))
		return true;
	else
		return false;
}

//=============计算两点之间的距离
float lenghtOf2P(CvPoint pt1, CvPoint pt2)
{
	return sqrt((float)(pt1.x - pt2.x)*(pt1.x - pt2.x) + (pt1.y - pt2.y)*(pt1.y - pt2.y));
}

void GetLinePara(Line &l)
{
	l.a = l.p1.y - l.p2.y;
	l.b = l.p2.x - l.p1.x;
	l.c = l.p1.x*l.p2.y - l.p1.y*l.p2.x;
}

//========================已知直线上两点，求直线的交点===================/
pPoint getCrossPoint(Line &l1, Line &l2)
{
	GetLinePara(l1);
	GetLinePara(l2);
	double D = l1.a*l2.b - l2.a*l1.b;
	pPoint p;
	p.x = (l1.b*l2.c - l2.b*l1.c) / D;
	p.y = (l1.c*l2.a - l2.c*l1.a) / D;
	return p;
}


/* 返回顶角在o点，起始边为os，终止边为oe的夹角(单位：弧度)
角度小于pi，返回正值
角度大于pi，返回负值
可以用于求线段之间的夹角
原理：
r = dotmultiply(s,e,o) / (dist(o,s)*dist(o,e))
r'= multiply(s,e,o)

r >= 1 angle = 0;
r <= -1 angle = -PI
-1<r<1 && r'>0 angle = arccos(r)
-1<r<1 && r'<=0 angle = -arccos(r)
*/
double angle(pPoint o, pPoint s, pPoint e)
{
	double cosfi, fi, norm;
	double dsx = s.x - o.x;
	double dsy = s.y - o.y;
	double dex = e.x - o.x;
	double dey = e.y - o.y;

	cosfi = dsx*dex + dsy*dey;
	norm = (dsx*dsx + dsy*dsy)*(dex*dex + dey*dey);
	cosfi /= sqrt(norm);

	if (cosfi >= 1.0) return 0;
	if (cosfi <= -1.0) return -3.1415926;

	fi = acos(cosfi);
	if (dsx*dey - dsy*dex>0) return fi;      // 说明矢量os 在矢量 oe的顺时针方向 
	return -fi;
}


//====================计算角度=========================/
//返回两条直线的交点和夹角
ptAndAngle computeIntersect(cv::Vec4i a, cv::Vec4i b)
{

	Line L1, L2;
	L1.p1.x = a[0];  L1.p1.y = a[1];  L1.p2.x = a[2];   L1.p2.y = a[3];
	L2.p1.x = b[0];  L2.p1.y = b[1];  L2.p2.x = b[2];   L2.p2.y = b[3];

	pPoint PointCross = getCrossPoint(L1, L2);
	if (showSteps)
	{
		//cout << "CrossPoint.x:" << PointCross.x << endl;
		//cout << "CrossPoint.y:" << PointCross.y << endl;
	}

	ptAndAngle pt;
	pt.ptf.x = PointCross.x;
	pt.ptf.y = PointCross.y;

	double pAngle = angle(PointCross, L1.p1, L2.p1);
	double temp = pAngle * 180 / 3.1415962;

	pt.Angle = temp;
	//两条直线夹角在70
	if (((temp >= LeftRightAngle) && (temp <= (90 + TopBottomAngle))) || ((temp >= -1 * (90 + TopBottomAngle)) && (temp <= -1* LeftRightAngle)))
	{
		if (showSteps)
		{
			//cout << "角度：  " << pAngle * 180 / 3.1415962 << endl;
		}
			
		return pt;
	}
	else 
	{
		pt.ptf.x = -100;
		pt.ptf.y = -100;
		return pt;
	}
}


//==================计算数组的最大和最小值=======================/
void GetMaxAndMin(double *arr, int n, double &max, double &min)
{
	int i = 0;
	if (n & 1)     // 奇数
	{
		max = min = arr[i++];
	}
	else
	{
		if (arr[0] > arr[1])
		{
			max = arr[0];
			min = arr[1];
		}
		else
		{
			max = arr[1];
			min = arr[0];
		}
		i += 2;
	}

	for (; i < n; i += 2)
	{
		if (arr[i] > arr[i + 1])
		{
			if (arr[i] > max)
				max = arr[i];
			if (arr[i + 1] < min)
				min = arr[i + 1];
		}
		else
		{
			if (arr[i + 1] > max)
				max = arr[i + 1];
			if (arr[i] < min)
				min = arr[i];
		}
	}
}


//===================按顺序给四点排序===============/
void sortCorners(std::vector<cv::Point2f>& corners, cv::Point2f center)
{
	std::vector<cv::Point2f> top, bot;

	for (unsigned int i = 0; i< corners.size(); i++){
		if (corners[i].y<center.y){
			top.push_back(corners[i]);
		}
		else{
			bot.push_back(corners[i]);
		}
	}

	cv::Point2f tl = top[0].x > top[1].x ? top[1] : top[0];
	cv::Point2f tr = top[0].x > top[1].x ? top[0] : top[1];
	cv::Point2f bl = bot[0].x > bot[1].x ? bot[1] : bot[0];
	cv::Point2f br = bot[0].x > bot[1].x ? bot[0] : bot[1];

	corners.clear();
	//注意以下存放顺序是顺时针，当时这里出错了，如果想任意顺序下文开辟的四边形矩阵注意对应
	corners.push_back(tl);
	corners.push_back(tr);
	corners.push_back(br);
	corners.push_back(bl);
}

//=======================计算所有线中的最长值=======================/
cv::Vec4i chooseLongest(vector<cv::Vec4i> lineT){
	float len = 0;
	for (unsigned int i = 0; i<lineT.size(); i++){
		CvPoint start_pt;   start_pt.x = lineT[i][0];   start_pt.y = lineT[i][1];
		CvPoint end_pt;     end_pt.x = lineT[i][2];     end_pt.y = lineT[i][3];
		float tempLen = lenghtOf2P(start_pt, end_pt);
		//cout << "templen:   " << tempLen << endl;
		if (len < tempLen)
		{
			len = tempLen;
		}
	}

	for (unsigned int i = 0; i<lineT.size(); i++)
	{
		CvPoint start_pt;   start_pt.x = lineT[i][0];   start_pt.y = lineT[i][1];
		CvPoint end_pt;     end_pt.x = lineT[i][2];     end_pt.y = lineT[i][3];
		float tempLen = lenghtOf2P(start_pt, end_pt);
		if (len == tempLen)
		{
			if (showSteps)
			{
				cout << "最长线的长度是：" << len << endl;
			}
				

			cv::Vec4i lineLonges;
			lineLonges[0] = lineT[i][0];
			lineLonges[1] = lineT[i][1];
			lineLonges[2] = lineT[i][2];
			lineLonges[3] = lineT[i][3];
			return lineLonges;
		}
	}

	return NULL;
}

/**/
//===============根据顶部或者底部线段，筛选出同角度下总长度最长的线段====================/
vector<cv::Vec4i> chooseLine(IplImage* src, vector<linePtAngle> lineTop)
{
	vector<cv::Vec4i> lines;
	if (lineTop.size() == 0)
		return lines;

	int sumTop[2 * TopBottomAngle] = { 0 };
	for (int jAngle = (-1*TopBottomAngle); jAngle<TopBottomAngle; jAngle++)
	{
		for (unsigned int i = 0; i < lineTop.size(); i++){
			if ((lineTop[i].angle == jAngle))
				sumTop[jAngle + TopBottomAngle] += lineTop[i].lineLength;
		}
	}

	int maxSumTop = sumTop[0];
	for (int jAngle = (-1 * TopBottomAngle); jAngle<TopBottomAngle; jAngle++)
	{
		if (maxSumTop<sumTop[jAngle + TopBottomAngle])
		{
			maxSumTop = sumTop[jAngle + TopBottomAngle];
		}
	}

	
	for (int jAngle = (-1 * TopBottomAngle); jAngle<TopBottomAngle; jAngle++)
	{
		if (maxSumTop == sumTop[jAngle + TopBottomAngle])
		{
			for (int j = 0; j<lineTop.size(); j++)
			{
				if ((jAngle == lineTop[j].angle) || ((jAngle + 1) == lineTop[j].angle) || ((jAngle - 1) == lineTop[j].angle)) 
				{
					//if (showSteps)
						//cvLine(src, lineTop[j].startPt, lineTop[j].endPt, CV_RGB(255, 255, 255), 6, CV_AA);
					cv::Vec4i v;
					v[0] = lineTop[j].startPt.x; v[1] = lineTop[j].startPt.y;
					v[2] = lineTop[j].endPt.x; v[3] = lineTop[j].endPt.y;

					lines.push_back(v);
				}
			}
		}
	}
	return lines;
}

/*
//===============根据顶部或者底部线段，筛选出同角度下总长度最长的线段====================/
vector<cv::Vec4i> chooseLine(IplImage* src, vector<linePtAngle> lineTop)
{	
	vector<cv::Vec4i> lines;
	if (lineTop.size() == 0)
		return lines;
	//第一步：确定所有线段都是首小于尾部(以X轴为参考计算)
	//for (unsigned int i = 0; i < lineTop.size(); i++)
	//{
	//	if (lineTop[i].startPt.x >lineTop[i].endPt.x)
	//	{
	//		CvPoint temp;
	//		temp.x = lineTop[i].startPt.x;     temp.y = lineTop[i].startPt.y;
	//		lineTop[i].startPt.x = lineTop[i].endPt.x;      lineTop[i].startPt.y = lineTop[i].endPt.y;
	//		lineTop[i].endPt.x = temp.x;        lineTop[i].endPt.y = temp.y;
	//	}
	//}

	//将平行，且两条线段中的一条首与一条尾相距很小时将这两天先合并
	vector<int> flag(lineTop.size(), 0);
	for (unsigned int i = 0; i < lineTop.size() - 1; i++)
	{
		for (unsigned int j = i + 1; j < lineTop.size(); j++)
		{
			if ((abs(lineTop[i].angle - lineTop[j].angle) <= 1) && (abs(lineTop[i].endPt.x - lineTop[j].startPt.x) <= 5) && (lineTop[i].startPt.x < lineTop[j].startPt.x)){
				//lineTop[i].startPt.x = lineTop[i].startPt.x;
				//lineTop[i].startPt.y = lineTop[i].startPt.y;

				//lineTop[i].endPt.x = lineTop[j].endPt.x;
				//lineTop[i].endPt.y = lineTop[j].endPt.y;
				lineTop[i].lineLength = sqrt((float)(lineTop[j].endPt.x - lineTop[i].startPt.x)*(lineTop[j].endPt.x - lineTop[i].startPt.x)
					+ (lineTop[j].endPt.y - lineTop[i].startPt.y)*(lineTop[j].endPt.y - lineTop[i].startPt.y));
			}
		}
	}

	int sumTop[2 * TopBottomAngle] = { 0 };
	for (int jAngle = (-1 * TopBottomAngle); jAngle<TopBottomAngle; jAngle++)
	{
		for (unsigned int i = 0; i < lineTop.size(); i++)
		{
			if ((lineTop[i].angle == jAngle))
				sumTop[jAngle + TopBottomAngle] += lineTop[i].lineLength;
		}
	}

	if (showSteps)
	{
		//for (int jAngle = -20; jAngle<20; jAngle++)
			//cout << sumTop[jAngle + 20] << " ";
	}

	int maxSumTop = sumTop[0];
	for (int jAngle = (-1 * TopBottomAngle); jAngle<TopBottomAngle; jAngle++)
	{
		if (maxSumTop<sumTop[jAngle + TopBottomAngle])
		{
			maxSumTop = sumTop[jAngle + TopBottomAngle];
		}
	}

	//if (showSteps)
		//cout << maxSumTop << endl;

	
	for (int jAngle = (-1 * TopBottomAngle); jAngle<TopBottomAngle; jAngle++)
	{
		if (maxSumTop == sumTop[jAngle + TopBottomAngle])
		{
			for (int j = 0; j<lineTop.size(); j++)
			{
				if ((jAngle == lineTop[j].angle) || ((jAngle + 1) == lineTop[j].angle) || ((jAngle - 1) == lineTop[j].angle))
				{
					//if (showSteps)
					//cvLine(src, lineTop[j].startPt, lineTop[j].endPt, CV_RGB(255, 255, 255), 6, CV_AA);
					Vec4i v;
					v[0] = lineTop[j].startPt.x; v[1] = lineTop[j].startPt.y;
					v[2] = lineTop[j].endPt.x; v[3] = lineTop[j].endPt.y;

					lines.push_back(v);
				}
			}
		}
	}
	return lines;
}*/


vector<cv::Vec4i> chooseLineLR(IplImage* src, vector<linePtAngle> lineTop)
{
	vector<cv::Vec4i> lines;
	if (lineTop.size() == 0)
		return lines;

	//===========垂直===============/
	int sumTop[2 * (90 - LeftRightAngle) + 1] = { 0 };
	for (int iAngle = -90; iAngle< -1*LeftRightAngle; iAngle++)
	{
		for (unsigned int i = 0; i < lineTop.size(); i++)
		{
			if (lineTop[i].angle == iAngle)
				sumTop[iAngle + 90] += lineTop[i].lineLength;
		}
	}
	for (int jAngle = LeftRightAngle; jAngle <= 90; jAngle++)
	{
		for (unsigned int i = 0; i < lineTop.size(); i++)
		{
			if (lineTop[i].angle == jAngle)
				sumTop[jAngle - 70] += lineTop[i].lineLength;
		}
	}

	/*
	if (showSteps)
	{
		for (int i = 0; i < 41; i++)
		{
			//cout << sumTop[i] << " ";
		}
			
	}*/

	int maxSumTop = sumTop[0];
	for (int i = 0; i < 2*(90 - LeftRightAngle)+1; i++)
	{
		if (maxSumTop<sumTop[i]) 
		{
			maxSumTop = sumTop[i];
		}
	}
	/*
	if (showSteps)
		cout << maxSumTop << endl;*/

	
	for (int iAngle = -90; iAngle< -1 * LeftRightAngle; iAngle++){
		if (maxSumTop == sumTop[iAngle + 90]){
			for (int j = 0; j<lineTop.size(); j++)
				if ((iAngle == lineTop[j].angle) || ((iAngle + 1) == lineTop[j].angle) || ((iAngle - 1) == lineTop[j].angle)){
					//if (showSteps)
						//cvLine(src, lineTop[j].startPt, lineTop[j].endPt, CV_RGB(255, 255, 255), 6, CV_AA);
					cv::Vec4i v;
					v[0] = lineTop[j].startPt.x; v[1] = lineTop[j].startPt.y;
					v[2] = lineTop[j].endPt.x; v[3] = lineTop[j].endPt.y;

					lines.push_back(v);
				}
		}
	}
	for (int jAngle = LeftRightAngle; jAngle <= 90; jAngle++){
		if (maxSumTop == sumTop[jAngle - 70]){
			for (int j = 0; j<lineTop.size(); j++)
				if ((jAngle == lineTop[j].angle) || ((jAngle + 1) == lineTop[j].angle) || ((jAngle - 1) == lineTop[j].angle)){
					//if (showSteps)
						//cvLine(src, lineTop[j].startPt, lineTop[j].endPt, CV_RGB(255, 255, 255), 6, CV_AA);
					cv::Vec4i v;
					v[0] = lineTop[j].startPt.x; v[1] = lineTop[j].startPt.y;
					v[2] = lineTop[j].endPt.x; v[3] = lineTop[j].endPt.y;

					lines.push_back(v);
				}
		}
	}
	return lines;
}

vector<cv::Vec4i> lineT;
vector<cv::Vec4i> lineB;
vector<cv::Vec4i> lineL;
vector<cv::Vec4i> lineR;
IplImage* res_im;
//===先调用vifLine 对 lineT，lineB, lineL,   lineR赋值。
int vifLine(IplImage * m_im)
{	
	IplImage * m_imResize = cvCreateImage(cvSize(ImgWidth, ImgHeight), 8, m_im->nChannels);
	//IplImage * m_imResize = cvCreateImage(cvSize(m_im->width, m_im->height), 8, m_im->nChannels);
	cvResize(m_im, m_imResize, CV_INTER_LINEAR);

	ntuple_list detected_lines;
	clock_t start, finish;
	double duration, rate;
	start = clock();
	//lsd函数处理灰度图像，需要进行转换
	IplImage* im_gray = cvCreateImage(cvSize(m_imResize->width, m_imResize->height), IPL_DEPTH_8U, 1);
	cvCvtColor(m_imResize, im_gray, CV_BGR2GRAY);
	image_double image = new_image_double(im_gray->width, im_gray->height);
	uchar* im_src = (uchar*)im_gray->imageData;
	int xsize = image->xsize;//宽度
	int ysize = image->ysize;//高度
	int y, x;

	for (y = 0; y < ysize; y++)
	{
		for (x = 0; x < xsize; x++)  //x是横坐标，y是纵坐标
		{
			image->data[y*xsize + x] = im_src[y*im_gray->widthStep + x];//im_gray是灰度图像，没有颜色通道
		}
	}
	cvReleaseImage(&im_gray);
	detected_lines = lsd(image);//detected_lines中存储提取直线的首位坐标及宽度，具体意义见说明文档
	free_image_double(image);
	finish = clock();

	if (detected_lines->size == 0)
		return -1;

	//cout << "检测到的直线条数： " << detected_lines->size << endl;//提取所有直线需要的总时间
	duration = (double)(finish - start) / CLOCKS_PER_SEC;
	//if (showSteps)
		//cout << "total time of extract lines is: " << duration << endl;//提取所有直线需要的总时间
	rate = duration / detected_lines->size;
	//if (showSteps)
		//cout << "time of extract per line is :   " << rate << endl;//提取一条直线需要的平均时间

	//将检测出的直线绘制在载入的彩色图像上
	int dim = detected_lines->dim;
	res_im = cvCreateImage(cvGetSize(m_imResize), 8, m_imResize->nChannels);
	cvCopy(m_imResize, res_im);
	cvReleaseImage(&m_imResize);

	vector<double> angle(detected_lines->size, 0);
	double angletemp;

	vector<linePtAngle> lineTop; vector<linePtAngle> lineBottom; vector<linePtAngle> lineLeft; vector<linePtAngle> lineRight;
	for (unsigned int j = 0; j < detected_lines->size; j++)
	{
		CvPoint start_pt = cvPoint((int)detected_lines->values[j*dim + 0], (int)detected_lines->values[j*dim + 1]);
		CvPoint end_pt = cvPoint((int)detected_lines->values[j*dim + 2], (int)detected_lines->values[j*dim + 3]);
		//cout << "线  " << j + 1 <<":\t"<< "起始坐标：" << start_pt.x << "\t" << start_pt.y << "\t" << "结束坐标：" << end_pt.x << "\t" << end_pt.y ;


		if (showSteps)//===========画出所有检测到的直线==============/
		{
			//cvLine(res_im, start_pt, end_pt, CV_RGB(255, 0, 0), 3, CV_AA);
		}
		  

		angletemp = (int)(atan((detected_lines->values[j*dim + 1] - detected_lines->values[j*dim + 3]) / (detected_lines->values[j*dim + 0] - detected_lines->values[j*dim + 2])) * 180 / 3.1416);
		angle[j] = angletemp;

		//if (showSteps)
			//cout << "\t角度：" << angle[j] << " \t" << "长度：" << lenghtOf2P(start_pt, end_pt) << endl;

		//===========根据上下左右四个方向，分别做筛选================/
		if (lenghtOf2P(start_pt, end_pt) > LineMinLength)    //筛选，长度大于20的线
		{
			int tempWidth = BoundWidth; //==========确定图片四周，的宽度内做筛选，这个宽度定为50.

			if (lenghtOf2P(start_pt, end_pt) > 30)    //左边界的线的长度需要大于长度大于30的线
			{
				CvRect rectL;
				rectL.x = 0;  rectL.y = 0;  rectL.width = tempWidth;  rectL.height = res_im->height;
				if ((InRectYesOrNo(start_pt, rectL) && InRectYesOrNo(end_pt, rectL)) && ((angle[j] < -1 * LeftRightAngle) || (angle[j] > LeftRightAngle))){
					if (showSteps){
						//cvLine(res_im,start_pt,end_pt,CV_RGB(j%255,(5*j)%255,(9*j)%255),4,CV_AA);
						//cvLine(res_im, start_pt, end_pt, CV_RGB(0, 255, 0), 1, CV_AA);
						//cout << "左边界直线的角度为 " << angle[j] << " 时， 直线序号为 " << j + 1 << endl;
					}

					linePtAngle temp;
					temp.startPt = start_pt;    temp.endPt = end_pt;    temp.angle = angle[j];   temp.lineLength = lenghtOf2P(start_pt, end_pt);
					lineLeft.push_back(temp);
				}
			}

			CvRect rectR;
			rectR.x = res_im->width - tempWidth;  rectR.y = 0;  rectR.width = tempWidth;  rectR.height = res_im->height;
			if ((InRectYesOrNo(start_pt, rectR) && InRectYesOrNo(end_pt, rectR)) && ((angle[j] < -1 * LeftRightAngle) || (angle[j] > LeftRightAngle))){
				if (showSteps){
					//cvLine(res_im,start_pt,end_pt,CV_RGB(j%255,(5*j)%255,(9*j)%255),4,CV_AA);
					//cvLine(res_im, start_pt, end_pt, CV_RGB(0, 255, 0), 1, CV_AA);
					//cout << "右边界直线的角度为 " << angle[j] << " 时， 直线序号为 " << j + 1 << endl;
				}

				linePtAngle temp;
				temp.startPt = start_pt;    temp.endPt = end_pt;    temp.angle = angle[j];   temp.lineLength = lenghtOf2P(start_pt, end_pt);
				lineRight.push_back(temp);
			}

			CvRect rectT;
			rectT.x = 0;  rectT.y = 0;  rectT.width = res_im->width;  rectT.height = tempWidth;
			if ((InRectYesOrNo(start_pt, rectT) && InRectYesOrNo(end_pt, rectT)) && ((angle[j] > -1 * TopBottomAngle) && (angle[j] < TopBottomAngle))){
				if (showSteps){
					//cvLine(res_im,start_pt,end_pt,CV_RGB(j%255,(5*j)%255,(9*j)%255),4,CV_AA);
					///cvLine(res_im, start_pt, end_pt, CV_RGB(0, 255, 0), 1, CV_AA);
					//cout << "上边界直线的角度为 " << angle[j] << " 时， 直线序号为 " << j + 1 << endl;
				}

				linePtAngle temp;
				temp.startPt = start_pt;    temp.endPt = end_pt;    temp.angle = angle[j];   temp.lineLength = lenghtOf2P(start_pt, end_pt);
				lineTop.push_back(temp);

			}

			CvRect rectB;
			rectB.x = 0;  rectB.y = res_im->height - tempWidth;  rectB.width = res_im->width;  rectB.height = tempWidth;
			if ((InRectYesOrNo(start_pt, rectB) && InRectYesOrNo(end_pt, rectB)) && ((angle[j] > -1 * TopBottomAngle) && (angle[j] < TopBottomAngle))){
				if (showSteps){
					//cvLine(res_im,start_pt,end_pt,CV_RGB(j%255,(5*j)%255,(9*j)%255),4,CV_AA);
					//cvLine(res_im, start_pt, end_pt, CV_RGB(0, 255, 0), 1, CV_AA);
					//cout << "下边界直线的角度为 " << angle[j] << " 时， 直线序号为 " << j + 1 << endl;
				}
				linePtAngle temp;
				temp.startPt = start_pt;    temp.endPt = end_pt;    temp.angle = angle[j];   temp.lineLength = lenghtOf2P(start_pt, end_pt);
				lineBottom.push_back(temp);
			}
		}

	}

	/*
	if (lineLeft.size() == 1)
		lineLeft.clear();
	if (lineRight.size() == 1)
		lineRight.clear();
	if (lineTop.size() == 1)
		lineTop.clear();
	if (lineBottom.size() == 1)
		lineBottom.clear();*/

	vector<double>(angle).swap(angle);

	//==================对 线段进行筛选处理  
	lineT = chooseLine(res_im, lineTop);
	lineB = chooseLine(res_im, lineBottom);
	lineL = chooseLineLR(res_im, lineLeft);
	lineR = chooseLineLR(res_im, lineRight);

	vector<linePtAngle>(lineTop).swap(lineTop);
	vector<linePtAngle>(lineBottom).swap(lineBottom);
	vector<linePtAngle>(lineLeft).swap(lineLeft);
	vector<linePtAngle>(lineRight).swap(lineRight);

	/*
	char *vifAllLine;
	vifAllLine = (char *)malloc(5 * sizeof(char));
	for (int i = 0; i<4; i++){
		vifAllLine[i] = '1';
	}

	if ((lineT.size() == 0))   vifAllLine[0] = '0';
	if ((lineB.size() == 0))   vifAllLine[1] = '0';
	if ((lineL.size() == 0))   vifAllLine[2] = '0';
	if ((lineR.size() == 0))   vifAllLine[3] = '0';

	return vifAllLine;*/

	if ((lineT.size() == 0))   return -1;
	if ((lineB.size() == 0))   return -1;
	if ((lineL.size() == 0))   return -1;
	if ((lineR.size() == 0))   return -1;

	return 0;
}

/*
IplImage *jiaozheng(IplImage *res_im){
	//============如果四条边没有找全========//
	if ((lineT.size() == 0) || (lineB.size() == 0) || (lineL.size() == 0) || (lineR.size() == 0)){
		return res_im;
	}

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
	//needed for visualization only//这里是将检测的线调整到延长至全屏，即射线的效果，其实可以不必这么做
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
	if (showSteps){
		for (unsigned int i = 0; i<linesAll.size(); i++){
			cv::Vec4i v = linesAll[i];
			cvLine(res_im, cvPoint(v[0], v[1]), cvPoint(v[2], v[3]), CV_RGB(0, 255, 255), 1, CV_AA);//四条延长的边界线
		}
	}

	std::vector<ptAndAngle> corners;//线的交点存储
	for (unsigned int i = 0; i<linesAll.size() - 1; i++){
		for (unsigned int j = i + 1; j<linesAll.size(); j++){
			//===========线的交点=============/
			ptAndAngle pt = computeIntersect(linesAll[i], linesAll[j]);
			if ((pt.ptf.x >= -10 && pt.ptf.y >= -10)
				&& (pt.ptf.x<res_im->width + 20) && (pt.ptf.y<res_im->height + 20)) {
				corners.push_back(pt);
			}
		}
	}

	vector<cv::Vec4i>(linesAll).swap(linesAll);

	if (showSteps){
	for (unsigned int i = 0; i<corners.size(); i++){
	//cout << corners[i].ptf.x << " " << corners[i].ptf.y << endl;
	//cvCircle(res_im, corners[i].ptf, 3, CV_RGB(255, 255, 0), 2);
		}
	}

	std::vector<cv::Point2f> cornersPt;
	for (unsigned int i = 0; i<corners.size(); i++){
		cornersPt.push_back(corners[i].ptf);
	}

	std::vector<cv::Point2f> approx;
	cv::approxPolyDP(cv::Mat(cornersPt), approx, cv::arcLength(cv::Mat(cornersPt), true)*0.02, false);
	if (approx.size() != 4){
		std::cout << "The object is not quadrilateral（四边形）!" << std::endl;
		return res_im;
	}

	if (showSteps){  //=============画出四边形拟合的顶点结果==================/
		for (unsigned int i = 0; i<approx.size(); i++){
			cvCircle(res_im, approx[i], 3, CV_RGB(0, 0, 255), 3);
			printf("[ %f\t %f ]\n", approx[i].x, approx[i].y);
		}
	}

	std::vector<cv::Point2f>(approx).swap(approx);

	cv::Point2f center(0, 0);
	//get mass center
	for (unsigned int i = 0; i < corners.size(); i++)
	{
		center += cornersPt[i];
	}
	center *= (1. / corners.size());

	std::vector<ptAndAngle>(corners).swap(corners);

	if (showSteps)
		cvCircle(res_im, center, 3, CV_RGB(255, 255, 0), 6);

	//========对四个顶点按照顺序排序=============/
	sortCorners(cornersPt, center);

	std::vector<cv::Point2f>(cornersPt).swap(cornersPt);


	//==========确定宽高==============/
	cv::Mat quad = cv::Mat::zeros(281, 442, CV_8UC3);//设定校正过的图片从320*240变为300*220

	//corners of the destination image
	std::vector<cv::Point2f> quad_pts;
	quad_pts.push_back(cv::Point2f(0, 0));
	quad_pts.push_back(cv::Point2f(quad.cols, 0));//(220,0)
	quad_pts.push_back(cv::Point2f(quad.cols, quad.rows));//(220,300)
	quad_pts.push_back(cv::Point2f(0, quad.rows));

	std::vector<cv::Point2f>(quad_pts).swap(quad_pts);

	// Get transformation matrix
	cv::Mat transmtx = cv::getPerspectiveTransform(cornersPt, quad_pts); //求源坐标系（已畸变的）与目标坐标系的转换矩阵

	Mat src(res_im);
	// Apply perspective transformation透视转换
	cv::warpPerspective(src, quad, transmtx, quad.size());

	IplImage *dst = cvCreateImage(cvSize(quad.cols, quad.rows), 8, res_im->nChannels);
	uchar *dataDst = (uchar *)(dst->imageData);

	for (size_t nrow = 0; nrow < quad.rows; nrow++)
	{
		for (size_t ncol = 0; ncol < quad.cols; ncol++)
		{
			Vec3i bgr = quad.at<Vec3b>(nrow, ncol);//用Vec3b也行 
			for (size_t k = 0; k<quad.channels(); k++){
				dataDst[nrow*(dst->widthStep) + ncol*(dst->nChannels) + k] = bgr.val[k];
			}

		}

	}

	if (showSteps){
		//cvNamedWindow("dst1", 1);
		//cvShowImage("dst1", dst);

		cvNamedWindow("Image1", 1);
		cvShowImage("Image1", res_im);

		//cvWaitKey(0);
	}

	cvReleaseImage(&res_im);

	return dst;

}*/
