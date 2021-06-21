//=============================================================================================
/**
 * @file	scr_boxpwd.c
 * @brief	ボックスパスワード壁紙関連処理
 * @author	taya
 * @date	2006/06/01
 */
//=============================================================================================
#include "common.h"
#include "system\pms_word.h"
#include "savedata\mystatus.h"
#include "application\pms_input_data.h"

#include "scr_boxpwd.h"

enum {
	EX_WALLPAPER_MAX = 8,
};

///固定値
#define BOXPWD_MAGIC_NUMBER		(6)


static void lotate( u8* ary, u8 targetNum, u8 lotNum )
{
	u8 lsb, i;

	while(lotNum--)
	{
		lsb = (ary[0] >> 7) & 1;
		for(i=0; i<(targetNum-1); i++)
		{
			ary[i] <<= 1;
			ary[i] |= ((ary[i+1] >> 7) & 1);
		}
		ary[i] <<= 1;
		ary[i] |= lsb;
	}
}

static void lotate_reverse( u8* ary, u8 targetNum, u8 lotNum )
{
	u8 lsb;
	int i;

	while(lotNum--)
	{
		lsb = ary[targetNum-1] & 1;
		for(i=targetNum-1; i>0; i--)
		{
			ary[i] >>= 1;
			ary[i] |= ((ary[i-1] & 1) << 7);
		}
		ary[i] >>= 1;
		ary[i] |= lsb << 7;
	}
}


static int hitcheck( const PMSW_AIKOTOBA_TBL* cp_pmsw_tbl, u16 player_id, PMS_WORD w1, PMS_WORD w2, PMS_WORD w3, PMS_WORD w4 )
{
	int maxValue, v, i;
	s16 pwd[4];
	u8  bit[4];

	maxValue = PMSW_AIKOTOBATBL_GetTblNum( cp_pmsw_tbl );

	pwd[0] = PMSW_AIKOTOBATBL_GetWordIdx( cp_pmsw_tbl, w1 );
	pwd[1] = PMSW_AIKOTOBATBL_GetWordIdx( cp_pmsw_tbl, w2 );
	pwd[2] = PMSW_AIKOTOBATBL_GetWordIdx( cp_pmsw_tbl, w3 );
	pwd[3] = PMSW_AIKOTOBATBL_GetWordIdx( cp_pmsw_tbl, w4 );

	OS_TPrintf("pwdMax = %d\n", maxValue);
	OS_TPrintf("word1:%4d -> %3d\n", w1, pwd[0]);
	OS_TPrintf("word2:%4d -> %3d\n", w2, pwd[1]);
	OS_TPrintf("word3:%4d -> %3d\n", w3, pwd[2]);
	OS_TPrintf("word4:%4d -> %3d\n", w4, pwd[3]);

	for(i=0; i<4; i++)
	{
		if( pwd[i] < 0 )
		{
			return BOXPWD_RESULT_ERROR;
		}

		if( i > 0 )
		{
			if( pwd[i] >= pwd[i-1] )
			{
				v = pwd[i] - pwd[i-1];
				if( v > 255 )
				{
					return BOXPWD_RESULT_ERROR;
				}
				bit[i] = v;
			}
			else
			{
				v = maxValue - (pwd[i-1] - pwd[i]);
				if( v > 255)
				{
					return BOXPWD_RESULT_ERROR;
				}
				bit[i] = v;
			}
		}
		else
		{
			if( pwd[0] > 255 )
			{
				return BOXPWD_RESULT_ERROR;
			}
			bit[0] = pwd[0];
		}
	}

//	lotate( bit, 4, 7 );
	lotate_reverse( bit, 4, 5 );

	OS_TPrintf("random = %d\n", bit[3]);

	for(i=0; i<3; i++)
	{
		bit[i] ^= ((bit[3] >> 4) | (bit[3] & 0xf0));
	}

	lotate_reverse( bit, 3, (bit[3] & 0x0f) );


	if( (bit[0] & 0x0f) >= EX_WALLPAPER_MAX )
	{
		return BOXPWD_RESULT_ERROR;
	}

	bit[1] ^= bit[0];
	bit[2] ^= bit[0];

	OS_TPrintf("id = %d\n", ((bit[1]<<8) | (bit[2])));

	if( (((bit[1]<<8) | (bit[2])) == player_id)		//プレイヤーID一致
		&& (((bit[0] & 0xf0)>>4) == BOXPWD_MAGIC_NUMBER)	//固定値一致
		&& (bit[3] == (((bit[0]+bit[1])*bit[2]) & 0xff)))	//チェックサム一致
	{
		OS_TPrintf("ok wp= %d\n", bit[0] & 0x0f );
		return bit[0] & 0x0f;
	}


	return BOXPWD_RESULT_ERROR;
}




//------------------------------------------------------------------
/**
 * ボックス隠し壁紙のパスワードチェック
 *
 * @param   mystatus	プレイヤーステータス（ID参照用）
 * @param   word1		入力された単語１
 * @param   word2		入力された単語２
 * @param   word3		入力された単語３
 * @param   word4		入力された単語４
 * @param	heapID		ヒープID
 *
 * @retval  int			パスワードが正しければ、壁紙ID（0～EX_WALLPAPER_MAX-1)
 *						パスワードが間違いなら、BOXPWD_RESULT_ERROR
 */
//------------------------------------------------------------------
int  BOXPWD_HitCheck( const MYSTATUS* mystatus, PMS_WORD word1, PMS_WORD word2, PMS_WORD word3, PMS_WORD word4, u32 heapID )
{
	int wp_num;
	u16  player_id;
	PMSW_AIKOTOBA_TBL* p_pmsw_tbl;

	// 合言葉用、単語リストテーブル取得
	p_pmsw_tbl = PMSW_AIKOTOBATBL_Init( heapID );

	player_id = MyStatus_GetID_Low(mystatus);

	wp_num = hitcheck( 
				p_pmsw_tbl,
				player_id,
				word1,
				word2,
				word3,
				word4 );

	PMSW_AIKOTOBATBL_Delete( p_pmsw_tbl );

	return wp_num;
}
//c01r0203

