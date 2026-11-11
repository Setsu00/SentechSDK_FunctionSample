"""
	Heartbeat Timeout: Set device heartbeat timeout to 30 sec. (30000000 us)

"""

import stapipy as st

number_of_images_to_grab = 100

try:
    st.initialize()

    st_system = st.create_system()

    st_device = st_system.create_first_device()

    print('Device=', st_device.info.display_name)

    # ============================================================
    # HeartbeatTimeoutを30秒（30000000 µs）に設定するデモです。
    # Demostration of set Heartbeat Timeout to 30 sec (30000000us)

    # カメラのパラメーター操作用のNodeMapを取得します。
    # Acquire NodeMap for accessing camera parameter
    nodemap_remote = st_device.remote_port.nodemap
    
    # HeartbeatTimeoutノードを取得します。
    # Get Node for Heartbeat Timeout
    heartbeattimeout = nodemap_remote.get_node("DeviceLinkHeartbeatTimeout")

    # 操作用にPyIFloatへ変換します。
    # Convert to IFloat type for operating
    float_heartbeattimeout = st.PyIFloat(heartbeattimeout)

    # ハートビートのタイムアウトを30秒に設定します。
    # Set heartbeattimeout to 30 sec.
    float_heartbeattimeout.value = 30000000

    # ============================================================

    st_datastream = st_device.create_datastream()

    st_datastream.start_acquisition(number_of_images_to_grab)

    st_device.acquisition_start()

    while st_datastream.is_grabbing:
        with st_datastream.retrieve_buffer() as st_buffer:
            if st_buffer.info.is_image_present:
                st_image = st_buffer.get_image()
                print("BlockID={0} Size={1} x {2} First Byte={3}".format(
                      st_buffer.info.frame_id,
                      st_image.width, st_image.height,
                      st_image.get_image_data()[0]))
            else:
                print("Image data does not exist.")

    st_device.acquisition_stop()

    st_datastream.stop_acquisition()

except Exception as exception:
    print(exception)
