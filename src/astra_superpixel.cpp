#include <time.h>
#include <stdio.h>

#include "gSLICr_Lib/gSLICr.h"
#include "NVTimer.h"

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/opencv.hpp"
#include <astra_utils.h>


void load_image(const Mat& inimg, gSLICr::UChar4Image* outimg)
{
	gSLICr::Vector4u* outimg_ptr = outimg->GetData(MEMORYDEVICE_CPU);

	for (int y = 0; y < outimg->noDims.y;y++)
		for (int x = 0; x < outimg->noDims.x; x++)
		{
			int idx = x + y * outimg->noDims.x;
			outimg_ptr[idx].b = inimg.at<Vec3b>(y, x)[0];
			outimg_ptr[idx].g = inimg.at<Vec3b>(y, x)[1];
			outimg_ptr[idx].r = inimg.at<Vec3b>(y, x)[2];
		}
}

void load_image(const gSLICr::UChar4Image* inimg, Mat& outimg)
{
	const gSLICr::Vector4u* inimg_ptr = inimg->GetData(MEMORYDEVICE_CPU);

	for (int y = 0; y < inimg->noDims.y; y++)
		for (int x = 0; x < inimg->noDims.x; x++)
		{
			int idx = x + y * inimg->noDims.x;
			outimg.at<Vec3b>(y, x)[0] = inimg_ptr[idx].b;
			outimg.at<Vec3b>(y, x)[1] = inimg_ptr[idx].g;
			outimg.at<Vec3b>(y, x)[2] = inimg_ptr[idx].r;
		}
}

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

	// gSLICr settings
	gSLICr::objects::settings my_settings;
	my_settings.img_size.x = 640;
	my_settings.img_size.y = 480;
	my_settings.no_segs = 2000;
	my_settings.spixel_size = 16;
	my_settings.coh_weight = 0.6f;
	my_settings.no_iters = 5;
	my_settings.color_space = gSLICr::XYZ; // gSLICr::CIELAB for Lab, or gSLICr::RGB for RGB
	my_settings.seg_method = gSLICr::GIVEN_SIZE; // or gSLICr::GIVEN_NUM for given number
	my_settings.do_enforce_connectivity = true; // whether or not run the enforce connectivity step

	// instantiate a core_engine
	gSLICr::engines::core_engine* gSLICr_engine = new gSLICr::engines::core_engine(my_settings);

	// gSLICr takes gSLICr::UChar4Image as input and out put
	gSLICr::UChar4Image* in_img = new gSLICr::UChar4Image(my_settings.img_size, true, true);
	gSLICr::UChar4Image* out_img = new gSLICr::UChar4Image(my_settings.img_size, true, true);

	Size s(my_settings.img_size.x, my_settings.img_size.y);
	Mat oldFrame, frame;
	Mat boundry_draw_frame; boundry_draw_frame.create(s, CV_8UC3);

    StopWatchInterface *my_timer; sdkCreateTimer(&my_timer);
    
	int key; int save_count = 0;

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
	// namedWindow("RGB Image", CV_WINDOW_AUTOSIZE);
	//namedWindow("Depth Image", CV_WINDOW_AUTOSIZE);
	//namedWindow("Filtered Depth Image", CV_WINDOW_AUTOSIZE);
	for (double index = 1.0; continueornot; index+=1.0)
	{
		rc = streamColor.readFrame(&frameColor);
		if(rc == STATUS_OK)
		{
			const Mat tImageRGB(frameColor.getHeight(), frameColor.getWidth(), CV_8UC3, (void*)frameColor.getData());
			cvtColor(tImageRGB, imRGB, CV_RGB2BGR);

			resize(imRGB, frame, s);
		
			load_image(frame, in_img);
	        
	        sdkResetTimer(&my_timer); sdkStartTimer(&my_timer);
			gSLICr_engine->Process_Frame(in_img);
	        sdkStopTimer(&my_timer); 
	        cout<<"\rsegmentation in:["<<sdkGetTimerValue(&my_timer)<<"]ms"<<flush;
	        
			gSLICr_engine->Draw_Segmentation_Result(out_img);
			
			load_image(out_img, boundry_draw_frame);
			imshow("segmentation", boundry_draw_frame);

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