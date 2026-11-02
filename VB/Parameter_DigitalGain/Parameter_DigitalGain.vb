' デジタルゲイン設定: デジタルゲインを2倍に設定します。
' Digital Gain Setting: Set digital gain to 2 times.

' このサンプルの基本操作と対象機能の制御手順を示します。
' This sample demonstrates the basic operation and control procedure for the target feature.
Imports Sentech.GenApiDotNET
Imports Sentech.StApiDotNET

Module DigitalGain

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
								' デジタルゲインを2倍に設定します。
								' Demostration of set digital Gain to 2 times

								' パラメーターへアクセスするNodeMapを取得します。
								' Create NodeMap pointer for accessing parameters
								Dim nodeMap As INodeMap = device.GetRemoteIStPort().GetINodeMap()

								' ゲインを設定できるよう、GainAutoをOffに設定します。
								' Switch off gain auto. If gain auto is on, gain value cannot be set.
								Dim enumGainAuto As IEnum = nodeMap.GetNode(Of IEnum)("GainAuto")
								enumGainAuto.FromString("Off")

								' デジタルゲインへアクセスするため、GainSelectorをDigitalAllに設定します。
								' For setting digital gain, gain selector need to be set to DigitalAll to access digital gain.
								Dim enumGainSelector As IEnum = nodeMap.GetNode(Of IEnum)("GainSelector")
								enumGainSelector.FromString("DigitalAll")

								' Gainノードを取得します。
								' Get Node for Gain
								Dim floatGain As IFloat = nodeMap.GetNode(Of IFloat)("Gain")

								' デジタルゲインを2倍（128）に設定します。
								' Set digital gain to 2 times(128).
								floatGain.Value = 128

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
