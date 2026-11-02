' SaveToUserSet1: カメラ設定をUserSet1へ保存し、デフォルトに設定します。
' SaveToUserSet1: Demostration of saving camera setting to UserSet1 as default.

' このサンプルの基本操作と対象機能の制御手順を示します。
' This sample demonstrates the basic operation and control procedure for the target feature.
Imports Sentech.GenApiDotNET
Imports Sentech.StApiDotNET

Module SaveToUserSet1

	Const nCountOfImagesToGrab As Integer = 100

	Sub Main()
		Try
			Using api As CStApiAutoInit = New CStApiAutoInit()

				Using system As CStSystem = New CStSystem(eStSystemVendor.Sentech)

					Using device As CStDevice = system.CreateFirstStDevice()

						Console.WriteLine("Device=" & device.GetIStDeviceInfo().DisplayName)

						' ==============================================================================================================
						' 現在のカメラ設定をUserSet1へ保存し、起動時のデフォルトに設定します。
						' Saving current setting to UserSet1 of camera, with setting it as default when camera power on.
						' UserSetへの保存は、カメラが画像取得中でない場合にのみ実行できます。
						' Please notice the UserSet saving can be only avaliable when camera is not in acquiring.

						' パラメーターへアクセスするNodeMapを取得します。
						' Create NodeMap pointer for accessing parameters
						Dim nodeMap As INodeMap = device.GetRemoteIStPort().GetINodeMap()

						' 設定の保存先としてUserSet1を選択します。
						' Select which UserSet to save the setting (UserSet1)
						Dim enumUserSetSelector As IEnum = nodeMap.GetNode(Of IEnum)("UserSetSelector")
						enumUserSetSelector.FromString("UserSet1")

						' UserSetへの設定保存コマンドを取得して実行します。
						' Acquire And execute saving setting to UserSet
						Dim cmdSaveToUserSet As ICommand = nodeMap.GetNode(Of ICommand)("UserSetSave")
						cmdSaveToUserSet.Execute()

						Console.WriteLine("Save Current setting to UserSet1 succeed.")

						' カメラ起動時に使用する設定としてUserSetDefaultをUserSet1に設定します。
						' Set UserSetDefault to UsetSet1 for using this setting when camera power on.
						Dim enumUserSetDefault As IEnum = nodeMap.GetNode(Of IEnum)("UserSetDefault")
						enumUserSetDefault.FromString("UserSet1")

						Console.WriteLine("Set UserSetDefault to UserSet1 succeed.")

						' ==============================================================================================================

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
