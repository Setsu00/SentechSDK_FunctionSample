/*
	Line0を入力とし、立ち上がりエッジでハードウェアトリガーを検出します。
	Hardware Trigger (Active High): Set camera to hardware trigger mode for Line0 as input and active high (Trigger activation as rising edge).
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


		// ==============================================================================================================
		// Active Highのハードウェアトリガーを有効にするデモです。
		// Demostration of Setting Hardware Trigger ON with active high

#ifdef ENABLED_ST_GUI
		CIStImageDisplayWndPtr pIStImageDisplayWnd(CreateIStWnd(StWindowType_ImageDisplay));
#endif
		CIStDataStreamPtr pIStDataStream(pIStDevice->CreateIStDataStream(0));

		// パラメーター操作用のNodeMapを取得します。
		// Create NodeMap pointer for accessing parameters
		GenApi::CNodeMapPtr pNodeMapCameraParam(pIStDevice->GetRemoteIStPort()->GetINodeMap());

		// LineSelectorをLine0に設定し、入力に使うラインを選択します。
		// Set LineSelector to Line0 to select the input line.
		GenApi::CEnumerationPtr pIEnumLineSelector(pNodeMapCameraParam->GetNode("LineSelector"));
		*pIEnumLineSelector = "Line0";
		GenApi::CEnumerationPtr pIEnumLine0Mode(pNodeMapCameraParam->GetNode("LineMode"));

		// LineModeをInputに設定し、選択したラインを入力にします。
		// Set LineMode to Input to use the selected line as an input.
		*pIEnumLine0Mode = "Input";

		// TriggerModeをOnに設定し、トリガーを有効にします。
		// Set TriggerMode to On to enable triggering.
		GenApi::CEnumerationPtr pIEnumTrigMode(pNodeMapCameraParam->GetNode("TriggerMode"));
		*pIEnumTrigMode = "On";

		// ハードウェア入力を使うため、TriggerSourceをLine0に設定します。
		// Set Trigger Source to Line0 as Hardware input
		GenApi::CEnumerationPtr pIEnumTrigSource(pNodeMapCameraParam->GetNode("TriggerSource"));
		*pIEnumTrigSource = "Line0";

		// TriggerActivationをRisingEdgeに設定し、立ち上がりエッジでトリガーを受け付けます。
		// Set TriggerActivation to RisingEdge to trigger on a rising edge.
		GenApi::CEnumerationPtr pIEnumTrigActivation(pNodeMapCameraParam->GetNode("TriggerActivation"));
		*pIEnumTrigActivation = "RisingEdge";

		// ==============================================================================================================

		pIStDataStream->StartAcquisition(nCountOfImagesToGrab);
		pIStDevice->AcquisitionStart();
		while (pIStDataStream->IsGrabbing())
		{

			// ハードウェアトリガーで取得したフレームを取り出します。
			// Retrieve a frame acquired by a hardware trigger.
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

		// 取得後はTriggerModeをOffに設定します。
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
