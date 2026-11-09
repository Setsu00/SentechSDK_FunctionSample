/*
Distance in Pixels: 選択した線分の距離をpixel単位で計算する。
Distance in Pixels: Calculate the selected line length in pixels.
*/
#include <StApi_C.h>
#include <stdio.h>
#include <math.h>

typedef struct DrawingContext
{
	PApiFunctions psApiFunctions;
	StApiHandle_t sIStWndHandle;
} DrawingContext;

// 選択された線分の座標を読み、画像のピクセル数へ変換
// Read selected line coordinates in pixels.
static void STAPI_CALLTYPE OnDrawingChanged(PStApiHandle_t psChangedNodeHandle, void *pUserContext)
{
	DrawingContext *pContext = (DrawingContext *)pUserContext;
	PApiFunctions psApiFunctions = pContext->psApiFunctions;
	EStApiCError_t eStApiCError = StApiCError_NoError;
	StApiHandle_t sINodeMapHandle = { NULL }, sIStImageHandle = { NULL };
	size_t nImageWidth = 0, nImageHeight = 0;
	double x1, y1, x2, y2;
	(void)psChangedNodeHandle;
	for (;;)
	{
		eStApiCError = psApiFunctions->StApi->IStWnd->GetINodeMap(&pContext->sIStWndHandle, &sINodeMapHandle);
		if (eStApiCError != StApiCError_NoError) break;
		eStApiCError = psApiFunctions->StApi->IStImageDisplayWnd->GetRegisteredIStImage(&pContext->sIStWndHandle, &sIStImageHandle);
		if (eStApiCError != StApiCError_NoError) break;
		eStApiCError = psApiFunctions->StApi->IStImage->GetImageWidth(&sIStImageHandle, &nImageWidth);
		if (eStApiCError != StApiCError_NoError) break;
		eStApiCError = psApiFunctions->StApi->IStImage->GetImageHeight(&sIStImageHandle, &nImageHeight);
		if (eStApiCError != StApiCError_NoError) break;
		
		// DrawingObjectPointSelectorノードを取得・制御 
		// Get and control DrawingObjectPointSelector.
		StApiHandle_t sIIntegerDrawingObjectPointSelectorHandle = { NULL };
		eStApiCError = psApiFunctions->GenApi->INodeMap->GetNodeA(&sINodeMapHandle, "DrawingObjectPointSelector", &sIIntegerDrawingObjectPointSelectorHandle);
		if (eStApiCError != StApiCError_NoError) break;
		
		// DrawingObjectPointXノードを取得・制御 
		// Get and control DrawingObjectPointX. 
		StApiHandle_t sIFloatDrawingObjectPointXHandle = { NULL };
		eStApiCError = psApiFunctions->GenApi->INodeMap->GetNodeA(&sINodeMapHandle, "DrawingObjectPointX", &sIFloatDrawingObjectPointXHandle);
		if (eStApiCError != StApiCError_NoError) break;
		
		// DrawingObjectPointYノードを取得・制御 
		// Get and control DrawingObjectPointY.
		StApiHandle_t sIFloatDrawingObjectPointYHandle = { NULL };
		eStApiCError = psApiFunctions->GenApi->INodeMap->GetNodeA(&sINodeMapHandle, "DrawingObjectPointY", &sIFloatDrawingObjectPointYHandle);
		if (eStApiCError != StApiCError_NoError) break;
		
		// DrawingObjectPointSelectorを0に設定し、始点を選択します。
		// Set DrawingObjectPointSelector to 0 to select the first endpoint.
		eStApiCError = psApiFunctions->GenApi->IInteger->SetValue(&sIIntegerDrawingObjectPointSelectorHandle, 0, true);
		if (eStApiCError != StApiCError_NoError) break;
		eStApiCError = psApiFunctions->GenApi->IFloat->GetValue(&sIFloatDrawingObjectPointXHandle, true, false, &x1);
		if (eStApiCError != StApiCError_NoError) break;
		eStApiCError = psApiFunctions->GenApi->IFloat->GetValue(&sIFloatDrawingObjectPointYHandle, true, false, &y1);
		if (eStApiCError != StApiCError_NoError) break;
		
		// DrawingObjectPointSelectorを1に設定し、終点を選択します。
		// Set DrawingObjectPointSelector to 1 to select the second endpoint.
		eStApiCError = psApiFunctions->GenApi->IInteger->SetValue(&sIIntegerDrawingObjectPointSelectorHandle, 1, true);
		if (eStApiCError != StApiCError_NoError) break;
		eStApiCError = psApiFunctions->GenApi->IFloat->GetValue(&sIFloatDrawingObjectPointXHandle, true, false, &x2);
		if (eStApiCError != StApiCError_NoError) break;
		eStApiCError = psApiFunctions->GenApi->IFloat->GetValue(&sIFloatDrawingObjectPointYHandle, true, false, &y2);
		if (eStApiCError != StApiCError_NoError) break;
		x1 = (x1-x2)*nImageWidth; y1 = (y1-y2)*nImageHeight;
		printf("Selected line distance: %.2f pixels\n", sqrt(x1*x1+y1*y1));
		break;
	}
	if (eStApiCError != StApiCError_NoError)
	{
		char text[1024]; size_t length=sizeof(text);
		if (psApiFunctions->StApi->GetLastErrorDescriptionA(text, &length)==StApiCError_NoError)
		fprintf(stderr, "%s\n", text);
	}
}

