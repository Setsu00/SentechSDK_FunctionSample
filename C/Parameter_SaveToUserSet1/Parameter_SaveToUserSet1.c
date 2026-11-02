/*
	Save To UserSet1: 現在の設定をUserSet1へ保存し、起動時設定に指定する。
	Save To UserSet1: Save current settings to UserSet1 and select it as the startup set.
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
		// 現在の設定をUserSet1へ保存し、起動時設定に指定します。
		// Save current settings to UserSet1 and select it as the startup set.

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

		// UserSetSelectorでUserSet1を選択します。
		// Select UserSet1 with UserSetSelector.
		StApiHandle_t sIEnumerationHandleUserSetSelector = { NULL };
		eStApiCError = psApiFunctions->GenApi->INodeMap->GetNodeA(&sINodeMapRemoteHandle, "UserSetSelector", &sIEnumerationHandleUserSetSelector);
		if (eStApiCError != StApiCError_NoError) break;
		eStApiCError = psApiFunctions->GenApi->IEnumeration->SetStringValueA(&sIEnumerationHandleUserSetSelector, "UserSet1");
		if (eStApiCError != StApiCError_NoError) break;

		// UserSetSaveを実行して現在の設定を保存します。
		// Execute UserSetSave to save the current settings.
		StApiHandle_t sICommandHandleUserSetSave = { NULL };
		eStApiCError = psApiFunctions->GenApi->INodeMap->GetNodeA(&sINodeMapRemoteHandle, "UserSetSave", &sICommandHandleUserSetSave);
		if (eStApiCError != StApiCError_NoError) break;
		eStApiCError = psApiFunctions->GenApi->ICommand->Execute(&sICommandHandleUserSetSave, false);
		if (eStApiCError != StApiCError_NoError) break;

		// UserSetDefaultをUserSet1に設定します。
		// Set UserSetDefault to UserSet1.
		StApiHandle_t sIEnumerationHandleUserSetDefault = { NULL };
		eStApiCError = psApiFunctions->GenApi->INodeMap->GetNodeA(&sINodeMapRemoteHandle, "UserSetDefault", &sIEnumerationHandleUserSetDefault);
		if (eStApiCError != StApiCError_NoError) break;
		eStApiCError = psApiFunctions->GenApi->IEnumeration->SetStringValueA(&sIEnumerationHandleUserSetDefault, "UserSet1");
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

