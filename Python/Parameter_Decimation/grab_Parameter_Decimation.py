"""
Decimation: Set horizontal/vertical decimation to 2.

"""

import stapipy as st

number_of_images_to_grab = 100

try:
    st.initialize()

    st_system = st.create_system()

    st_device = st_system.create_first_device()

    print('Device=', st_device.info.display_name)
    
    # ============================================================
    # Demostration of setting decimation horizontal/vertical to 2.
    
    # カメラのパラメーター操作用のNodeMapを取得します。
    # Acquire NodeMap for accessing camera parameter
    nodemap_remote = st_device.remote_port.nodemap
    
    # DecimationHorizontalノードを取得し、2に設定します。
    # Get the DecimationHorizontal node and set it to 2.
    decimation_h = nodemap_remote.get_node("DecimationHorizontal")
    decimation_h.value = 2
    
    # DecimationVerticalノードを取得し、2に設定します。
    # Get the DecimationVertical node and set it to 2.
    decimation_v = nodemap_remote.get_node("DecimationVertical")
    decimation_v.value = 2
    
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
