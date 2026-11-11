/*
    CameraAutoFuncSample.cpp
*/

// StApi用ヘッダーをインクルードします。
// Include files for using StApi.
#include <StApi_TL.h>
#include <StApi_GUI.h>

// 入出力用ヘッダーをインクルードします。
// Include for input/output related functions.
#include <iomanip>

#include <thread>

#include <unistd.h>   //getch
#include <termios.h>  //getch

// キー入力を検出する補助関数
// Helper function to detect key pressed
#include "kbhit.h" 

// StApiの名前空間
// Namespace for using StApi.
using namespace StApi;

// cout用の名前空間
// Namespace for using cout.
using namespace std;

// GenApiの名前空間
// Namespace for using GenApi.
using namespace GenApi;

typedef IStImageDisplayWnd* UserParam_t;

// ESCキーの定義
// Define for ESC key input.
#define ESC_KEY        0x1B
#define BACKSPACE_KEY   0x7F

// 入力補助関数の宣言
// Declearation of input helper function.
bool ValueInputChk(string * result);

// getch入力補助関数の宣言
// Declearation of input helper of getch().
char getch();

//-----------------------------------------------------------------------------
// コールバック処理用の関数
// Function for handling callback action
//-----------------------------------------------------------------------------
void OnCallback(IStCallbackParamBase *pIStCallbackParamBase, UserParam_t pvContext)
{
    // コールバック種別を確認し、NewBufferイベントだけを処理します。
    // Check callback type. Only NewBuffer event is handled in here.
    if(pIStCallbackParamBase->GetCallbackType() == StCallbackType_GenTLEvent_DataStreamNewBuffer)
    {
        // NewBufferイベントの受信パラメーターをIStCallbackParamGenTLEventNewBufferへ変換します。
        // In case of receiving a NewBuffer events:
        // Convert received callback parameter into IStCallbackParamGenTLEventNewBuffer for acquiring additional information.
        IStCallbackParamGenTLEventNewBuffer *pIStCallbackParamGenTLEventNewBuffer = dynamic_cast<IStCallbackParamGenTLEventNewBuffer*>(pIStCallbackParamBase);
        
        try
        {
            // 受信したコールバックパラメーターからIStDataStreamを取得します。
            // Get the IStDataStream interface pointer from the received callback parameter.
            IStDataStream *pIStDataStream = pIStCallbackParamGenTLEventNewBuffer->GetIStDataStream();

            // コールバックで通知されたバッファーを取得します。
            // Retrieve the buffer pointer of image data for that callback indicated there is a buffer received.
            CIStStreamBufferPtr pIStStreamBuffer(pIStDataStream->RetrieveBuffer(0));

            // 取得データに画像が含まれているか確認します。
            // Check if the acquired data contains image data.
            if (pIStStreamBuffer->GetIStStreamBufferInfo()->IsImagePresent())
            {

                // 画像を扱うためのIStImageオブジェクトを生成します。
                // If yes, we create a IStImage object for further image handling.
                IStImage *pIStImage = pIStStreamBuffer->GetIStImage();

                // 表示ウィンドウが表示されているか確認します。
                // Check if display window is visible.
                if (!pvContext->IsVisible())
                {
                    // ウィンドウの位置とサイズを設定します。
                    // Set the position and size of the window.
                    pvContext->SetPosition(600, 0, pIStImage->GetImageWidth(), pIStImage->GetImageHeight());

                    // 別スレッドでウィンドウを表示します。
                    // Create a new thread to display the window.
                    pvContext->Show(NULL, StWindowMode_ModalessOnNewThread);
                }

                // 表示画像を登録します。画像データはコピーされ、元のバッファーを解放できます。
                // Register the image to be displayed.
                // This will have a copy of the image data and original buffer can be released if necessary.
                pvContext->RegisterIStImage(pIStImage);

            }
            else
            {
                // 取得データに画像が含まれていない場合
                // If the acquired data contains no image data...
                //cout << "Image data does not exist." << endl;
            }
        }
        catch (const GenICam::GenericException &e)
        {
            // 例外の説明を表示します。
            // If any exception occurred, display the description of the error here.
            cerr << endl << "An exception occurred." << endl << e.GetDescription() << endl;
        }
    }
}

//-----------------------------------------------------------------------------
// 登録用のコールバック関数
// Callback function for registering
//-----------------------------------------------------------------------------
void OnStCallbackCFunction(IStCallbackParamBase *pIStCallbackParamBase, UserParam_t pvContext)
{
    OnCallback(pIStCallbackParamBase, pvContext);
}

