#include "id_card_font.h"

void horizontalProjectionMat(cv::Mat srcImg, int *index)//水平投影  
{
	cv::Mat binImg = srcImg.clone();
	//blur(srcImg, binImg, Size(3, 3));
	threshold(binImg, binImg, 0, 255, CV_THRESH_OTSU);

	//int th = threshold(binImg, binImg, 0, 255, CV_THRESH_OTSU);
	//threshold(binImg, binImg, th-1, 255, CV_THRESH_BINARY);
	//imshow("binImg", binImg);
	int perPixelValue = 0;//每个像素的值  
	int width = srcImg.cols;
	int height = srcImg.rows;
	int* projectValArry = new int[height];//创建一个储存每行白色像素个数的数组  
	memset(projectValArry, 0, height * 4);//初始化数组  
	for (int col = 0; col < height; col++)//遍历每个像素点  
	{
		for (int row = 0; row < width; row++)
		{
			perPixelValue = binImg.at<uchar>(col, row);
			if (perPixelValue == 0)//如果是白底黑字  
			{
				projectValArry[col]++;
			}
		}
	}
	cv::Mat horizontalProjectionMat(height, width, CV_8UC1);//创建画布  
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			perPixelValue = 255;
			horizontalProjectionMat.at<uchar>(i, j) = perPixelValue;//设置背景为白色  
		}
	}
	for (int i = 0; i < height; i++)//水平直方图  
	{
		for (int j = 0; j < projectValArry[i]; j++)
		{
			perPixelValue = 0;
			horizontalProjectionMat.at<uchar>(i, width - 1 - j) = perPixelValue;//设置直方图为黑色  
		}
	}
#ifdef _DEBUG
	imshow("horizontalProjectionMat", horizontalProjectionMat);
#endif
	vector<cv::Mat> roiList;//用于储存分割出来的每个字符  
	int startIndex = 0;//记录进入字符区的索引  
	int endIndex = 0;//记录进入空白区域的索引
	int j = 0;

	bool inBlock = false;//是否遍历到了字符区内
	for (int i = 0; i < srcImg.rows; i++)
	{
		if (!inBlock && projectValArry[i] > 1)//进入字符区  
		{
			inBlock = true;
			startIndex = i;
			index[j] = i;
			j++;
		}
		else if (inBlock && projectValArry[i] < 1)//进入空白区  
		{
			inBlock = false;
			endIndex = i;
			index[j] = i;
			j++;
		}
	}
	delete[] projectValArry;
}


void verticalProjectionMat(cv::Mat srcImg, int *index)//垂直投影  
{
	cv::Mat binImg = srcImg.clone();
	//blur(srcImg, binImg, Size(3, 3));
	threshold(binImg, binImg, 0, 255, CV_THRESH_OTSU);
	int perPixelValue;//每个像素的值  
	int width = srcImg.cols;
	int height = srcImg.rows;
	int* projectValArry = new int[width];//创建用于储存每列白色像素个数的数组  
	memset(projectValArry, 0, width * 4);//初始化数组  
	for (int col = 0; col < width; col++)
	{
		for (int row = 0; row < height; row++)
		{
			perPixelValue = binImg.at<uchar>(row, col);
			if (perPixelValue == 0)//如果是白底黑字  
			{
				projectValArry[col]++;
			}
		}
	}
	cv::Mat verticalProjectionMat(height, width, CV_8UC1);//垂直投影的画布  
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			perPixelValue = 255;  //背景设置为白色  
			verticalProjectionMat.at<uchar>(i, j) = perPixelValue;
		}
	}
	for (int i = 0; i < width; i++)//垂直投影直方图  
	{
		for (int j = 0; j < projectValArry[i]; j++)
		{
			perPixelValue = 0;  //直方图设置为黑色    
			verticalProjectionMat.at<uchar>(height - 1 - j, i) = perPixelValue;
		}
	}
#ifdef _DEBUG
	imshow("verticalProjectionMat", verticalProjectionMat);
