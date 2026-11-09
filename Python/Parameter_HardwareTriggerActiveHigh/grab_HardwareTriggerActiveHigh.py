"""
Hardware Trigger (Active High): Set camera to hardware trigger mode for Line0 as input and active high (Trigger activation as rising edge).

"""

import stapipy as st

number_of_images_to_grab = 100

try:
    st.initialize()

    st_system = st.create_system()

    st_device = st_system.create_first_device()

    print('Device=', st_device.info.display_name)

    # ============================================================
    # Demostration of setting trigger source to Line0 with rising edge as trigger activation.

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
    
    # TriggerModeをOnに設定し、トリガーを有効にします。
    # Set TriggerMode to On to enable triggering.
    trigger_mode = nodemap_remote.get_node("TriggerMode")
    enum_trigger_mode = st.PyIEnumeration(trigger_mode)
    enum_trigger_mode.from_string("On")
    
    # ハードウェア入力を使うため、TriggerSourceをLine0に設定します。
    # Set Trigger Source to Line0 as Hardware input
    trigger_source = nodemap_remote.get_node("TriggerSource")
    enum_trigger_source = st.PyIEnumeration(trigger_source)
    enum_trigger_source.from_string("Line0")
    
    # TriggerActivationをRisingEdgeに設定し、立ち上がりエッジでトリガーを受け付けます。
    # Set TriggerActivation to RisingEdge to trigger on a rising edge.
    trigger_activation = nodemap_remote.get_node("TriggerActivation")
    enum_trigger_activation = st.PyIEnumeration(trigger_activation)
    enum_trigger_activation.from_string("RisingEdge")
    
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
