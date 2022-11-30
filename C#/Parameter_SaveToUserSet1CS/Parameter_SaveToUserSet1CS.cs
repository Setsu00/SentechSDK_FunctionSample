/*
    Parameter_SaveToUserSet1CS.cs : カメラ設定をUserSet1に保存し、デフォルトに設定するデモ
	Parameter_SaveToUserSet1CS.cs : Demostration of saving camera setting to UserSet1 as default.
	
*/

#define ENABLED_ST_GUI

using System;
using Sentech.GenApiDotNET;
using Sentech.StApiDotNET;

namespace Grab
{
	class Parameter_SaveToUserSet1CS
	{
		const int nCountOfImagesToGrab = 100;

		static void Main(string[] args)
		{
			try
			{
				using (CStApiAutoInit api = new CStApiAutoInit())

				using (CStSystem system = new CStSystem(eStSystemVendor.Sentech))

				using (CStDevice device = system.CreateFirstStDevice())

				using (CStImageDisplayWnd wnd = new CStImageDisplayWnd())

				using (CStDataStream dataStream = device.CreateStDataStream(0))
				{
					Console.WriteLine("Device=" + device.GetIStDeviceInfo().DisplayName);

                    // ==============================================================================================================
                    // 現在の設定をカメラのUserSet1に保存し、電源ON時のデフォルトに設定する
                    // UserSetが保存できるのはカメラが取得中でない場合にのみ使用できます。
                    // Saving current setting to UserSet1 of camera, with setting it as default when camera power on.
                    // Please notice the UserSet saving can be only avaliable when camera is not in acquiring.

                    // パラメータにアクセスするためのノードマップポインタを生成
                    // Create NodeMap pointer for accessing parameters
                    INodeMap nodeMap = device.GetRemoteIStPort().GetINodeMap();

                    // 設定を保存するUserSet(UserSet1)を選択
                    // Select which UserSet to save the setting (UserSet1)
                    IEnum enumUserSetSelector = nodeMap.GetNode<IEnum>("UserSetSelector");
					enumUserSetSelector.FromString("UserSet1");

                    // 設定を保存するUserSetを取得し、実行する
                    // Acquire and execute saving setting to UserSet
                    ICommand cmdSaveToUserSet = nodeMap.GetNode<ICommand>("UserSetSave");
					cmdSaveToUserSet.Execute();

					Console.WriteLine("Save Current setting to UserSet1 succeed.");

                    // 電源ON時にこの設定を使用するためにUserSetDefaultにUserSet1を設定
                    // Set UserSetDefault to UsetSet1 for using this setting when camera power on.
                    IEnum enumUserSetDefault = nodeMap.GetNode<IEnum>("UserSetDefault");
					enumUserSetDefault.FromString("UserSet1");

					Console.WriteLine("Set UserSetDefault to UserSet1 succeed.");

                    // ==============================================================================================================
				}
			}
			catch (Exception e)
			{
				Console.Error.WriteLine("An exception occurred. \r\n" + e.Message);
			}
			finally
			{
				Console.WriteLine("\r\nPress Enter to exit.");
				Console.ReadLine();
			}
		}
	}
}
