
//============================================================================
/**
 *
 *@file		sub_295.s
 *@brief	戦闘シーケンス
 *			フィールド天候でトリックルーム
 *@author	HisashiSogabe
 *@data		2007.09.11
 *
 */
//============================================================================
	.text

	.include	"waza_seq_def.h"

SUB_295:
	VALUE			VAL_SET,BUF_PARA_WAZA_WORK,WAZANO_TORIKKURUUMU
	WAZA_EFFECT		SIDE_WORK
	SERVER_WAIT
	//エフェクトカウントを元に戻す
	VALUE			VAL_SET,BUF_PARA_WAZA_EFF_CNT,0
	//技エフェクトを起動するようにフラグを落とす
	VALUE			VAL_NBIT,BUF_PARA_SERVER_STATUS_FLAG,SERVER_STATUS_FLAG_NO_WAZA_EFFECT
	MESSAGE			FieldTrickRoomMsg,TAG_NONE
	SERVER_WAIT
	WAIT			MSG_WAIT
	VALUE			VAL_BIT,BUF_PARA_FIELD_CONDITION,TRICKROOM_COUNT<<FIELD_TRICKROOM_SHIFT
	SEQ_END
