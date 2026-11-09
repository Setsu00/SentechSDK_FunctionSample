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

    # カメラのパラメーター操作用のNodeMapを取得します。
    # Acquire NodeMap for accessing camera parameter
    nodemap_remote = st_device.remote_port.nodemap

    # Enable PTP function.
    ptp_enable = nodemap_remote.get_node("PtpEnable")
    # Convert to IBool type for operating
    bool_ptp_enable = st.PyIBoolean(ptp_enable)

    # PtpEnableをTrueに設定し、PTPを有効にします。
    # Set PtpEnable to True to enable PTP.
    bool_ptp_enable.set_value(True)

    # Acquire Node for updating PTP status
    ptp_latch = nodemap_remote.get_node("PtpDataSetLatch")
    command_ptp_latch = st.PyICommand(ptp_latch)

    # Acquire Node for checking PTP status
    ptp_status = nodemap_remote.get_node("PtpStatus")
    enum_ptp_status = st.PyIEnumeration(ptp_status)

    # ============================================================

    while True:
        # PtpDataSetLatchコマンドで現在のPTP情報を更新します。
        # Execute PtpDataSetLatch to update the current PTP information.
        command_ptp_latch.execute()

        # キャッシュを使わずに現在のPTP状態を取得して表示します。
        # Read and display the current PTP status without using the cache.
        current_ptp_status = enum_ptp_status.to_string(True, True)
        print("Current PTP status: " , current_ptp_status)

        # MasterまたはSlaveになるまで待ちます。
        # Wait until the status becomes Master or Slave.
        if current_ptp_status in ("Master", "Slave"):
            break
        time.sleep(1)
        

    print("PTP role confirmed.")
    print("Note PTP sync will be disabled when connection lost.")
        

except Exception as exception:
    print(exception)