//-----------------------------------------------------------------------------
// 自動露光設定用の関数
// Auto Exposure Setting Handling function
//-----------------------------------------------------------------------------
void funcAutoExposureSetting(GenApi::CNodeMapPtr nodeMap)
{
    try
    {
        // ExposureAutoノードを取得します。
        // Acquire Node of ExposureAuto for later usage.
        GenApi::CEnumerationPtr pIEnumExpoAuto(nodeMap->GetNode("ExposureAuto"));

        for (;;)
        {
            // 現在の自動露光モードを表示し、選択を待ちます。
            // Display current Auto Exposure Mode and wait for selection.
            cout << "Current Exposure Mode: " << pIEnumExpoAuto->GetCurrentEntry()->GetSymbolic()<< endl;
            cout << "1. Switch On Auto Exposure. (Continuous)" << endl;
            cout << "2. Switch Off Auto Exposure. (Off)" << endl;
            cout << "3. Exit Function." << endl;
            cout << "Please select: ";

            int iExpoAutoSel;
            cin >> iExpoAutoSel;

            switch (iExpoAutoSel)
            {
            case 1:
                // ExposureAutoをContinuousに設定し、自動露光を継続します。
                // Set Auto Exposure Mode to Continuous.
                *pIEnumExpoAuto = "Continuous";
                break;
            case 2:
                // ExposureAutoをOffに設定し、自動露光を無効にします。
                // Set Auto Exposure Mode Off.
                *pIEnumExpoAuto = "Off";
                break;
            case 3:
                return;
            default:
                break;
            }
        }
    }
    catch (const GenICam::GenericException &e)
    {
        cerr << endl << "An exception occurred." << endl << e.GetDescription() << endl;
        cout << "Press any key to exit." << endl;
        getch();
    }
}

//-----------------------------------------------------------------------------
// ゲイン設定用の関数
// Gain Setting Handling function
//-----------------------------------------------------------------------------
void funcAutoGainSetting(GenApi::CNodeMapPtr nodeMap)
{
    try
    {
        // GainAutoノードを取得します。
        // Get Node for GainAuto for later usage
        GenApi::CEnumerationPtr pIEnumGainAuto(nodeMap->GetNode("GainAuto"));

        for (;;)
        {
            // 現在の自動ゲイン設定を表示し、選択を待ちます。
            // Display Current Auto Gain Setting and wait for selection.
            cout << "Current Gain Mode: " << pIEnumGainAuto->GetCurrentEntry()->GetSymbolic() << endl;
            cout << "1. Switch On Auto Gain. (Continuous)" << endl;
            cout << "2. Switch Off Auto Gain. (Off)" << endl;
            cout << "3. Exit Function." << endl;
            cout << "Please select: ";

            int iGainAutoSel;
            cin >> iGainAutoSel;

            switch (iGainAutoSel)
            {
            case 1:
                // GainAutoをContinuousに設定し、自動ゲイン調整を継続します。
                // Set Auto Gain to continuous.
                *pIEnumGainAuto = "Continuous";
                break;
            case 2:
                // GainAutoをOffに設定し、自動ゲイン調整を無効にします。
                // Set Auto Gain to Off.
                *pIEnumGainAuto = "Off";
                break;
            case 3:
                return;
            default:
                break;
            }

        }
    }
    catch (const GenICam::GenericException &e)
    {
        cerr << endl << "An exception occurred." << endl << e.GetDescription() << endl;
        cout << "Press any key to exit." << endl;
        getch();
    }
}

