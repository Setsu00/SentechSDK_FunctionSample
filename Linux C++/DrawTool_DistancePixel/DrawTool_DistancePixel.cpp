/*
	Distance in Pixels: 選択した線分の距離をpixel単位で計算する。
	Distance in Pixels: Calculate the selected line length in pixels.
*/
#include <StApi_TL.h>
#include <StApi_GUI.h>
#include <cmath>
#include <iostream>
#include <thread>
#include <chrono>
using namespace StApi;
using namespace std;

// 描画ノードの座標比率をピクセルへ変換
// Convert normalized drawing coordinates to pixels.
void OnDrawingChanged(GenApi::INode *, IStImageDisplayWnd *pIStImageDisplayWnd)
{
    try
    {
        GenApi::CIntegerPtr pIntegerPointSelector = pIStImageDisplayWnd->GetINodeMap()->GetNode("DrawingObjectPointSelector");
        GenApi::CFloatPtr pFloatPointX = pIStImageDisplayWnd->GetINodeMap()->GetNode("DrawingObjectPointX");
        GenApi::CFloatPtr pFloatPointY = pIStImageDisplayWnd->GetINodeMap()->GetNode("DrawingObjectPointY");
        IStImage *pIStImage = pIStImageDisplayWnd->GetRegisteredIStImage();
        // DrawingObjectPointSelectorを0に設定し、始点を選択します。
        // Set DrawingObjectPointSelector to 0 to select the first endpoint.
        pIntegerPointSelector->SetValue(0);
        const double x1 = pFloatPointX->GetValue() * pIStImage->GetImageWidth();
        const double y1 = pFloatPointY->GetValue() * pIStImage->GetImageHeight();
        // DrawingObjectPointSelectorを1に設定し、終点を選択します。
        // Set DrawingObjectPointSelector to 1 to select the second endpoint.
        pIntegerPointSelector->SetValue(1);
        const double x2 = pFloatPointX->GetValue() * pIStImage->GetImageWidth();
        const double y2 = pFloatPointY->GetValue() * pIStImage->GetImageHeight();
        cout << "Selected line distance: " << std::hypot(x1-x2, y1-y2) << " pixels" << endl;
    }
    catch (const GenICam::GenericException &e) { cerr << e.GetDescription() << endl; }
}
int main()
{
    try
    {
        CStApiAutoInit objStApiAutoInit;
        CIStSystemPtr pIStSystem(CreateIStSystem(StSystemVendor_Sentech));
        CIStDevicePtr pIStDevice(pIStSystem->CreateFirstIStDevice());
        CIStDataStreamPtr pIStDataStream(pIStDevice->CreateIStDataStream(0));
        CIStImageDisplayWndPtr pIStImageDisplayWnd(CreateIStWnd(StWindowType_ImageDisplay));
        cout << "Device=" << pIStDevice->GetIStDeviceInfo()->GetDisplayName() << endl;

        // 最初の画像を登録してから描画イベントを有効化
        // Register an image before enabling drawing callbacks.
        pIStDataStream->StartAcquisition(1);
        pIStDevice->AcquisitionStart();
        {
            CIStStreamBufferPtr pIStStreamBuffer(pIStDataStream->RetrieveBuffer(5000));
            if (!pIStStreamBuffer->GetIStStreamBufferInfo()->IsImagePresent())
            {
                pIStDevice->AcquisitionStop();
                pIStDataStream->StopAcquisition();
                cerr << "Image data does not exist." << endl;
                return 1;
            }
            pIStImageDisplayWnd->RegisterIStImage(pIStStreamBuffer->GetIStImage());
        }
        pIStDevice->AcquisitionStop();
        pIStDataStream->StopAcquisition();
        // ==============================================================================================================
        // 描画ツールを有効化し選択ノードへコールバック登録
        // Enable drawing and register the selection callback.
        pIStImageDisplayWnd->SetBuiltInDrawingToolbar(true);
        GenApi::CIntegerPtr pIntegerDrawingObjectSelector = pIStImageDisplayWnd->GetINodeMap()->GetNode("DrawingObjectSelector");
        CIStRegisteredCallbackPtr pIStRegisteredCallback(StApi::RegisterCallback(pIntegerDrawingObjectSelector->GetNode(), OnDrawingChanged,
                                              (IStImageDisplayWnd *)pIStImageDisplayWnd, GenApi::cbPostOutsideLock));
        pIStImageDisplayWnd->Show(NULL, StWindowMode_Modaless);

        // Linux Qtのイベントはメインスレッドで処理
        // Process Linux Qt events on the main thread.
        while (pIStImageDisplayWnd->IsVisible())
        {
            processEventGUI();
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        // ==============================================================================================================
    }
    catch (const GenICam::GenericException &e) { cerr << e.GetDescription() << endl; return 1; }
    return 0;
}
