' 使用中のストリームドライバー種別を表示します。
' Display the current stream driver type.
Imports System
Imports Sentech.GenApiDotNET
Imports Sentech.StApiDotNET

Module DataStream_CheckFilterDriver

    Sub Main()
        Try
            Using api As New CStApiAutoInit()
            Using system As New CStSystem(eStSystemVendor.Sentech)

            Using device As CStDevice = system.CreateFirstStDevice()
                Console.WriteLine("Device=" & device.GetIStDeviceInfo().DisplayName)
            Using dataStream As CStDataStream = device.CreateStDataStream(0)

                ' ==============================================================================================================
                ' StreamDriverTypeを取得して表示します。
                ' Get and display StreamDriverType.
                ' DataStreamのNodeMapを取得します。
                ' Get the DataStream NodeMap.
                Dim nodeMap As INodeMap = dataStream.GetIStPort().GetINodeMap()

                ' StreamDriverTypeノードを取得します。
                ' Get the StreamDriverType node.
                Dim driver As IEnum = nodeMap.GetNode(Of IEnum)("StreamDriverType")

                ' 使用中のストリームドライバー種別を表示します。
                ' Display the current stream driver type.
                Console.WriteLine("StreamDriverType=" & driver.CurrentEntryNode.Symbolic)
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
