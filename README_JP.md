# Sentech SDK 単機能サンプル
これらのサンプルコードは、オムロンセンテックのGigE Vision、USB 3 Vision、およびCoaXPressカメラ用のSDK(Sentech SDK)用です。

各サンプルコードは、Sentech SDKを使用してGenICamのSFNC準拠のパラメータ調整を簡単に理解するためのカメラ専用の関数です。

詳しくはhttps://sentech.co.jp/ Sentech SDK packageのドキュメントをご覧ください。


(注) すべてのサンプルでパラメータの固定値が提供されているため、すべてのカメラで動作できず、設定値の変更が必要になる場合があります。

## オムロンセンテック製GigEVisionカメラ
オムロンセンテック製GigEカメラは2種類となります

 IEAシリーズ: 35 mm x 35 mm GigEカメラ、モジュール名のイニシャルは「S」となります
 
  例：STC-SCS241POE
  
 KWAシリーズ: 28 mm x 28 mm GigEカメラ、モジュール名のイニシャルは「M」となります
 
  例：STC-MCS500POE

Sentech SDKはこれら2つのカメラモジュールで動作しますが、IEAシリーズのカメラ機能にはKWAシリーズと異なる名前が付けられている場合があり、プログラムで処理する必要があります。

## オムロンセンテック製USB3Visionカメラ
USB3Vision (U3V) カメラには、U3V標準ベースのUSB3.0カメラではない「MB/MCA5MUSB3」は含まれていません。

一部のU3Vカメラは、リリースされたSFNCの時期が異なるため、機能の名前が異なる場合があります。

## オムロンセンテック製CoaXPressカメラ
CoaXPressカメラでは、GenTL producerにCoaXPressのグラバーが関わっています。
グラバーは少なくともSentech SDKで動くGenTL producerを提供しなければなりません。

## PythonでSentech SDK使用する
PythonでSentech SDKを使用するには、まずSentech SDKをインストールしてから、使用する環境のPythonバージョンに対応する.whlをインストールする必要があります。
詳細にてつ、Python対応の.whlファイルのダウンロードページ内のマニュアルまでご参照ください。

(注)

PythonのIDLE以外のIDEでは、ファイルやコンポーネントをインクルードまたはインポートするときに問題が発生することがあります。
詳細について、各IDEにライブラリインクルードについて確認して下さい。

## その他の実装
Sentech SDKでのその他の実装については、フルインストールのSDKパッケージに付属するヘルプファイルを参照してください。


**2021 オムロンセンテック技術サポートチーム
