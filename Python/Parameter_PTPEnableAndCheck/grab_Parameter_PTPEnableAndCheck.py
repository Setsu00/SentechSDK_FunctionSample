"""
grab_Parameter_PTPEnableAndCheck.py

"""

import stapipy as st
import time


try:
    st.initialize()

    st_system = st.create_system()

    st_device = st_system.create_first_device()

    print('Device=', st_device.info.display_name)

    # ============================================================
    # Demostration of enable PTP function and wait for status confirmed

    # Acquire NodeMap for accessing camera parameter
    nodemap_remote = st_device.remote_port.nodemap

    # Enable PTP function.
    ptp_enable = nodemap_remote.get_node("PtpEnable")
    # Convert to IBool type for operating
    bool_ptp_enable = st.PyIBoolean(ptp_enable)
    # Set PTP on
    bool_ptp_enable.set_value(True)

    # Acquire Node for updating PTP status
    ptp_latch = nodemap_remote.get_node("PtpDataSetLatch")
    command_ptp_latch = st.PyICommand(ptp_latch)

    # Acquire Node for checking PTP status
    ptp_status = nodemap_remote.get_node("PtpStatus")
    enum_ptp_status = st.PyIEnumeration(ptp_status)

    # ============================================================

    while True:
        # Update current PTP status
        command_ptp_latch.execute()
        # Acquire current PTP status
        print("Current PTP status: " , enum_ptp_status.to_string(True, True))
        if(enum_ptp_status.to_string() != "Listening"):
            break
        time.sleep(1)
        

    print("PTP sync done.")
    print("Note PTP sync will be disabled when connection lost.")
        

except Exception as exception:
    print(exception)
