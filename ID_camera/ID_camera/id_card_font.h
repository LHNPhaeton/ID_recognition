#ifndef _ID_CARD_FONT_H_
#define _ID_CARD_FONT_H_


#include"opencv2/opencv.hpp"
#include <opencv2/core/core.hpp> 
#include <opencv2/highgui/highgui.hpp>  
#include <opencv2/imgproc/imgproc.hpp> 
#include"string"
#include"iostream"
#include"cmath"
#include"vector"
#include <stdio.h> 
#include "time.h"

//using namespace cv;

using namespace std;

void horizontalProjectionMat(cv::Mat srcImg, int *index);//水平投影 
void verticalProjectionMat(cv::Mat srcImg, int *index);//垂直投影  
void font_location(cv::Mat &img, int *h_index, int *v_index, int max);
bool is_blur(int *h_index, int *v_index, int max);
double cal_coeff(cv::Mat matA, cv::Mat matB);//计算两个矩阵的相关系数
cv::Mat img_cut(cv::Mat img, int *h_index, int *v_index, int num_ch);//裁剪图像中第num_ch个汉字或数字
bool num_recognition(cv::Mat srcImg, int *idnum);//识别身份证号码
bool char_recognition(cv::Mat srcImg, string *ch);//识别身份证中的汉字 名、别、生、址
void img_correct(cv::Mat &img);//对倾斜的文字进行二次裁剪，测试用
void input_cut(cv::Mat srcImg, cv::Mat &idimg_name, cv::Mat &idimg_sex,
	cv::Mat &idimg_birth, cv::Mat &idimg_addr, cv::Mat &idimg_num);//对输入图像进行裁剪
bool get_ID_info(cv::Mat srcImg);

#endif