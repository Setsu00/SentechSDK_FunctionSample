' ソフトウェアトリガー: カメラをソフトウェアトリガーモードに設定し、トリガーコマンドを送信します。
' Software Trigger: Demostrate how to set camera to Software Trigger mode and how to send Trigger command.

' このサンプルの基本操作と対象機能の制御手順を示します。
' This sample demonstrates the basic operation and control procedure for the target feature.
Imports Sentech.GenApiDotNET
Imports Sentech.StApiDotNET

Module SoftwareTrigger

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
								' ソフトウェアトリガーを有効にします。
								' Demostration of Setting Software Trigger ON

								' パラメーターへアクセスするNodeMapを取得します。
								' Create NodeMap pointer for accessing parameters
								Dim nodeMap As INodeMap = device.GetRemoteIStPort().GetINodeMap()

								' TriggerModeをOnに設定し、トリガーを有効にします。
								' Set TriggerMode to On to enable triggering.
								Dim enumTriggerMode As IEnum = nodeMap.GetNode(Of IEnum)("TriggerMode")
								enumTriggerMode.FromString("On")

								' TriggerSourceをSoftwareに設定します。
								' Set Trigger Source to Software
								Dim enumTriggerSource As IEnum = nodeMap.GetNode(Of IEnum)("TriggerSource")
								enumTriggerSource.FromString("Software")


								' 後で実行するSoftwareTriggerコマンドを取得します。
								' Prepear Software Trigger Command for later calling
								Dim cmdTriggerSoftware As ICommand = nodeMap.GetNode(Of ICommand)("TriggerSoftware")

								' ==============================================================================================================

								dataStream.StartAcquisition(nCountOfImagesToGrab)

								device.AcquisitionStart()

								While dataStream.IsGrabbing

									' ===============================================================================
									' ソフトウェアトリガーを送信します。
									' Demostration sending software trigger

									cmdTriggerSoftware.Execute()
									Console.WriteLine("Software Trigger Sent.")

									' ===============================================================================

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

								' ==============================================================================================================

								' 画像取得後にTriggerMode（IEnumeration）をOffに設定します。
								' Switch off Trigger Mode(IEnumeration) after acquiring.
								enumTriggerMode.FromString("Off")

								' ==============================================================================================================
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
