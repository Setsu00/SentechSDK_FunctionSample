/*
	ROI: Offset(100,200)、640x480でROI設定をする
	ROI: Set ROI to Offset (100, 200) and width/height (640, 480)
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
		// オフセット(100,200)、640x480でROI設定を行うデモ
		// Demostration of setting ROI to 640 x 480 with offset (100, 200).

		// パラメータにアクセスするためのノードマップポインタを生成
		// Create NodeMap pointer for accessing parameters
		GenApi::CNodeMapPtr pNodeMapCameraParam(pIStDevice->GetRemoteIStPort()->GetINodeMap());

		// Widthノードを取得
		// Get Node for Width
		GenApi::CNodePtr pNodeWidth = pNodeMapCameraParam->GetNode("Width");
		// 値を設定するためにNodeをCIntegerPtrに変換
		// Convert Node to CIntegerPtr for setting value
		GenApi::CIntegerPtr pIntWidthl(pNodeWidth);
		// 幅を640に設定
		// Set Width to 640
		pIntWidthl->SetValue(640);

		// Heightノードを取得
		// Get Node for Height
		GenApi::CNodePtr pNodeHeight = pNodeMapCameraParam->GetNode("Height");
		// 値を設定するためにNodeをCIntegerPtrに変換
		// Convert Node to CIntegerPtr for setting value
		GenApi::CIntegerPtr pIntHeight(pNodeHeight);
		// 高さに480を設定
		// Set Height to 480
		pIntHeight->SetValue(480);

		// OffsetXのノードを取得
		// Get Node for Offset X
		GenApi::CNodePtr pNodeOffX = pNodeMapCameraParam->GetNode("OffsetX");
		// 値を設定するためにNodeをCIntegerPtrに変換
		// Convert Node to CIntegerPtr for setting value
		GenApi::CIntegerPtr pIntOffX(pNodeOffX);
		// Xオフセットに100を設定
		// Set Offset X to 100
		pIntOffX->SetValue(100);

		// OffsetYノードを取得
		// Get Node for Offset Y
		GenApi::CNodePtr pNodeOffY = pNodeMapCameraParam->GetNode("OffsetY");
		// 値を設定するためにNodeをCIntegerPtrに変換
		// Convert Node to CIntegerPtr for setting value
		GenApi::CIntegerPtr pIntOffY(pNodeOffY);
		// Yオフセットに200を設定
		// Set Offset Y to 200
		pIntOffY->SetValue(200);

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