//-----------------------------------------------------------------------------
// 自動調整の目標輝度
// Auto Luminance Target
//-----------------------------------------------------------------------------
void funcAutoLuminanceTarget(GenApi::CNodeMapPtr nodeMap)
{
    try
    {
        // AE/AGCの目標輝度を設定するAutoLuminanceTargetノードを取得します。
        // Acquire Auto Luminance Target node, which as the target brightness for AE/AGC.
        GenApi::CIntegerPtr pIIntLuminTgt(nodeMap->GetNode("AutoLuminanceTarget"));

        // ノードの有無を確認します。
        // Check if this node exist.
        if(pIIntLuminTgt == 0)
        {
            cerr << endl << "Auto Luminance setting method is not available for this camera." << endl;
            cout << "Press any key to exit." << endl;
            getch();
            return;
        }

        // 書き込み可能か確認します。自動露光または自動ゲインがContinuousの場合に設定できます。
        // Check if the node is writable.
        // There should be at least auto exposure or auto gain need to be switch to "Contiunous" or this value cannot be set.
        for (;;)
        {
            bool bWritable = GenApi::IsWritable(pIIntLuminTgt);
            if (!bWritable)
            {
                cout << "At least one of AE or AGC need to be switch on for setting this parameter!" << endl;
                cout << "Press any key to exit." << endl;
                getch();
                return;
            }

            // 目標輝度の設定範囲を取得し、設定値を入力します。
            // Acquire the range of AUto Luminance and read the input value.
            int64_t almMin, almMax;
            string sInput = "";
            almMin = pIIntLuminTgt->GetMin();
            almMax = pIIntLuminTgt->GetMax();
            cout << "Current Auto Luminance Target: " << pIIntLuminTgt->GetValue() << endl;
            cout << "Input New Auto Luminance Target (" << almMin  << " ~ "  << almMax << "), or press ESC to exit: " ;
            if (!ValueInputChk(&sInput))
                return;

            int64_t iInputValue = ::atoi(sInput.c_str());

            // 入力値をAutoLuminanceTargetに設定します。範囲外の値はここでは無視します。
            // Set target value. The value not in the range will be ignored here.
            if (iInputValue >= almMin && iInputValue <= almMax)
            {
                pIIntLuminTgt->SetValue(iInputValue);
            }
        }
    }
    catch (const GenICam::GenericException &e)
    {
        cerr << endl << "An exception occurred." << endl << e.GetDescription() << endl;
        cout << "Press any key to exit." << endl;
        getch();
    }
}

//-----------------------------------------------------------------------------
// 自動ホワイトバランス設定用の関数
// AWB Setting function
//-----------------------------------------------------------------------------
void funcAutoWBSetting(GenApi::CNodeMapPtr nodeMap)
{
    try
    {
        // BalanceWhiteAutoノードを取得します。
        // Acquire Node for Balance White Auto Setting.
        GenApi::CEnumerationPtr pIEnumBalanceWhite(nodeMap->GetNode("BalanceWhiteAuto"));
        
        // BalanceWhiteAutoノードを取得できない場合は、自動ホワイトバランスに対応していないため終了します。
        // If Node acquire failed, the camera should be monochrome one or have to AWB function.
        // Exit current setting.
        if (pIEnumBalanceWhite == 0)
        {
            cerr << endl << "Manual white balance setting is not available for this camera." << endl;
            cout << "Press any key to quit manual white balance setting." << endl;
            getch();
            return;
        }

        for (;;)
        {
            // 自動ホワイトバランスのモードを選択します。
            // Select AWB mode.
            cout << "Current AWB setting: " << pIEnumBalanceWhite->GetCurrentEntry()->GetSymbolic() << endl;
            cout << "Please select target AWB mode: " << endl;
            cout << "1. Off" << endl;
            cout << "2. Once (Will adjust WB then switch back to Off)" << endl;
            cout << "3. Continuous " << endl;
            cout << "4. Exit" << endl;

            int iAWBSel;
            cin >> iAWBSel;

            switch (iAWBSel)
            {
            case 1:
                // BalanceWhiteAutoをOffに設定し、現在のホワイトバランス値を使います。
                // Switch off AWB and use current WB value of camera.
                *pIEnumBalanceWhite = "Off";
                break;
            case 2:
                // BalanceWhiteAutoをOnceに設定し、一度だけ自動調整します。
                // Switch to AWB Once.
                // カメラは複数フレームでホワイトバランスを計算し、その後Offへ戻ります。
                // The camera will takes several frames for WB calculation then switch back to "Off".
                *pIEnumBalanceWhite = "Once";
                break;
            case 3:
                // BalanceWhiteAutoをContinuousに設定し、ホワイトバランスを継続して自動調整します。
                // Switch to AWB Continuous. The camera will continuously check and adjust WB value.
                *pIEnumBalanceWhite = "Continuous";
                break;
            default:
                return;
                break;
            }
        }
    }
    catch (const GenICam::GenericException &e)
    {
        cerr << endl << "An exception occurred." << endl << e.GetDescription() << endl;
        cout << "Press any key to exit." << endl;
        getch();
    }
}

