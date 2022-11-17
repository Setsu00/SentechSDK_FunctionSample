/*
	DrawToolで選択したピクセル間の距離を計算
	このサンプルコードはSentechSDKイメージディスプレイウィンドウの描画ツールで選択した2点間の距離を取得するデモである。
	Draw Tool Selected Line Distance Calculation in Pixel.
	This sample code demostrate how to acquire the distances between 2 points as a Line that created by draw tool of Sentech SDK Image display window.
*/

#define ENABLED_ST_GUI

#include <StApi_TL.h>
#ifdef ENABLED_ST_GUI
#include <StApi_GUI.h>
#include <iomanip>	//std::setprecision
#endif

using namespace StApi;
using namespace std;
const uint64_t nCountOfImagesToGrab = INFINITE;

typedef IStImageDisplayWnd* UserParam_t;

// 描画コールバックイベントの型。複数のノードを処理する必要がある場合の構造体として定義
// Drawing callback event type. Define as a structure for if more than one node need to be handled.
typedef enum _EDrawingEventType_t
{
	// DrawingEventType_ItemType,	// Sample of other type declearation.(他の型宣言のサンプル)
	DrawingEventType_ObjectSelector
}EDrawingEventType_t;

// Drawing Object SelectorノードのCIntegerPtrを宣言
// Declear Integer Pointer of INode for Drawing Object Selector
GenApi::CIntegerPtr		m_pIInteger_DrawingObjectSelector;

// 表示のためのIStImageDisplyWindowの宣言
// Declear IStImageDisplyWindow for display
CIStImageDisplayWndPtr	m_pIStImageDisplayWnd;

//-----------------------------------------------------------------------------
// コールバック処理機能
// Function for handling callback action
//-----------------------------------------------------------------------------
void OnCallback(IStCallbackParamBase* pIStCallbackParamBase, UserParam_t pvContext)
{
	// コールバックタイプを確認。NewBufferイベントだけがここで処理される。
	// Check callback type. Only NewBuffer event is handled in here
	if (pIStCallbackParamBase->GetCallbackType() == StCallbackType_GenTLEvent_DataStreamNewBuffer)
	{
		// NewBufferイベントを受信した場合
		// 追加情報を取得するために、受け取ったコールバックパラメータをIStCallbackParamGenTLEventNewBufferに変換します。
		// In case of receiving a NewBuffer events:
		// Convert received callback parameter into IStCallbackParamGenTLEventNewBuffer for acquiring additional information.
		IStCallbackParamGenTLEventNewBuffer* pIStCallbackParamGenTLEventNewBuffer = dynamic_cast<IStCallbackParamGenTLEventNewBuffer*>(pIStCallbackParamBase);

		try
		{
			// 受け取ったコールバックパラメータからIStDataStreamポインターを取得
			// Get the IStDataStream interface pointer from the received callback parameter.
			IStDataStream* pIStDataStream = pIStCallbackParamGenTLEventNewBuffer->GetIStDataStream();

			// 受信したバッファがあることを示すコールバックの画像データのバッファポインタを取得
			// Retrieve the buffer pointer of image data for that callback indicated there is a buffer received.
			CIStStreamBufferPtr pIStStreamBuffer(pIStDataStream->RetrieveBuffer(0));

			// 取得したデータに画像データが含まれていないか確認
			// Check if the acquired data contains image data.
			if (pIStStreamBuffer->GetIStStreamBufferInfo()->IsImagePresent())
			{
				// ある場合は、さらにイメージを処理するためにIStImageオブジェクトを作成
				// If yes, we create a IStImage object for further image handling.
				IStImage* pIStImage = pIStStreamBuffer->GetIStImage();

				// ディスプレイウィンドウが表示されているか確認
				// Check if display window is visible.
				if (!pvContext->IsVisible())
				{
					// ウィンドウの位置とサイズを設定
					// Set the position and size of the window.
					pvContext->SetPosition(0, 0, pIStImage->GetImageWidth(), pIStImage->GetImageHeight());

					// ウィンドウ表示の新スレッドを生成
					// Create a new thread to display the window.
					pvContext->Show(NULL, StWindowMode_ModalessOnNewThread);
				}

				// 表示する画像を登録
				// これにはイメージデータのコピーが含まれ、必要に応じて元のバッファを解放できます。
				// Register the image to be displayed.
				// This will have a copy of the image data and original buffer can be released if necessary.
				pvContext->RegisterIStImage(pIStImage);

			}
			else
			{
				// 取得したデータに画像データがない場合
				// If the acquired data contains no image data...
				cout << "Image data does not exist." << endl;
			}
		}
		catch (const GenICam::GenericException& e)
		{
			// 例外が発生した場合、ここにエラー詳細を表示
			// If any exception occurred, display the description of the error here.
			cerr << endl << "An exception occurred." << endl << e.GetDescription() << endl;
		}
	}
}

//-----------------------------------------------------------------------------
// 登録のためのコールバック機能
// Callback function for registering
//-----------------------------------------------------------------------------
void __stdcall OnStCallbackCFunction(IStCallbackParamBase* pIStCallbackParamBase, UserParam_t pvContext)
{
	OnCallback(pIStCallbackParamBase, pvContext);
}


// ピクセル間距離計算
// Distance Pixel Calculation function
double CalculateDistancePixel(double aX, double aY, double bX, double bY)
{
	double difX = abs(aX - bX);
	double difY = abs(aY - bY);
	return sqrt( pow(difX,2) + pow(difY, 2));
}

