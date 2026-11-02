"""
grab_Parameter_SaveToUserSet1.py

Save the current camera settings to UserSet1 and set UserSet1 as the startup default.
Warning: This sample changes persistent camera settings. Do not run it during acquisition.
"""

import stapipy as st

try:
    st.initialize()

    st_system = st.create_system()

    st_device = st_system.create_first_device()

    print('Device=', st_device.info.display_name)

    # ============================================================
    # 現在の設定をUserSet1に保存し、電源ON時のデフォルトに設定するデモ
    # Demonstration of saving the current settings to UserSet1 and setting it as the startup default.
    # UserSetの保存は、カメラが取得中でない場合にだけ実行してください。
    # Save a UserSet only while the camera is not acquiring.

    # パラメータにアクセスするためのノードマップを取得
    # Get the NodeMap for accessing camera parameters.
    nodemap_remote = st_device.remote_port.nodemap

    # 保存先のUserSet1を選択
    # Select UserSet1 as the destination.
    user_set_selector = nodemap_remote.get_node("UserSetSelector")
    enum_user_set_selector = st.PyIEnumeration(user_set_selector)
    enum_user_set_selector.from_string("UserSet1")

    # 現在の設定を選択したUserSetへ保存
    # Save the current settings to the selected UserSet.
    user_set_save = nodemap_remote.get_node("UserSetSave")
    command_user_set_save = st.PyICommand(user_set_save)
    command_user_set_save.execute()
    print("Save current settings to UserSet1 succeeded.")

    # 電源ON時にUserSet1を使用するよう設定
    # Set UserSet1 as the settings loaded at power on.
    user_set_default = nodemap_remote.get_node("UserSetDefault")
    enum_user_set_default = st.PyIEnumeration(user_set_default)
    enum_user_set_default.from_string("UserSet1")
    print("Set UserSetDefault to UserSet1 succeeded.")

    # ============================================================

except Exception as exception:
    print(exception)
