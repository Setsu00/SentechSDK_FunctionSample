/*
	Interface_UnicastScanCS.cs
	指定されたIPアドレスを持つユニキャストを使用してデバイスをスキャンするようにGigEインターフェイスを設定する方法をデモします。
	Demstrate the way of setting GigE interface to scan device with unicast with indicated IP address.
*/

using System;
using System.Net;
using System.Threading;
using Sentech.GenApiDotNET;
using Sentech.StApiDotNET;

namespace Grab
{
	class Interface_UnicastScanCS
	{
		const int nCountOfImagesToGrab = 100;

		static void Main(string[] args)
		{
			try
			{
				using (CStApiAutoInit api = new CStApiAutoInit())

                // =============================================================================================
                // Note: システムを作成すると、ブロードキャストデバイススキャンが生成されます。
                // Note: Creating system will  generate a broadcast device scan.
                using (CStSystem system = new CStSystem(eStSystemVendor.Sentech, eStInterfaceType.GigEVision))
				{
                    // スキャンするためのIPアドレスを入力
                    // Input target IP address for scanning.
                    Console.Write("Please input camera IP address for unicast camera scanning: ");
					string value = Console.ReadLine();

					IPAddress ipaddr;

                    // 入力アドレスを解析し、解析に失敗した場合は終了
                    // Try parse the input address, if failed exit program.
                    if (!IPAddress.TryParse(value.Trim(), out ipaddr))
					{
						Console.WriteLine("IP address is not valid.");
						Console.WriteLine("\r\nPress Enter to exit.");
						Console.ReadLine();
						Environment.Exit(0);
					}

                    // IPアドレス文字列を32ビットの数値に変換します。
                    // Convert the IP address string to a 32-bit number.
                    byte[] bytes = ipaddr.GetAddressBytes();

					// IPv4アドレスを、SDKノードへ設定する32ビット値へ変換します。
					// Convert the IPv4 address to the 32-bit value used by the SDK node.
					uint uiTgtDevIPAddress = ((uint)bytes[0] << 24) |
					                             ((uint)bytes[1] << 16) |
					                             ((uint)bytes[2] << 8) |
					                              bytes[3];

                    // インターフェースの数を取得
                    // Acquire interface counts
                    uint uiCntInterface = system.InterfaceCount;

					for (uint i = 0; i < uiCntInterface; i++)
					{
						IStInterface pInterface = system.GetIStInterface(i);

                        // 検出コマンドの宛先IPアドレスを設定して、ユニキャストカメラスキャンを有効にします。
                        // *ブロードキャストデバイススキャンのデフォルトは255.255.255.255です。
                        // Set the discovery-command destination IP address to enable unicast camera scanning.
                        // *The default value is 255.255.255.255 for broadcast device scanning.
                        INodeMap nodeMapInterface = pInterface.GetIStPort().GetINodeMap();
						IInteger intDestinationIPAddress = nodeMapInterface.GetNode<IInteger>("GevDeviceDiscoveryCommandDestinationIPAddress");
						intDestinationIPAddress.SetValue(uiTgtDevIPAddress, false);

                        // 接続先IPの設定後、実際のスキャンプロセスの更新を呼び出します。
                        // ユニキャストスキャンパッケージは、この機能を介して送信されます。
                        // After setting distination IP, call update for actual scanning process.
                        // Unicast scanning package will be sent via this function.
                        pInterface.UpdateDeviceList();

					}
					using (CStDevice device = system.CreateFirstStDevice())
					{
                        // デバイスが見つかり、接続されている場合は、デバイスの情報を出力します。
                        // それ以外の場合は例外処理に入ります。
                        // Print out information of device if found and connected.
                        // Otherwise, continue to exception handling.
                        Console.WriteLine("Device=" + device.GetIStDeviceInfo().DisplayName);
						Console.WriteLine("Device found and connected.");
					}
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
