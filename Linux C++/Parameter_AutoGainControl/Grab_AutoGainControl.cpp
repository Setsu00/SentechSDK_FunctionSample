/*
	自動ゲインを有効にし、範囲を0～10 dBに設定します。
	Auto Gain Control: Set auto gain control on with range 0 dB to 10 dB.
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
		// 自動ゲインの範囲を設定するデモです。
		// Demostration of Setting Auto Gain Control with dedicated range.

		// パラメーター操作用のNodeMapを取得します。
		// Create NodeMap pointer for accessing parameters
		GenApi::CNodeMapPtr pNodeMapCameraParam(pIStDevice->GetRemoteIStPort()->GetINodeMap());

		// GainAutoをContinuousに設定し、自動ゲイン調整を継続します。
		// Set GainAuto to Continuous to adjust gain automatically and continuously.
		GenApi::CEnumerationPtr pIEnumGainAuto(pNodeMapCameraParam->GetNode("GainAuto"));
		*pIEnumGainAuto = "Continuous";

		// AutoLuminanceTarget（IInteger）ノードを取得します。
		// Get Node for Auto Luminance Target(IInteger)
		GenApi::CNodePtr pNodeAutoLuminTgt = pNodeMapCameraParam->GetNode("AutoLuminanceTarget");

		// 値を設定するため、ノードをCIntegerPtrに変換します。
		// Convert Node to CIntegerPtr for setting value
		GenApi::CIntegerPtr pIntNodeAutoLuminTgt(pNodeAutoLuminTgt);

		// AutoLuminanceTargetを128に設定します。
		// Set Auto Luminance Target to 128
		pIntNodeAutoLuminTgt->SetValue(128);

		// アナログゲインを制御するため、GainSelectorをAnalogAllに設定します。
		// For setting analog gain, gain selector need to be set to AnalogAll to access analog gain.
		GenApi::CEnumerationPtr pIEnumGainSelector(pNodeMapCameraParam->GetNode("GainSelector"));
		*pIEnumGainSelector = "AnalogAll";
		
		// GainAutoLimitMin（IFloat）ノードを取得します。
		// Get Node for GainAutoLimitMin(IFloat).
		GenApi::CNodePtr pNodeAutoGainLimitMin = pNodeMapCameraParam->GetNode("GainAutoLimitMin");

		// 値を設定するため、ノードをCFloatPtrに変換します。
		// Convert Node to CFloatPtr for setting value
		GenApi::CFloatPtr pFloatNodeAutoGainLimitMin(pNodeAutoGainLimitMin);

		// 自動ゲインの下限を0 dB（設定値0）に設定します。
		// Set Auto Gain Min to 0 dB (0).
		pFloatNodeAutoGainLimitMin->SetValue(0);

		// GainAutoLimitMax（IFloat）ノードを取得します。
		// Get Node for GainAutoLimitMax(IFloat).
		GenApi::CNodePtr pNodeAutoGainLimitMax = pNodeMapCameraParam->GetNode("GainAutoLimitMax");

		// 値を設定するため、ノードをCFloatPtrに変換します。
		// Convert Node to CFloatPtr for setting value
		GenApi::CFloatPtr pFloatNodeAutoGainLimitMax(pNodeAutoGainLimitMax);

		// 自動ゲインの上限を10 dB（設定値100）に設定します。
		// Set Auto Gain Max to 10 dB (100).
		pFloatNodeAutoGainLimitMax->SetValue(100);

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