#endif
	int j = 0;
	int startIndex = 0;//记录进入字符区的索引  
	int endIndex = 0;//记录进入空白区域的索引  
	bool inBlock = false;//是否遍历到了字符区内  
	for (int i = 0; i < srcImg.cols; i++)//cols=width  
	{
		if (!inBlock && projectValArry[i] > 1)//进入字符区  
		{
			inBlock = true;
			startIndex = i;
			index[j] = i;
			j++;
		}
		else if (projectValArry[i] < 1 && inBlock)//进入空白区  
		{
			endIndex = i;
			inBlock = false;
			index[j] = i;
			j++;
		}
	}
	delete[] projectValArry;
}


void font_location(cv::Mat &img, int *h_index, int *v_index, int max)
{
	int h_max = 0;
	int v_max = 0;
	for (int i = 0; i < max; i++)
	{
		if (h_index[i] != 0)
			h_max = i;
		if (v_index[i] != 0)
			v_max = i;
	}
	h_max += 1;
	v_max += 1;
#ifdef _DEBUG
	cout << "h_max=" << h_max << "," << "v_max=" << v_max << endl;
#endif
	/*for (int k = 0; k < h_max; k++)
	{
		cout << h_index[k] << ",";
	}
	cout << endl;
	for (int k = 0; k < v_max; k++)
	{
		cout << v_index[k] << ",";
	}*/

	//rectangle(srcImg, Point(v_index[0], h_index[0]), Point(v_index[1], h_index[1]), Scalar(0, 0, 255), 3, 8, 0);//用点画矩形窗
	//line(img, Point(300, 300), Point(758, 300), Scalar(89, 90, 90), 3);
	/*for (int i = 0; i < v_max; i++)
	{
		line(img, Point(v_index[i], h_index[0]), Point(v_index[i], h_index[7]), Scalar(0, 255, 0), 1);
	}
	for (int i = 0; i < h_max; i++)
	{
		line(img, Point(v_index[0], h_index[i]), Point(v_index[3], h_index[i]), Scalar(0, 255, 0), 1);
	}*/
}


bool is_blur(int *h_index, int *v_index, int max)
{
	int h_max = 0, v_max = 0;
	bool B = 0;
	for (int i = 0; i < max; i++)
	{
		if (h_index[i] != 0)
			h_max = i;
		if (v_index[i] != 0)
			v_max = i;
	}
	h_max += 1;
	v_max += 1;
#ifdef _DEBUG
	cout << "h_max=" << h_max << "," << "v_max=" << v_max << endl;
#endif
	if (v_max == 36)
		B = 1;
	else if (v_max == 4)
		B = 1;
	else 
		B = 0;
	return B;
}


void input_cut(cv::Mat srcImg, cv::Mat &idimg_name, cv::Mat &idimg_sex, cv::Mat &idimg_birth, cv::Mat &idimg_addr, cv::Mat &idimg_num)
{
	idimg_name = srcImg(cv::Range(30, 60), cv::Range(20, 76));
	idimg_sex = srcImg(cv::Range(65, 95), cv::Range(20, 76));
	idimg_birth = srcImg(cv::Range(95, 130), cv::Range(20, 76));
	idimg_addr = srcImg(cv::Range(130, 165), cv::Range(20, 76));
	idimg_num = srcImg(cv::Range(214, 250), cv::Range(140, 390));
}


double cal_coeff(cv::Mat matA, cv::Mat matB)
{
	//计算两个相同大小矩阵的二维相关系数
	double coeff = 0;

	double Amean2 = 0;
	double Bmean2 = 0;
	for (int m = 0; m < matA.rows; m++) {
		uchar* dataA = matA.ptr<uchar>(m);
		uchar* dataB = matB.ptr<uchar>(m);
		for (int n = 0; n < matA.cols; n++) {
			Amean2 = Amean2 + dataA[n];
			Bmean2 = Bmean2 + dataB[n];
		}
	}
	Amean2 = Amean2 / (matA.rows * matA.cols);
	Bmean2 = Bmean2 / (matB.rows * matB.cols);

	double Cov = 0;
	double Astd = 0;
	double Bstd = 0;
	for (int m = 0; m < matA.rows; m++) {
		uchar* dataA = matA.ptr<uchar>(m);
		uchar* dataB = matB.ptr<uchar>(m);
		for (int n = 0; n < matA.cols; n++) {
			//协方差
			Cov = Cov + (dataA[n] - Amean2) * (dataB[n] - Bmean2);
			//A的方差
			Astd = Astd + (dataA[n] - Amean2) * (dataA[n] - Amean2);
			//B的方差
			Bstd = Bstd + (dataB[n] - Bmean2) * (dataB[n] - Bmean2);
		}
	}
	coeff = Cov / (sqrt(Astd * Bstd));

	return coeff;
}


