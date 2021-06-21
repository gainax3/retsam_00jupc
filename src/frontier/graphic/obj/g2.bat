REM グラフィッカーが関連づけ設定をされているファイルもコンバートされる
REM cellを使用しないBGはオプション指定(-bg)をつける
REM
REM nscを指定すると、ncgも合わせてコンバートされる
REM nceを指定すると、ncg,nclも合わせてコンバートされる
REM 
REM -brオプションで、矩形領域情報を持つセル情報を出力します(necにはつけておく)
REM バトルステージ**************************************

del *.N*R

g2dcvtr bs_select_cursor.nce -br

REM バトルキャッスル**************************************
g2dcvtr bc_obj.nce -br

REM バトルファクトリー**************************************
g2dcvtr bf_rental_oam.nce -br
g2dcvtr bf_rental_obj.nce -br
g2dcvtr bf_rental_obj_start.nce -br
g2dcvtr bf_rental_obj_3poke.nce -br
g2dcvtr bf_rental_obj_multi.nce -br

REM バトルルーレット**************************************
g2dcvtr br_obj.nce -br
g2dcvtr br_button.nce -br
g2dcvtr br_ramp.nce -br
g2dcvtr br_pokepanel.nce -br
g2dcvtr br_pokepanel2.nce -br

REM バトルタワー**************************************
g2dcvtr bt_obj_door.nce -br

REM 関係ないが最初に全てdelしてるのでコンバート対象に入れておく************************
REM g2dcvtr hero_ine_kage.nce -br
REM g2dcvtr pl_boy01.ncl -br



REM .lstに追加する時は最後に改行を入れるとエラーになるので注意！


