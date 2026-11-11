' ハードウェアトリガー（アクティブHigh）: Line0を入力、立ち上がりエッジをトリガー条件として設定します。
' Hardware Trigger (Active High): Set camera to hardware trigger mode for Line0 as input and active high (Trigger activation as rising edge).

' このサンプルの基本操作と対象機能の制御手順を示します。
' This sample demonstrates the basic operation and control procedure for the target feature.
Imports Sentech.GenApiDotNET
Imports Sentech.StApiDotNET

Module HardwareTriggerActiveHigh

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
								' アクティブHighでハードウェアトリガーを有効にします。
								' Demostration of Setting Hardware Trigger ON with active high

								' パラメーターへアクセスするNodeMapを取得します。
								' Create NodeMap pointer for accessing parameters
								Dim nodeMap As INodeMap = device.GetRemoteIStPort().GetINodeMap()

								' LineSelectorをLine0に設定し、入力に使うラインを選択します。
								' Set LineSelector to Line0 to select the input line.
								Dim enumLineSelector As IEnum = nodeMap.GetNode(Of IEnum)("LineSelector")
								enumLineSelector.FromString("Line0")
								Dim enumLineMode As IEnum = nodeMap.GetNode(Of IEnum)("LineMode")

								' LineModeをInputに設定し、選択したラインを入力にします。
								' Set LineMode to Input to use the selected line as an input.
								enumLineMode.FromString("Input")

								' TriggerModeをOnに設定し、トリガーを有効にします。
								' Set TriggerMode to On to enable triggering.
								Dim enumTriggerMode As IEnum = nodeMap.GetNode(Of IEnum)("TriggerMode")
								enumTriggerMode.FromString("On")

								' TriggerSourceをハードウェア入力のLine0に設定します。
								' Set Trigger Source to Line0 as Hardware input
								Dim enumTriggerSource As IEnum = nodeMap.GetNode(Of IEnum)("TriggerSource")
								enumTriggerSource.FromString("Line0")

								' TriggerActivationをRisingEdgeに設定し、立ち上がりエッジでトリガーを受け付けます。
								' Set TriggerActivation to RisingEdge to trigger on a rising edge.
								Dim enumTriggerActive As IEnum = nodeMap.GetNode(Of IEnum)("TriggerActivation")
								enumTriggerActive.FromString("RisingEdge")

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
