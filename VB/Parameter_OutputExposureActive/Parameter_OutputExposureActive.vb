' ExposureActive出力: センサーの露光期間を示す信号をLine1から出力します。
' Output Exposure Active: Set camera Line1 as output for sensor exposure active signal(period of exposuring of sensor).

' このサンプルの基本操作と対象機能の制御手順を示します。
' This sample demonstrates the basic operation and control procedure for the target feature.
Imports Sentech.GenApiDotNET
Imports Sentech.StApiDotNET

Module OutputExposureActive

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
								' Line1をExposureActive出力に設定します。
								' Demostration of Setting Line1 as Exposure Active

								' パラメーターへアクセスするNodeMapを取得します。
								' Create NodeMap pointer for accessing parameters
								Dim nodeMap As INodeMap = device.GetRemoteIStPort().GetINodeMap()

								' LineSelectorをLine1に設定し、出力に使うラインを選択します。
								' Set LineSelector to Line1 to select the output line.
								Dim enumLineSelector As IEnum = nodeMap.GetNode(Of IEnum)("LineSelector")
								enumLineSelector.FromString("Line1")
								Dim enumLineMode As IEnum = nodeMap.GetNode(Of IEnum)("LineMode")

								' LineModeをOutputに設定し、選択したラインを出力にします。
								' Set LineMode to Output to use the selected line as an output.
								enumLineMode.FromString("Output")

								' LineSourceをExposureActiveに設定します。
								' Set LineSource to Exposure Active
								Dim enumLineSource As IEnum = nodeMap.GetNode(Of IEnum)("LineSource")
								enumLineSource.FromString("ExposureActive")

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
