' Rotate90cwFilter: フィルターを使用して画像を時計回りに90度回転します。
' Rotate90cwFilter: using Filter to rotate image clock wise 90 degree.

' このサンプルの基本操作と対象機能の制御手順を示します。
' This sample demonstrates the basic operation and control procedure for the target feature.
Imports Sentech.StApiDotNET

Module Rotate90cwFilter

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
								' フィルターを使用して画像を時計回りに90度回転します。
								' Demostration of using Filter to rotate image clock wise 90 degree

								' ReverseConverterフィルターを生成します。
								' Create an ReverseConverter filter object.
								Dim filter As CStReverseConverter = New CStReverseConverter()

								' ReverseConverterを時計回り90度回転に設定します。
								' Set ReverseConverter reverse to clock wise 90 degree.
								filter.RotationMode = eStRotationMode.Clockwise90

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

											' ==============================================================================================================
											' 回転後の画像を格納するバッファーを生成します。
											' Create another buffer for storing rotated image
											Dim imageBuffer As CStImageBuffer = CStApiDotNet.CreateStImageBuffer()

											' 元画像を回転し、別のバッファーへ出力します。
											' Rotate original image And output to another buffer
											filter.Convert(stImage, imageBuffer)

											' 回転後の画像を表示します。
											' Display rotated image
											wnd.RegisterIStImage(imageBuffer.GetIStImage())
											' ==============================================================================================================
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
