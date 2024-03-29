﻿/*
	Rotate90cwFilter: フィルターを使用して画像を時計回りに90度回転させます。
	Rotate90cwFilter: using Filter to rotate image clock wise 90 degree.
*/

#define ENABLED_ST_GUI

#include <StApi_TL.h>
#include <StApi_IP.h>
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
		// フィルターを使用して画像を時計回りに90度回転させるデモ
		// Demostration of using Filter to rotate image clock wise 90 degree.

		// ReverseConverterフィルターオブジェクトの生成
		// Create an ReverseConverter filter object.
		CIStReverseConverterPtr filter(CreateIStConverter(StConverterType_Reverse));

		// ReverseConverterを逆時計回り90度に設定します。
		// Set ReverseConverter reverse to clock wise 90 degree.
		filter->SetRotationMode(StRotationMode_Clockwise90);

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

				// ==============================================================================================================
				// 回転した画像を保存する別のバッファを生成
				// Create another buffer for storing rotated image
				CIStImageBufferPtr imageBuffer = CreateIStImageBuffer();

				// オリジナル画像を回転させ別のバッファに出力
				// Rotate original image and output to another buffer
				filter->Convert(pIStImage, imageBuffer);

				// 回転させた画像を表示
				// Display rotated image
				pIStImageDisplayWnd->RegisterIStImage(imageBuffer->GetIStImage());
				// ==============================================================================================================

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
