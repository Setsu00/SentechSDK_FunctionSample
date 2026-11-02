' オートゲイン制御: オートゲインを有効にし、範囲を0～10 dBに設定します。
' Auto Gain Control: Set auto gain control on with range 0 dB to 10 dB.

' このサンプルの基本操作と対象機能の制御手順を示します。
' This sample demonstrates the basic operation and control procedure for the target feature.
Imports Sentech.GenApiDotNET
Imports Sentech.StApiDotNET

Module AutoGainControl

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
								' 指定した範囲でオートゲイン制御を設定します。
								' Demostration of Setting Auto Gain Control with dedicated range

								' パラメーターへアクセスするNodeMapを取得します。
								' Create NodeMap pointer for accessing parameters
								Dim nodeMap As INodeMap = device.GetRemoteIStPort().GetINodeMap()

								' GainAuto（IEnumeration）を有効にします。
								' Switch on Gain Auto(IEnumeration).
								Dim enumGainAuto As IEnum = nodeMap.GetNode(Of IEnum)("GainAuto")
								enumGainAuto.FromString("Continuous")

								' AutoLuminanceTargetノード（IInteger）を取得します。
								' Get Node for Auto Luminance Target(IInteger)
								Dim intAutoLuminTgt As IInteger = nodeMap.GetNode(Of IInteger)("AutoLuminanceTarget")

								' AutoLuminanceTargetを128に設定します。
								' Set Auto Luminance Target to 128
								intAutoLuminTgt.Value = 128

								' アナログゲインへアクセスするため、GainSelectorをAnalogAllに設定します。
								' For setting analog gain, gain selector need to be set to AnalogAll to access analog gain.
								Dim enumGainSelector As IEnum = nodeMap.GetNode(Of IEnum)("GainSelector")
								enumGainSelector.FromString("AnalogAll")

								' GainAutoLimitMinノード（IFloat）を取得します。
								' Get Node for GainAutoLimitMin(IFloat).
								Dim floatGainAutoMin As IFloat = nodeMap.GetNode(Of IFloat)("GainAutoLimitMin")

								' オートゲインの最小値を0 dB（0）に設定します。
								' Set Auto Gain Min to 0 dB (0).
								floatGainAutoMin.Value = 0

								' GainAutoLimitMaxノード（IFloat）を取得します。
								' Get Node for GainAutoLimitMax(IFloat).
								Dim floatGainAutoMax As IFloat = nodeMap.GetNode(Of IFloat)("GainAutoLimitMax")

								' オートゲインの最大値を10 dB（100）に設定します。
								' Set Auto Gain Max to 10 dB (100).
								floatGainAutoMax.Value = 100

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
