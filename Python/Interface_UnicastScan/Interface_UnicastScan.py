"""
    Interface_UnicastScan.py
    Demstrate the way of setting GigE interface to scan device with unicast with indicated IP address.
"""

import ipaddress
import stapipy as st

try:
    st.initialize()

    # Note: Creating system will  generate a broadcast device scan.
    st_system = st.create_system(st.EStSystemVendor.Default, st.EStInterfaceType.GigEVision)

    # ============================================================
    # Prepear PyStDevice for connection
    st_device = None
    
    # Acquire target device Serial
    tgt_device_ip_str = input("Input target IP address for scan: ")
    tgt_device_ip_int = int(ipaddress.ip_address(tgt_device_ip_str))

    for index in range(st_system.interface_count):
        st_interface = st_system.get_interface(index)
        st_node_discovery_cmd_desIP = st_interface.port.nodemap.get_node("GevDeviceDiscoveryCommandDestinationIPAddress")
        
        if (st_node_discovery_cmd_desIP.is_readable):
            # Set Discovery Command Distination IP Address to enable the unicast camera scanning.
            st_node_discovery_cmd_desIP.get().set_value(tgt_device_ip_int)
            # After setting distination IP, call update for actual scanning process.
            #nicast scanning package will be sent via this function.
            st_system.get_interface(index).update_device_list()
            
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
