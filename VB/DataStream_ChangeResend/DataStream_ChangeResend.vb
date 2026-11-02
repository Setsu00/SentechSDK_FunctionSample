' 再送要求の上限を0（単位なし、無制限）に設定します。
' Set the resend-request limit to 0 (unitless, unlimited).
Imports System
Imports Sentech.GenApiDotNET
Imports Sentech.StApiDotNET

Module DataStream_ChangeResend

    Sub Main()
        Try
            Using api As New CStApiAutoInit()
            Using system As New CStSystem(eStSystemVendor.Sentech)

            Using device As CStDevice = system.CreateFirstStDevice()
                Console.WriteLine("Device=" & device.GetIStDeviceInfo().DisplayName)
            Using dataStream As CStDataStream = device.CreateStDataStream(0)

                ' ==============================================================================================================
                ' MaximumResendRequestsPerBlockを0（単位なし、無制限）に設定します。
                ' Set MaximumResendRequestsPerBlock to 0 (unitless, unlimited).
                ' DataStreamのNodeMapを取得します。
                ' Get the DataStream NodeMap.
                Dim nodeMap As INodeMap = dataStream.GetIStPort().GetINodeMap()

                ' MaximumResendRequestsPerBlockノードを取得します。
                ' Get the MaximumResendRequestsPerBlock node.
                Dim resend As IInteger = nodeMap.GetNode(Of IInteger)("MaximumResendRequestsPerBlock")

                ' 再送要求の上限を0（単位なし、無制限）に設定します。
                ' Set the resend-request limit to 0 (unitless, unlimited).
                resend.Value = 0
                ' ==============================================================================================================
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
