REM awk -f snd_test_msg.awk pl_sound_data.sadl > tmp.txt
REM awk -f snd_test_id.awk tmp.txt > tmp2.txt
REM awk -f snd_test_seq.awk tmp.txt > tmp3.txt
ruby gmm_make.rb
copy snd_test_name.gmm c:\home\platinum\pokemon_pl\convert\message\src\debug
