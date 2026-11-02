"""
datastream_check_filter_driver.py

This sample shows how to acquire StreamDriverType from IStDataStream.
The value can be used to check whether the current DataStream uses a filter driver.
"""

import stapipy as st

number_of_images_to_grab = 100

try:
    st.initialize()

    st_system = st.create_system()

    st_device = st_system.create_first_device()

    print('Device=', st_device.info.display_name)

    st_datastream = st_device.create_datastream()

    # ============================================================
    # DataStreamのStreamDriverTypeを表示するデモ
    # Demonstration of displaying the DataStream StreamDriverType.

    # DataStreamに対するノードマップを取得する。この設定はホスト側の設定です。
    # Get the NodeMap for DataStream. This is a host-side setting.
    datastream_nodemap = st_datastream.port.nodemap

    # StreamDriverTypeノードを取得して現在の設定を表示
    # Get the StreamDriverType node and display its current setting.
    stream_driver_type = datastream_nodemap.get_node("StreamDriverType")
    enum_stream_driver_type = st.PyIEnumeration(stream_driver_type)
    print("Current stream driver type: " + enum_stream_driver_type.to_string())

    # ============================================================

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
