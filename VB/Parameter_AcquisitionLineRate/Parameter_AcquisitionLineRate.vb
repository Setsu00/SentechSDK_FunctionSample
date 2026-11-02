' ラインレート設定: AcquisitionLineRateを385 Hzに設定します。
' Acquisition Line Rate: Set AcquisitionLineRate to 385 Hz.

' このサンプルの基本操作と対象機能の制御手順を示します。
' This sample demonstrates the basic operation and control procedure for the target feature.
Imports System
Imports Sentech.GenApiDotNET
Imports Sentech.StApiDotNET

Module AcquisitionLineRate

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
								' カメラのAcquisitionLineRateを385 Hzに設定します。
								' Set the camera AcquisitionLineRate to 385 Hz.

								' パラメーター操作用のNodeMapを取得します。
								' Get the NodeMap for parameter access.
								Dim nodeMap As INodeMap = device.GetRemoteIStPort().GetINodeMap()

								' AcquisitionLineRateノードを取得します。
								' Get the AcquisitionLineRate node.
								Dim floatAcquisitionLineRate As IFloat = nodeMap.GetNode(Of IFloat)("AcquisitionLineRate")

								' AcquisitionLineRateを385 Hzに設定します。
								' Set AcquisitionLineRate to 385 Hz.
								floatAcquisitionLineRate.SetValue(385, False)

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
