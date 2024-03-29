﻿/*
	Manual White Balance: 手動で全色のホワイトバランス比率を10に設定する
	Manual White Balance: Manually set white balance ratio of all color to 10.
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
		// ホワイトバランスを手動に変更し、全てのバランス比率を10に設定するデモ
		// Demostration of changing white balance to manual with setting all balance ratio to 10

		// パラメータにアクセスするためのノードマップポインタを生成
		// Create NodeMap pointer for accessing parameters
		GenApi::CNodeMapPtr pNodeMapCameraParam(pIStDevice->GetRemoteIStPort()->GetINodeMap());

		// 入力値を手動にするためBalanceWhiteAutoをPreset0に切替
		// Switch BalanceWhiteAuto to Preset0 for manual input value
		GenApi::CEnumerationPtr pIEnumBalanceWhite(pNodeMapCameraParam->GetNode("BalanceWhiteAuto"));
		*pIEnumBalanceWhite = "Preset0";

		// 赤の比率値にアクセスするためBalanceRatioSelectorをRedに切替
		// Switch balance ration selector to Red for access Red ratio value
		GenApi::CEnumerationPtr pIEnumBalanceRatioSelect(pNodeMapCameraParam->GetNode("BalanceRatioSelector"));
		*pIEnumBalanceRatioSelect = "Red";

		// BalanceRatioのノードを取得
		// Get Node for BalanceRatio
		GenApi::CNodePtr pNodeBalanceRatio = pNodeMapCameraParam->GetNode("BalanceRatio");
		// 値を設定するためにNodeをCFloatPtrに変換
		// Convert Node to CFloatPtr for setting value
		GenApi::CFloatPtr pFloatBalanceRatio(pNodeBalanceRatio);
		// バランス比率に10を設定
		// Set BalanceRatio to 10
		pFloatBalanceRatio->SetValue(10);

		// 青の比率値にアクセスするためBalanceRatioSelectorをBlueに切替
		// Switch balance ration selector to Blue for access Blue ratio value
		*pIEnumBalanceRatioSelect = "Blue";
		// バランス比率に10を設定
		// Set BalanceRatio to 10
		pFloatBalanceRatio->SetValue(10);

		// 緑の比率値にアクセスするためBalanceRatioSelectorをGreenに切替
		// Switch balance ration selector to Green for access Green ratio value
		*pIEnumBalanceRatioSelect = "Green";
		// バランス比率に10を設定
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
