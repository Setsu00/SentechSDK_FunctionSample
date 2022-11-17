/*
	Auto Gain Control: オートゲイン操作を0dBから10dBの範囲で設定
	Auto Gain Control: Set auto gain control on with range 0 dB to 10 dB.
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
		// オートゲイン操作を特定の範囲で設定するデモ
		// Demostration of Setting Auto Gain Control with dedicated range.

		// パラメータにアクセスするためのノードマップポインタを生成
		// Create NodeMap pointer for accessing parameters
		GenApi::CNodeMapPtr pNodeMapCameraParam(pIStDevice->GetRemoteIStPort()->GetINodeMap());

		// GainAuto(IEnumeration)に切替
		// Switch on Gain Auto(IEnumeration).
		GenApi::CEnumerationPtr pIEnumGainAuto(pNodeMapCameraParam->GetNode("GainAuto"));
		*pIEnumGainAuto = "Continuous";

		// AutoLuminanceTarget(IInteger)ノードを取得
		// Get Node for Auto Luminance Target(IInteger)
		GenApi::CNodePtr pNodeAutoLuminTgt = pNodeMapCameraParam->GetNode("AutoLuminanceTarget");
		// 値を設定するためにNodeをCFloatPtrに変換
		// Convert Node to CIntegerPtr for setting value
		GenApi::CIntegerPtr pIntNodeAutoLuminTgt(pNodeAutoLuminTgt);
		// AutoLuminanceTargetを128に設定
		// Set Auto Luminance Target to 128
		pIntNodeAutoLuminTgt->SetValue(128);

		// アナログゲインを設定するため、アナログゲインにアクセスするためにGainSelectorをAnalogAllに設定する必要がある
		// For setting analog gain, gain selector need to be set to AnalogAll to access analog gain.
		GenApi::CEnumerationPtr pIEnumGainSelector(pNodeMapCameraParam->GetNode("GainSelector"));
		*pIEnumGainSelector = "AnalogAll";
		
		// GainAutoLimitMin(IFloat)ノードを取得
		// Get Node for GainAutoLimitMin(IFloat).
		GenApi::CNodePtr pNodeAutoGainLimitMin = pNodeMapCameraParam->GetNode("GainAutoLimitMin");
		// 値を設定するためにNodeをCFloatPtrに変換
		// Convert Node to CFloatPtr for setting value
		GenApi::CFloatPtr pFloatNodeAutoGainLimitMin(pNodeAutoGainLimitMin);
		// AutoGainの最小値に0dB(0)を設定
		// Set Auto Gain Min to 0 dB (0).
		pFloatNodeAutoGainLimitMin->SetValue(0);

		// GainAutoLimitMax(IFloat)ノードを取得
		// Get Node for GainAutoLimitMax(IFloat).
		GenApi::CNodePtr pNodeAutoGainLimitMax = pNodeMapCameraParam->GetNode("GainAutoLimitMax");
		// 値を設定するためにNodeをCFloatPtrに変換
		// Convert Node to CFloatPtr for setting value
		GenApi::CFloatPtr pFloatNodeAutoGainLimitMax(pNodeAutoGainLimitMax);
		// AutoGainの最大値に10dB(100)を設定
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
