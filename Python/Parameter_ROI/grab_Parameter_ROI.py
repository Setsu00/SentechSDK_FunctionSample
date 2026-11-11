"""
ROI: Set ROI to Offset (100, 200) and width/height (640, 480)

"""

import stapipy as st

number_of_images_to_grab = 100

try:
    st.initialize()

    st_system = st.create_system()

    st_device = st_system.create_first_device()

    print('Device=', st_device.info.display_name)

    # ============================================================
    # ROIを640×480、オフセットを（100, 200）に設定するデモです。
    # Demostration of setting ROI to 640 x 480 with offset (100, 200).

    # カメラのパラメーター操作用のNodeMapを取得します。
    # Acquire NodeMap for accessing camera parameter
    nodemap_remote = st_device.remote_port.nodemap

    # Widthノードを取得します。
    # Acquire Width node
    width = nodemap_remote.get_node("Width")

    # 操作用にPyIIntegerへ変換します。
    # Convert to int node for operating
    int_width = st.PyIInteger(width)

    # Widthを640 pixelに設定します。
    # Set the width to 640
    width.value = 640
    
    # Heightノードを取得します。
    # Acquire Height node
    height = nodemap_remote.get_node("Height")

    # 操作用にPyIIntegerへ変換します。
    # Convert to int node for operating
    int_height = st.PyIInteger(height)

    # Heightを480 pixelに設定します。
    # Set the height to 480
    int_height.value = 480
    
    # OffsetXノードを取得します。
    # Acquire OffsetX node
    offset_x = nodemap_remote.get_node("OffsetX")

    # 操作用にPyIIntegerへ変換します。
    # Convert to int node for operating
    int_offset_x = st.PyIInteger(offset_x)

    # OffsetXを100 pixelに設定します。
    # Set the Offset X to 100
    int_offset_x.value = 100
    
    # OffsetYノードを取得します。
    # Acquire OffsetY node
    offset_y = nodemap_remote.get_node("OffsetY")

    # 操作用にPyIIntegerへ変換します。
    # Convert to int node for operating
    int_offset_y = st.PyIInteger(offset_y)

    # OffsetYを200 pixelに設定します。
    # Set the Offset Y to 200
    int_offset_y.value = 200
    
    
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