//-----------------------------------------------------------------------------
// 露光時間・ゲイン値の確認用関数
// Exposure / Gain Value checking function
//-----------------------------------------------------------------------------
void funcExposureGainMonitor(GenApi::CNodeMapPtr nodeMap)
{
    try
    {
        // ExposureTimeノードを取得します。
        // Get Node for ExposureTime
        GenApi::CNodePtr pNodeExposureTime = nodeMap->GetNode("ExposureTime");

        // ExposureTimeノードがない場合は、露光時間設定に対応していないため終了します。
        // If ExposureTime node is not exist, the exposure time setting method is not supported by the camera thus we end current setting.
        if (pNodeExposureTime == 0)
        {
            cerr << endl << "Exposure Time setting method is not available for this camera." << endl;
            cout << "Press any key to exit." << endl;
            getch();
            return;
        }
        // 値を設定するため、ノードをCFloatPtrに変換します。
        // Convert Node to CFloatPtr for setting value
        GenApi::CFloatPtr pFloatExpoTime(pNodeExposureTime);

        // Gainノードを取得します。
        // Get Node for Gain
        GenApi::CNodePtr pNodeGain = nodeMap->GetNode("Gain");

        // 値を設定するため、ノードをCFloatPtrに変換します。
        // Convert Node to CFloatPtr for setting value
        GenApi::CFloatPtr pFloatGain(pNodeGain);

        cout << "Monitoring current Exposure Time and Gain Value...Press anykey to Exit." << endl;
        for(;;)
        {
            cout << "Current Exposure Time: " << pFloatExpoTime->GetValue() << " ; Gain: " << pFloatGain->GetValue() << "        \r";

            // NodeMapをポーリングしてカメラの最新値を取得します。
            // Set NodeMap Poll value to make it acquire latest data from camera.
            nodeMap->Poll(500);
            usleep(10000);

            // ESC入力で終了します。
            // Wait ESC for exit
            if (_kbhit())
            {
                getch();
                break;
            }
        }
    }
    catch (const GenICam::GenericException &e)
    {
        cerr << endl << "An exception occurred." << endl << e.GetDescription() << endl;
        cout << "Press any key to exit." << endl;
        getch();
    }

    
}

//-----------------------------------------------------------------------------
// 選択肢の表示処理
// Display Choice Worker
//-----------------------------------------------------------------------------
void displayChoiceWorker(CIStDevicePtr *pIStDevice, CIStDataStreamPtr *pIStDataStream, CNodeMapPtr *pNodeMapCameraParam, bool *isCompleted)
{
    // ホスト側の画像取得を開始します。
    // Start the image acquisition of the host (local machine) side.
    (*pIStDataStream)->StartAcquisition();

    // カメラ側の画像取得を開始します。
    // Start the image acquisition of the camera side.
    (*pIStDevice)->AcquisitionStart();
        
    // Enter入力まで画像取得を続けます。
    // Keep getting image until Enter key pressed.
    bool endLoop = false;

    // 選択した機能のデモを実行するループ
    // Loop for entering selected function for demostration.
    do
    {
        system("clear");
        cout << "========= Camera Auto Function Sample =============" << endl;
        cout << "1. Auto Exposure" << endl;
        cout << "2. Auto Gain" << endl;
        cout << "3. Auto Luminance Target" << endl;
        cout << "4. Balance White Auto" << endl;
        cout << "5. Monitor Current Exposure / Gain Value" << endl;
        cout << "6. Exit" << endl;
        cout << "===================================================" << endl;
        cout << "Please Select Function Number: ";

        int input;
        cin >> input;
            
        switch (input)
        {
        case 1:
            funcAutoExposureSetting(*pNodeMapCameraParam);
            break;
        case 2:
            funcAutoGainSetting(*pNodeMapCameraParam);
            break;
        case 3:
            funcAutoLuminanceTarget(*pNodeMapCameraParam);
            break;
        case 4:
            funcAutoWBSetting(*pNodeMapCameraParam);
            break;
        case 5:
            funcExposureGainMonitor(*pNodeMapCameraParam);
            break;
        default:
            endLoop = true;
            break;
        }
    } while (!endLoop);

    // カメラ側の画像取得を停止します。
    // Stop the image acquisition of the camera side.
    (*pIStDevice)->AcquisitionStop();

    // ホスト側の画像取得を停止します。
    // Stop the image acquisition of the host side.
    (*pIStDataStream)->StopAcquisition();

    *isCompleted = true;

}

