#include "stdio.h"
#include "iostream"
#include "opencv2/core/core.hpp"
//#include "opencv2/nonfree/features2d.hpp"
#include "opencv2/highgui/highgui.hpp"
//#include "opencv2/legacy/legacy.hpp"
#include "opencv2/opencv.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <stdlib.h>
#include <ctype.h>

using namespace cv;
using namespace std;

int l1, l2, s;
void my_mouse_callback(int mevent, int x, int y, int flags, void *param);
//保存视频
//VideoWriter writer("video.avi",CV_FOURCC('P','I','M','1'),30.0,Size(640,480));
int vmin = 10, vmax = 256, smin = 30;
int cs_flag = 0;
Rect selection;
Rect trackWindow;
int main()
{
	VideoCapture capture(0);
	namedWindow("原图像", 1);
	cvSetMouseCallback("原图像", my_mouse_callback, NULL);//鼠标动作

													   //char imgName[1000]; 

	Mat frame;
	int count = 0;
	while (capture.isOpened())
	{
		count++;
		//读取下一帧
		if (!capture.read(frame))
		{
			cerr << "读取视频失败" << endl;
			return -1;
		}

		cout << "count=" << count << endl;
		//Mat frameROI(frame,Rect(160,80,frame.cols/3,frame.rows*2/3));
		Mat frameROI(frame, Rect(150, 80, frame.cols / 2, frame.rows / 2));
		Mat temp, img_gray;
		frameROI.copyTo(temp);
		if (cs_flag<6)
		{
			cvtColor(temp, img_gray, CV_BGR2GRAY);
			GaussianBlur(img_gray, img_gray, Size(3, 3), 1.5, 1.5);


			//mask 
# if 1
			Mat mask;
			mask.create(Size(320, 240), CV_8UC1);
			mask.setTo(0);
			//画图 
			Point pt[2][4];
			pt[0][0].x = 100;
			pt[0][0].y = 120;
			pt[0][1].x = 15;
			pt[0][1].y = 193;
			pt[0][2].x = 253;
			pt[0][2].y = 193;
			pt[0][3].x = 163;
			pt[0][3].y = 120;

			const Point* ppt[1] = { pt[0] };
			int npt[] = { 4 };

			polylines(mask, ppt, npt, 1, true, CV_RGB(0, 0, 0), 2, 8);
			fillPoly(mask, ppt, npt, 1, CV_RGB(255, 255, 255), 8);


# endif
			//增加多个窗口计算灰度均值
			Mat temp1, temp2, temp3, temp4, temp5;
			//NO.1
			Mat tempROI_1(img_gray, Rect(100, 0, temp.cols / 3, temp.rows / 5));
			tempROI_1.copyTo(temp1);

			//NO.2
			Mat tempROI_2(img_gray, Rect(100, 48, temp.cols / 3, temp.rows / 5));
			tempROI_2.copyTo(temp2);
			//NO.3
			Mat tempROI_3(img_gray, Rect(100, 96, temp.cols / 3, temp.rows / 5));
			tempROI_3.copyTo(temp3);
			//NO.4
			Mat tempROI_4(img_gray, Rect(100, 144, temp.cols / 3, temp.rows / 5));
			tempROI_4.copyTo(temp4);
			//NO.5
			Mat tempROI_5(img_gray, Rect(100, 192, temp.cols / 3, temp.rows / 5));
			tempROI_5.copyTo(temp5);
			//计算五个子窗口的灰度均值与方差
			Mat m1, m2, m3, m4, m5, s1, s2, s3, s4, s5;
			meanStdDev(temp1, m1, s1);
			meanStdDev(temp2, m2, s2);
			meanStdDev(temp3, m3, s3);
			meanStdDev(temp4, m4, s4);
			meanStdDev(temp5, m5, s5);

			double *t1 = (double *)m1.data;
			double *t2 = (double *)m2.data;
			double *t3 = (double *)m3.data;
			double *t4 = (double *)m4.data;
			double *t5 = (double *)m5.data;

			double *k1 = (double *)s1.data;
			double *k2 = (double *)s2.data;
			double *k3 = (double *)s3.data;
			double *k4 = (double *)s4.data;
			double *k5 = (double *)s5.data;

			//对t1~5按从小到大排序，并做投票删选

			double t;
			//t1 比较
			if (*t1>*t2)
			{
				t = *t1;
				*t1 = *t2;
				*t2 = t;
			}
			if (*t1>*t3)
			{
				t = *t1;
				*t1 = *t3;
				*t3 = t;
			}
			if (*t1>*t4)
			{
				t = *t1;
				*t1 = *t4;
				*t4 = t;
			}
			if (*t1>*t5)
			{
				t = *t1;
				*t1 = *t5;
				*t5 = t;
			}
			// t2
			if (*t2>*t3)
			{
				t = *t2;
				*t2 = *t3;
				*t3 = t;
			}
			if (*t2>*t4)
			{
				t = *t2;
				*t2 = *t4;
				*t4 = t;
			}
			if (*t2>*t5)
			{
				t = *t2;
				*t2 = *t5;
				*t5 = t;
			}
			// t3
			if (*t3>*t4)
			{
				t = *t3;
				*t3 = *t4;
				*t4 = t;
			}
			if (*t3>*t5)
			{
				t = *t3;
				*t3 = *t5;
				*t5 = t;
			}
			// t4
			if (*t4>*t5)
			{
				t = *t4;
				*t4 = *t5;
				*t5 = t;
			}

			double j = ((*t2) + (*t3) + (*t4));
			double i = j / 3;

			double k;
			//k1 比较
			if (*k1>*k2)
			{
				k = *k1;
				*k1 = *k2;
				*k2 = k;
			}
			if (*k1>*k3)
			{
				k = *k1;
				*k1 = *k3;
				*k3 = k;
			}
			if (*k1>*k4)
			{
				k = *k1;
				*k1 = *k4;
				*k4 = k;
			}
			if (*k1>*k5)
			{
				k = *k1;
				*k1 = *k5;
				*k5 = k;
			}
			// k2
			if (*k2>*k3)
			{
				k = *k2;
				*k2 = *k3;
				*k3 = k;
			}
			if (*k2>*k4)
			{
				k = *k2;
				*k2 = *k4;
				*k4 = k;
			}
			if (*k2>*k5)
			{
				k = *k2;
				*k2 = *k5;
				*k5 = k;
			}
			// k3
			if (*k3>*k4)
			{
				k = *k3;
				*k3 = *k4;
				*k4 = k;
			}
			if (*k3>*k5)
			{
				k = *k3;
				*k3 = *k5;
				*k5 = k;
			}
			// k4
			if (*k4>*k5)
			{
				k = *k4;
				*k4 = *k5;
				*k5 = k;
			}
			double n = ((*k2) + (*k3) + (*k4));
			double l = n / 3;

			cout << "i=" << i << endl;
			cout << "l=" << l << endl;
			double x = 0.0;


# if 1

			// 分多个情况分割灰度图像 
			// i,灰度均值；l,均方差；
			if (i<1.5*l)
			{
				x = i;
			}
			if (i<3 * l)
			{
				x = i - 0.5*l;
			}
			if (i<5 * l)
			{
				x = i - l;
			}
			if (i<8 * l)
			{
				x = i - 2 * l;
			}
			if (i<12 * l)
			{
				x = i - l;
			}
			else
				x = i - 4 * l;
#endif	
			//	c1 = (*a1)-(*b1)*(*b1)/(*a1);
			Mat binary1, binary2;
			/*double x=( (i)-(l*l)/(i)); */
			cout << "x=" << x << endl;
			threshold(img_gray, binary1, x, 255, THRESH_TRUNC);
			imshow("binary1", binary1);


			//二次分割
#if 1
			Mat temp11, temp22, temp33, temp44, temp55;
			//NO.1
			Mat tempROI_11(binary1, Rect(100, 0, temp.cols / 3, temp.rows / 5));
			tempROI_11.copyTo(temp11);

			//NO.2
			Mat tempROI_22(binary1, Rect(100, 48, temp.cols / 3, temp.rows / 5));
			tempROI_22.copyTo(temp22);
			//NO.3
			Mat tempROI_33(binary1, Rect(100, 96, temp.cols / 3, temp.rows / 5));
			tempROI_33.copyTo(temp33);
			//NO.4
			Mat tempROI_44(binary1, Rect(100, 144, temp.cols / 3, temp.rows / 5));
			tempROI_44.copyTo(temp44);
			//NO.5
			Mat tempROI_55(binary1, Rect(100, 192, temp.cols / 3, temp.rows / 5));
			tempROI_55.copyTo(temp55);
			/*  显示五个子窗口

			imshow("1",temp1);
			imshow("2",temp2);
			imshow("3",temp3);
			imshow("4",temp4);
			imshow("5",temp5);
			waitKey(1);
			*/
			//计算五个子窗口的灰度均值与方差
			Mat m11, m22, m33, m44, m55, s11, s22, s33, s44, s55;
			meanStdDev(temp11, m11, s11);
			meanStdDev(temp22, m22, s22);
			meanStdDev(temp33, m33, s33);
			meanStdDev(temp44, m44, s44);
			meanStdDev(temp55, m55, s55);

			double *t11 = (double *)m11.data;
			double *t22 = (double *)m22.data;
			double *t33 = (double *)m33.data;
			double *t44 = (double *)m44.data;
			double *t55 = (double *)m55.data;

			double *k11 = (double *)s11.data;
			double *k22 = (double *)s22.data;
			double *k33 = (double *)s33.data;
			double *k44 = (double *)s44.data;
			double *k55 = (double *)s55.data;

			/*
			cout<<"t1="<<*t1<<endl;
			cout<<"t2="<<*t2<<endl;
			cout<<"t3="<<*t3<<endl;
			cout<<"t4="<<*t4<<endl;
			cout<<"t5="<<*t5<<endl;

			cout<<"k1="<<*k1<<endl;
			cout<<"k2="<<*k2<<endl;
			cout<<"k3="<<*k3<<endl;
			cout<<"k4="<<*k4<<endl;
			cout<<"k5="<<*k5<<endl;

			*/
			//对t1~5按从小到大排序，并做投票删选

			double t0;
			//t1 比较
			if (*t11>*t22)
			{
				t0 = *t11;
				*t11 = *t22;
				*t22 = t0;
			}
			if (*t11>*t33)
			{
				t0 = *t11;
				*t11 = *t33;
				*t33 = t0;
			}
			if (*t11>*t44)
			{
				t0 = *t11;
				*t11 = *t44;
				*t44 = t0;
			}
			if (*t11>*t55)
			{
				t0 = *t11;
				*t11 = *t55;
				*t55 = t0;
			}
			// t2
			if (*t22>*t33)
			{
				t0 = *t22;
				*t22 = *t33;
				*t33 = t0;
			}
			if (*t22>*t44)
			{
				t0 = *t22;
				*t22 = *t44;
				*t44 = t0;
			}
			if (*t22>*t55)
			{
				t0 = *t22;
				*t22 = *t55;
				*t55 = t0;
			}
			// t3
			if (*t33>*t44)
			{
				t0 = *t33;
				*t33 = *t44;
				*t44 = t0;
			}
			if (*t33>*t55)
			{
				t0 = *t33;
				*t33 = *t55;
				*t55 = t0;
			}
			// t4
			if (*t44>*t55)
			{
				t0 = *t44;
				*t44 = *t55;
				*t55 = t0;
			}

			double j0 = ((*t22) + (*t33) + (*t44));
			double i0 = j0 / 3;

			double k0;
			//k1 比较
			if (*k11>*k22)
			{
				k0 = *k11;
				*k11 = *k22;
				*k22 = k0;
			}
			if (*k11>*k33)
			{
				k0 = *k11;
				*k11 = *k33;
				*k33 = k0;
			}
			if (*k11>*k44)
			{
				k0 = *k11;
				*k11 = *k44;
				*k44 = k0;
			}
			if (*k11>*k55)
			{
				k0 = *k11;
				*k11 = *k55;
				*k55 = k0;
			}
			// k2
			if (*k22>*k33)
			{
				k0 = *k22;
				*k22 = *k33;
				*k33 = k0;
			}
			if (*k22>*k44)
			{
				k0 = *k22;
				*k22 = *k44;
				*k44 = k0;
			}
			if (*k22>*k55)
			{
				k0 = *k22;
				*k22 = *k55;
				*k55 = k0;
			}
			// k3
			if (*k33>*k44)
			{
				k0 = *k33;
				*k33 = *k44;
				*k44 = k0;
			}
			if (*k33>*k55)
			{
				k0 = *k33;
				*k33 = *k55;
				*k55 = k0;
			}
			// k4
			if (*k44>*k55)
			{
				k0 = *k44;
				*k44 = *k55;
				*k55 = k0;
			}
			double n0 = ((*k22) + (*k33) + (*k44));
			double l0 = n0 / 3;

			double thresh = 0.0;
			cout << "i0=" << i0 << endl;
			cout << "l0=" << l0 << endl;
# if 1

			// 分多个情况分割灰度图像 
			// i,灰度均值；l,均方差；

			if (i0<1.5*l0)		thresh = i0;

			if (i0<3 * l0)		thresh = i0 - 0.5*l0;

			if (i0<5 * l0)			thresh = i0 - l0;

			if (i0<8 * l)			thresh = i0 - 2 * l0;

			if (i0<12 * l0)		thresh = i0 - 3 * l0;

			else
				thresh = 3 * (i0) / 4 - 4 * l0;
#endif	

			cout << "thresh=" << thresh << endl;
			threshold(binary1, binary2, thresh, 255, THRESH_BINARY_INV);

# if 1
			Mat pyr1, pyr2;

			// 向下采样，去掉噪声

			pyrDown(binary2, pyr1, Size((binary2.cols + 1) / 2, (binary2.rows + 1) / 2));

			pyrDown(pyr1, pyr2, Size((pyr1.cols + 1) / 2, (pyr1.rows + 1) / 2));


			erode(pyr2, pyr2, (3, 3));
			erode(pyr2, pyr2, (3, 3));

			dilate(pyr2, pyr2, (3, 3)); // 做膨胀操作，消除目标的不连续空洞
			dilate(pyr2, pyr2, (3, 3));
			dilate(pyr2, pyr2, (3, 3));
			dilate(pyr2, pyr2, (3, 3));

			pyrUp(pyr2, pyr1, Size((pyr2.cols) * 2, (pyr2.rows) * 2));
			pyrUp(pyr1, binary2, Size((pyr1.cols) * 2, (pyr1.rows) * 2));

#endif
# endif
			Mat binary;
			threshold(binary2, binary, thresh / 2, 255, THRESH_BINARY);
			imshow("二值化", binary);

			Mat img_contours;
			binary.copyTo(img_contours, mask);
			imshow("img_contours", img_contours);

			vector<vector<Point> > contours;
			vector<Vec4i> hierarchy;

			findContours(img_contours, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));


			vector<vector<Point> > contours_poly(contours.size());
			vector<Rect> boundRect(contours.size());
			/*	if(!contours.size())
			{
			continue;
			}*/
			for (int i = 0; i < contours.size(); i++)
			{
				approxPolyDP(Mat(contours[i]), contours_poly[i], 3, true);
				boundRect[i] = boundingRect(Mat(contours_poly[i]));
			}
			int idx = -1;
			for (int i = 0; i< contours.size(); i++)
			{



				l1 = abs(boundRect[i].tl().x - boundRect[i].br().x);
				l2 = abs(boundRect[i].tl().y - boundRect[i].br().y);
				s = l1*l2;
				if ((s>500) && (s<12000))//排除面积过
				{
					if (idx == -1)  //第一次找到一个面积大于1000的轮廓
					{
						idx = i;
					}
					else   //第二次找到，并做比较
					{
						// 做比较 看y值大小
						if (boundRect[idx].tl().y < boundRect[i].tl().y)
						{
							idx = i;
						}
					}
				}
			}
			if (idx != -1)
			{
				Point2d a(145 + boundRect[idx].tl().x, 55 + boundRect[idx].tl().y);
				Point2d b(150 + boundRect[idx].br().x, 70 + boundRect[idx].br().y);

				//rectangle( frame, a,b,CV_RGB(255,0,0), 2, 8, 0 );
				cs_flag++;
				Rect sel(boundRect[idx].tl().x, 10 + boundRect[idx].tl().x, l1, l2);//换成合适的搜索窗口
				selection = sel;
# if 0
				// 提取车辆原图

				Point2d tl;
				tl = boundRect[idx].tl();

				double x0 = 145 + tl.x - l1 / 8;
				double y0 = 40 + tl.y - l1 / 12;

				double length = 1.25*l1;
				double heigth = l1;


				Mat dst, img;
				if (((x0)>0) && ((y0)>0))
				{
					Mat roi(frame, Rect(x0, y0, length, heigth));//容易出错的地方
					roi.copyTo(img);


					cout << "x0=" << x0 << endl;
					cout << "y0=" << y0 << endl;
					cout << "length=" << length << endl;
					cout << "length=" << heigth << endl;

					imshow("img", img);

					/*	sprintf(imgName,"image[%d].jpg",count);
					imwrite(imgName,img);*/
					//void Sobel(InputArray src, OutputArray dst, int ddepth, int xorder, int yorder, int ksize=3, double scale=1,					double delta=0, int borderType=BORDER_DEFAULT );
					//Sobel(img,dst,CV_8U,0,1,3,1,0,BORDER_DEFAULT);

				}
#endif
				//屏幕显示
				string words = "FCW";
				putText(frame, words, Point(20, 50),
					FONT_HERSHEY_SIMPLEX, 2, Scalar(0, 0, 255));
				idx = -1;
			}
		}
		// camshift跟踪
# if 1
		if (cs_flag>3)
		{

			int hsize = 16;
			float hranges[] = { 0,180 };
			const float* phranges = hranges;

			Mat cs, hsv, hue, maskcs, hist, histimg = Mat::zeros(640, 480, CV_8UC3), backproj;

			temp.copyTo(cs);
			cvtColor(cs, hsv, COLOR_BGR2HSV);
			cout << "cs_flag=" << cs_flag << endl;
			if (cs_flag>4)
			{
				int _vmin = vmin, _vmax = vmax;
				inRange(hsv, Scalar(0, smin, MIN(_vmin, _vmax)),
					Scalar(180, 256, MAX(_vmin, _vmax)), maskcs);
				int ch[] = { 0, 0 };
				hue.create(hsv.size(), hsv.depth());
				mixChannels(&hsv, 1, &hue, 1, ch, 1);

				Mat roi(hue, selection), maskroi(maskcs, selection);
				calcHist(&roi, 1, 0, maskroi, hist, 1, &hsize, &phranges);
				normalize(hist, hist, 0, 255, CV_MINMAX);

				trackWindow = selection;// 替换

				calcBackProject(&hue, 1, 0, hist, backproj, &phranges);
				backproj &= maskcs;
				RotatedRect trackBox = CamShift(backproj, trackWindow,
					TermCriteria(CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 10, 1));

				if (trackWindow.area() <= 1)
				{
					cs_flag = 0;
				}
				ellipse(temp, trackBox, Scalar(0, 0, 255), 2, CV_AA);//换成矩形
																	 //rectangle( image, trackWindow,CV_RGB(255,0,0), 2, 8, 0 );
			}
		}

#endif

		imshow("temp", temp);
		imshow("原图像", frame);

		waitKey(60);
		/******************************保存视频
		Mat sp(frame);
		writer <<sp;
		*********************************/
	}
	return 0;
}
//鼠标事件
void my_mouse_callback(int mevent, int x, int y, int flags, void *param)
{
	switch (mevent)
	{
	case CV_EVENT_LBUTTONDOWN:
		system("pause");
		break;
	default:
		break;
	}
	return;
}