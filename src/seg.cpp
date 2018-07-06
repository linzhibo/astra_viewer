#include <opencv2/opencv.hpp>
#include <iostream>
#include <astra_utils.h>
using namespace std;
using namespace cv;

// variable declaration

int g_std = 1;
int g_mean = -8;

Mat kernel = (Mat_<float>(3,3)<<
			g_std, g_std, g_std,
			g_std, g_mean, g_std,
			g_std, g_std, g_std);
Mat kernel1 = Mat::ones(3,3,CV_8UC1);

Mat imgLaplacian, sharp, imgResult,bw, dist, dist_8u, markers, mark ;

vector<Vec3b> colors;

//#####################
void window_pause();

Mat seg(Mat src);

Mat seg(Mat src)
{

	// for (int x = 0; x < src.rows; x++)
	// {
	// 	for (int y = 0; y < src.cols; y++)
	// 	{
	// 		if(src.at<Vec3b>(x,y) == Vec3b(255,255,255))
	// 			src.at<Vec3b>(x,y) = Vec3b(0,0,0);
	// 	}
	// }
	imshow("source Image", src);
	//window_pause();

	
	sharp = src;
	filter2D(sharp, imgLaplacian, CV_32F,kernel);
	src.convertTo(sharp,CV_32F);
	imgResult = sharp - 1.5*imgLaplacian;
	 //imshow( "Laplace Filtered Image", imgLaplacian );
	 //imshow("new sharped image", imgResult);
	// waitKey(0);

	imgResult.convertTo(imgResult,CV_8UC3);
	imgLaplacian.convertTo(imgLaplacian,CV_8UC3);
	imshow( "Laplace Filtered Image", imgLaplacian );
	imshow("new sharped image", imgResult);
	//window_pause();

	src = imgResult;
	cvtColor(src, bw, COLOR_BGR2GRAY);
	threshold(bw,bw,40,255,THRESH_BINARY | THRESH_OTSU);
	imshow("Binary Image", bw);
	//window_pause();

	distanceTransform(bw, dist, CV_DIST_L2, 3);

	normalize(dist, dist,0,1., NORM_MINMAX);
	imshow("Distance Tranform image ", dist);
	//window_pause();

	threshold(dist, dist, .4, 1., THRESH_BINARY);
	dilate(dist,dist,kernel1);
	//imshow("Peaks", dist);
	//window_pause();

	dist.convertTo(dist_8u, CV_8U);
	vector<std::vector<Point>> contours;
	findContours(dist_8u,contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

	markers = Mat::zeros(dist.size(), CV_32SC1);
	for (size_t i = 0; i < contours.size(); i++)
	{
		drawContours(markers, contours, static_cast<int>(i), Scalar::all(static_cast<int>(i)+1), -1);
	}
	circle(markers, Point(5,5), 3, CV_RGB(255,255,255), -1);
	//imshow("Markers", markers*10000);
	//window_pause();

	watershed(src,markers);
	mark = Mat::zeros(markers.size(), CV_8UC1);

	markers.convertTo(mark, CV_8UC1);
    bitwise_not(mark, mark);
    //imshow("Markers_v2", mark); // uncomment this if you want to see how the mark
                                  // image looks like at that point
    // Generate random colors

    for (size_t i = 0; i < contours.size(); i++)
    {
        int b = theRNG().uniform(0, 255);
        int g = theRNG().uniform(0, 255);
        int r = theRNG().uniform(0, 255);

        colors.push_back(Vec3b((uchar)b, (uchar)g, (uchar)r));
    }
    // Create the result image
    Mat dst = Mat::zeros(markers.size(), CV_8UC3);
    // Fill labeled objects with random colors
    for (int i = 0; i < markers.rows; i++)
    {
        for (int j = 0; j < markers.cols; j++)
        {
            int index = markers.at<int>(i,j);
            if (index > 0 && index <= static_cast<int>(contours.size()))
                dst.at<Vec3b>(i,j) = colors[index-1];
            else
                dst.at<Vec3b>(i,j) = Vec3b(0,0,0);
        }
    }
    // Visualize the final image
    imshow("Segmented image", dst);
    //window_pause();
	return dst;
}
void window_pause()
{
	waitKey(0);
	destroyAllWindows();
}

int main()
{
	cout << endl << ".........." << endl;
	cout << "Openning Astra..." << endl;

	Status rc = STATUS_OK;
	OpenNI::initialize();
	showdevice();
	Device astra;
	const char * deviceURL = openni::ANY_DEVICE;
	rc = astra.open(deviceURL);

	VideoStream streamColor;
	VideoStream streamDepth;

	if(initstream(rc, astra, streamDepth, streamColor) == STATUS_OK)
		cout << "Open Astra successfully!" << endl;
	else
	{
		cout << "Open Astra failed! " <<endl;
		return 0;
	}

	cv::Mat imRGB, imD, im8u,imD_filtered;
	bool continueornot = true;

	VideoFrameRef frameColor;
	VideoFrameRef frameDepth;
	//namedWindow("segmented Image", CV_WINDOW_AUTOSIZE);
	//namedWindow("Depth Image", CV_WINDOW_AUTOSIZE);
	//namedWindow("Filtered Depth Image", CV_WINDOW_AUTOSIZE);
	for (double index = 1.0; continueornot; index+=1.0)
	{
		// rc = streamDepth.readFrame(&frameDepth);
		// if(rc == STATUS_OK)
		// {
		// 	imD = cv::Mat(frameDepth.getHeight(), frameDepth.getWidth(), CV_16UC1, (void*)frameDepth.getData());
		// 	imD.convertTo(imD,CV_8U,0.00390625);
		// 	cvtColor(imD,imD,CV_GRAY2RGB);
		// 	//cout<<imD.at<Vec3b>(50,50)<<endl;
		// 	imD = seg(imD);

		// }

		rc = streamColor.readFrame(&frameColor);
		if(rc == STATUS_OK)
		{
			const Mat tImageRGB(frameColor.getHeight(), frameColor.getWidth(), CV_8UC3, (void*)frameColor.getData());
			cvtColor(tImageRGB, imRGB, CV_RGB2BGR);
			imRGB = seg(imRGB);
			//imshow("segmented Image", imRGB);
		}
		char c = cv::waitKey(5);
		switch(c)
		{
			case 'q':
			case 27:
				continueornot = false;
				break;
			case 'p':
				cv::waitKey(0);
				break;
			default:
				break;
		}
	}

	cv::destroyAllWindows();

	return 0;
}
