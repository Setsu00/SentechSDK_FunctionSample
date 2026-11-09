/*
	BinningHV1: 水平・垂直ビニングを1（単位なし）に設定する。
	BinningHV1: Set horizontal and vertical binning to 1 (unitless).
*/

#ifdef _MSC_VER
#pragma warning(disable : 4819)	
#endif
#include <StApi_C.h>
#ifdef _MSC_VER
#pragma warning(default : 4819)	
#endif

const uint64_t nCountOfImagesToGrab = 100;

void OutputErrorInfo(PApiFunctions psApiFunctions)
{
	char szText[1024];
	size_t nLen = sizeof(szText);
	EStApiCError_t eStApiCError = psApiFunctions->StApi->GetLastErrorDescriptionA(szText, &nLen);
	if (eStApiCError == StApiCError_NoError)
	{
		printf("Description:%s\r\n", szText);
	}
}


int main(int argc, char **argv)
{
	PApiFunctions psApiFunctions = NULL;
	EStApiCError_t eStApiCError = StApiCInitialize(STAPI_VERSION, &psApiFunctions);

	StApiHandle_t sIStSystemHandle = { NULL };
	StApiHandle_t sIStDeviceHandle = { NULL };
	StApiHandle_t sIStDataStreamHandle = { NULL };

	if (eStApiCError != StApiCError_NoError)
	{
		printf("initializing StApiC was failed.\n");
		return(0);
	}
	for (;;)
	{
		eStApiCError = psApiFunctions->StApi->IStSystem->CreateIStSystem(StSystemVendor_Default, StInterfaceType_All, &sIStSystemHandle);
		if (eStApiCError != StApiCError_NoError) break;

		eStApiCError = psApiFunctions->StApi->IStSystem->CreateFirstIStDevice(&sIStSystemHandle, DEVICE_ACCESS_CONTROL, &sIStDeviceHandle);
		if (eStApiCError != StApiCError_NoError) break;

		{
			StApiHandle_t sIStDeviceInfoHandle = { NULL };
			char szDisplayName[1024];
			size_t nLen = sizeof(szDisplayName);
			eStApiCError = psApiFunctions->StApi->IStDevice->GetIStDeviceInfo(&sIStDeviceHandle, &sIStDeviceInfoHandle);
			if (eStApiCError != StApiCError_NoError) break;

			eStApiCError = psApiFunctions->StApi->IStDeviceInfo->GetDisplayNameA(&sIStDeviceInfoHandle, szDisplayName, &nLen);
			if (eStApiCError != StApiCError_NoError) break;

			printf("Device=%s\r\n", szDisplayName);
		}

		// ==============================================================================================================
		// 水平・垂直ビニングを1に設定し、WidthとHeightを最大値に設定するデモ。
		// Demonstration of setting horizontal and vertical binning to 1, then setting Width and Height to their maximum values.

		StApiHandle_t sIStPortRemoteHandle = { NULL };
		StApiHandle_t sINodeMapRemoteHandle = { NULL };

		// Remote Portを取得します。
		// Acquire the Remote Port.
		eStApiCError = psApiFunctions->StApi->IStDevice->GetRemoteIStPort(&sIStDeviceHandle, &sIStPortRemoteHandle);
		if (eStApiCError != StApiCError_NoError) break;

		// NodeMapを取得します。
		// Acquire the NodeMap.
		eStApiCError = psApiFunctions->StApi->IStPort->GetINodeMap(&sIStPortRemoteHandle, &sINodeMapRemoteHandle);
		if (eStApiCError != StApiCError_NoError) break;

		// BinningHorizontalノードを取得し、単位なしの係数1を設定します。
		// Get the BinningHorizontal node and set the unitless factor to 1.
		StApiHandle_t sIIntegerHandleBinningHorizontal = { NULL };
		eStApiCError = psApiFunctions->GenApi->INodeMap->GetNodeA(&sINodeMapRemoteHandle, "BinningHorizontal", &sIIntegerHandleBinningHorizontal);
		if (eStApiCError != StApiCError_NoError) break;

		// BinningHorizontalを1に設定します。
		// Set BinningHorizontal to 1.
		eStApiCError = psApiFunctions->GenApi->IInteger->SetValue(&sIIntegerHandleBinningHorizontal, 1, false);
		if (eStApiCError != StApiCError_NoError) break;

		// BinningVerticalノードを取得し、単位なしの係数1を設定します。
		// Get the BinningVertical node and set the unitless factor to 1.
		StApiHandle_t sIIntegerHandleBinningVertical = { NULL };
		eStApiCError = psApiFunctions->GenApi->INodeMap->GetNodeA(&sINodeMapRemoteHandle, "BinningVertical", &sIIntegerHandleBinningVertical);
		if (eStApiCError != StApiCError_NoError) break;

		// BinningVerticalを1に設定します。
		// Set BinningVertical to 1.
		eStApiCError = psApiFunctions->GenApi->IInteger->SetValue(&sIIntegerHandleBinningVertical, 1, false);
		if (eStApiCError != StApiCError_NoError) break;

		// Widthノードを取得し、最大値を設定します。
		// Get the Width node and set it to its maximum value.
		StApiHandle_t sIIntegerHandleWidth = { NULL };
		int64_t nMaximumWidth = 0;
		eStApiCError = psApiFunctions->GenApi->INodeMap->GetNodeA(&sINodeMapRemoteHandle, "Width", &sIIntegerHandleWidth);
		if (eStApiCError != StApiCError_NoError) break;

		eStApiCError = psApiFunctions->GenApi->IInteger->GetMax(&sIIntegerHandleWidth, &nMaximumWidth);
		if (eStApiCError != StApiCError_NoError) break;

		// Widthを最大値に設定します。
		// Set Width to its maximum value.
		eStApiCError = psApiFunctions->GenApi->IInteger->SetValue(&sIIntegerHandleWidth, nMaximumWidth, false);
		if (eStApiCError != StApiCError_NoError) break;

		// Heightノードを取得し、最大値を設定します。
		// Get the Height node and set it to its maximum value.
		StApiHandle_t sIIntegerHandleHeight = { NULL };
		int64_t nMaximumHeight = 0;
		eStApiCError = psApiFunctions->GenApi->INodeMap->GetNodeA(&sINodeMapRemoteHandle, "Height", &sIIntegerHandleHeight);
		if (eStApiCError != StApiCError_NoError) break;

		eStApiCError = psApiFunctions->GenApi->IInteger->GetMax(&sIIntegerHandleHeight, &nMaximumHeight);
		if (eStApiCError != StApiCError_NoError) break;

		// Heightを最大値に設定します。
		// Set Height to its maximum value.
		eStApiCError = psApiFunctions->GenApi->IInteger->SetValue(&sIIntegerHandleHeight, nMaximumHeight, false);
		if (eStApiCError != StApiCError_NoError) break;

		// ==============================================================================================================

		eStApiCError = psApiFunctions->StApi->IStDevice->CreateIStDataStream(&sIStDeviceHandle, 0, NULL, &sIStDataStreamHandle);
		if (eStApiCError != StApiCError_NoError) break;
		eStApiCError = psApiFunctions->StApi->IStDataStream->StartAcquisition(&sIStDataStreamHandle, nCountOfImagesToGrab, ACQ_START_FLAGS_DEFAULT);
		if (eStApiCError != StApiCError_NoError) break;
		eStApiCError = psApiFunctions->StApi->IStDevice->AcquisitionStart(&sIStDeviceHandle);
		if (eStApiCError != StApiCError_NoError) break;

		for (;;)
		{
			bool8_t isGrabbing;
			StApiHandle_t sIStStreamBufferHandle = { NULL };
			StApiHandle_t sIStStreamBufferInfoHandle = { NULL };
			bool8_t isImagePresent;

			eStApiCError = psApiFunctions->StApi->IStDataStream->IsGrabbing(&sIStDataStreamHandle, &isGrabbing);
			if (eStApiCError != StApiCError_NoError) break;
			if (!isGrabbing) break;

			eStApiCError = psApiFunctions->StApi->IStDataStream->RetrieveBuffer(&sIStDataStreamHandle, 5000, &sIStStreamBufferHandle);
			if (eStApiCError != StApiCError_NoError) break;

			eStApiCError = psApiFunctions->StApi->IStStreamBuffer->GetIStStreamBufferInfo(&sIStStreamBufferHandle, &sIStStreamBufferInfoHandle);
			if (eStApiCError != StApiCError_NoError) break;

			eStApiCError = psApiFunctions->StApi->IStStreamBufferInfo->IsImagePresent(&sIStStreamBufferInfoHandle, &isImagePresent);
			if (eStApiCError != StApiCError_NoError) break;

			if (isImagePresent)
			{
				StApiHandle_t sIStImageHandle = { NULL };
				size_t nImageWidth;
				size_t nImageHeight;

				eStApiCError = psApiFunctions->StApi->IStStreamBuffer->GetIStImage(&sIStStreamBufferHandle, &sIStImageHandle);
				if (eStApiCError != StApiCError_NoError) break;

				eStApiCError = psApiFunctions->StApi->IStImage->GetImageWidth(&sIStImageHandle, &nImageWidth);
				if (eStApiCError != StApiCError_NoError) break;

				eStApiCError = psApiFunctions->StApi->IStImage->GetImageHeight(&sIStImageHandle, &nImageHeight);
				if (eStApiCError != StApiCError_NoError) break;

				{

					StApiHandle_t sIStStreamBufferInfoHandle = { NULL };
					uint64_t iFrameID = 0;
					void *pBuffer = NULL;

					eStApiCError = psApiFunctions->StApi->IStStreamBuffer->GetIStStreamBufferInfo(&sIStStreamBufferHandle, &sIStStreamBufferInfoHandle);
					if (eStApiCError != StApiCError_NoError) break;

					eStApiCError = psApiFunctions->StApi->IStStreamBufferInfo->GetFrameID(&sIStStreamBufferInfoHandle, &iFrameID);
					if (eStApiCError != StApiCError_NoError) break;

					eStApiCError = psApiFunctions->StApi->IStImage->GetImageBuffer(&sIStImageHandle, &pBuffer);
					if (eStApiCError != StApiCError_NoError) break;

#ifdef _MSC_VER
					printf("BlockID=%llu Size: %Iu x %Iu First byte = %u\r\n", iFrameID, nImageWidth, nImageHeight, *(uint8_t*)pBuffer);
#else
					printf("BlockID=%llu Size: %zu x %zu First byte = %u\r\n", iFrameID, nImageWidth, nImageHeight, *(uint8_t*)pBuffer);
#endif
				}
			}
			else
			{
				printf("Image data does not exist\r\n");
			}
			eStApiCError = psApiFunctions->StApi->IStStreamBuffer->Release(&sIStStreamBufferHandle);
			if (eStApiCError != StApiCError_NoError) break;
		}

		psApiFunctions->StApi->IStDevice->AcquisitionStop(&sIStDeviceHandle);
		psApiFunctions->StApi->IStDataStream->StopAcquisition(&sIStDataStreamHandle, ACQ_STOP_FLAGS_DEFAULT);

		break;
	} 

	if (eStApiCError != StApiCError_NoError)
	{
		OutputErrorInfo(psApiFunctions);
	}

	if (sIStDataStreamHandle.Handle)
	{
		psApiFunctions->StApi->IStDataStream->Release(&sIStDataStreamHandle);
	}

	if (sIStDeviceHandle.Handle)
	{
		psApiFunctions->StApi->IStDevice->Release(&sIStDeviceHandle);
	}

	if (sIStSystemHandle.Handle)
	{
		psApiFunctions->StApi->IStSystem->Release(&sIStSystemHandle);
	}

	psApiFunctions->StApi->StApiCTerminate();

	printf("Press Enter to exit.");
	getchar();

	return 0;
}

