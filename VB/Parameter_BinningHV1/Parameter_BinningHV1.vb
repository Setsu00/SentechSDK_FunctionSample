' ビニング設定: 水平・垂直ビニングを1（単位なし）に設定します。
' Binning HV 1: Set horizontal and vertical binning to 1 (unitless).

' このサンプルの基本操作と対象機能の制御手順を示します。
' This sample demonstrates the basic operation and control procedure for the target feature.
Imports System
Imports Sentech.GenApiDotNET
Imports Sentech.StApiDotNET

Module BinningHV1

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
								' 水平・垂直ビニングを1（単位なし）に設定し、幅と高さを最大値（pixel）に設定します。
								' Set horizontal and vertical binning to 1 (unitless), then set width and height to their maximum values in pixels.

								' パラメーター操作用のNodeMapを取得します。
								' Get the NodeMap for parameter access.
								Dim nodeMap As INodeMap = device.GetRemoteIStPort().GetINodeMap()

								' ビニングと画像サイズのノードを取得します。
								' Get the binning and image-size nodes.
								Dim integerBinningHorizontal As IInteger = nodeMap.GetNode(Of IInteger)("BinningHorizontal")
								Dim integerBinningVertical As IInteger = nodeMap.GetNode(Of IInteger)("BinningVertical")
								Dim integerWidth As IInteger = nodeMap.GetNode(Of IInteger)("Width")
								Dim integerHeight As IInteger = nodeMap.GetNode(Of IInteger)("Height")

								' 水平・垂直ビニングを1（単位なし）に設定します。
								' Set horizontal and vertical binning to 1 (unitless).
								integerBinningHorizontal.Value = 1
								integerBinningVertical.Value = 1

								' WidthとHeightを最大値（pixel）に設定します。
								' Set Width and Height to their maximum values in pixels.
								integerWidth.Value = integerWidth.Maximum
								integerHeight.Value = integerHeight.Maximum

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
