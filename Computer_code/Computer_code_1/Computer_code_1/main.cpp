#include<opencv2\opencv.hpp>
using namespace cv;
Mat frame;

Mat  tou = frame;
int main()
{
	VideoCapture capture(0);
	while (1)
	{
	
		capture >> frame;

		imshow("voide", frame);
		waitKey(30);
	}
	return 0;
}