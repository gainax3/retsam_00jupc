■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
■　Readme-NITRO-System-Library-071126-patch1.txt 　　　　　　　　　　　　　■
■　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　■
■　NITRO-System-Library 071126 patch1　　　　　　　　　　　　　　　　　　　■
■　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　■
■　Jan. 23, 2008 　　　　　　　　　　　　　　　　　　　　　　　　　　　　　■
■　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　■
■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■


目次
==============================================================================
　　1. はじめに
　　2. 修正内容
　　3. ファイルリスト


1. はじめに
==============================================================================
　　このパッチは、NITRO-System Library 2007/11/26 版のリリース以降に発見された
　　不具合の修正と、NITRO-SDK4.2への対応を行うためのものです。
　　（patch-NitroSDK4_2での修正点を含みます。）

　　このパッチをインストールするためには、同梱されているファイルをNITRO-System
　　 Library 2007/11/26 版がインストールされているディレクトリに上書きコピーし
　　てください。


2. 修正内容
===============================================================================

　　NITRO-System-Library 071126 patch1 で修正された内容
　　---------------------------------------------------------------------------

　　(1) g3d: アニメーションが無効化されない不具合を修正

　　　　・NNS_G3dAnmObjDisableID()関数でマテリアルアニメーション、またはビジビ
　　　　　リティアニメーションを指定してもアニメーションが無効にならない不具合
　　　　　を修正しました。


　　(2) g3d: DMA 転送を使用する関数について各所修正

　　　　・NNS_G3dTexLoad(), NNS_G3dPlttLoad(), NNS_G3dDraw(), 
　　　　　NNS_G3dDraw1Mat1Shp(), NNS_G3dResDefaultSetup() 関数について、関数内
　　　　　部でDMA 転送を用いているため、呼び出し前に転送データをキャッシュから
　　　　　メモリにライトバックしておく必要がある旨を関数リファレンスマニュアル
　　　　　に追記しました。

　　　　・上記に関連し、各種サンプルデモを修正しました。


　　(3) g3d: demolib におけるカメラ計算の不具合を修正

　　　　・demolib の G3DDemo_CalcCamera() 関数について、カメラ位置の計算方法に
　　　　　誤りがあったので修正しました。


　　(4) g3dcvtr: SBC コマンドの一部を誤って削除してしまう不具合を修正

　　(5) g3d: リファレンスマニュアルを修正


3. ファイルリスト
===============================================================================

　　■ソースファイル
　　　NitroSystem/build/libraries/g3d/src/anm.c
　　　NitroSystem/build/libraries/g3d/src/util.c
　　　NitroSystem/include/nnsys/version.h


　　■ライブラリファイル
　　　NitroSystem/lib/ARM9-TS/Debug/libnnsfnd.a
　　　NitroSystem/lib/ARM9-TS/Debug/libnnsfnd.thumb.a
　　　NitroSystem/lib/ARM9-TS/Debug/libnnsg2d.a
　　　NitroSystem/lib/ARM9-TS/Debug/libnnsg2d.thumb.a
　　　NitroSystem/lib/ARM9-TS/Debug/libnnsg3d.a
　　　NitroSystem/lib/ARM9-TS/Debug/libnnsg3d.thumb.a
　　　NitroSystem/lib/ARM9-TS/Debug/libnnsgfd.a
　　　NitroSystem/lib/ARM9-TS/Debug/libnnsgfd.thumb.a
　　　NitroSystem/lib/ARM9-TS/Debug/libnnsmcs.a
　　　NitroSystem/lib/ARM9-TS/Debug/libnnsmcs.thumb.a
　　　NitroSystem/lib/ARM9-TS/Debug/libnnssnd.a
　　　NitroSystem/lib/ARM9-TS/Debug/libnnssnd.thumb.a

　　　NitroSystem/lib/ARM9-TS/Release/libnnsfnd.a
　　　NitroSystem/lib/ARM9-TS/Release/libnnsfnd.thumb.a
　　　NitroSystem/lib/ARM9-TS/Release/libnnsg2d.a
　　　NitroSystem/lib/ARM9-TS/Release/libnnsg2d.thumb.a
　　　NitroSystem/lib/ARM9-TS/Release/libnnsg3d.a
　　　NitroSystem/lib/ARM9-TS/Release/libnnsg3d.thumb.a
　　　NitroSystem/lib/ARM9-TS/Release/libnnsgfd.a
　　　NitroSystem/lib/ARM9-TS/Release/libnnsgfd.thumb.a
　　　NitroSystem/lib/ARM9-TS/Release/libnnsmcs.a
　　　NitroSystem/lib/ARM9-TS/Release/libnnsmcs.thumb.a
　　　NitroSystem/lib/ARM9-TS/Release/libnnssnd.a
　　　NitroSystem/lib/ARM9-TS/Release/libnnssnd.thumb.a

