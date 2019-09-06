/*
	BinningHV1: Set Camera Binning Horizontal / Binning Vertical to 1
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
		// Demostration of setting Binning Horizontal / Binning Veritcal to 1

		// Create NodeMap pointer for accessing parameters
		GenApi::CNodeMapPtr pNodeMapCameraParam(pIStDevice->GetRemoteIStPort()->GetINodeMap());

		// Get Node for Binning Horizontal
		GenApi::CNodePtr pNodeBinningH = pNodeMapCameraParam->GetNode("BinningHorizontal");
		// Convert Node to CIntegerPtr for setting value
		GenApi::CIntegerPtr pIntBinningH(pNodeBinningH);
		// Set BinningHorizontal to 1
		pIntBinningH->SetValue(1);

		// Get Node for WiBinning Vertical
		GenApi::CNodePtr pNodeBinningV = pNodeMapCameraParam->GetNode("BinningVertical");
		// Convert Node to CIntegerPtr for setting value
		GenApi::CIntegerPtr pIntBinningV(pNodeBinningV);
		// Set BinningVertical to 1
		pIntBinningV->SetValue(1);

		// For setting Binning to 1, both height and width need to be set to max manually.

		// Get Node for Width
		GenApi::CNodePtr pNodeWidth = pNodeMapCameraParam->GetNode("Width");
		// Convert Node to CIntegerPtr for setting value
		GenApi::CIntegerPtr pIntWidthl(pNodeWidth);
		// Set Width to 640
		pIntWidthl->SetValue(pIntWidthl->GetMax());

		// Get Node for Height
		GenApi::CNodePtr pNodeHeight = pNodeMapCameraParam->GetNode("Height");
		// Convert Node to CIntegerPtr for setting value
		GenApi::CIntegerPtr pIntHeight(pNodeHeight);
		// Set Height to 480
		pIntHeight->SetValue(pIntHeight->GetMax());

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
