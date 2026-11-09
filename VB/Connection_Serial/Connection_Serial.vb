' シリアル番号でカメラを選択して取得
' Select a camera by serial number.
Imports System
Imports Sentech.GenApiDotNET
Imports Sentech.StApiDotNET

Module Connection_Serial

    Sub Main()
        Try
            Using api As New CStApiAutoInit()
            Using system As New CStSystem(eStSystemVendor.Sentech)
                ' カメラのシリアル番号を入力します。
                ' Enter the camera serial number.
                Console.Write("Camera serial number: ")
                Dim serial As String = Console.ReadLine()
                Dim targetInterface As UInteger = 0
                Dim targetID As String = Nothing

                ' 全インターフェースから指定シリアル番号のカメラを検索します。
                ' Search all interfaces for the specified camera serial number.
                For i As Integer = 0 To CInt(system.InterfaceCount) - 1
                    Dim iface As IStInterface = system.GetIStInterface(CUInt(i))
                    iface.UpdateDeviceList()
                    For j As Integer = 0 To CInt(iface.DeviceCount) - 1
                        Dim info As IStDeviceInfo = iface.GetIStDeviceInfo(CUInt(j))
                        If info.SerialNumber = serial Then
                            targetInterface = CUInt(i)
                            targetID = info.ID
                            Exit For
                        End If
                    Next
                    If targetID IsNot Nothing Then Exit For
                Next
                If targetID Is Nothing Then Throw New InvalidOperationException("Target camera not found.")

            ' 一致したデバイスIDを使用してカメラへ接続します。
            ' Connect to the camera using the matched device ID.
            Using device As CStDevice = system.GetIStInterface(targetInterface).CreateStDevice(targetID)
                Console.WriteLine("Device=" & device.GetIStDeviceInfo().DisplayName)
            Using dataStream As CStDataStream = device.CreateStDataStream(0)

                dataStream.StartAcquisition(100)
                device.AcquisitionStart()
                Try
                    While dataStream.IsGrabbing
                        Using buffer As CStStreamBuffer = dataStream.RetrieveBuffer(5000)
                            If buffer.GetIStStreamBufferInfo().IsImagePresent Then
                                Dim image As IStImage = buffer.GetIStImage()

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
        Catch e As Exception
            Console.Error.WriteLine("An exception occurred." & vbCrLf & e.Message)
        Finally
            Console.WriteLine("Press Enter to exit.")
            Console.ReadLine()
        End Try
    End Sub
End Module
