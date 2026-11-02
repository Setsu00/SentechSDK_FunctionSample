/*
	Serial Connection: 入力したシリアル番号のカメラへ接続する。
	Serial Connection: Connect to the camera with the entered serial number.
*/

//#define ENABLED_ST_GUI

#include <StApi_TL.h>
#ifdef ENABLED_ST_GUI
#include <StApi_GUI.h>
#include <iomanip>	//std::setprecision
#endif

using namespace StApi;
using namespace std;
const uint64_t nCountOfImagesToGrab = 100;

using namespace GenApi;

int main(int /* argc */, char ** /* argv */)
{
	try
	{
		CStApiAutoInit objStApiAutoInit;

		CIStSystemPtr pIStSystem(CreateIStSystem(StSystemVendor_Sentech));

		// カメラのシリアル番号を入力します。
		// Enter the camera serial number.
		cout << "Please input serial number of target camera: " << endl;
		string wsSerial;
		getline(cin, wsSerial);
		GenICam::gcstring strDeviceSerial(wsSerial.c_str());

		// インターフェース数を取得します。
		// Get the interface count.
		uint32_t uintInterfaceCnt = pIStSystem->GetInterfaceCount();

		// 一致したデバイスのIDとインターフェース番号を保持します。
		// Store the device ID and interface index when a match is found.
		GenICam::gcstring strTgtDeviceID;
		uint32_t uintTgtInterfaceNo = 0;
		bool bHit = false;

		// 全インターフェースから指定シリアル番号のカメラを検索します。
		// Search all interfaces for the specified camera serial number.
		for (uint32_t i = 0; i < uintInterfaceCnt; i++)
		{
			// インターフェースを取得し、デバイスリストを更新します。
			// Get the interface and update its device list.
			IStInterface * pInterface = pIStSystem->GetIStInterface(i);
			pInterface->UpdateDeviceList();

			uint32_t uintCamCnt = pInterface->GetDeviceCount();
			for (uint32_t j = 0; j < uintCamCnt; j++)
			{
				const IStDeviceInfo * tmpDeviceInfoPtr = pInterface->GetIStDeviceInfo(j);
				if (tmpDeviceInfoPtr->GetSerialNumber() == strDeviceSerial)
				{
					strTgtDeviceID = tmpDeviceInfoPtr->GetID();
					uintTgtInterfaceNo = i;
					bHit = true;
					break;
				}
			}
			if (bHit)
				break;
		}

		if (!bHit)
		{
			// 指定したカメラが見つからない場合は終了します。
			// Exit if the specified camera is not found.
			cout << "Target camera not found." << endl << "Press Enter to exit." << endl;
			cin.get();
			return (0);
		}

		// 一致したデバイスIDを使用してカメラへ接続します。
		// Create IStDevice using the matched device ID.
		CIStDevicePtr pIStDevice(pIStSystem->GetIStInterface(uintTgtInterfaceNo)->CreateIStDevice(strTgtDeviceID));

		// ==================================================================================================================

		cout << "Device=" << pIStDevice->GetIStDeviceInfo()->GetDisplayName() << endl;

#ifdef ENABLED_ST_GUI

		CIStImageDisplayWndPtr pIStImageDisplayWnd(CreateIStWnd(StWindowType_ImageDisplay));
#endif

		CIStDataStreamPtr pIStDataStream(pIStDevice->CreateIStDataStream(0));

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

					pIStImageDisplayWnd->Show(NULL, StWindowMode_Modaless);
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
