"""
BinningHV1: Set Camera Binning Horizontal / Binning Vertical to 1

"""

import stapipy as st

number_of_images_to_grab = 100

try:
    st.initialize()

    st_system = st.create_system()

    st_device = st_system.create_first_device()

    print('Device=', st_device.info.display_name)
    
    # ============================================================
    # 水平方向・垂直方向のビニングを1に設定するデモです。
    # Demostration of setting Binning Horizontal / Binning Veritcal to 1
    
    # カメラのパラメーター操作用のNodeMapを取得します。
    # Acquire NodeMap for accessing camera parameter
    nodemap_remote = st_device.remote_port.nodemap
    
    # BinningHorizontalノードを取得し、1に設定します。
    # Get the BinningHorizontal node and set it to 1.
    binning_h = nodemap_remote.get_node("BinningHorizontal")
    binning_h.value = 1
    
    # BinningVerticalノードを取得し、1に設定します。
    # Get the BinningVertical node and set it to 1.
    binning_v = nodemap_remote.get_node("BinningVertical")
    binning_v.value = 1
    
    # ビニングを1に設定する場合は、幅・高さを手動で最大値に設定します。
    # For setting Binning to 1, both height and width need to be set to max manually.
    
    # Widthノードを取得します。
    # Acquire Width node
    width = nodemap_remote.get_node("Width")

    # 最大値を取得するため、PyIIntegerへ変換します。
    # Convert to int node for acquire max value
    int_width = st.PyIInteger(width)

    # Widthを最大値に設定します。
    # Set the width to max as it can
    width.value = int_width.max
    
    # Heightノードを取得します。
    # Acquire Height node
    height = nodemap_remote.get_node("Height")

    # 最大値を取得するため、PyIIntegerへ変換します。
    # Convert to int node for acquire max value
    int_height = st.PyIInteger(height)

    # Heightを最大値に設定します。
    # Set Height to its maximum value.
    height.value = int_height.max
    
    
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