int main(void)
{
	PApiFunctions psApiFunctions = NULL;
	EStApiCError_t eStApiCError = StApiCInitialize(STAPI_VERSION, &psApiFunctions);
	StApiHandle_t sIStSystemHandle = { NULL }, sIStDeviceHandle = { NULL }, sIStDataStreamHandle = { NULL };
	StApiHandle_t sIStStreamBufferHandle = { NULL }, sINodeMapHandle = { NULL };
	bool8_t isDataStreamStarted = false, isDeviceStarted = false;
	int nResult = 0;
	StApiHandle_t sIStWndHandle = { NULL }, sIStImageDisplayWndHandle = { NULL }, sIStRegisteredCallbackHandle = { NULL };
	DrawingContext sDrawingContext;
	if (eStApiCError != StApiCError_NoError) return 1;
	for (;;)
	{
		eStApiCError = psApiFunctions->StApi->IStSystem->CreateIStSystem(StSystemVendor_Sentech, StInterfaceType_All, &sIStSystemHandle);
		if (eStApiCError != StApiCError_NoError) break;
		eStApiCError = psApiFunctions->StApi->IStSystem->CreateFirstIStDevice(&sIStSystemHandle, DEVICE_ACCESS_CONTROL, &sIStDeviceHandle);
		if (eStApiCError != StApiCError_NoError) break;
		{
			StApiHandle_t sIStDeviceInfoHandle = { NULL };
			char name[1024]; size_t length = sizeof(name);
			eStApiCError = psApiFunctions->StApi->IStDevice->GetIStDeviceInfo(&sIStDeviceHandle, &sIStDeviceInfoHandle);
			if (eStApiCError != StApiCError_NoError) break;
			eStApiCError = psApiFunctions->StApi->IStDeviceInfo->GetDisplayNameA(&sIStDeviceInfoHandle, name, &length);
			if (eStApiCError != StApiCError_NoError) break;
			printf("Device=%s\n", name);
		}
		eStApiCError = psApiFunctions->StApi->IStDevice->CreateIStDataStream(&sIStDeviceHandle, 0, NULL, &sIStDataStreamHandle);
		if (eStApiCError != StApiCError_NoError) break;
		// ==============================================================================================================
		// 描画ツールを有効にします。
		// Enable the drawing tool.
		eStApiCError = psApiFunctions->StApi->IStWnd->CreateIStWnd(StWindowType_ImageDisplay, &sIStWndHandle);
		if (eStApiCError != StApiCError_NoError) break;
		eStApiCError = psApiFunctions->StApi->IStImageDisplayWnd->GetIStImageDisplayWnd(&sIStWndHandle, &sIStImageDisplayWndHandle);
		if (eStApiCError != StApiCError_NoError) break;
		eStApiCError = psApiFunctions->StApi->IStImageDisplayWnd->SetBuiltInDrawingToolbar(&sIStImageDisplayWndHandle, true);
		if (eStApiCError != StApiCError_NoError) break;
		sDrawingContext.psApiFunctions = psApiFunctions; sDrawingContext.sIStWndHandle = sIStImageDisplayWndHandle;
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
			StApiHandle_t sIStStreamBufferInfoHandle = { NULL }, sIStImageHandle = { NULL };
			size_t nImageWidth = 0, nImageHeight = 0;
			uint64_t iFrameID = 0;
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
				eStApiCError = psApiFunctions->StApi->IStStreamBuffer->GetIStImage(&sIStStreamBufferHandle, &sIStImageHandle);
				if (eStApiCError != StApiCError_NoError) break;
				eStApiCError = psApiFunctions->StApi->IStImage->GetImageWidth(&sIStImageHandle, &nImageWidth);
				if (eStApiCError != StApiCError_NoError) break;
				eStApiCError = psApiFunctions->StApi->IStImage->GetImageHeight(&sIStImageHandle, &nImageHeight);
				if (eStApiCError != StApiCError_NoError) break;
				eStApiCError = psApiFunctions->StApi->IStStreamBufferInfo->GetFrameID(&sIStStreamBufferInfoHandle, &iFrameID);
				if (eStApiCError != StApiCError_NoError) break;
				printf("BlockID=%llu Size=%zu x %zu\n", (unsigned long long)iFrameID, nImageWidth, nImageHeight);
				eStApiCError = psApiFunctions->StApi->IStImageDisplayWnd->RegisterIStImage(&sIStImageDisplayWndHandle, &sIStImageHandle);
				if (eStApiCError != StApiCError_NoError) break;
				if (!sIStRegisteredCallbackHandle.Handle)
				{
					eStApiCError = psApiFunctions->StApi->IStWnd->GetINodeMap(&sIStWndHandle, &sINodeMapHandle);
					if (eStApiCError != StApiCError_NoError) break;

					// DrawingObjectSelectorノードを取得・制御
					// Get and control DrawingObjectSelector.
					StApiHandle_t sIIntegerDrawingObjectSelectorHandle = { NULL };
					eStApiCError = psApiFunctions->GenApi->INodeMap->GetNodeA(&sINodeMapHandle, "DrawingObjectSelector", &sIIntegerDrawingObjectSelectorHandle);
					if (eStApiCError != StApiCError_NoError) break;

					// DrawingObjectSelectorの変更コールバックを登録します。
					// Register a callback for changes to DrawingObjectSelector.
					eStApiCError = psApiFunctions->GenApi->INode->RegisterCallback(&sIIntegerDrawingObjectSelectorHandle, OnDrawingChanged, cbPostOutsideLock, &sDrawingContext, &sIStRegisteredCallbackHandle);
					if (eStApiCError != StApiCError_NoError) break;
					eStApiCError = psApiFunctions->StApi->IStWnd->Show(&sIStWndHandle, NULL, StWindowMode_ModalessOnNewThread);
					if (eStApiCError != StApiCError_NoError) break;
				}
			}
			else printf("Image data does not exist.\n");
			eStApiCError = psApiFunctions->StApi->IStStreamBuffer->Release(&sIStStreamBufferHandle);
			if (eStApiCError != StApiCError_NoError) break;
			sIStStreamBufferHandle.Handle = NULL;
		}

		if (eStApiCError != StApiCError_NoError) break;
		printf("Draw a line and select it to print its length. Press Enter to finish.\n");
		getchar();
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
	if (sIStRegisteredCallbackHandle.Handle) psApiFunctions->StApi->IStRegisteredCallback->Release(&sIStRegisteredCallbackHandle);
	if (sIStWndHandle.Handle) psApiFunctions->StApi->IStWnd->Close(&sIStWndHandle);
	if (sIStWndHandle.Handle) psApiFunctions->StApi->IStWnd->Release(&sIStWndHandle);

	if (sIStDataStreamHandle.Handle) psApiFunctions->StApi->IStDataStream->Release(&sIStDataStreamHandle);
	if (sIStDeviceHandle.Handle) psApiFunctions->StApi->IStDevice->Release(&sIStDeviceHandle);
	if (sIStSystemHandle.Handle) psApiFunctions->StApi->IStSystem->Release(&sIStSystemHandle);
	psApiFunctions->StApi->StApiCTerminate();
	printf("Press Enter to exit.\n");
	getchar();
	return nResult;
}
