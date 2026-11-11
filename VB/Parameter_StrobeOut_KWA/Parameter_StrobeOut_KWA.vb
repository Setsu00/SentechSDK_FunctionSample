' Strobe Out: 露光開始を契機に、Line2から1,000 µsのストローブ信号を出力する。
' Strobe Out: Output a 1,000 µs strobe signal on Line2 when exposure starts.
' 対象: STC-MB/MCSシリーズのみ。
' Applies to the STC-MB/MCS series only.

Imports Sentech.GenApiDotNET
Imports Sentech.StApiDotNET

Module StrobeOut_KWA

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
								' Line2をストローブ出力として設定する。
								' Configure Line2 as the strobe output.

								' カメラのパラメーターへアクセスするNodeMapを取得する。
								' Get the NodeMap used to access camera parameters.
								Dim nodeMap As INodeMap = device.GetRemoteIStPort().GetINodeMap()

								' LineSelectorでLine2を選択し、LineModeをOutputに設定する。
								' Select Line2 with LineSelector and set LineMode to Output.
								Dim enumLineSelector As IEnum = nodeMap.GetNode(Of IEnum)("LineSelector")
								enumLineSelector.FromString("Line2")
								Dim enumLineMode As IEnum = nodeMap.GetNode(Of IEnum)("LineMode")

								' LineModeをOutputに設定し、選択したラインを出力にします。
								' Set LineMode to Output to use the selected line as an output.
								enumLineMode.FromString("Output")

								' Line2のLineSourceをTimer0Activeに設定する。
								' Set LineSource for Line2 to Timer0Active.
								Dim enumLineSource As IEnum = nodeMap.GetNode(Of IEnum)("LineSource")
								enumLineSource.FromString("Timer0Active")

								' TimerSelectorでTimer0を選択する。
								' Select Timer0 with TimerSelector.
								Dim enumTimerSelector As IEnum = nodeMap.GetNode(Of IEnum)("TimerSelector")
								enumTimerSelector.FromString("Timer0")

								' Timer0のTimerTriggerSourceをExposureStartに設定する。
								' Set TimerTriggerSource for Timer0 to ExposureStart.
								Dim enumTimerTriggerSource As IEnum = nodeMap.GetNode(Of IEnum)("TimerTriggerSource")
								enumTimerTriggerSource.FromString("ExposureStart")

								' TimerDurationでTimer0の出力時間を1,000 µsに設定する。
								' Set the Timer0 output duration to 1,000 µs with TimerDuration.
								Dim floatTimerDuration As IFloat = nodeMap.GetNode(Of IFloat)("TimerDuration")
								floatTimerDuration.Value = 1000

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
