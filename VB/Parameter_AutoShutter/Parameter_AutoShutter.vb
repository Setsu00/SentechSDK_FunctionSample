' Auto Shutter: 自動露光を有効にし、露光時間範囲を1,000～150,000 µsに設定する。
' Auto Shutter: Enable auto exposure and set its exposure-time range to 1,000–150,000 µs.

Imports Sentech.GenApiDotNET
Imports Sentech.StApiDotNET

Module AutoShutter

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
								' 指定した露光時間範囲でオートシャッターを有効にする。
								' Enable auto shutter within the specified exposure-time range.

								' カメラのパラメーターへアクセスするNodeMapを取得する。
								' Get the NodeMap used to access camera parameters.
								Dim nodeMap As INodeMap = device.GetRemoteIStPort().GetINodeMap()

								' ExposureAutoをContinuousに設定し、自動露光を継続します。
								' Set ExposureAuto to Continuous to adjust exposure automatically and continuously.
								Dim enumExposureAuto As IEnum = nodeMap.GetNode(Of IEnum)("ExposureAuto")
								enumExposureAuto.FromString("Continuous")

								' AutoLuminanceTarget（IInteger）ノードを取得する。
								' Get the AutoLuminanceTarget (IInteger) node.
								Dim intAutoLuminTgt As IInteger = nodeMap.GetNode(Of IInteger)("AutoLuminanceTarget")

								' AutoLuminanceTargetに128を設定する。
								' Set AutoLuminanceTarget to 128.
								intAutoLuminTgt.Value = 128

								' ExposureAutoLimitMin（IFloat）ノードを取得する。
								' Get the ExposureAutoLimitMin (IFloat) node.
								Dim floatExpoAutoMin As IFloat = nodeMap.GetNode(Of IFloat)("ExposureAutoLimitMin")

								' 自動露光時間の最小値を1,000 µsに設定する。
								' Set the minimum auto exposure time to 1,000 µs.
								floatExpoAutoMin.Value = 1000

								' ExposureAutoLimitMax（IFloat）ノードを取得する。
								' Get the ExposureAutoLimitMax (IFloat) node.
								Dim floatExpoAutoMax As IFloat = nodeMap.GetNode(Of IFloat)("ExposureAutoLimitMax")

								' 自動露光時間の最大値を150,000 µsに設定する。
								' Set the maximum auto exposure time to 150,000 µs.
								floatExpoAutoMax.Value = 150000

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
