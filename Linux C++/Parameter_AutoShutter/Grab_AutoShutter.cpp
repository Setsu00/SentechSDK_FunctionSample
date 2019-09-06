/*
	Auto Shutter: Set auto shutter on with range 1,000 us to 150,000 us.
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

#ifdef ENABLED_ST_GUI
		CIStImageDisplayWndPtr pIStImageDisplayWnd(CreateIStWnd(StWindowType_ImageDisplay));
#endif
		CIStDataStreamPtr pIStDataStream(pIStDevice->CreateIStDataStream(0));

		// ==============================================================================================================
		// Demostration of Setting Auto Shutter with dedicated range.

		// Create NodeMap pointer for accessing parameters
		GenApi::CNodeMapPtr pNodeMapCameraParam(pIStDevice->GetRemoteIStPort()->GetINodeMap());

		// Switch on Exposure Auto(IEnumeration).
		GenApi::CEnumerationPtr pIEnumExpoAuto(pNodeMapCameraParam->GetNode("ExposureAuto"));
		*pIEnumExpoAuto = "Continuous";

		// Get Node for Auto Luminance Target(IInteger)
		GenApi::CNodePtr pNodeAutoLuminTgt = pNodeMapCameraParam->GetNode("AutoLuminanceTarget");
		// Convert Node to CIntegerPtr for setting value
		GenApi::CIntegerPtr pIntNodeAutoLuminTgt(pNodeAutoLuminTgt);
		// Set Auto Luminance Target to 128
		pIntNodeAutoLuminTgt->SetValue(128);

		// Get Node for ExposureAutoLimitMin(IFloat).
		GenApi::CNodePtr pNodeAutoExpoLimitMin = pNodeMapCameraParam->GetNode("ExposureAutoLimitMin");
		// Convert Node to CFloatPtr for setting value
		GenApi::CFloatPtr pFloatNodeAutoExpoLimitMin(pNodeAutoExpoLimitMin);
		// Set Auto Exposure time Min to 1,000 usec
		pFloatNodeAutoExpoLimitMin->SetValue(1000);

		// Get Node for ExposureAutoLimitMax(IFloat).
		GenApi::CNodePtr pNodeAutoExpoLimitMax = pNodeMapCameraParam->GetNode("ExposureAutoLimitMin");
		// Convert Node to CFloatPtr for setting value
		GenApi::CFloatPtr pFloatNodeAutoExpoLimitMax(pNodeAutoExpoLimitMax);
		// Set Auto Exposure time Max to 150,000 usec
		pFloatNodeAutoExpoLimitMin->SetValue(150000);

		// ==============================================================================================================

		pIStDataStream->StartAcquisition(nCountOfImagesToGrab);
		pIStDevice->AcquisitionStart();
		while (pIStDataStream->IsGrabbing())
		{
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
	}
	catch (const GenICam::GenericException &e)
	{
		cerr << endl << "An exception occurred." << endl << e.GetDescription() << endl;
	}

	cout << endl << "Press Enter to exit." << endl;
	while (cin.get() != '\n');

	return(0);
}
