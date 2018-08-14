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

void horizontalProjectionMat(cv::Mat srcImg, int *index);//ˮƽͶӰ 
void verticalProjectionMat(cv::Mat srcImg, int *index);//��ֱͶӰ  
void font_location(cv::Mat &img, int *h_index, int *v_index, int max);
bool is_blur(int *h_index, int *v_index, int max);
double cal_coeff(cv::Mat matA, cv::Mat matB);//����������������ϵ��
cv::Mat img_cut(cv::Mat img, int *h_index, int *v_index, int num_ch);//�ü�ͼ���е�num_ch�����ֻ�����
bool num_recognition(cv::Mat srcImg, int *idnum);//ʶ�����֤����
bool char_recognition(cv::Mat srcImg, string *ch);//ʶ�����֤�еĺ��� ����������ַ
void img_correct(cv::Mat &img);//����б�����ֽ��ж��βü���������
void input_cut(cv::Mat srcImg, cv::Mat &idimg_name, cv::Mat &idimg_sex,
	cv::Mat &idimg_birth, cv::Mat &idimg_addr, cv::Mat &idimg_num);//������ͼ����вü�
bool get_ID_info(cv::Mat srcImg);

#endif