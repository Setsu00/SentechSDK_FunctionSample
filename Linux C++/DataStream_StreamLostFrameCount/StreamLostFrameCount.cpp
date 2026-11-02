/*
	Stream Lost Frame Count: IStDataStreamのStreamLostFrameCountを表示する。
	Stream Lost Frame Count: Display StreamLostFrameCount from IStDataStream.
*/

#include <StApi_TL.h>

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

		CIStDataStreamPtr pIStDataStream(pIStDevice->CreateIStDataStream(0));

		// ==============================================================================================================
		// DataStreamへアクセスするためのノードマップを取得
		// Get the NodeMap used to access the DataStream.
		GenApi::CNodeMapPtr pNodeMapDataStream(pIStDataStream->GetIStPort()->GetINodeMap());

		// 欠落フレーム数を表示するため、DataStreamのNodeMapからStreamLostFrameCountノードを取得
		// Get the StreamLostFrameCount node from the DataStream NodeMap to display the lost-frame count.
		GenApi::CIntegerPtr pIntegerLostFrameCount(pNodeMapDataStream->GetNode("StreamLostFrameCount"));

		// ==============================================================================================================

		pIStDataStream->StartAcquisition(nCountOfImagesToGrab);

		pIStDevice->AcquisitionStart();

		while (pIStDataStream->IsGrabbing())
		{
			CIStStreamBufferPtr pIStStreamBuffer(pIStDataStream->RetrieveBuffer(5000));

			if (pIStStreamBuffer->GetIStStreamBufferInfo()->IsImagePresent())
			{
				IStImage *pIStImage = pIStStreamBuffer->GetIStImage();

				// フレームごとにStreamLostFrameCountを表示します。
				// Display StreamLostFrameCount for each frame.
				cout << "BlockId=" << pIStStreamBuffer->GetIStStreamBufferInfo()->GetFrameID()
					<< " Size:" << pIStImage->GetImageWidth() << " x " << pIStImage->GetImageHeight()
					<< " First byte =" << (uint32_t)*(uint8_t*)pIStImage->GetImageBuffer()
					<< " LostFrameCount =" << pIntegerLostFrameCount->GetValue() << endl;
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
