#pragma once

#include <iostream>
#include <algorithm>
#include <fstream>
#include <chrono>
#include <OpenNI.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace std;
using namespace cv;
using namespace openni;

void showdevice()
{
	// get device information
	Array<DeviceInfo> aDeviceList;
	OpenNI::enumerateDevices(&aDeviceList);
	cout<< "Number of device(s) connected: "<< aDeviceList.getSize() <<endl;

	for (int i = 0; i< aDeviceList.getSize(); ++i)
	{
		cout <<"Device: "<< i << endl;
		const DeviceInfo& rDevInfo = aDeviceList[i];
		cout <<"Device name: "	<< rDevInfo.getName() 			<<endl;
		cout <<"Device Id: " 	<< rDevInfo.getUsbProductId() 	<<endl;
		cout <<"Vendor name: "	<< rDevInfo.getVendor()			<<endl;
		cout <<"Vendor Id:"		<< rDevInfo.getUsbVendorId()	<<endl;
		cout <<"Device URI: "	<< rDevInfo.getUri()			<<endl;
	}
}

Status initstream(Status& rc, Device& astra, VideoStream& streamDepth, VideoStream& streamColor)
{
	rc = STATUS_OK;

	rc = streamDepth.create(astra, SENSOR_DEPTH);
	if(rc == STATUS_OK)
	{
		VideoMode mModeDepth;
		mModeDepth.setResolution(640, 480);
		mModeDepth.setFps(30);
		//mModeDepth.setPixelFormat(PIXEL_FORMAT_DEPTH_1_MM);
		mModeDepth.setPixelFormat(PIXEL_FORMAT_DEPTH_100_UM);

		streamDepth.setVideoMode(mModeDepth);
		streamDepth.setMirroringEnabled(false); 

		rc = streamDepth.start();
		if (rc != STATUS_OK)
		{
			cerr << "Can not read depth data: "<< OpenNI::getExtendedError() <<endl;
			streamDepth.destroy();
		}
	}

	else
	{
		cerr << "can not create depth data: "<< OpenNI::getExtendedError() <<endl;
	}

	rc = streamColor.create(astra, SENSOR_COLOR);
	if (rc == STATUS_OK)
	{
		VideoMode mModeColor;
		mModeColor.setResolution(640, 480);
		mModeColor.setFps(30);
		mModeColor.setPixelFormat(PIXEL_FORMAT_RGB888);

		streamColor.setVideoMode(mModeColor);
		streamColor.setMirroringEnabled(false);

		rc = streamColor.start();
		if (rc != STATUS_OK)
		{
			cerr << "can not open color data stream: "<< OpenNI::getExtendedError() <<endl;
			streamColor.destroy();
		}
	}
	else
	{
		cerr << "can not create color data: "<< OpenNI::getExtendedError() <<endl;
	}

	if  (!streamColor.isValid() || !streamDepth.isValid())
	{
		cerr << "color or depth data invalid" << endl;
		rc = STATUS_ERROR;
		return rc;
	}

	if (astra.isImageRegistrationModeSupported(IMAGE_REGISTRATION_DEPTH_TO_COLOR))
	{
		astra.setImageRegistrationMode(IMAGE_REGISTRATION_DEPTH_TO_COLOR);
	}

	return rc;
}