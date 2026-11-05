/*
	Grab_PolarizationADOLP.cpp
	PixelFormatConverterで取得した偏光画像をADOLP画像へ変換して表示します。
	Convert acquired polarization images to ADOLP images with PixelFormatConverter and display them.
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
		// PixelFormatConverterでADOLP変換を設定します。
		// Configure ADOLP conversion with PixelFormatConverter.
		CIStPixelFormatConverterPtr pIStPixelFormatConverter(CreateIStConverter(StConverterType_PixelFormat));

		// 変換後のピクセルフォーマットをBGR8に設定します。
		// Set the destination pixel format to BGR8.
		pIStPixelFormatConverter->SetDestinationPixelFormat(StPFNC_BGR8);

		// 変換器のNodeMapを取得します。
		// Acquire the converter's NodeMap.
		GenApi::CNodeMapPtr pNodeMapConverter(pIStPixelFormatConverter->GetINodeMap());

		// 偏光画像変換方式のノードを取得します。
		// Get the polarization image transformation method node.
		GenApi::CEnumerationPtr pEnumPolarizationMethod(pNodeMapConverter->GetNode("PolarizationImageTransformationMethod"));

		// 直線偏光の角度と偏光度（ADOLP）への変換を設定します。
		// Set conversion to angle and degree of linear polarization (ADOLP).
		pEnumPolarizationMethod->FromString("AngleAndDegreeOfLinearPolarization");

		// ==============================================================================================================

		pIStDataStream->StartAcquisition(nCountOfImagesToGrab);
		pIStDevice->AcquisitionStart();
		while (pIStDataStream->IsGrabbing())
		{
			CIStStreamBufferPtr pIStStreamBuffer(pIStDataStream->RetrieveBuffer(5000));
			if (pIStStreamBuffer->GetIStStreamBufferInfo()->IsImagePresent())
			{
				IStImage *pIStImage = pIStStreamBuffer->GetIStImage();

				// ==============================================================================================================
				// 変換後の画像を格納するバッファを生成します。
				// Create a buffer for the converted image.
				CIStImageBufferPtr imageBuffer(CreateIStImageBuffer());

				// 表示前に、取得した偏光画像をADOLP画像へ変換します。
				// Convert the acquired polarization image to an ADOLP image before display.
				pIStPixelFormatConverter->Convert(pIStImage, imageBuffer);
				pIStImage = imageBuffer->GetIStImage();
				// ==============================================================================================================

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

				// 変換後の画像を表示します。
				// Display the converted image.
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
