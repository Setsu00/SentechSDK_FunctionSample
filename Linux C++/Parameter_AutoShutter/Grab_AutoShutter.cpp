/*
	Auto Shutter: 自動露光を有効にし、露光時間範囲を1,000～150,000 µsに設定する。
	Auto Shutter: Enable auto exposure and set its exposure-time range to 1,000–150,000 µs.
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
		// 指定した露光時間範囲でオートシャッターを有効にする。
		// Enable auto shutter within the specified exposure-time range.

		// カメラのパラメーターへアクセスするNodeMapを取得する。
		// Get the NodeMap used to access camera parameters.
		GenApi::CNodeMapPtr pNodeMapCameraParam(pIStDevice->GetRemoteIStPort()->GetINodeMap());

		// ExposureAuto（IEnumeration）をContinuousに設定する。
		// Set ExposureAuto (IEnumeration) to Continuous.
		GenApi::CEnumerationPtr pIEnumExpoAuto(pNodeMapCameraParam->GetNode("ExposureAuto"));
		*pIEnumExpoAuto = "Continuous";

		// AutoLuminanceTarget（IInteger）ノードを取得する。
		// Get the AutoLuminanceTarget (IInteger) node.
		GenApi::CNodePtr pNodeAutoLuminTgt = pNodeMapCameraParam->GetNode("AutoLuminanceTarget");
		// 値を設定するためにノードをCIntegerPtrへ変換する。
		// Convert the node to CIntegerPtr to set its value.
		GenApi::CIntegerPtr pIntNodeAutoLuminTgt(pNodeAutoLuminTgt);
		// AutoLuminanceTargetに128を設定する。
		// Set AutoLuminanceTarget to 128.
		pIntNodeAutoLuminTgt->SetValue(128);

		// ExposureAutoLimitMin（IFloat）ノードを取得する。
		// Get the ExposureAutoLimitMin (IFloat) node.
		GenApi::CNodePtr pNodeAutoExpoLimitMin = pNodeMapCameraParam->GetNode("ExposureAutoLimitMin");
		// 値を設定するためにノードをCFloatPtrへ変換する。
		// Convert the node to CFloatPtr to set its value.
		GenApi::CFloatPtr pFloatNodeAutoExpoLimitMin(pNodeAutoExpoLimitMin);
		// 自動露光時間の最小値を1,000 µsに設定する。
		// Set the minimum auto exposure time to 1,000 µs.
		pFloatNodeAutoExpoLimitMin->SetValue(1000);

		// ExposureAutoLimitMax（IFloat）ノードを取得する。
		// Get the ExposureAutoLimitMax (IFloat) node.
		GenApi::CNodePtr pNodeAutoExpoLimitMax = pNodeMapCameraParam->GetNode("ExposureAutoLimitMax");
		// 値を設定するためにノードをCFloatPtrへ変換する。
		// Convert the node to CFloatPtr to set its value.
		GenApi::CFloatPtr pFloatNodeAutoExpoLimitMax(pNodeAutoExpoLimitMax);
		// 自動露光時間の最大値を150,000 µsに設定する。
		// Set the maximum auto exposure time to 150,000 µs.
		pFloatNodeAutoExpoLimitMax->SetValue(150000);

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