cv::Mat img_cut(cv::Mat img, int *h_index, int *v_index, int num_ch)
{
	num_ch = 2 * num_ch - 1;
	cv::Size sz(32, 32);
	cv::Mat imgr = cv::Mat(sz, CV_8U);
	if ((h_index[1] - h_index[0] < 2)||(v_index[num_ch] - v_index[num_ch - 1])<2)
	{
		return imgr;
	}

	cv::Mat imgd = cv::Mat(h_index[1] - h_index[0], v_index[num_ch] - v_index[num_ch - 1], CV_8U);
	imgd = img(cv::Range(h_index[0], h_index[1]), cv::Range(v_index[num_ch - 1], v_index[num_ch]));
	
	const int max2 = 10;
	int h_index2[max2] = { 0 };
	int v_index2[max2] = { 0 };

	horizontalProjectionMat(imgd, h_index2);
	verticalProjectionMat(imgd, v_index2);
	if (h_index2[1] == 0)
		h_index2[1] = h_index[1] - h_index[0];

	
	if ((h_index2[1] - h_index2[0] > 4) && (v_index[num_ch] - v_index[num_ch - 1])>2)
	{
		cv::Mat img2 = cv::Mat(h_index2[1] - h_index2[0], v_index[num_ch] - v_index[num_ch - 1], CV_8U);
		img2 = imgd(cv::Range(h_index2[0], h_index2[1]), cv::Range(0, v_index[num_ch] - v_index[num_ch - 1]));
		resize(img2, imgr, sz);
	}
	
	return  imgr;
}


bool num_recognition(cv::Mat srcImg, int *idnum)
{
	const int max = 50;
	int h_index[max] = { 0 };
	int v_index[max] = { 0 };
	string img_path;
	const int num = 11;
	string img_name[num] = { "0.jpg", "1.jpg", "2.jpg", "3.jpg", "4.jpg", "5.jpg",
		"6.jpg", "7.jpg", "8.jpg", "9.jpg", "10.jpg" };
	bool B = 0;
	cv::Size sz(32, 32);

	horizontalProjectionMat(srcImg, h_index);
	verticalProjectionMat(srcImg, v_index);
	
	B = is_blur(h_index, v_index, max);//判断身份号码是不是18位
	if (!B)
	{
#ifdef _DEBUG
		cout << "无法识别" << endl;
#endif
	}
	else
	{
		cv::Mat imgd = cv::Mat(sz, CV_8U);
		cv::Mat imgd2 = cv::Mat(sz, CV_8U);
		double coeff = 0, temp;
		int id_num[18] = { 0 };

		for (int i = 0; i < 18; i++)
		{
			imgd2 = img_cut(srcImg, h_index, v_index, i + 1);
			temp = 0.5;
			for (int j = 0; j < num; j++)
			{
				img_path = "..\\image\\";
				img_path += img_name[j];
				imgd = cv::imread(img_path, 0);
				coeff = cal_coeff(imgd, imgd2);
				if (coeff>temp)
				{
					temp = coeff;
					id_num[i] = j;
				}
			}
		}
#ifdef _DEBUG
		cout << "身份证号码是：";
		for (int j = 0; j < 18; j++)
			cout << id_num[j];
		cout << endl;
#endif
		idnum = id_num;
	}
	return B;
}


