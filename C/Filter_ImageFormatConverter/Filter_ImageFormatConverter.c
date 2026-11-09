/*
	画像をMono8へ変換
	Convert acquired images to Mono8.
*/
#include <StApi_C.h>
#include <stdio.h>

int main(void)
{
	PApiFunctions psApiFunctions = NULL;
	EStApiCError_t eStApiCError = StApiCInitialize(STAPI_VERSION, &psApiFunctions);
	StApiHandle_t sIStSystemHandle = { NULL }, sIStDeviceHandle = { NULL }, sIStDataStreamHandle = { NULL };
	StApiHandle_t sIStStreamBufferHandle = { NULL };
	bool8_t isDataStreamStarted = false, isDeviceStarted = false;
	int nResult = 0;
	StApiHandle_t sIStConverterHandle = { NULL }, sIStPixelFormatConverterHandle = { NULL }, sIStImageBufferHandle = { NULL };
	if (eStApiCError != StApiCError_NoError) return 1;
	for (;;)
	{
		eStApiCError = psApiFunctions->StApi->IStSystem->CreateIStSystem(StSystemVendor_Default, StInterfaceType_All, &sIStSystemHandle);
		if (eStApiCError != StApiCError_NoError) break;
		eStApiCError = psApiFunctions->StApi->IStSystem->CreateFirstIStDevice(&sIStSystemHandle, DEVICE_ACCESS_CONTROL, &sIStDeviceHandle);
		if (eStApiCError != StApiCError_NoError) break;
		{
			StApiHandle_t sIStInfoHandle = { NULL };
			char name[1024]; size_t length = sizeof(name);
			eStApiCError = psApiFunctions->StApi->IStDevice->GetIStDeviceInfo(&sIStDeviceHandle, &sIStInfoHandle);
			if (eStApiCError != StApiCError_NoError) break;
			eStApiCError = psApiFunctions->StApi->IStDeviceInfo->GetDisplayNameA(&sIStInfoHandle, name, &length);
			if (eStApiCError != StApiCError_NoError) break;
			printf("Device=%s\n", name);
		}
		eStApiCError = psApiFunctions->StApi->IStDevice->CreateIStDataStream(&sIStDeviceHandle, 0, NULL, &sIStDataStreamHandle);
		if (eStApiCError != StApiCError_NoError) break;
		// ==============================================================================================================
		// PixelFormatConverterを生成します。
		// Create the PixelFormatConverter.
		eStApiCError = psApiFunctions->StApi->IStConverter->CreateIStConverter(StConverterType_PixelFormat, &sIStConverterHandle);
		if (eStApiCError != StApiCError_NoError) break;
		eStApiCError = psApiFunctions->StApi->IStPixelFormatConverter->GetIStPixelFormatConverter(&sIStConverterHandle, &sIStPixelFormatConverterHandle);
		if (eStApiCError != StApiCError_NoError) break;

		// 変換後のピクセルフォーマットをMono8に設定します。
		// Set the destination pixel format to Mono8.
		eStApiCError = psApiFunctions->StApi->IStPixelFormatConverter->SetDestinationPixelFormat(&sIStPixelFormatConverterHandle, StPFNC_Mono8);
		if (eStApiCError != StApiCError_NoError) break;

		// 変換後の画像を格納するバッファを生成します。
		// Create a buffer for the converted image.
		eStApiCError = psApiFunctions->StApi->IStImageBuffer->CreateIStImageBuffer(NULL, &sIStImageBufferHandle);
		if (eStApiCError != StApiCError_NoError) break;
		// ==============================================================================================================
		eStApiCError = psApiFunctions->StApi->IStDataStream->StartAcquisition(&sIStDataStreamHandle, 100, ACQ_START_FLAGS_DEFAULT);
		if (eStApiCError != StApiCError_NoError) break;
		isDataStreamStarted = true;
		eStApiCError = psApiFunctions->StApi->IStDevice->AcquisitionStart(&sIStDeviceHandle);
		if (eStApiCError != StApiCError_NoError) break;
		isDeviceStarted = true;
		for (;;)
		{
			bool8_t isGrabbing = false, isImagePresent = false;
			StApiHandle_t sIStInfoHandle = { NULL }, sIStImageHandle = { NULL };
			size_t nImageWidth = 0, nImageHeight = 0;
			uint64_t iFrameID = 0;
			eStApiCError = psApiFunctions->StApi->IStDataStream->IsGrabbing(&sIStDataStreamHandle, &isGrabbing);
			if (eStApiCError != StApiCError_NoError) break;
			if (!isGrabbing) break;
			eStApiCError = psApiFunctions->StApi->IStDataStream->RetrieveBuffer(&sIStDataStreamHandle, 5000, &sIStStreamBufferHandle);
			if (eStApiCError != StApiCError_NoError) break;
			eStApiCError = psApiFunctions->StApi->IStStreamBuffer->GetIStStreamBufferInfo(&sIStStreamBufferHandle, &sIStInfoHandle);
			if (eStApiCError != StApiCError_NoError) break;
			eStApiCError = psApiFunctions->StApi->IStStreamBufferInfo->IsImagePresent(&sIStInfoHandle, &isImagePresent);
			if (eStApiCError != StApiCError_NoError) break;
			if (isImagePresent)
			{
				eStApiCError = psApiFunctions->StApi->IStStreamBuffer->GetIStImage(&sIStStreamBufferHandle, &sIStImageHandle);
				if (eStApiCError != StApiCError_NoError) break;

				// 取得画像をMono8へ変換します。
				// Convert the acquired image to Mono8.
				eStApiCError = psApiFunctions->StApi->IStConverter->Convert(&sIStConverterHandle, &sIStImageHandle, &sIStImageBufferHandle);
				if (eStApiCError != StApiCError_NoError) break;
				eStApiCError = psApiFunctions->StApi->IStImageBuffer->GetIStImage(&sIStImageBufferHandle, &sIStImageHandle);
				if (eStApiCError != StApiCError_NoError) break;
				eStApiCError = psApiFunctions->StApi->IStImage->GetImageWidth(&sIStImageHandle, &nImageWidth);
				if (eStApiCError != StApiCError_NoError) break;
				eStApiCError = psApiFunctions->StApi->IStImage->GetImageHeight(&sIStImageHandle, &nImageHeight);
				if (eStApiCError != StApiCError_NoError) break;
				eStApiCError = psApiFunctions->StApi->IStStreamBufferInfo->GetFrameID(&sIStInfoHandle, &iFrameID);
				if (eStApiCError != StApiCError_NoError) break;
				printf("BlockID=%llu Size=%zu x %zu\n", (unsigned long long)iFrameID, nImageWidth, nImageHeight);
			}
			else printf("Image data does not exist.\n");
			eStApiCError = psApiFunctions->StApi->IStStreamBuffer->Release(&sIStStreamBufferHandle);
			if (eStApiCError != StApiCError_NoError) break;
			sIStStreamBufferHandle.Handle = NULL;
		}

		break;
	}
	if (eStApiCError != StApiCError_NoError)
	{
		char description[1024]; size_t length = sizeof(description);
		nResult = 1;
		if (psApiFunctions->StApi->GetLastErrorDescriptionA(description, &length) == StApiCError_NoError)
		fprintf(stderr, "%s\n", description);
	}
	if (isDeviceStarted) psApiFunctions->StApi->IStDevice->AcquisitionStop(&sIStDeviceHandle);
	if (isDataStreamStarted) psApiFunctions->StApi->IStDataStream->StopAcquisition(&sIStDataStreamHandle, ACQ_STOP_FLAGS_DEFAULT);
	if (sIStStreamBufferHandle.Handle) psApiFunctions->StApi->IStStreamBuffer->Release(&sIStStreamBufferHandle);
	if (sIStImageBufferHandle.Handle) psApiFunctions->StApi->IStImageBuffer->Release(&sIStImageBufferHandle);
	if (sIStConverterHandle.Handle) psApiFunctions->StApi->IStConverter->Release(&sIStConverterHandle);

	if (sIStDataStreamHandle.Handle) psApiFunctions->StApi->IStDataStream->Release(&sIStDataStreamHandle);
	if (sIStDeviceHandle.Handle) psApiFunctions->StApi->IStDevice->Release(&sIStDeviceHandle);
	if (sIStSystemHandle.Handle) psApiFunctions->StApi->IStSystem->Release(&sIStSystemHandle);
	psApiFunctions->StApi->StApiCTerminate();
	printf("Press Enter to exit.\n");
	getchar();
	return nResult;
}
