' 露光時間設定: カメラの露光時間を100,000 µsに設定します。
' Exposure Time Setting: Set camera exposure time to 100,000 us.

' このサンプルの基本操作と対象機能の制御手順を示します。
' This sample demonstrates the basic operation and control procedure for the target feature.
Imports Sentech.GenApiDotNET
Imports Sentech.StApiDotNET

Module ExposureTime

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
								' カメラの露光時間（µs）を変更します。
								' Demostration of changing exposure time(us) of camera

								' パラメーターへアクセスするNodeMapを取得します。
								' Create NodeMap pointer for accessing parameters
								Dim nodeMap As INodeMap = device.GetRemoteIStPort().GetINodeMap()

								' 露光時間を設定できるよう、ExposureAutoをOffに設定します。
								' Switch off exposure auto. If exposure auto Is on, exposure time cannot be set.
								Dim enumExposureAuto As IEnum = nodeMap.GetNode(Of IEnum)("ExposureAuto")
								enumExposureAuto.FromString("Off")

								' 露光時間を入力できるよう、ExposureModeをTimedに設定します。
								' For setting camera exposure time, exposure mode must set to Timed to enable value input
								Dim enumExpoMode As IEnum = nodeMap.GetNode(Of IEnum)("ExposureMode")
								enumExpoMode.FromString("Timed")

								' ExposureTimeノードを取得します。
								' Get Node for ExposureTime
								Dim floatExpoTime As IFloat = nodeMap.GetNode(Of IFloat)("ExposureTime")

								' 露光時間を100,000 µsに設定します。
								' Set Exposure time to 100,000 usec
								floatExpoTime.Value = 100000

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
