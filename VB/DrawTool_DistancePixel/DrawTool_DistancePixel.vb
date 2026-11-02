' このサンプルコードはSentech SDK画像表示ウィンドウの描画ツールで選択した線分の2点間距離を取得するデモです。
' This sample demonstrates how to obtain the distance between the two points of a line selected with the Sentech SDK image display window drawing tool.
'
' 表示ウィンドウ上で線分を描画してから、その線分を選択してください。
' Draw a line in the display window, then select that line to print its length in pixels.
Imports System
Imports Sentech.GenApiDotNET
Imports Sentech.StApiDotNET

Module DrawTool_DistancePixel

    ' コールバックで受信した新しい画像を表示する。
    ' Display a new image received by the callback.
    Private Sub OnDataStreamCallback(paramBase As IStCallbackParamBase, parameters As Object())
        If paramBase.CallbackType <> eStCallbackType.TL_DataStreamNewBuffer Then Return

        Dim callbackParam As IStCallbackParamGenTLEventNewBuffer = TryCast(paramBase, IStCallbackParamGenTLEventNewBuffer)
        If callbackParam Is Nothing Then Return

        Try
            Dim dataStream As IStDataStream = callbackParam.GetIStDataStream()
            Using streamBuffer As CStStreamBuffer = dataStream.RetrieveBuffer(0)
                If Not streamBuffer.GetIStStreamBufferInfo().IsImagePresent Then
                    Console.WriteLine("Image data does not exist.")
                    Return
                End If

                Dim stImage As IStImage = streamBuffer.GetIStImage()
                Dim wnd As CStImageDisplayWnd = TryCast(parameters(0), CStImageDisplayWnd)

                If Not wnd.IsVisible Then
                    wnd.SetPosition(0, 0, CInt(stImage.ImageWidth), CInt(stImage.ImageHeight))
                    wnd.Show(eStWindowMode.ModalessOnNewThread)
                End If

                ' 表示ウィンドウは画像データをコピーするため、取得バッファはここで解放できます。
                ' The display window copies the image data, so the stream buffer can be released here.
                wnd.RegisterIStImage(stImage)
            End Using
        Catch e As Exception
            Console.Error.WriteLine("An exception occurred." & vbNewLine & e.Message)
        End Try
    End Sub

    ' 選択された描画オブジェクトの2点間距離を出力する。
    ' Print the distance between the two points of the selected drawing object.
    Private Sub OnDrawingObjectSelectorChanged(node As INode, parameters As Object())
        Dim wnd As CStImageDisplayWnd = TryCast(parameters(0), CStImageDisplayWnd)
        If wnd Is Nothing OrElse Not wnd.HasImage Then Return

        Try
            Dim nodeMap As INodeMap = wnd.GetINodeMap()

            ' 描画オブジェクトの2点の座標を取得するノードを取得する。
            ' Get the nodes used to acquire the selected drawing object's two points.
            Dim pointSelector As IInteger = nodeMap.GetNode(Of IInteger)("DrawingObjectPointSelector")
            Dim pointX As IFloat = nodeMap.GetNode(Of IFloat)("DrawingObjectPointX")
            Dim pointY As IFloat = nodeMap.GetNode(Of IFloat)("DrawingObjectPointY")

            Dim image As IStImage = wnd.GetRegisteredIStImage()

            ' 座標は画像サイズに対する比率で返されるため、ピクセル単位へ変換する。
            ' Coordinates are returned as ratios of the image size, so convert them to pixels.
            pointSelector.Value = 0
            Dim x1 As Double = pointX.Value * image.ImageWidth
            Dim y1 As Double = pointY.Value * image.ImageHeight

            pointSelector.Value = 1
            Dim x2 As Double = pointX.Value * image.ImageWidth
            Dim y2 As Double = pointY.Value * image.ImageHeight

            Dim distance As Double = Math.Sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2))

            ' 描画開始時にもコールバックが発生して距離が0になるため、0以外だけを表示する。
            ' The callback is also raised when drawing starts and the distance is then 0, so ignore it.
            If distance <> 0 Then
                Console.WriteLine("Selected Item Point Distance: {0:F2} pixels", distance)
            End If
        Catch e As Exception
            Console.Error.WriteLine("An exception occurred." & vbCrLf & e.Message)
        End Try
    End Sub

    Sub Main()
        Try
            Using api As CStApiAutoInit = New CStApiAutoInit()

                Using system As CStSystem = New CStSystem(eStSystemVendor.Sentech)

                    Using device As CStDevice = system.CreateFirstStDevice()

                        Using wnd As CStImageDisplayWnd = New CStImageDisplayWnd()

                            Using dataStream As CStDataStream = device.CreateStDataStream(0)

                                Console.WriteLine("Device=" & device.GetIStDeviceInfo().DisplayName)

                                ' ==============================================================================================================
                                ' 描画ツールを有効にし、選択された描画オブジェクトを監視するデモ。
                                ' Demonstration of enabling the drawing tool and monitoring the selected drawing object.

                                ' 表示ウィンドウの組み込み描画ツールバーを有効にする。
                                ' Enable the display window's built-in drawing toolbar.
                                wnd.BuiltInDrawingToolbar = True

                                ' DisplayWindowのDrawingObjectSelectorノードを取得し、変更コールバックを登録する。
                                ' Get the display window's DrawingObjectSelector node and register a change callback.
                                Dim nodeMap As INodeMap = wnd.GetINodeMap()
                                Dim drawingObjectSelector As IInteger = nodeMap.GetNode(Of IInteger)("DrawingObjectSelector")
                                Dim drawCallbackParam As Object() = {wnd}
                                Dim drawingCallbackHandle As IntPtr = drawingObjectSelector.RegisterCallbackMethod(
                                    AddressOf OnDrawingObjectSelectorChanged, drawCallbackParam, eCallbackType.PostOutsideLock)

                                ' 画像表示用のDataStreamコールバックを登録する。
                                ' Register a DataStream callback for displaying images.
                                Dim streamCallbackParam As Object() = {wnd}
                                dataStream.RegisterCallbackMethod(AddressOf OnDataStreamCallback, streamCallbackParam)

                                ' ==============================================================================================================

                                dataStream.StartAcquisition()

                                device.AcquisitionStart()

                                Console.WriteLine("Draw a line in the display window, then select the line to show its length in pixels.")
                                Console.WriteLine("Press Enter to exit.")
                                Console.ReadLine()

                                device.AcquisitionStop()

                                dataStream.StopAcquisition()

                                drawingObjectSelector.DeregisterCallbackMethod(drawingCallbackHandle)

                            End Using
                        End Using
                    End Using
                End Using
            End Using
        Catch e As Exception
            Console.Error.WriteLine("An exception occurred." & vbCrLf & e.Message)
        Finally
            Console.WriteLine("Press Enter to exit.")
            Console.ReadLine()
        End Try
    End Sub
End Module
