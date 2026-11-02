' Focus Value: FocusValueの範囲と現在値を表示し、入力した値（単位なし）を設定します。
' Focus Value: Display the FocusValue range and current value, then set the entered value (unitless).

' このサンプルの基本操作と対象機能の制御手順を示します。
' This sample demonstrates the basic operation and control procedure for the target feature.
Imports System
Imports Sentech.GenApiDotNET
Imports Sentech.StApiDotNET

Module Parameter_FocusValue

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
								' FocusValueの範囲と現在値を表示し、入力した値（単位なし）を設定します。
								' Display the FocusValue range and current value, then set the entered value (unitless).

								' パラメーター操作用のNodeMapを取得します。
								' Get the NodeMap for parameter access.
								Dim nodeMap As INodeMap = device.GetRemoteIStPort().GetINodeMap()

								' FocusValueノードを取得します。
								' Get the FocusValue node.
								Dim integerFocusValue As IInteger = nodeMap.GetNode(Of IInteger)("FocusValue")

								' FocusValueの最小値、最大値、現在値を取得します。
								' Get the minimum, maximum, and current FocusValue values.
								Dim minimumFocusValue As Long = integerFocusValue.Minimum
								Dim maximumFocusValue As Long = integerFocusValue.Maximum
								Dim currentFocusValue As Long = integerFocusValue.GetValue(True, False)

								' 範囲と現在値を表示し、設定値を入力します。
								' Display the range and current value, then enter the value to set.
								Console.WriteLine("FocusValue: " & currentFocusValue & ", range " & minimumFocusValue & " .. " & maximumFocusValue)
								Console.Write("Enter FocusValue (99999 to exit): ")
								Dim inputFocusValue As Long = Convert.ToInt64(Console.ReadLine())
								If inputFocusValue = 99999 Then Return

								' 入力したFocusValueを設定します。
								' Set the entered FocusValue.
								integerFocusValue.SetValue(inputFocusValue, True)
								Console.WriteLine("FocusValue set to: " & inputFocusValue)

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
