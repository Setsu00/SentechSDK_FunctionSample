' PixelFormatConverterで取得した偏光画像をADOLP画像へ変換して表示します。
' Convert acquired polarization images to ADOLP images with PixelFormatConverter and display them.
Imports System
Imports Sentech.GenApiDotNET
Imports Sentech.StApiDotNET

Module Filter_PolarizationADOLP

    Sub Main()
        Try
            Using api As New CStApiAutoInit()
                Using system As New CStSystem(eStSystemVendor.Sentech)

                    Using device As CStDevice = system.CreateFirstStDevice()
                        Console.WriteLine("Device=" & device.GetIStDeviceInfo().DisplayName)
                        Using dataStream As CStDataStream = device.CreateStDataStream(0)

                            ' ==============================================================================================================
                            ' PixelFormatConverterでADOLP変換を設定します。
                            ' Configure ADOLP conversion with PixelFormatConverter.

                            ' ADOLP画像を表示するウィンドウを生成します。
                            ' Create a window to display ADOLP images.
                            Using wnd As New CStImageDisplayWnd()
                                ' 変換器と変換後画像用のバッファーを生成します。
                                ' Create the converter and an image buffer for the converted image.
                                Using converter As New CStPixelFormatConverter()
                                    Using converted As CStImageBuffer = CStApiDotNet.CreateStImageBuffer()
                                        ' 変換後のPixelFormatをBGR8に設定します。
                                        ' Set the destination PixelFormat to BGR8.
                                        converter.DestinationPixelFormat = eStPixelFormatNamingConvention.BGR8

                                        ' 変換器のNodeMapを取得します。
                                        ' Acquire the converter's NodeMap.
                                        Dim nodeMapConverter As INodeMap = converter.GetINodeMap()

                                        ' 偏光画像変換方式のノードを取得します。
                                        ' Get the polarization image transformation method node.
                                        Dim enumPolarizationMethod As IEnum = nodeMapConverter.GetNode(Of IEnum)("PolarizationImageTransformationMethod")

                                        ' 直線偏光の角度と偏光度（ADOLP）への変換を設定します。
                                        ' Set conversion to angle and degree of linear polarization (ADOLP).
                                        enumPolarizationMethod.FromString("AngleAndDegreeOfLinearPolarization")
                                        ' ==============================================================================================================
                                        dataStream.StartAcquisition(100)
                                        device.AcquisitionStart()
                                        Try
                                            While dataStream.IsGrabbing
                                                Using buffer As CStStreamBuffer = dataStream.RetrieveBuffer(5000)
                                                    If buffer.GetIStStreamBufferInfo().IsImagePresent Then
                                                        Dim image As IStImage = buffer.GetIStImage()

                                                        ' 表示前に、取得した偏光画像をADOLP画像へ変換します。
                                                        ' Convert the acquired polarization image to an ADOLP image before display.
                                                        converter.Convert(image, converted)
                                                        image = converted.GetIStImage()
                                                        Console.WriteLine("BlockID=" & buffer.GetIStStreamBufferInfo().FrameID &
                                                            " Size=" & image.ImageWidth & " x " & image.ImageHeight)

                                                        ' 変換後の画像を表示します。
                                                        ' Display the converted image.
                                                        If Not wnd.IsVisible Then
                                                            wnd.SetPosition(0, 0, CInt(image.ImageWidth), CInt(image.ImageHeight))
                                                            wnd.Show(eStWindowMode.ModalessOnNewThread)
                                                        End If
                                                        wnd.RegisterIStImage(image)

                                                    Else
                                                        Console.WriteLine("Image data does not exist.")
                                                    End If
                                                End Using
                                            End While

                                        Finally
                                            device.AcquisitionStop()
                                            dataStream.StopAcquisition()
                                        End Try
                                    End Using
                                End Using
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
