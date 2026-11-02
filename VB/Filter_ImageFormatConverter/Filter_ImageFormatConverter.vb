' 取得画像をMono8へ変換します。
' Convert acquired images to Mono8.
Imports System
Imports Sentech.GenApiDotNET
Imports Sentech.StApiDotNET

Module Filter_ImageFormatConverter

    Sub Main()
        Try
            Using api As New CStApiAutoInit()
                Using system As New CStSystem(eStSystemVendor.Sentech)

                    Using device As CStDevice = system.CreateFirstStDevice()
                        Console.WriteLine("Device=" & device.GetIStDeviceInfo().DisplayName)
                        Using dataStream As CStDataStream = device.CreateStDataStream(0)

                            ' ==============================================================================================================
                            ' PixelFormatConverterを生成し、変換後の形式をMono8に設定します。
                            ' Create the PixelFormatConverter and set the destination format to Mono8.

                            ' 変換器と変換後画像用のバッファーを生成します。
                            ' Create the converter and an image buffer for the converted image.
                            Using converter As New CStPixelFormatConverter()
                                Using converted As CStImageBuffer = CStApiDotNet.CreateStImageBuffer()
                                    ' 変換後のPixelFormatをMono8に設定します。
                                    ' Set the destination PixelFormat to Mono8.
                                    converter.DestinationPixelFormat = eStPixelFormatNamingConvention.Mono8
                                    ' ==============================================================================================================
                                    dataStream.StartAcquisition(100)
                                    device.AcquisitionStart()
                                    Try
                                        While dataStream.IsGrabbing
                                            Using buffer As CStStreamBuffer = dataStream.RetrieveBuffer(5000)
                                                If buffer.GetIStStreamBufferInfo().IsImagePresent Then
                                                    Dim image As IStImage = buffer.GetIStImage()

                                                    ' 取得画像をMono8へ変換します。
                                                    ' Convert the acquired image to Mono8.
                                                    converter.Convert(image, converted)
                                                    image = converted.GetIStImage()
                                                    Console.WriteLine("BlockID=" & buffer.GetIStStreamBufferInfo().FrameID &
                                                        " Size=" & image.ImageWidth & " x " & image.ImageHeight)

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
        Catch e As Exception
            Console.Error.WriteLine("An exception occurred." & vbCrLf & e.Message)
        Finally
            Console.WriteLine("Press Enter to exit.")
            Console.ReadLine()
        End Try
    End Sub
End Module
