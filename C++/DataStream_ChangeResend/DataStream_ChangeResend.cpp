//
// DataStream_ChangeResend.cpp : 
//
// このサンプルはIStDataStreamのMaximum Resend Requests Per Blockを50(Default)から0に変更する方法を示します。
// * 0に変更することよりリセンド要求の上限を外れますことで、ずっと転送不安定な環境の場合ドロップ悪化する可能性はあります。
//
// This sample shows how to modify "Maximum Resend Requests Per Block" of DataStream from 50(default) to 0.
// * Please note that by setting this value to 0, the limitation of resend request block will be disabled.
//   If you are using a unstable transferring environment, modify this value may cause multiple frame drops.
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
		// Create NodeMap for accessing DataStream.
		GenApi::CNodeMapPtr pNodeMapCameraParam(pIStDataStream->GetIStPort()->GetINodeMap());

		// Maximum Resend Requests Per Blockのノードを取得
		// Acquire Node for "MaximumResendRequestsPerBlock"
		GenApi::CIntegerPtr pIntegerResendRequests(pNodeMapCameraParam->GetNode("MaximumResendRequestsPerBlock"));
		// Maximum Resend Requests Per Blockに0を設定
		// Set MaximumResendRequestsPerBloxk to 0 (to disable the limited of resend.
		pIntegerResendRequests->SetValue(0);

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
					<< " First byte =" << (uint32_t)*(uint8_t*)pIStImage->GetImageBuffer() << endl;
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