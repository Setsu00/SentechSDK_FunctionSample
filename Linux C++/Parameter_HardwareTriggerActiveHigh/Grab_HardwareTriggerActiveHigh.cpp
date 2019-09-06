﻿/*
	Hardware Trigger (Active High): Set camera to hardware trigger mode for Line0 as input and active high (Trigger activation as rising edge).
*/

#define ENABLED_ST_GUI

#include <StApi_TL.h>
#ifdef ENABLED_ST_GUI
#include <StApi_GUI.h>
#include <iomanip>	//std::setprecision
#endif

using namespace StApi;
using namespace std;
const uint64_t nCountOfImagesToGrab = 100;

int main(int /* argc */, char ** /* argv */)
{
	try
	{
		CStApiAutoInit objStApiAutoInit;
		CIStSystemPtr pIStSystem(CreateIStSystem(StSystemVendor_Sentech));
		CIStDevicePtr pIStDevice(pIStSystem->CreateFirstIStDevice());
		cout << "Device=" << pIStDevice->GetIStDeviceInfo()->GetDisplayName() << endl;


		// ==============================================================================================================
		// Demostration of Setting Hardware Trigger ON with active high

#ifdef ENABLED_ST_GUI
		CIStImageDisplayWndPtr pIStImageDisplayWnd(CreateIStWnd(StWindowType_ImageDisplay));
#endif
		CIStDataStreamPtr pIStDataStream(pIStDevice->CreateIStDataStream(0));

		// Create NodeMap pointer for accessing parameters
		GenApi::CNodeMapPtr pNodeMapCameraParam(pIStDevice->GetRemoteIStPort()->GetINodeMap());

		// Set Line0 to input
		GenApi::CEnumerationPtr pIEnumLineSelector(pNodeMapCameraParam->GetNode("LineSelector"));
		*pIEnumLineSelector = "Line0";
		GenApi::CEnumerationPtr pIEnumLine0Mode(pNodeMapCameraParam->GetNode("LineMode"));
		*pIEnumLine0Mode = "Input";

		// Switch on Trigger Mode(IEnumeration).
		GenApi::CEnumerationPtr pIEnumTrigMode(pNodeMapCameraParam->GetNode("TriggerMode"));
		*pIEnumTrigMode = "On";

		// Set Trigger Source to Line0 as Hardware input
		GenApi::CEnumerationPtr pIEnumTrigSource(pNodeMapCameraParam->GetNode("TriggerSource"));
		*pIEnumTrigSource = "Line0";

		// Set trigger activation to active high
		GenApi::CEnumerationPtr pIEnumTrigActivation(pNodeMapCameraParam->GetNode("TriggerActivation"));
		*pIEnumTrigActivation = "RisingEdge";

		// ==============================================================================================================

		pIStDataStream->StartAcquisition(nCountOfImagesToGrab);
		pIStDevice->AcquisitionStart();
		while (pIStDataStream->IsGrabbing())
		{

			// Retrieve a frame right after a software trigger is sent.
			CIStStreamBufferPtr pIStStreamBuffer(pIStDataStream->RetrieveBuffer(5000));
			if (pIStStreamBuffer->GetIStStreamBufferInfo()->IsImagePresent())
			{
				IStImage *pIStImage = pIStStreamBuffer->GetIStImage();

#ifdef ENABLED_ST_GUI
				stringstream ss;
				ss << pIStDevice->GetIStDeviceInfo()->GetDisplayName();
				ss << "  ";
				ss << pIStImage->GetImageWidth() << " x " << pIStImage->GetImageHeight();
				ss << "  ";
				ss << fixed << std::setprecision(2) << pIStDataStream->GetCurrentFPS();
				ss << "[fps]";
                GenICam::gcstring strText(ss.str().c_str());
				pIStImageDisplayWnd->SetUserStatusBarText(strText);

				if (!pIStImageDisplayWnd->IsVisible())
				{
					pIStImageDisplayWnd->Show(NULL, StWindowMode_Modaless);
					pIStImageDisplayWnd->SetPosition(0, 0, pIStImage->GetImageWidth(), pIStImage->GetImageHeight());
				}

				pIStImageDisplayWnd->RegisterIStImage(pIStImage);
				processEventGUI();
#else
				cout << "BlockId=" << pIStStreamBuffer->GetIStStreamBufferInfo()->GetFrameID()
					<< " Size:" << pIStImage->GetImageWidth() << " x " << pIStImage->GetImageHeight()
					<< " First byte =" << (uint32_t)*(uint8_t*)pIStImage->GetImageBuffer() << endl;
#endif
			}
			else
			{
				cout << "Image data does not exist" << endl;
			}
		}
		pIStDevice->AcquisitionStop();
		pIStDataStream->StopAcquisition();


		// ==============================================================================================================
		// Set Software Trigger OFF after using

		// Switch off Trigger Mode(IEnumeration) after acquiring.
		*pIEnumTrigMode = "Off";

		// ==============================================================================================================
	}
	catch (const GenICam::GenericException &e)
	{
		cerr << endl << "An exception occurred." << endl << e.GetDescription() << endl;
	}

	cout << endl << "Press Enter to exit." << endl;
	while (cin.get() != '\n');

	return(0);
}