bool char_recognition(cv::Mat srcImg, string *ch)
{
	const int max = 20;
	int h_index[max] = { 0 };
	int v_index[max] = { 0 };
	string img_path;
	const int num = 4;
	string img_name[num] = { "a.jpg", "b.jpg", "c.jpg", "d.jpg"};
	cv::Size sz(32, 32);
	bool B = 0;
	horizontalProjectionMat(srcImg, h_index);
	verticalProjectionMat(srcImg, v_index);

	B = is_blur(h_index, v_index, max);
	if (!B)
	{
#ifdef _DEBUG
		cout << "无法识别" << endl;
#endif
	}
	else
	{
		cv::Mat imgd = cv::Mat(sz, CV_8U);
		cv::Mat imgd2 = cv::Mat(sz, CV_8U);
		double coeff = 0, temp;
		int c = 0;
		string cr[4] = { "名", "别", "生", "址" };

		imgd2 = img_cut(srcImg, h_index, v_index, 2);
		temp = 0.6;
		for (int j = 0; j < num; j++)
		{
			img_path = "..\\image\\";
			img_path += img_name[j];
			imgd = cv::imread(img_path, 0);
			coeff = cal_coeff(imgd, imgd2);
			if (coeff>temp)
			{
				temp = coeff;
				c = j;
			}
		}
#ifdef _DEBUG
		cout << "汉字是: " << cr[c] << endl;
#endif
		*ch = cr[c];
	}
	return B;
}


void img_correct(cv::Mat &img)
{
	const int max = 40;
	int h_index[max] = { 0 };
	int v_index[max] = { 0 };
	

	horizontalProjectionMat(img, h_index);
	verticalProjectionMat(img, v_index);

	int num_ch = 10;
	num_ch = 2 * num_ch - 1;
	cv::Size sz(32, 32);
	cv::Mat img1 = cv::Mat(h_index[1] - h_index[0], v_index[num_ch] - v_index[num_ch - 1], CV_8U);
	img1 = img(cv::Range(h_index[0], h_index[1]), cv::Range(v_index[num_ch - 1], v_index[num_ch]));
	imshow("img1", img1);
	const int max2 = 4;
	int h_index2[max2] = { 0 };
	int v_index2[max2] = { 0 };

	horizontalProjectionMat(img1, h_index2);
	verticalProjectionMat(img1, v_index2);
	if (h_index2[1] == 0)
		h_index2[1] = h_index[1] - h_index[0];

	cv::Mat img2 = cv::Mat(h_index2[1] - h_index2[0], v_index[num_ch] - v_index[num_ch - 1], CV_8U);
	img2 = img1(cv::Range(h_index2[0], h_index2[1]), cv::Range(0, v_index[num_ch] - v_index[num_ch - 1]));
	
	for (int i = 0; i < 4; i++)
	{
		cout << h_index[i] << endl;
		cout << h_index2[i] << endl;
	}

	cv::Mat imgr = cv::Mat(sz, CV_8U);
	resize(img2, imgr, sz);
	imshow("img2", imgr);
}


bool get_ID_info(cv::Mat srcImg)
{
	bool B = 0, B1 = 0, B2 = 0, B3 = 0, B4 = 0, B5 = 0;
	int id_num[18] = { 0 };
	string ch[4];
	string cr[4] = { "名", "别", "生", "址" };
	cv::Mat idimg_name, idimg_sex, idimg_birth, idimg_addr, idimg_num;
	input_cut(srcImg, idimg_name, idimg_sex, idimg_birth, idimg_addr, idimg_num);

#ifdef _DEBUG
	imshow("srcImg", srcImg);
	imshow("idimg_name", idimg_name);
	imshow("idimg_sex", idimg_sex);
	imshow("idimg_birth", idimg_birth);
	imshow("idimg_addr", idimg_addr);
	imshow("idimg_num", idimg_num);
#endif

	B1 = num_recognition(idimg_num, id_num);
	B2 = char_recognition(idimg_name, ch);
	B3 = char_recognition(idimg_sex, ch+1);
	B4 = char_recognition(idimg_birth, ch+2);
	B5 = char_recognition(idimg_addr, ch+3);

#ifdef _DEBUG
	cout << "汉字识别结果是：";
	for (int i = 0; i < 4; i++)
		cout << ch[i];
	cout << endl;
	//cout << "B1=" << B1 << "," << "B2=" << B2 << endl;
#endif

	/*if (B1 && B2 && B3 && B4 && B5)
		B = 1;
	else
		B = 0;*/
	if ((ch[0] == cr[0]) && (ch[1] == cr[1]) && (ch[2] == cr[2]) && (ch[3] == cr[3]))
		B = 1;
	else
		B = 0;
	return B;
}
