"""
AcquireLine0Status : Demostrate of acquiring line status of input pin.

"""

import stapipy as st

number_of_images_to_grab = 100

try:
    st.initialize()

    st_system = st.create_system()

    st_device = st_system.create_first_device()

    print('Device=', st_device.info.display_name)

    # ============================================================
    # 

    # カメラのパラメーター操作用のNodeMapを取得します。
    # Acquire NodeMap for accessing camera parameter
    nodemap_remote = st_device.remote_port.nodemap

    # LineSelectorをLine0に設定し、入力に使うラインを選択します。
    # Set LineSelector to Line0 to select the input line.
    line_selector = nodemap_remote.get_node("LineSelector")
    enum_line_selector = st.PyIEnumeration(line_selector)
    enum_line_selector.from_string("Line0")
    
    line_mode = nodemap_remote.get_node("LineMode")
    enum_line_mode = st.PyIEnumeration(line_mode)

    # LineModeをInputに設定し、選択したラインを入力にします。
    # Set LineMode to Input to use the selected line as an input.
    enum_line_mode.from_string("Input")

    # ライン状態取得用のインターフェースを準備します。
    # Prepear interface for acquiring Line status
    line_status = nodemap_remote.get_node("LineStatus")
    line_status_bool = st.PyIBoolean(line_status)
    
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

                # ラインの状態を取得します。
                # Acquire line status
                line_status_value = line_status_bool.get_value(False, True)

                # 現在のライン状態を表示します。
                # Print out current line status
                print("Current Line0 status:", line_status_value)
            else:
                print("Image data does not exist.")

    st_device.acquisition_stop()

    st_datastream.stop_acquisition()

except Exception as exception:
    print(exception)
