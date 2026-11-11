"""
    Interface_UnicastScan.py
    Demstrate the way of setting GigE interface to scan device with unicast with indicated IP address.
"""

import ipaddress
import stapipy as st

try:
    st.initialize()

    # System生成時にブロードキャストでデバイス検出が行われます。
    # Note: Creating system will  generate a broadcast device scan.
    st_system = st.create_system(st.EStSystemVendor.Default, st.EStInterfaceType.GigEVision)

    # ============================================================
    # 接続用のPyStDeviceを準備します。
    # Prepear PyStDevice for connection
    st_device = None
    
    # 対象デバイスのシリアル番号を取得します。
    # Acquire target device Serial
    tgt_device_ip_str = input("Input target IP address for scan: ")
    tgt_device_ip_int = int(ipaddress.ip_address(tgt_device_ip_str))

    for index in range(st_system.interface_count):
        st_interface = st_system.get_interface(index)
        st_node_discovery_cmd_desIP = st_interface.port.nodemap.get_node("GevDeviceDiscoveryCommandDestinationIPAddress")
        
        if (st_node_discovery_cmd_desIP.is_readable):
            # 検出コマンドの宛先IPアドレスを設定して、ユニキャスト検出を行います。
            # Set the discovery-command destination IP address for unicast discovery.
            st_node_discovery_cmd_desIP.get().set_value(tgt_device_ip_int)

            # 宛先IP設定後にデバイス一覧を更新し、ユニキャストでカメラを検出します。
            # After setting distination IP, call update for actual scanning process.
            # Unicast scanning package will be sent via this function.
            st_system.get_interface(index).update_device_list()
            
    # 指定IPのカメラへ接続します。見つかればデバイス名を表示し、見つからなければ例外処理へ進みます。
    # Try to connect to any camera with indicated IP address.
    # If found, show the device display name;
    # if not found it will goes to exception handling.
    st_device = st_system.create_first_device()
    
    if st_device == None:
        print("Target camera not found.")
        exit()
    
    # ============================================================
    
    print('Device=', st_device.info.display_name, 'found and connected.')
    print('Exiting...')

except Exception as exception:
    print(exception)
