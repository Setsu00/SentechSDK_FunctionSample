' 手動ホワイトバランス: 各色のホワイトバランス比率を10に設定します。
' Manual White Balance: Manually set white balance ratio of all color to 10.

' このサンプルの基本操作と対象機能の制御手順を示します。
' This sample demonstrates the basic operation and control procedure for the target feature.
Imports Sentech.GenApiDotNET
Imports Sentech.StApiDotNET

Module ManualWhiteBalance_KWA

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
								' 各色のホワイトバランス比率を手動で10に設定します。
								' Demostration of manually set white balance ratio of all color to 10

								' パラメーターへアクセスするNodeMapを取得します。
								' Create NodeMap pointer for accessing parameters
								Dim nodeMap As INodeMap = device.GetRemoteIStPort().GetINodeMap()

								' 手動で値を設定するため、BalanceWhiteAutoをPreset0に設定します。
								' Switch BalanceWhiteAuto to Preset0 for manual input value
								Dim enumBalanceWhiteAuto As IEnum = nodeMap.GetNode(Of IEnum)("BalanceWhiteAuto")
								enumBalanceWhiteAuto.FromString("Preset0")

								' 赤の比率へアクセスするため、BalanceRatioSelectorをRedに設定します。
								' Switch balance ration selector to Red for access Red ratio value
								Dim enumBalanceRatioSelector As IEnum = nodeMap.GetNode(Of IEnum)("BalanceRatioSelector")
								enumBalanceRatioSelector.FromString("Red")

								' BalanceRatioノードを取得します。
								' Get Node for BalanceRatio
								Dim floatBalanceRatio As IFloat = nodeMap.GetNode(Of IFloat)("BalanceRatio")

								' BalanceRatioを10に設定します。
								' Set BalanceRatio to 10
								floatBalanceRatio.Value = 10

								' 青の比率へアクセスするため、BalanceRatioSelectorをBlueに設定します。
								' Switch balance ration selector to Blue for access Blue ratio value
								enumBalanceRatioSelector.FromString("Blue")

								' BalanceRatioを10に設定します。
								' Set BalanceRatio to 10
								floatBalanceRatio.Value = 10

								' 緑の比率へアクセスするため、BalanceRatioSelectorをGreenに設定します。
								' Switch balance ration selector to Green for access Green ratio value
								enumBalanceRatioSelector.FromString("Green")

								' BalanceRatioを10に設定します。
								' Set BalanceRatio to 10
								floatBalanceRatio.Value = 10

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