//-----------------------------------------------------------------------------
// メインスレッド
// Main thread
//-----------------------------------------------------------------------------
int main(int /* argc */, char ** /* argv */)
{
    try
    {
        // 使用前にStApiを初期化します。
        // Initialize StApi before using.
        CStApiAutoInit objStApiAutoInit;

        // デバイス検出・接続用のSystemオブジェクトを生成します。
        // Create a system object for device scan and connection.
        CIStSystemPtr pIStSystem(CreateIStSystem(StSystemVendor_Sentech));

        // カメラオブジェクトを生成し、最初に検出したデバイスへ接続します。
        // Create a camera device object and connect to first detected device.
        CIStDevicePtr pIStDevice(pIStSystem->CreateFirstIStDevice());

        // GUIで表示する場合は、表示ウィンドウを生成します。
        // If using GUI for display, create a display window here.
        CIStImageDisplayWndPtr pIStImageDisplayWnd(CreateIStWnd(StWindowType_ImageDisplay));

        // 画像ストリーム用のDataStreamオブジェクトを生成します。
        // Create a DataStream object for handling image stream data.
        CIStDataStreamPtr pIStDataStream(pIStDevice->CreateIStDataStream(0));

        // コールバックを登録します。用途に応じて引数の種類・数を変更します。
        // Register callback function. Note that by different usage, we pass different kinds/numbers of parameters in.
        RegisterCallback(pIStDataStream, &OnStCallbackCFunction, (UserParam_t)pIStImageDisplayWnd);

        // パラメーター操作用のNodeMapを取得します。
        // Create NodeMap pointer for accessing parameters
        GenApi::CNodeMapPtr pNodeMapCameraParam(pIStDevice->GetRemoteIStPort()->GetINodeMap());

        // ==== Camera Setting Initial Part ====
        // TriggerModeをOffに設定し、トリガーを無効にします。
        // Switch off Trigger Mode(IEnumeration).
        GenApi::CEnumerationPtr pIEnumTrigMode(pNodeMapCameraParam->GetNode("TriggerMode"));
        *pIEnumTrigMode = "Off";
        // =====================================

        bool isCompleted = false;
        thread worker(displayChoiceWorker, &pIStDevice, &pIStDataStream, &pNodeMapCameraParam, &isCompleted);

        // スレッドの終了を待つ間、GUIイベントを処理します。
        // Process the GUI event while waiting for thread completion        
        while(!isCompleted)
        {
            if(!pIStImageDisplayWnd->IsVisible())
            {
                // 別スレッドでウィンドウを表示します。
                // Create a new thread to display the window.
                pIStImageDisplayWnd->Show(NULL, StWindowMode_Modaless);
            }
            processEventGUI();
        }        
        if (worker.joinable()) worker.join();
    }
    catch (const GenICam::GenericException &e)
    {
        // 例外の説明を表示します。
        // If any exception occurred, display the description of the error here.
        cerr << endl << "An exception occurred." << endl << e.GetDescription() << endl;
    }

    return(0);
}

//-----------------------------------------------------------------------------
// 設定値入力用の補助関数
// Helper function for handling value input
//-----------------------------------------------------------------------------
bool ValueInputChk(string * result)
{
    // 設定値の入力確認ループ
    // Value input checking loop
    char chInput;
    bool bChkLoop = true;
    while (bChkLoop)
    {
        chInput = getch();
        switch (chInput)
        {
        // ESCで終了します。
        // ESC : Force quit
        case ESC_KEY:
            return false;
            break;
        // Enterで入力を終了します。
        // Enter : End input
        case '\n': // Enter
            bChkLoop = false;
            cout << endl;
            break;
        // Backspaceで1文字削除します。
        // Backspace : Erease one character
        case BACKSPACE_KEY: // Backspace
            if (result->size() > 0)
            {
                // 表示と入力データから1文字削除します。
                cout << "\b"; cout << " "; cout << "\b";    // Clean up one character of display
                *result = result->substr(0, result->size() - 1);    // Remove one character of input data
            }
            break;
        // 数字以外の入力は無視します。
        // Only hanlde number input. Otherwise ignore.
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
        case '.':
            cout << chInput;
            *result += chInput;
            break;
        default:
            break;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Linux用getch補助関数
// Helper function for getch() in Linux
//-----------------------------------------------------------------------------
char getch(){
    char buf=0;
    struct termios old={0};
    fflush(stdout);
    if(tcgetattr(0, &old)<0)
        perror("tcsetattr()");
    old.c_lflag&=~ICANON;
    old.c_lflag&=~ECHO;
    old.c_cc[VMIN]=1;
    old.c_cc[VTIME]=0;
    if(tcsetattr(0, TCSANOW, &old)<0)
        perror("tcsetattr ICANON");
    if(read(0,&buf,1)<0)
        perror("read()");
    old.c_lflag|=ICANON;
    old.c_lflag|=ECHO;
    if(tcsetattr(0, TCSADRAIN, &old)<0)
        perror ("tcsetattr ~ICANON");
    return buf;
 }

