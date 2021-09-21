REM グラフィッカーが関連づけ設定をされているファイルもコンバートされる
REM cellを使用しないBGはオプション指定(-bg)をつける
REM
REM バトルステージ***********************************
REM nscを指定すると、ncgも合わせてコンバートされる

del *.N*R

g2dcvtr bs_stage_01_a.nsc
g2dcvtr bs_stage_02_a.nsc
g2dcvtr bs_stage_alpha.nsc
g2dcvtr bs_stage_alpha_a.nsc
g2dcvtr bs_select.nsc
g2dcvtr bs_stage_rouka.nsc
REM
REM nceを指定すると、ncg,nclも合わせてコンバートされる
REM g2dcvtr bs_stage.ncg 
g2dcvtr bs_stage.ncl
g2dcvtr bs_stage_alpha.ncl
g2dcvtr bs_select.ncl
g2dcvtr bs_stage_rouka.ncl

REM バトルキャッスル***********************************
g2dcvtr bc_ohma.nsc
g2dcvtr bc_ohma.ncl
g2dcvtr bc_rouka.nsc
g2dcvtr bc_rouka.ncl
g2dcvtr bc_room.nsc
g2dcvtr bc_room01.nsc
g2dcvtr bc_room01.ncl
g2dcvtr bc_room.ncl
g2dcvtr bc_room_a.nsc
g2dcvtr bc_temochi01.nsc
g2dcvtr bc_temochi02.nsc
g2dcvtr bc_temochi_item.nsc
g2dcvtr bc_temochi_status.nsc
g2dcvtr bc_temochi_waza.nsc
g2dcvtr bc_temochi.ncl
g2dcvtr bc_taisen.ncl
g2dcvtr bc_taisen01.nsc
g2dcvtr bc_taisen02.nsc
g2dcvtr bc_taisen_status.nsc
g2dcvtr bc_taisen_waza.nsc

REM バトルタワー***********************************
g2dcvtr bt_mulbattle.nsc
g2dcvtr bt_mulway.nsc
g2dcvtr bt_mulway_a.nsc
g2dcvtr bt_sbattle.nsc
g2dcvtr bt_way.nsc
g2dcvtr bt_way_a.nsc
REM g2dcvtr bt.ncg
g2dcvtr bt.ncl

REM バトルファクトリー***********************************
REM g2dcvtr bf_room.ncg
g2dcvtr bf_room_battle.nsc
g2dcvtr bf_room_exchange.nsc
g2dcvtr bf_room_rail.nsc
REM g2dcvtr bf_rental.ncg
g2dcvtr bf_rental_back.nsc
g2dcvtr bf_rental_rail.nsc
g2dcvtr bf_rental_maru.nsc
g2dcvtr bf_rental.nsc
g2dcvtr bf_rental2.nsc
g2dcvtr bf_rental_3poke.nsc
g2dcvtr bf_rental_multi.nsc
g2dcvtr bf_rental_multi_2poke.nsc
g2dcvtr bf_rental_wifimode.nsc
g2dcvtr bf_room.ncl
g2dcvtr bf_rental.ncl

