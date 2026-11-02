"""
datastream_stream_lost_frame_count.py

This sample demonstrates how to acquire and display StreamLostFrameCount from IStDataStream.
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
    # DataStreamのノードマップからロストフレーム数を取得するデモ
    # Demonstration of acquiring the lost frame count from the DataStream NodeMap.

    # DataStreamに対するノードマップを取得する。この設定はホスト側の設定です。
    # Get the NodeMap for DataStream. This is a host-side setting.
    datastream_nodemap = st_datastream.port.nodemap

    # StreamLostFrameCountノードを取得
    # Get the StreamLostFrameCount node.
    stream_lost_frame_count = datastream_nodemap.get_node("StreamLostFrameCount")
    int_stream_lost_frame_count = st.PyIInteger(stream_lost_frame_count)

    # ============================================================

    st_datastream.start_acquisition(number_of_images_to_grab)

    st_device.acquisition_start()

    while st_datastream.is_grabbing:
        with st_datastream.retrieve_buffer() as st_buffer:
            if st_buffer.info.is_image_present:
                st_image = st_buffer.get_image()
                print("BlockID={0} Size={1} x {2} First Byte={3} LostFrameCount={4}".format(
                    st_buffer.info.frame_id,
                    st_image.width, st_image.height,
                    st_image.get_image_data()[0],
                    int_stream_lost_frame_count.value))
            else:
                print("Image data does not exist.")

    st_device.acquisition_stop()

    st_datastream.stop_acquisition()

except Exception as exception:
    print(exception)