//-----------------------------------------------------------------------------
// IStDisplayWindowのノードアクションを処理する
// Drawing Callback for registering to handle Node actions of IStDisplayWindow.
//-----------------------------------------------------------------------------
void OnNodeCallbackDrawingEvent(GenApi::INode* pINode, EDrawingEventType_t eType)
{
	// コールバックのタイプを確認。これは、複数の種類のアクションを処理しようとする場合に便利です。
	// Make sure of the type of callback is called. This is useful when trying to handle more than one kind of action.
	if (eType == DrawingEventType_ObjectSelector)
	{
		// 2点間を選択するためのDrawingObjectPointSelectorを取得
		// Acquire DrawingObjectPointSelector for select between 2 points
		GenApi::CIntegerPtr pIInteger_Point = m_pIStImageDisplayWnd->GetINodeMap()->GetNode("DrawingObjectPointSelector");
		// ポイントのX座標を取得するためのノード
		// Node for acquiring X value of point.
		GenApi::CFloatPtr pIFloat_X = m_pIStImageDisplayWnd->GetINodeMap()->GetNode("DrawingObjectPointX");
		// ポイントのY座標を取得するためのノード
		// Node for acquiring Y value of point.
		GenApi::CFloatPtr pIFloat_Y = m_pIStImageDisplayWnd->GetINodeMap()->GetNode("DrawingObjectPointY");

		if (GenApi::IsWritable(pIInteger_Point))
		{
			// 後の計算のため、現イメージサイズを取得
			// Acquire current image Size for later calculation
			StApi::IStImage* pIStImage = m_pIStImageDisplayWnd->GetRegisteredIStImage();

			// ポイント1の座標を取得し、その単位を比率からピクセルに変換する
			// Acquire Point 1 Coordinate and conver its unit from proportion to pixel
			pIInteger_Point->SetValue(0);
			const double dblX1 = pIFloat_X->GetValue() * pIStImage->GetImageWidth();
			const double dblY1 = pIFloat_Y->GetValue() * pIStImage->GetImageHeight();

			// ポイント2の座標を取得し、その単位を比率からピクセルに変換する
			// Acquire Point 2 Coordinate and conver its unit from proportion to pixel
			pIInteger_Point->SetValue(1);
			const double dblX2 = pIFloat_X->GetValue() * pIStImage->GetImageWidth();
			const double dblY2 = pIFloat_Y->GetValue() * pIStImage->GetImageHeight();

			// 距離を計算
			// Calculate distance
			const double dDistance = CalculateDistancePixel(dblX1, dblY1, dblX2, dblY2);

			// コンソールに結果を表示
			// 描画を開始すると、このコールバックもまた呼び出され、結果は距離0になることに注意してください。
			// ここでは0の結果を無視し、0でない結果を出力します。
			// Print out result on console
			// Note that when start drawing, this callback will also be triggered and will have a distance 0 result.
			// Here we ignore the result of 0 and output the result which is not 0.
			if (dDistance != 0)
			{
				cout << "   Selected Item Point Distance: " << CalculateDistancePixel(dblX1, dblY1, dblX2, dblY2) << endl;
			}
		}
	}
}

//-----------------------------------------------------------------------------
// メイン
// Main function
//-----------------------------------------------------------------------------
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
		// 2点間の計算距離のノード変化をキャッチするコールバックの登録方法をデモします。
		// ここでは、SDKサンプルGrabCallbackから参照した別のコールバックを表示に使用します。
		// Demostrate registering callback to catch Node change for calculation distance between 2 points.
		// Here we use another callback for display, which referenced from SDK sample GrabCallback.

		// 表示ウィンドウを生成
		// Create display window
		m_pIStImageDisplayWnd.Reset(StApi::CreateIStWnd(StApi::StWindowType_ImageDisplay));

		// DisplayWindowのドローツールを有効に
		// Enable Draw tool of DisplayWindow
		m_pIStImageDisplayWnd->SetBuiltInDrawingToolbar(true);

		// 後で使用するために、DisplayWindowのDrawingObjectSelectorノードを取得する
		// Get DrawingObjectSelector Node of DisplayWindow for later use
		m_pIInteger_DrawingObjectSelector = m_pIStImageDisplayWnd->GetINodeMap()->GetNode("DrawingObjectSelector");

		// DrawingObjectSelectorが変更されるたびに呼び出されるコールバックを登録する
		// DrawingObjectSelectorが変更された場合、コールバック関数に"DrawingEventType_ObjectSelector"を送信してコールバックを呼び出します。
		// Register callback to be called whenever DrawingObjectSelector is changed
		// For DrawingObjectSelector changed, trigger callback with sending "DrawingEventType_ObjectSelector" to callback function.
		StApi::RegisterCallback(m_pIInteger_DrawingObjectSelector->GetNode(), OnNodeCallbackDrawingEvent, DrawingEventType_ObjectSelector, GenApi::cbPostInsideLock);

		// 表示のためにもう一つコールバック登録
		// Register another callback for display.
		StApi::RegisterCallback(pIStDataStream, &OnStCallbackCFunction, (UserParam_t)m_pIStImageDisplayWnd);

		// ==============================================================================================================
		
		pIStDataStream->StartAcquisition(nCountOfImagesToGrab);
		pIStDevice->AcquisitionStart();
		
		// Enterが押されるまで画像を取得し続ける
		// Keep getting image until Enter key pressed.
		cout << "Press Enter to exit." << endl;
		cout << "Draw then click a Line the acquire the distance between two point in pixel unit." << endl;
		for (;;)
		{
			if (cin.get() == '\n') break;
			Sleep(500);
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
