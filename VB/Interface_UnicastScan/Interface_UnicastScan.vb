' ユニキャスト検出先のIPv4アドレスを設定します。
' Set the unicast discovery destination IPv4 address.
Imports System
Imports Sentech.GenApiDotNET
Imports Sentech.StApiDotNET

Module Interface_UnicastScan

    Sub Main()
        Try
            Using api As New CStApiAutoInit()
            Using system As New CStSystem(eStSystemVendor.Sentech, eStInterfaceType.GigEVision)
                ' ユニキャスト検出先のIPv4アドレスを入力し、数値へ変換します。
                ' Enter the unicast discovery destination IPv4 address and convert it to a numeric value.
                Console.Write("Camera IPv4 address: ")
                Dim address As Global.System.Net.IPAddress = Global.System.Net.IPAddress.Parse(Console.ReadLine())
                Dim bytes As Byte() = address.GetAddressBytes()
                If bytes.Length <> 4 Then Throw New ArgumentException("IPv4 address required.")
                Dim ip As Long = CLng(bytes(0)) * 16777216L + CLng(bytes(1)) * 65536L +
                                 CLng(bytes(2)) * 256L + bytes(3)

                ' 各インターフェースの検出先を設定し、デバイスリストを更新します。
                ' Set the destination and update the device list for each interface.
                For i As Integer = 0 To CInt(system.InterfaceCount) - 1
                    Dim iface As IStInterface = system.GetIStInterface(CUInt(i))

                    ' インターフェースのNodeMapからユニキャスト検出先ノードを取得します。
                    ' Get the unicast discovery destination node from the interface NodeMap.
                    Dim nodeMap As INodeMap = iface.GetIStPort().GetINodeMap()
                    Dim destination As IInteger = nodeMap.GetNode(Of IInteger)("GevDeviceDiscoveryCommandDestinationIPAddress")

                    ' 検出先を設定し、デバイスリストを更新します。
                    ' Set the destination and update the device list.
                    destination.Value = ip
                    iface.UpdateDeviceList()
                Next
            Using device As CStDevice = system.CreateFirstStDevice()
                Console.WriteLine("Device=" & device.GetIStDeviceInfo().DisplayName)

            End Using
            End Using
            End Using
        Catch e As Exception
            Console.Error.WriteLine("An exception occurred." & vbCrLf & e.Message)
        Finally
            Console.WriteLine("Press Enter to exit.")
            Console.ReadLine()
        End Try
    End Sub
End Module
