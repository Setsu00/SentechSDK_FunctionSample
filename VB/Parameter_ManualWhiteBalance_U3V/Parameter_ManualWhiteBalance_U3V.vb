' 手動ホワイトバランス設定: 各色のBalanceRatioを10（単位なし）に設定します。
' Manual White Balance U3V: Set each BalanceRatio to 10 (unitless).

' このサンプルの基本操作と対象機能の制御手順を示します。
' This sample demonstrates the basic operation and control procedure for the target feature.
Imports System
Imports Sentech.GenApiDotNET
Imports Sentech.StApiDotNET

Module ManualWhiteBalance_U3V

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
								' 自動ホワイトバランスを無効にし、各色のBalanceRatioを10（単位なし）に設定します。
								' Disable automatic white balance and set each color's BalanceRatio to 10 (unitless).

								' パラメーター操作用のNodeMapを取得します。
								' Get the NodeMap for parameter access.
								Dim nodeMap As INodeMap = device.GetRemoteIStPort().GetINodeMap()

								' 自動ホワイトバランス、色選択、比率設定のノードを取得します。
								' Get the auto white balance, color selector, and ratio nodes.
								Dim enumBalanceWhiteAuto As IEnum = nodeMap.GetNode(Of IEnum)("BalanceWhiteAuto")
								Dim enumBalanceRatioSelector As IEnum = nodeMap.GetNode(Of IEnum)("BalanceRatioSelector")
								Dim floatBalanceRatio As IFloat = nodeMap.GetNode(Of IFloat)("BalanceRatio")

								' 自動ホワイトバランスを無効にします。
								' Disable automatic white balance.
								enumBalanceWhiteAuto.FromString("Off")

								' 赤のBalanceRatioを10（単位なし）に設定します。
								' Set the red BalanceRatio to 10 (unitless).
								enumBalanceRatioSelector.FromString("Red")
								floatBalanceRatio.Value = 10

								' 緑のBalanceRatioを10（単位なし）に設定します。
								' Set the green BalanceRatio to 10 (unitless).
								enumBalanceRatioSelector.FromString("Green")
								floatBalanceRatio.Value = 10

								' 青のBalanceRatioを10（単位なし）に設定します。
								' Set the blue BalanceRatio to 10 (unitless).
								enumBalanceRatioSelector.FromString("Blue")
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
