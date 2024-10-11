/*
    DeviceUserID : カメラの Device User IDを設置して、また読み出すこと。
	DeviceUserID : Demostration for setting and getting Device User ID of camera.
*/

#define ENABLED_ST_GUI

using System;
using Sentech.GenApiDotNET;
using Sentech.StApiDotNET;

namespace Grab
{
	class Grab_DeviceUserID
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
                    // 

                    // パラメータにアクセスするためのノードマップポインタを生成
                    // Create NodeMap pointer for accessing parameters
                    INodeMap nodeMap = device.GetRemoteIStPort().GetINodeMap();

					// DeviceUserIDアクセス用のノードを取得する
					// Acquire node for accessing DeviceUserID
					IString strDeviceUserID = nodeMap.GetNode<IString>("DeviceUserID");

					// 今カメラのDeviceUserIDを示す
					// Display current Device User ID
					Console.WriteLine("Current DeviceUserID = " + strDeviceUserID.Value);

                    // 新しいDeviceUserID入力
					// Acquire new DeviceUserID for writing to camera
                    System.Console.WriteLine("Please input new DeviceUserID for target camera: ");
                    string strNewID = System.Console.ReadLine();

					// 入力したDeviceUserIDをカメラに設置
					// Set the input ID to camera
					strDeviceUserID.SetValue(strNewID, true);

                    // 設置後のDeviceUserIDを示す
                    // Display the ID after setting.
                    Console.WriteLine("Current DeviceUserID = " + strDeviceUserID.GetValue(true, true));

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
