"""
    datastream_chage_resend.py
    このサンプルはIStDataStreamのMaximum Resend Requests Per Blockを50(Default)から0に変更する方法を示します。
    * 0に変更することよりリセンド要求の上限を外れますことで、ずっと転送不安定な環境の場合ドロップ悪化する可能性はあります。

    This sample shows how to modify "Maximum Resend Requests Per Block" of DataStream from 50(default) to 0.
    * Please note that by setting this value to 0, the limitation of resend request block will be disabled.
      If you are using a unstable transferring environment, modify this value may cause multiple frame drops.
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
    # DataSteam対してのノードマップ生成
    # 注意：この設定はホストサイドの設定となります
    # Acquire NodeMap of DataStream for setting.
    # Note: This is a host side setting, not camera side.
    datastream_nodemap = st_datastream.port.nodemap
    
    # Maximum Resend Requests Per Blockのノードを取得
    # Acquire node for MaximumResendRequestsPerBlock
    max_resend_block_cnt_node = datastream_nodemap.get_node("MaximumResendRequestsPerBlock")

    # IIntタイプに変換で操作
    # Convert to IInt type for operation
    int_max_resend_block_cnt = st.PyIInteger(max_resend_block_cnt_node)
    
    # 0に設置よりリセンド要求の上限を外す
    # Set the value to 0 to disable the limitation of resend.
    int_max_resend_block_cnt.set_value(0)
    
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