　　　NitroSystem/lib/ARM9-TS/Rom/libnnsfnd.a
　　　NitroSystem/lib/ARM9-TS/Rom/libnnsfnd.thumb.a
　　　NitroSystem/lib/ARM9-TS/Rom/libnnsg2d.a
　　　NitroSystem/lib/ARM9-TS/Rom/libnnsg2d.thumb.a
　　　NitroSystem/lib/ARM9-TS/Rom/libnnsg3d.a
　　　NitroSystem/lib/ARM9-TS/Rom/libnnsg3d.thumb.a
　　　NitroSystem/lib/ARM9-TS/Rom/libnnsgfd.a
　　　NitroSystem/lib/ARM9-TS/Rom/libnnsgfd.thumb.a
　　　NitroSystem/lib/ARM9-TS/Rom/libnnsmcs.a
　　　NitroSystem/lib/ARM9-TS/Rom/libnnsmcs.thumb.a
　　　NitroSystem/lib/ARM9-TS/Rom/libnnssnd.a
　　　NitroSystem/lib/ARM9-TS/Rom/libnnssnd.thumb.a



　　■サンプル
　　　NitroSystem/build/demos/g3d/demolib/src/camera.c
　　　NitroSystem/build/demos/g3d/samples/1mat1shp/src/main.c
　　　NitroSystem/build/demos/g3d/samples/callback1/src/main.c
　　　NitroSystem/build/demos/g3d/samples/callback2/src/main.c
　　　NitroSystem/build/demos/g3d/samples/callback3/src/main.c
　　　NitroSystem/build/demos/g3d/samples/callback4/src/main.c
　　　NitroSystem/build/demos/g3d/samples/callback5/src/main.c
　　　NitroSystem/build/demos/g3d/samples/CameraMtx/src/main.c
　　　NitroSystem/build/demos/g3d/samples/CustomizeSbcOp/src/main.c
　　　NitroSystem/build/demos/g3d/samples/DisposeTex/src/main.c
　　　NitroSystem/build/demos/g3d/samples/Envelope/src/main.c
　　　NitroSystem/build/demos/g3d/samples/EnvMap/src/main.c
　　　NitroSystem/build/demos/g3d/samples/GeComBuffering/src/main.c
　　　NitroSystem/build/demos/g3d/samples/JointAnm/src/main.c
　　　NitroSystem/build/demos/g3d/samples/ManualSetup/src/main.c
　　　NitroSystem/build/demos/g3d/samples/MatColAnm/src/main.c
　　　NitroSystem/build/demos/g3d/samples/ModifyNsbmd/src/main.c
　　　NitroSystem/build/demos/g3d/samples/MotionLOD/src/main.c
　　　NitroSystem/build/demos/g3d/samples/MultiModel/src/main.c
　　　NitroSystem/build/demos/g3d/samples/PartialAnm1/src/main.c
　　　NitroSystem/build/demos/g3d/samples/PartialAnm2/src/main.c
　　　NitroSystem/build/demos/g3d/samples/ProjMap/src/main.c
　　　NitroSystem/build/demos/g3d/samples/RecordJoint/src/main.c
　　　NitroSystem/build/demos/g3d/samples/RecordMaterial/src/main.c
　　　NitroSystem/build/demos/g3d/samples/RecordMtx/src/main.c
　　　NitroSystem/build/demos/g3d/samples/ScreenUtil/src/main.c
　　　NitroSystem/build/demos/g3d/samples/SeparateTex/src/main.c
　　　NitroSystem/build/demos/g3d/samples/ShadowVolume/src/main.c
　　　NitroSystem/build/demos/g3d/samples/SharedMotion/src/main.c
　　　NitroSystem/build/demos/g3d/samples/simple/src/main.c
　　　NitroSystem/build/demos/g3d/samples/SlowMotion/src/main.c
　　　NitroSystem/build/demos/g3d/samples/TexPatternAnm/src/main.c
　　　NitroSystem/build/demos/g3d/samples/TexSRTAnm/src/main.c
　　　NitroSystem/build/demos/g3d/samples/Translucent/src/main.c
　　　NitroSystem/build/demos/g3d/samples/UnbindTex/src/main.c
　　　NitroSystem/build/demos/g3d/samples/VisibilityAnm/src/main.c


　　■リファレンス
　　　NitroSystem/man/ja_JP/main.html
　　　NitroSystem/man/ja_JP/g3d/list_g3d.html
　　　NitroSystem/man/ja_JP/g3d/kernel/NNS_G3dPlttLoad.html
　　　NitroSystem/man/ja_JP/g3d/kernel/NNS_G3dTexLoad.html
　　　NitroSystem/man/ja_JP/g3d/sbc/NNS_G3dDraw.html
　　　NitroSystem/man/ja_JP/g3d/sbc/NNS_G3dDraw1Mat1Shp.html
　　　NitroSystem/man/ja_JP/g3d/util/NNS_G3dResDefaultSetup.html


