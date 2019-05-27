/*
	Strobe Out: Set Line2 as Strobe Out with output 1000us signal. (Output 1000us signal when camera exposure active.)
	* For STC-MB/MCS Series only
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
		// Demostration of Setting Line2 as Strobe Out

#ifdef ENABLED_ST_GUI
		CIStImageDisplayWndPtr pIStImageDisplayWnd(CreateIStWnd(StWindowType_ImageDisplay));
#endif
		CIStDataStreamPtr pIStDataStream(pIStDevice->CreateIStDataStream(0));

		// Create NodeMap pointer for accessing parameters
		GenApi::CNodeMapPtr pNodeMapCameraParam(pIStDevice->GetRemoteIStPort()->GetINodeMap());

		// Set Line2 to output
		GenApi::CEnumerationPtr pIEnumLineSelector(pNodeMapCameraParam->GetNode("LineSelector"));
		*pIEnumLineSelector = "Line2";
		GenApi::CEnumerationPtr pIEnumLineMode(pNodeMapCameraParam->GetNode("LineMode"));
		*pIEnumLineMode = "Output";

		// Switch Line2 output source to Timer 0 Active
		GenApi::CEnumerationPtr pIEnumLineSource(pNodeMapCameraParam->GetNode("LineSource"));
		*pIEnumLineSource = "Timer0Active";

		// Set Timer 0 trigger source to Exposure Start, which means Timer 0 will output signal when camera start exposure
		GenApi::CEnumerationPtr pIEnumTimerSelector(pNodeMapCameraParam->GetNode("TimerSelector"));
		*pIEnumTimerSelector = "Timer0";

		// Set exposure start as the trigger source of Timer0
		GenApi::CEnumerationPtr pIEnumTimerTrigSource(pNodeMapCameraParam->GetNode("TimerTriggerSource"));
		*pIEnumTimerTrigSource = "ExposureStart";

		// Set Timer0 output duration to 1000us
		GenApi::CFloatPtr pIFloatTimerDuration(pNodeMapCameraParam->GetNode("TimerDuration"));
		pIFloatTimerDuration->SetValue(1000);

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
					pIStImageDisplayWnd->SetPosition(0, 0, pIStImage->GetImageWidth(), pIStImage->GetImageHeight());

					pIStImageDisplayWnd->Show(NULL, StWindowMode_ModalessOnNewThread);
				}

				pIStImageDisplayWnd->RegisterIStImage(pIStImage);
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
	}
	catch (const GenICam::GenericException &e)
	{
		cerr << endl << "An exception occurred." << endl << e.GetDescription() << endl;
	}

	cout << endl << "Press Enter to exit." << endl;
	while (cin.get() != '\n');

	return(0);
}
