#include <astra_utils.h>


int main(int argc, char **argv)
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

	VideoFrameRef frameDepth;
	VideoFrameRef frameColor;
	namedWindow("RGB Image", CV_WINDOW_AUTOSIZE);
	//namedWindow("Depth Image", CV_WINDOW_AUTOSIZE);
	//namedWindow("Filtered Depth Image", CV_WINDOW_AUTOSIZE);
	for (double index = 1.0; continueornot; index+=1.0)
	{
		rc = streamDepth.readFrame(&frameDepth);
		if(rc == STATUS_OK)
		{
			imD = cv::Mat(frameDepth.getHeight(), frameDepth.getWidth(), CV_16UC1, (void*)frameDepth.getData());
			imD.convertTo(im8u,CV_8U,0.00390625);
			bilateralFilter(im8u,imD_filtered,30,50,50);
			//imshow("Depth Image", imD);
			//imshow("Filtered Depth Image", imD_filtered);
		}

		rc = streamColor.readFrame(&frameColor);
		if(rc == STATUS_OK)
		{
			const Mat tImageRGB(frameColor.getHeight(), frameColor.getWidth(), CV_8UC3, (void*)frameColor.getData());
			cvtColor(tImageRGB, imRGB, CV_RGB2BGR);
			imshow("RGB Image", imRGB);
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