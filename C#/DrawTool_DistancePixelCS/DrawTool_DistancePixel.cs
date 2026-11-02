/*
    Draw Tool Selected Line Distance Calculation in Pixel.
    このサンプルコードはSentech SDK画像表示ウィンドウの描画ツールで選択した線分の2点間距離を取得するデモです。

    表示ウィンドウ上で線分を描画してから、その線分を選択してください。
    Draw a line in the display window, then select that line to print its length in pixels.
*/

#define ENABLED_ST_GUI

using System;
using Sentech.GenApiDotNET;
using Sentech.StApiDotNET;

namespace DrawTool_DistancePixel
{
    class DrawTool_DistancePixel
    {
        // --------------------------------------------------------------------------------------------------------------
        // コールバックで受信した新しい画像を表示する。
        // Display a new image received by the callback.
        // --------------------------------------------------------------------------------------------------------------
        static void OnDataStreamCallback(IStCallbackParamBase paramBase, object[] param)
        {
            if (paramBase.CallbackType != eStCallbackType.TL_DataStreamNewBuffer)
            {
                return;
            }

            IStCallbackParamGenTLEventNewBuffer callbackParam = paramBase as IStCallbackParamGenTLEventNewBuffer;
            if (callbackParam == null)
            {
                return;
            }

            try
            {
                IStDataStream dataStream = callbackParam.GetIStDataStream();
                using (CStStreamBuffer streamBuffer = dataStream.RetrieveBuffer(0))
                {
                    if (!streamBuffer.GetIStStreamBufferInfo().IsImagePresent)
                    {
                        Console.WriteLine("Image data does not exist.");
                        return;
                    }

                    IStImage stImage = streamBuffer.GetIStImage();
                    CStImageDisplayWnd wnd = param[0] as CStImageDisplayWnd;

                    if (!wnd.IsVisible)
                    {
                        wnd.SetPosition(0, 0, (int)stImage.ImageWidth, (int)stImage.ImageHeight);
                        wnd.Show(eStWindowMode.ModalessOnNewThread);
                    }

                    // 表示ウィンドウは画像データをコピーするため、取得バッファはここで解放できます。
                    // The display window copies the image data, so the stream buffer can be released here.
                    wnd.RegisterIStImage(stImage);
                }
            }
            catch (Exception e)
            {
                Console.Error.WriteLine("An exception occurred. \r\n" + e.Message);
            }
        }

        // --------------------------------------------------------------------------------------------------------------
        // 選択された描画オブジェクトの2点間距離を出力する。
        // Print the distance between the two points of the selected drawing object.
        // --------------------------------------------------------------------------------------------------------------
        static void OnDrawingObjectSelectorChanged(INode node, object[] param)
        {
            CStImageDisplayWnd wnd = param[0] as CStImageDisplayWnd;
            if (wnd == null || !wnd.HasImage)
            {
                return;
            }

            try
            {
                INodeMap nodeMap = wnd.GetINodeMap();

                // 描画オブジェクトの2点の座標を取得するノードを取得する。
                // Get the nodes used to acquire the selected drawing object's two points.
                IInteger pointSelector = nodeMap.GetNode<IInteger>("DrawingObjectPointSelector");
                IFloat pointX = nodeMap.GetNode<IFloat>("DrawingObjectPointX");
                IFloat pointY = nodeMap.GetNode<IFloat>("DrawingObjectPointY");

                IStImage image = wnd.GetRegisteredIStImage();

                // 座標は画像サイズに対する比率で返されるため、ピクセル単位へ変換する。
                // Coordinates are returned as ratios of the image size, so convert them to pixels.
                pointSelector.Value = 0;
                double x1 = pointX.Value * image.ImageWidth;
                double y1 = pointY.Value * image.ImageHeight;

                pointSelector.Value = 1;
                double x2 = pointX.Value * image.ImageWidth;
                double y2 = pointY.Value * image.ImageHeight;

                double distance = Math.Sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));

                // 描画開始時にもコールバックが発生して距離が0になるため、0以外だけを表示する。
                // The callback is also raised when drawing starts and the distance is then 0, so ignore it.
                if (distance != 0)
                {
                    Console.WriteLine("Selected Item Point Distance: {0:F2} pixels", distance);
                }
            }
            catch (Exception e)
            {
                Console.Error.WriteLine("An exception occurred. \r\n" + e.Message);
            }
        }

        static void Main(string[] args)
        {
            try
            {
                using (CStApiAutoInit api = new CStApiAutoInit())

                using (CStSystem system = new CStSystem(eStSystemVendor.Sentech))

                using (CStDevice device = system.CreateFirstStDevice())

                using (CStImageDisplayWnd wnd = new CStImageDisplayWnd())

                using (CStDataStream dataStream = device.CreateStDataStream(0))
                {
                    Console.WriteLine("Device=" + device.GetIStDeviceInfo().DisplayName);

                    // ==============================================================================================================
                    // 描画ツールを有効にし、選択された描画オブジェクトを監視するデモ
                    // Demonstration of enabling the drawing tool and monitoring the selected drawing object.

                    // 表示ウィンドウの組み込み描画ツールバーを有効にする。
                    // Enable the display window's built-in drawing toolbar.
                    wnd.BuiltInDrawingToolbar = true;

                    // DisplayWindowのDrawingObjectSelectorノードを取得し、変更コールバックを登録する。
                    // Get the display window's DrawingObjectSelector node and register a change callback.
                    INodeMap nodeMap = wnd.GetINodeMap();
                    IInteger drawingObjectSelector = nodeMap.GetNode<IInteger>("DrawingObjectSelector");
                    object[] drawCallbackParam = { wnd };
                    IntPtr drawingCallbackHandle = drawingObjectSelector.RegisterCallbackMethod(
                        OnDrawingObjectSelectorChanged,
                        drawCallbackParam,
                        eCallbackType.PostOutsideLock);

                    // 画像表示用のDataStreamコールバックを登録する。
                    // Register a DataStream callback for displaying images.
                    object[] streamCallbackParam = { wnd };
                    dataStream.RegisterCallbackMethod(OnDataStreamCallback, streamCallbackParam);

                    // ==============================================================================================================

                    dataStream.StartAcquisition();

                    device.AcquisitionStart();

                    Console.WriteLine("Draw a line in the display window, then select the line to show its length in pixels.");
                    Console.WriteLine("Press Enter to exit.");
                    Console.ReadLine();

                    device.AcquisitionStop();

                    dataStream.StopAcquisition();

                    drawingObjectSelector.DeregisterCallbackMethod(drawingCallbackHandle);
                }
            }
            catch (Exception e)
            {
                Console.Error.WriteLine("An exception occurred. \r\n" + e.Message);
            }
            finally
            {
                Console.WriteLine("\r\nPress Enter to exit.");
                Console.ReadLine();
            }
        }
    }
}
