"""
    BlackLevelを0.0に設定します。
    Set BlackLevel to 0.0.

"""

import stapipy as st

number_of_images_to_grab = 100

try:
    st.initialize()

    st_system = st.create_system()

    st_device = st_system.create_first_device()

    print('Device=', st_device.info.display_name)

    # ============================================================
    # BlackLevelを0.0に設定します。 
    # Set BlackLevel to 0.0.

    # リモートNodeMapを取得します。 
    # Acquire the remote NodeMap.
    nodemap_remote = st_device.remote_port.nodemap

    # BlackLevelノードを取得します。 
    # Get the BlackLevel node.
    black_level = nodemap_remote.get_node("BlackLevel")
    
    # PyIFloatに変換します。 
    # Convert to PyIFloat.
    ifloat_black_level = st.PyIFloat(black_level)

    # 値を0.0に設定します。 
    # Set the value to 0.0.
    ifloat_black_level.value = 0.0

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
