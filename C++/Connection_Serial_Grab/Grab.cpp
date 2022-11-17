/*

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

		// ==== カメラシリアルでターゲットカメラに接続 ================================================================
		// ==== Connect to target camera with camera serial. ================================================================
		// シリアル入力
		// Input Serial.
		cout << "Please input serial number of target camera: " << endl;
		wstring wsSerial;
		wcin >> wsSerial;
		GenICam::gcstring strDeviceSerial(wsSerial.c_str());

		// 入力をクリア
		// Clear enter input.
		fflush(stdin);

		// インターフェースの数を取得
		// Acquire interface count
		uint32_t uintInterfaceCnt = pIStSystem->GetInterfaceCount();

		// ヒットした場合、デバイスIDを保存するためのgcstringを準備
		// Prepear gcstring for saving device ID if hit.
		GenICam::gcstring strTgtDeviceID;

		// 後に使うためインターフェースのナンバーを準備
		// Prepear for # of interface for later use.
		uint32_t uintTgtInterfaceNo = 0;

		// ヒットフラグ
		// Camera found hit flag.
		bool bHit = false;

		// ターゲットカメラが存在するか全てのインターフェースをチェック
		// Check all interface if target camera is exist.
		for (uint32_t i = 0; i < uintInterfaceCnt; i++)
		{
			// インターフェース取得
			// Acquire interface
			IStInterface * pInterface = pIStSystem->GetIStInterface(i);
			// 新しいデバイスを検出した場合インターフェースを更新
			// Update interface to see if any newer device comes in.
			pInterface->UpdateDeviceList();

			uint32_t uintCamCnt = pInterface->GetDeviceCount();
			for (uint32_t j = 0; j < uintCamCnt; j++)
			{
				const IStDeviceInfo * tmpDeviceInfoPtr = pInterface->GetIStDeviceInfo(j);
				if (tmpDeviceInfoPtr->GetSerialNumber() == strDeviceSerial)
				{
					// ターゲットカメラの発見
					// Hit target camera
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
			// 見つからなければメッセージと共に終了
			// Not found, exit program with message
			cout << "Target camera not found." << endl << "Press Enter to exit." << endl;
			cin.get();
			return (0);
		}

		// 見つけたデバイスIDを使うためIStDevice経由で生成
		// Create IStDevice via using found device ID.
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
