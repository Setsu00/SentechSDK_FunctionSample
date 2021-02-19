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
				// Note: Creating system will  generate a broadcast device scan.
				using (CStSystem system = new CStSystem(eStSystemVendor.Sentech, eStInterfaceType.GigEVision))
				{
					// Input target IP address for scanning.
					Console.Write("Please input camera IP address for unicast camera scanning: ");
					string value = Console.ReadLine();

					IPAddress ipaddr;

					// Try parse the input address, if failed exit program.
					if (!IPAddress.TryParse(value.Trim(), out ipaddr))
					{
						Console.WriteLine("IP address is not valid.");
						Console.WriteLine("\r\nPress Enter to exit.");
						Console.ReadLine();
						Environment.Exit(0);
					}

					// Convert the IP address string to a 32-bit number.
					byte[] bytes = ipaddr.GetAddressBytes();
					uint uiTgtDevIPAddress = (uint)(IPAddress.NetworkToHostOrder(BitConverter.ToUInt32(bytes, 0)) >> 32);

					// Acquire interface counts
					uint uiCntInterface = system.InterfaceCount;

					for (uint i = 0; i < uiCntInterface; i++)
					{
						IStInterface pInterface = system.GetIStInterface(i);
						// Set Discovery Command Distination IP Address to enable the unicast camera scanning.
						// *This is 255.255.255.255 in default for boradcast device scanning.
						INodeMap nodeMapInterface = pInterface.GetIStPort().GetINodeMap();
						IInteger intDistiantionIPAddress = nodeMapInterface.GetNode<IInteger>("GevDeviceDiscoveryCommandDestinationIPAddress");
						intDistiantionIPAddress.SetValue(uiTgtDevIPAddress, false);

						// After setting distination IP, call update for actual scanning process.
						// Unicast scanning package will be sent via this function.
						pInterface.UpdateDeviceList();

					}
					using (CStDevice device = system.CreateFirstStDevice())
					{
						// Print out information of device if found and connected.
						// Otherwise it will goes into exception handling.
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
