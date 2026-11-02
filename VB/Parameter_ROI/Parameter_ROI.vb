' ROI設定: オフセットを（100, 200）、幅と高さを（640, 480）に設定します。
' ROI: Set ROI to Offset (100, 200) and width/height (640, 480)

' このサンプルの基本操作と対象機能の制御手順を示します。
' This sample demonstrates the basic operation and control procedure for the target feature.
Imports Sentech.GenApiDotNET
Imports Sentech.StApiDotNET

Module ROI

	Const nCountOfImagesToGrab As Integer = 100

	Sub Main()
		Try
			Using api As CStApiAutoInit = New CStApiAutoInit()

				Using system As CStSystem = New CStSystem(eStSystemVendor.Sentech)

					Using device As CStDevice = system.CreateFirstStDevice()

						Using wnd As CStImageDisplayWnd = New CStImageDisplayWnd()

							Using dataStream As CStDataStream = device.CreateStDataStream(0)

								Console.WriteLine("Device=" & device.GetIStDeviceInfo().DisplayName)

								' ==============================================================================================================
								' ROIを640×480、オフセットを（100, 200）に設定します。
								' Demostration of setting ROI to 640 x 480 with offset (100, 200)

								' パラメーターへアクセスするNodeMapを取得します。
								' Create NodeMap pointer for accessing parameters
								Dim nodeMap As INodeMap = device.GetRemoteIStPort().GetINodeMap()

								' Widthノードを取得します。
								' Get Node for Width
								Dim intWidth As IInteger = nodeMap.GetNode(Of IInteger)("Width")

								' Widthを640 pixelに設定します。
								' Set Width to 640
								intWidth.Value = 640

								' Heightノードを取得します。
								' Get Node for Height
								Dim intHeight As IInteger = nodeMap.GetNode(Of IInteger)("Height")

								' Heightを480 pixelに設定します。
								' Set Height to 480
								intHeight.Value = 480

								' OffsetXノードを取得します。
								' Get Node for Offset X
								Dim intOffsetX As IInteger = nodeMap.GetNode(Of IInteger)("OffsetX")

								' OffsetXを100 pixelに設定します。
								' Set Offset X to 100
								intOffsetX.Value = 100

								' OffsetYノードを取得します。
								' Get Node for Offset Y
								Dim intOffsetY As IInteger = nodeMap.GetNode(Of IInteger)("OffsetY")

								' OffsetYを200 pixelに設定します。
								' Set Offset Y to 200
								intOffsetY.Value = 200

								' ==============================================================================================================

								dataStream.StartAcquisition(nCountOfImagesToGrab)

								device.AcquisitionStart()

								While dataStream.IsGrabbing

									Using streamBuffer As CStStreamBuffer = dataStream.RetrieveBuffer(5000)

										If streamBuffer.GetIStStreamBufferInfo().IsImagePresent Then
											Dim stImage As IStImage = streamBuffer.GetIStImage()
											Dim strText As String = device.GetIStDeviceInfo().DisplayName & " "
											strText &= stImage.ImageWidth & " x " & stImage.ImageHeight & " "
											strText &= String.Format("{0:F2}[fps]", dataStream.CurrentFPS)
											wnd.SetUserStatusBarText(strText)

											If Not wnd.IsVisible Then
												wnd.SetPosition(0, 0, CInt(stImage.ImageWidth), CInt(stImage.ImageHeight))
												wnd.Show(eStWindowMode.ModalessOnNewThread)
											End If

											wnd.RegisterIStImage(stImage)
										Else
											Console.WriteLine("Image data does not exist.")
										End If

									End Using

								End While

								device.AcquisitionStop()

								dataStream.StopAcquisition()

							End Using
						End Using
					End Using
				End Using
			End Using
		Catch e As Exception
			Console.Error.WriteLine("An exception occurred." & vbNewLine & e.Message)
		Finally
			Console.WriteLine(vbNewLine & "Press Enter to exit.")
			Console.ReadLine()
		End Try
	End Sub

End Module