　　■サウンドプレイヤ用ファイル
　　　NitroSystem/tools/nitro/SoundPlayer/components/ARM7-TS/Debug/sub.nef
　　　NitroSystem/tools/nitro/SoundPlayer/components/ARM7-TS/Debug/sub.sbin
　　　NitroSystem/tools/nitro/SoundPlayer/components/ARM7-TS/Debug/sub_defs.sbin
　　　NitroSystem/tools/nitro/SoundPlayer/components/ARM7-TS/Release/sub.nef
　　　NitroSystem/tools/nitro/SoundPlayer/components/ARM7-TS/Release/sub.sbin
　　　NitroSystem/tools/nitro/SoundPlayer/components/ARM7-TS/Release/sub_defs.sbin
　　　NitroSystem/tools/nitro/SoundPlayer/components/ARM7-TS/Rom/sub.nef
　　　NitroSystem/tools/nitro/SoundPlayer/components/ARM7-TS/Rom/sub.sbin
　　　NitroSystem/tools/nitro/SoundPlayer/components/ARM7-TS/Rom/sub_defs.sbin
　　　NitroSystem/tools/nitro/SoundPlayer/components/ARM7-TS.thumb/Debug/sub.nef
　　　NitroSystem/tools/nitro/SoundPlayer/components/ARM7-TS.thumb/Debug/sub.sbin
　　　NitroSystem/tools/nitro/SoundPlayer/components/ARM7-TS.thumb/Debug/sub_defs.sbin
　　　NitroSystem/tools/nitro/SoundPlayer/components/ARM7-TS.thumb/Release/sub.nef
　　　NitroSystem/tools/nitro/SoundPlayer/components/ARM7-TS.thumb/Release/sub.sbin
　　　NitroSystem/tools/nitro/SoundPlayer/components/ARM7-TS.thumb/Release/sub_defs.sbin
　　　NitroSystem/tools/nitro/SoundPlayer/components/ARM7-TS.thumb/Rom/sub.nef
　　　NitroSystem/tools/nitro/SoundPlayer/components/ARM7-TS.thumb/Rom/sub.sbin
　　　NitroSystem/tools/nitro/SoundPlayer/components/ARM7-TS.thumb/Rom/sub_defs.sbin
　　　NitroSystem/tools/nitro/SoundPlayer/components/ARM9-TS/Debug/main.nef
　　　NitroSystem/tools/nitro/SoundPlayer/components/ARM9-TS/Debug/main.sbin
　　　NitroSystem/tools/nitro/SoundPlayer/components/ARM9-TS/Debug/main_defs.sbin
　　　NitroSystem/tools/nitro/SoundPlayer/components/ARM9-TS/Release/main.nef
　　　NitroSystem/tools/nitro/SoundPlayer/components/ARM9-TS/Release/main.sbin
　　　NitroSystem/tools/nitro/SoundPlayer/components/ARM9-TS/Release/main_defs.sbin
　　　NitroSystem/tools/nitro/SoundPlayer/components/ARM9-TS/Rom/main.nef
　　　NitroSystem/tools/nitro/SoundPlayer/components/ARM9-TS/Rom/main.sbin
　　　NitroSystem/tools/nitro/SoundPlayer/components/ARM9-TS/Rom/main_defs.sbin
　　　NitroSystem/tools/nitro/SoundPlayer/components/ARM9-TS.thumb/Debug/main.nef
　　　NitroSystem/tools/nitro/SoundPlayer/components/ARM9-TS.thumb/Debug/main.sbin
　　　NitroSystem/tools/nitro/SoundPlayer/components/ARM9-TS.thumb/Debug/main_defs.sbin
　　　NitroSystem/tools/nitro/SoundPlayer/components/ARM9-TS.thumb/Release/main.nef
　　　NitroSystem/tools/nitro/SoundPlayer/components/ARM9-TS.thumb/Release/main.sbin
　　　NitroSystem/tools/nitro/SoundPlayer/components/ARM9-TS.thumb/Release/main_defs.sbin
　　　NitroSystem/tools/nitro/SoundPlayer/components/ARM9-TS.thumb/Rom/main.nef
　　　NitroSystem/tools/nitro/SoundPlayer/components/ARM9-TS.thumb/Rom/main.sbin
　　　NitroSystem/tools/nitro/SoundPlayer/components/ARM9-TS.thumb/Rom/main_defs.sbin


　　■NITROプレイヤ用ファイル
　　　NitroSystem/tools/win/NitroPlayer/nitro/ARM9-TS/Debug/NitroPlayer.srl
　　　NitroSystem/tools/win/NitroPlayer/nitro/ARM9-TS/Release/NitroPlayer.srl
　　　NitroSystem/tools/win/NitroPlayer/nitro/ARM9-TS.thumb/Debug/NitroPlayer.srl
　　　NitroSystem/tools/win/NitroPlayer/nitro/ARM9-TS.thumb/Release/NitroPlayer.srl


　　■コンバータ
　　　NitroSystem/tools/win/bin/g3dcvtr.exe

