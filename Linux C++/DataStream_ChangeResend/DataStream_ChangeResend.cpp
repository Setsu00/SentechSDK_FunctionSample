/*
	Change Resend: MaximumResendRequestsPerBlockを0（単位なし、無制限）に設定する。
	Change Resend: Set MaximumResendRequestsPerBlock to 0 (unitless, unlimited).
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

		// MaximumResendRequestsPerBlockノードを取得
		// Get the MaximumResendRequestsPerBlock node.
		GenApi::CIntegerPtr pIntegerResendRequests(pNodeMapDataStream->GetNode("MaximumResendRequestsPerBlock"));

		// MaximumResendRequestsPerBlockを0（単位なし、無制限）に設定します。
		// Set MaximumResendRequestsPerBlock to 0 (unitless, unlimited).
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