REM バトルルーレット***********************************
REM 対戦部屋
g2dcvtr br_room.nsc
g2dcvtr br_room_mul.nsc
g2dcvtr br_rouka.nsc
REM g2dcvtr br_room.ncg
REM g2dcvtr br_rouka.ncg
g2dcvtr br_room.ncl
g2dcvtr br_rouka.ncl
REM 対戦部屋の結果パネル
g2dcvtr br_kekka01_hp_a.nsc
g2dcvtr br_kekka01_hp_b.nsc
g2dcvtr br_kekka02_doku_a.nsc
g2dcvtr br_kekka02_doku_b.nsc
g2dcvtr br_kekka03_mahi_a.nsc
g2dcvtr br_kekka03_mahi_b.nsc
g2dcvtr br_kekka04_yakedoi_a.nsc
g2dcvtr br_kekka04_yakedoi_b.nsc
g2dcvtr br_kekka05_nemuri_a.nsc
g2dcvtr br_kekka05_nemuri_b.nsc
g2dcvtr br_kekka06_koori_a.nsc
g2dcvtr br_kekka06_koori_b.nsc
g2dcvtr br_kekka07_kinomi_a.nsc
g2dcvtr br_kekka07_kinomi_b.nsc
g2dcvtr br_kekka08_dougu_a.nsc
g2dcvtr br_kekka08_dougu_b.nsc
g2dcvtr br_kekka09_level_a.nsc
g2dcvtr br_kekka09_level_b.nsc
g2dcvtr br_kekka10_hare.nsc
g2dcvtr br_kekka11_ame.nsc
g2dcvtr br_kekka12_suna.nsc
g2dcvtr br_kekka13_arare.nsc
g2dcvtr br_kekka14_kiri.nsc
g2dcvtr br_kekka15_trick.nsc
g2dcvtr br_kekka16_change.nsc
g2dcvtr br_kekka17_speedup.nsc
g2dcvtr br_kekka18_speeddn.nsc
g2dcvtr br_kekka19_random.nsc
g2dcvtr br_kekka20_through.nsc
g2dcvtr br_kekka21_bp.nsc
g2dcvtr br_kekka22_suka.nsc
g2dcvtr br_kekka23_bp2.nsc
REM ルーレット画面
g2dcvtr br_panel.nsc
g2dcvtr br_panel2.nsc
g2dcvtr br_panel_mul.nsc
g2dcvtr br_panel_mul2.nsc
g2dcvtr br_panel_alpha.nsc
g2dcvtr br_panel_alpha_mul.nsc
g2dcvtr br_panel_alpha2.nsc
g2dcvtr br_panel_alpha2_mul.nsc
REM g2dcvtr br_panel.ncg
g2dcvtr br_panel.ncl
REM した画面分けた
g2dcvtr br_button_bg.nsc
g2dcvtr br_button_bg.ncl
g2dcvtr br_room_kekka.ncl
g2dcvtr br_room_kekka01_hp.ncl
g2dcvtr br_room_kekka02_doku.ncl
g2dcvtr br_room_kekka03_mahi.ncl
g2dcvtr br_room_kekka04_yakedoi.ncl
g2dcvtr br_room_kekka05_nemuri.ncl
g2dcvtr br_room_kekka06_koori.ncl
g2dcvtr br_room_kekka07_kinomi.ncl
g2dcvtr br_room_kekka08_dougu.ncl
g2dcvtr br_room_kekka09_level.ncl
g2dcvtr br_room_kekka10_hare.ncl
g2dcvtr br_room_kekka11_ame.ncl
g2dcvtr br_room_kekka12_suna.ncl
g2dcvtr br_room_kekka13_arare.ncl
g2dcvtr br_room_kekka14_kiri.ncl
g2dcvtr br_room_kekka15_trick.ncl
g2dcvtr br_room_kekka16_change.ncl
g2dcvtr br_room_kekka17_speedup.ncl
g2dcvtr br_room_kekka18_speeddn.ncl
g2dcvtr br_room_kekka19_random.ncl
g2dcvtr br_room_kekka20_through.ncl
g2dcvtr br_room_kekka21_bp.ncl
g2dcvtr br_room_kekka22_suka.ncl
g2dcvtr br_room_kekka23_bp2.ncl

REM フロンティア成績***********************************
g2dcvtr seiseki.nsc
g2dcvtr seiseki.ncl

REM wifiカウンター***********************************
g2dcvtr bf_connect.nsc
g2dcvtr bf_connect.ncl

REM 下画面***********************************
g2dcvtr batt_frontier.nsc
g2dcvtr batt_frontier.ncl


REM .lstに追加する時は最後に改行を入れるとエラーになるので注意！


