/*
	全色のホワイトバランス比率を手動で10に設定します。
	Manual White Balance: Manually set white balance ratio of all color to 10.
*/

#define ENABLED_ST_GUI

// このサンプルの基本操作と対象機能の制御手順を示します。
// This sample demonstrates the basic operation and control procedure for the target feature.
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
		// 自動ホワイトバランスを無効にし、全色の比率を10に設定するデモです。
		// Demostration of changing white balance to manual with setting all balance ratio to 10

		// パラメーター操作用のNodeMapを取得します。
		// Create NodeMap pointer for accessing parameters
		GenApi::CNodeMapPtr pNodeMapCameraParam(pIStDevice->GetRemoteIStPort()->GetINodeMap());

		// 手動設定のため、BalanceWhiteAutoをOffに設定します。
		// Set BalanceWhiteAuto to Off for manual adjustment.
		GenApi::CEnumerationPtr pIEnumBalanceWhite(pNodeMapCameraParam->GetNode("BalanceWhiteAuto"));
		*pIEnumBalanceWhite = "Off";

		// 赤の比率にアクセスするため、BalanceRatioSelectorをRedに設定します。
		// Switch balance ration selector to Red for access Red ratio value
		GenApi::CEnumerationPtr pIEnumBalanceRatioSelect(pNodeMapCameraParam->GetNode("BalanceRatioSelector"));
		*pIEnumBalanceRatioSelect = "Red";

		// BalanceRatioノードを取得します。
		// Get Node for BalanceRatio
		GenApi::CNodePtr pNodeBalanceRatio = pNodeMapCameraParam->GetNode("BalanceRatio");

		// 値を設定するため、ノードをCFloatPtrに変換します。
		// Convert Node to CFloatPtr for setting value
		GenApi::CFloatPtr pFloatBalanceRatio(pNodeBalanceRatio);

		// BalanceRatioを10に設定します。
		// Set BalanceRatio to 10
		pFloatBalanceRatio->SetValue(10);

		// 青の比率にアクセスするため、BalanceRatioSelectorをBlueに設定します。
		// Switch balance ration selector to Blue for access Blue ratio value
		*pIEnumBalanceRatioSelect = "Blue";

		// BalanceRatioを10に設定します。
		// Set BalanceRatio to 10
		pFloatBalanceRatio->SetValue(10);

		// 緑の比率にアクセスするため、BalanceRatioSelectorをGreenに設定します。
		// Switch balance ration selector to Green for access Green ratio value
		*pIEnumBalanceRatioSelect = "Green";

		// BalanceRatioを10に設定します。
		// Set BalanceRatio to 10
		pFloatBalanceRatio->SetValue(10);

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
