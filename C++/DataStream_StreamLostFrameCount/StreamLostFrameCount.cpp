<<<<<<< HEAD
﻿// StreamLostFrameCount.cpp : このサンプルは、IStDataStreamよりLost Frame Countを取得、表示する方法を示します
=======
﻿// StreamLostFrameCount.cpp : 
// 
// このサンプルは、IStDataStreamよりLost Frame Countを取得、表示する方法を示します
// This sample demonstrate the way of acquiring Lost Frame Count from IStDataStream.
>>>>>>> 6196dfcda454323bf1a61dd8171ddc1b3b27f9d7
//

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
		// DataSteam対してのノードマップ生成
<<<<<<< HEAD
		GenApi::CNodeMapPtr pNodeMapCameraParam(pIStDataStream->GetIStPort()->GetINodeMap());

		// StreamLostFrameCountのノードを取得
=======
		// Create NodeMap for DataStream
		GenApi::CNodeMapPtr pNodeMapCameraParam(pIStDataStream->GetIStPort()->GetINodeMap());

		// StreamLostFrameCountのノードを取得
		// Acquire StreamLostFrameCount Node from NodeMap of DataStream for showing lost frame count.
>>>>>>> 6196dfcda454323bf1a61dd8171ddc1b3b27f9d7
		GenApi::CIntegerPtr pIntegerLostFrameCount(pNodeMapCameraParam->GetNode("StreamLostFrameCount"));
		
		// ==============================================================================================================

		pIStDataStream->StartAcquisition(nCountOfImagesToGrab);

		pIStDevice->AcquisitionStart();

		while (pIStDataStream->IsGrabbing())
		{
			CIStStreamBufferPtr pIStStreamBuffer(pIStDataStream->RetrieveBuffer(5000));

			if (pIStStreamBuffer->GetIStStreamBufferInfo()->IsImagePresent())
			{
				IStImage *pIStImage = pIStStreamBuffer->GetIStImage();

				cout << "BlockId=" << pIStStreamBuffer->GetIStStreamBufferInfo()->GetFrameID()
					<< " Size:" << pIStImage->GetImageWidth() << " x " << pIStImage->GetImageHeight()
					<< " First byte =" << (uint32_t)*(uint8_t*)pIStImage->GetImageBuffer()
					// ==============================================================================================================
					// StreamLostFrameCountの出力
<<<<<<< HEAD
=======
					// Output StreamLostFrameCount every frame.
>>>>>>> 6196dfcda454323bf1a61dd8171ddc1b3b27f9d7
					<< " LostFrameCount =" << pIntegerLostFrameCount->GetValue() << endl;
					// ==============================================================================================================
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

