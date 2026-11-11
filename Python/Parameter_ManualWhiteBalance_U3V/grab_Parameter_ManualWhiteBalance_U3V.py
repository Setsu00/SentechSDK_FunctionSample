"""
	Manual White Balance: Manually set white balance ratio of all color to 10.

"""

import stapipy as st

number_of_images_to_grab = 100

try:
    st.initialize()

    st_system = st.create_system()

    st_device = st_system.create_first_device()

    print('Device=', st_device.info.display_name)

    # ============================================================
    # 自動ホワイトバランスを無効にし、全色の比率を10に設定するデモです。
    # Demostration of changing white balance to manual with setting all balance ratio to 10

    # カメラのパラメーター操作用のNodeMapを取得します。
    # Acquire NodeMap for accessing camera parameter
    nodemap_remote = st_device.remote_port.nodemap

    # 手動設定のため自動ホワイトバランスを無効にするBalanceWhiteAutoノードを取得します。
    # Switch BalanceWhiteAuto to Off for manual input value
    # Acquire Node for BalanceWhiteAuto
    balance_auto = nodemap_remote.get_node("BalanceWhiteAuto")

    # 操作用にPyIEnumerationへ変換します。
    # Convert to IEnum type for operating
    enum_balance_auto = st.PyIEnumeration(balance_auto)

    # BalanceWhiteAutoをOffに設定し、自動ホワイトバランスを無効にします。
    # Set BalanceWhiteAuto to Off
    enum_balance_auto.from_string("Off")
    
    # 赤の比率にアクセスするため、BalanceRatioSelectorをRedに設定します。
    # Switch balance ration selector to Red for access Red ratio value
    # Acquire Node for BalanceRatioSelector
    balance_ratio_selector = nodemap_remote.get_node("BalanceRatioSelector")

    # 操作用にPyIEnumerationへ変換します。
    # Convert to IEnum type for operating
    enum_balance_ratio_selector = st.PyIEnumeration(balance_ratio_selector)

    # 赤の比率にアクセスするため、BalanceRatioSelectorをRedに設定します。
    # Switch to Red for Red ratio handling
    enum_balance_ratio_selector.from_string("Red")
    
    # BalanceRatioの値を操作するノードを取得します。
    # Get Node for value of BalanceRatio
    balance_ratio = nodemap_remote.get_node("BalanceRatio")

    # 操作用にPyIFloatへ変換します。
    # Convert to IFloat type for operating
    float_balance_ratio = st.PyIFloat(balance_ratio)

    # BalanceRatioを10に設定します。
    # Set Balance Ratio to 10.
    # For that selector is now Red, the value modified is the Red balance ratio rather than other color.
    float_balance_ratio.value = 10
    
    # 青の比率にアクセスするため、BalanceRatioSelectorをBlueに設定します。
    # Switch BalanceRatioSelector to Blue for accessing Blue ratio value
    enum_balance_ratio_selector.from_string("Blue")

    # BalanceRatioを10に設定します。
    # Set Balance Ratio to 10.
    # For that selector is now set to Blue, the ablue modified is the Blue balance ratio rather than other color.
    float_balance_ratio.value = 10
    
    # 緑の比率にアクセスするため、BalanceRatioSelectorをGreenに設定します。
    # Switch BalanceRatioSelector to Green for accessing Green ratio value
    enum_balance_ratio_selector.from_string("Green")

    # BalanceRatioを10に設定します。
    # Set Balance Ratio to 10.
    # For that selector is now set to Green, the ablue modified is the Green balance ratio rather than other color.
    float_balance_ratio.value = 10
    
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
