//-----------------------------------------------------------------------------
/**
 *			Wi-Fi広場入場許可確認データ構造体
 */
//-----------------------------------------------------------------------------


//-------------------------------------
/// 人物の名前の長さ（自分も含む）
//=====================================
#define PERSON_NAME_SIZE	(7)	

//-------------------------------------
/// 終了コードの長さ
//=====================================
#define	EOM_SIZE			(1)	

//-------------------------------------
///	手持ちポケモンの最大数
//=====================================
#define TEMOTI_POKEMAX	(6)


//-------------------------------------
///	行ったことの履歴　保持数
//=====================================
#define WFLBY_LASTACT_BUFFNUM	( 12 )


//-------------------------------------
///	ユーザがプロフィール用に選択する属性タイプ
//=====================================
#define WFLBY_SELECT_TYPENUM		(2)								// 選択数



//-------------------------------------
///	時間		4byte
//=====================================
typedef struct {
	union{
		u32 time;
		struct{
			u8		hour;
			u8		minute;
			u8		second;
			u8		dummy;
		};
	};
} WFLBY_TIME;

//-------------------------------------
///	文字コード		2byte
//=====================================
typedef u16		STRCODE;


//-------------------------------------
///	アンケート保持ワーク
//=====================================
typedef struct {
	u32	anketo_no;		// アンケート通しナンバー
	u32	select;			// 選択ナンバー
} WFLBY_ANKETO;


//-------------------------------------
///	ユーザ基本情報
//=====================================
typedef struct _WFLBY_USER_PROFILE{
	s32				userid;						//  ロビー内ユーザID
	u32				trainerid;					// トレーナーID					
	STRCODE			name[PERSON_NAME_SIZE + EOM_SIZE];	// ユーザ名
	WFLBY_TIME		intime;						// 入室時間
	WFLBY_TIME		wldtime;					// 自分の国のGMT時間
	u16				monsno[ TEMOTI_POKEMAX ];	// 手持ちポケモン
	u8				formid[ TEMOTI_POKEMAX ];	// ポケモンのフォルムデータ
	u8				tamago[ TEMOTI_POKEMAX ];	// 卵フラグ	
	u8				sex;						// 性別
	u8				region_code;				// 言語コード LANG_JAPANなどなど
	u16				tr_type;					// トレーナの見た目
	u16				nation;						// 国コード
	u8				area;						// 地域コード
	u8				zukan_zenkoku;				// 全国図鑑保持フラグ
	u8				game_clear;					// ゲームクリアフラグ
	u8				item;						// タッチトイ
	u8				rom_code;					// ロムバージョン	VERSION_PLATINUMとか
	u8				status;						// プレイヤーステータス
	s64				start_sec;					// 冒険を始めた日時
	u8				last_action[WFLBY_LASTACT_BUFFNUM];	// 最後に行ったこと　WFLBY_LASTACTION_TYPEが入ります。
	s32				last_action_userid[WFLBY_LASTACT_BUFFNUM];	// 最後に行ったこと　人物との接触時にuseridが設定されるバッファ
	u16				waza_type[WFLBY_SELECT_TYPENUM];	// 選択したユーザの属性タイプ
	WFLBY_ANKETO	anketo;						// アンケート選択データ


	// プラチナ以後のシステムはここに追加していく
} WFLBY_USER_PROFILE;

