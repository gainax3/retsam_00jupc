//******************************************************************************
/**
 * 
 * @file	field_tornworld_def.h
 * @brief	破れた世界　define ヘッダー スクリプト参照有り
 * @author	kagaya
 * @data	05.07.20
 */
//******************************************************************************
#ifndef FLD_TORNWORLD_DEF_H
#define FLD_TORNWORLD_DEF_H

//--------------------------------------------------------------
///	SYS_WORK_TW_EVSEQ。
///	TWEVSEQ		TWEVSEQ_階層_イベント番号
//--------------------------------------------------------------
///シロナ破れた世界解説開始。解説終了後、次へ
#define TWEVSEQ_01_00 (0)
///シロナ移動床解説。解説後 次へ
#define TWEVSEQ_01_01	(1)
///シロナ床移動解説後。一層リフト移動イベント終了で次へ
#define TWEVSEQ_01_02	(2)
///エムリット誘導イベント。　誘導イベント終了で次へ
#define TWEVSEQ_02_00	(3)
///三層シロナ壁面会話。　会話終了後で次へ
#define TWEVSEQ_03_00	(4)
///三層シロナ壁面会話終了後。四層アカギ登場イベント発生で次へ。
#define TWEVSEQ_03_01	(5)
///四層アカギ登場イベント終了。シロナストッパー状態。岩落とし完了後次へ
#define TWEVSEQ_04_00	(6)
///七層岩落とし完了後のシロナ移動後。八層に移動すると次へ
#define TWEVSEQ_07_00	(7)
///シロナと共に八層到達。シロナ、アカギ会話後次へ
#define TWEVSEQ_08_00	(8)
///八層、シロナ、アカギ会話後。アカギ戦闘後に次へ
#define TWEVSEQ_08_01	(9)
///八層　アカギ戦闘後。ギラ影その１発生後に次へ
#define TWEVSEQ_08_02	(10)
///八層深部　ギラ影その１発生後　ギラ影その２発生後に次へ
#define TWEVSEQ_08D_00	(11)
///八層深部　ギラ影その２発生後　ギラ落下登場後に次へ
#define	TWEVSEQ_08D_01	(12)
///八層深部　ギラ落下登場後　ギラティナ戦闘終了後に次へ
#define TWEVSEQ_08D_02	(13)
///八層深部　ギラティナ戦闘終了後。
#define TWEVSEQ_08D_03	(14)

///最大,無条件
#define TWEVSEQ_ANY		(15)

///破れた世界をクリアした
#define TWEVSEQ_CLEAR	(TWEVSEQ_08D_03)

//--------------------------------------------------------------
///	破れた世界 OBJID基準
//--------------------------------------------------------------
#define TWOBJID					(0x80)

//--------------------------------------------------------------
///	第一層OBJID
//--------------------------------------------------------------
#define TW_OBJID_0101_SIRONA_00	(TWOBJID+0)	//シロナイベント用
#define TW_OBJID_0101_SIRONA_01	(TWOBJID+1)	//シロナリフト配置

//--------------------------------------------------------------
///	第２階層OBJID
//--------------------------------------------------------------
#define TW_OBJID_0102_SIRONA	(TWOBJID+0)	//シロナリフトイベント用
#define TW_OBJID_0102_SPPOKE2	(TWOBJID+1)	//エムリットイベント

//--------------------------------------------------------------
///	第三階層OBJID
//--------------------------------------------------------------
#define TW_OBJID_0103_SIRONA	(TWOBJID+0)	//シロナ壁案内

//--------------------------------------------------------------
///	第四階層OBJID
//--------------------------------------------------------------
#define TW_OBJID_0104_AKAGI		(TWOBJID+0)	//アカギ

//--------------------------------------------------------------
///	第五階層OBJID
//--------------------------------------------------------------
#define TW_OBJID_0105_AKAGI		(TWOBJID+6)	//アカギ(4,6層OBJと被らない様に

//--------------------------------------------------------------
///	第六階層OBJID
//--------------------------------------------------------------
#define TW_OBJID_0107_ROCK_0	(TWOBJID+0)	//落下岩0
#define TW_OBJID_0107_ROCK_1	(TWOBJID+1)	//落下岩1
#define TW_OBJID_0107_ROCK_2	(TWOBJID+2)	//落下岩2
#define TW_OBJID_0107_YUKUSHI	(TWOBJID+3)	//ユクシー
#define TW_OBJID_0107_AGUNOMU	(TWOBJID+4)	//アグノム
#define TW_OBJID_0107_EMURIT	(TWOBJID+5)	//エムリット

//--------------------------------------------------------------
///	第七階層OBJID
//--------------------------------------------------------------
#define TW_OBJID_0108_ROCK_0	(TWOBJID+0)	//落下岩0
#define TW_OBJID_0108_ROCK_1	(TWOBJID+1)	//落下岩1
#define TW_OBJID_0108_ROCK_2	(TWOBJID+2)	//落下岩2
#define TW_OBJID_0108_EM		(TWOBJID+3)	//エムリット
#define TW_OBJID_0108_YUKU		(TWOBJID+4)	//ユクシー
#define TW_OBJID_0108_AGU		(TWOBJID+5)	//アグノム
#define TW_OBJID_0108_SIRONA	(TWOBJID+6)	//シロナ

#define TW_OBJID_0108_HOLEMSG0	(TWOBJID+7)	//岩落としメッセージOBJ
#define TW_OBJID_0108_HOLEMSG1	(TWOBJID+8)	//岩落としメッセージOBJ
#define TW_OBJID_0108_HOLEMSG2	(TWOBJID+9)	//岩落としメッセージOBJ
#define TW_OBJID_0108_HOLEMSG3	(TWOBJID+10)	//岩落としメッセージOBJ
#define TW_OBJID_0108_HOLEMSG4	(TWOBJID+11)	//岩落としメッセージOBJ
#define TW_OBJID_0108_HOLEMSG5	(TWOBJID+12)	//岩落としメッセージOBJ
#define TW_OBJID_0108_HOLEMSG6	(TWOBJID+13)	//岩落としメッセージOBJ
#define TW_OBJID_0108_HOLEMSG7	(TWOBJID+14)	//岩落としメッセージOBJ
#define TW_OBJID_0108_HOLEMSG8	(TWOBJID+15)	//岩落としメッセージOBJ

#define TW_OBJID_0108_ROCK_OK_0	(TWOBJID+16)	//落下済み岩0
#define TW_OBJID_0108_ROCK_OK_1	(TWOBJID+17)	//落下済み岩1
#define TW_OBJID_0108_ROCK_OK_2	(TWOBJID+18)	//落下済み岩2

//--------------------------------------------------------------
///	第八階層OBJID
//--------------------------------------------------------------
#define TW_OBJID_0109_SIRONA	(TWOBJID+0) //シロナ
#define TW_OBJID_0109_AKAGI		(TWOBJID+1)	//アカギ

//--------------------------------------------------------------
///	第八階層深部OBJID
//--------------------------------------------------------------
#define TW_OBJID_0110_GIRA		(TWOBJID+0)	//ギラティナ
#define TW_OBJID_0110_SIRONA	(TWOBJID+1)
#define TW_OBJID_0110_AKAGI		(TWOBJID+2)
#define TW_OBJID_0110_HOLE		(TWOBJID+3)
#define TW_OBJID_0110_NONSIRONA (TWOBJID+4)

//--------------------------------------------------------------
///	第八階層その２OBJID
//--------------------------------------------------------------
#define TW_OBJID_0111_HOLE		(TWOBJID+0)
#define TW_OBJID_0111_ITEM		(TWOBJID+1)

//--------------------------------------------------------------
///	ギラティナ影番号
//--------------------------------------------------------------
#define TW_GIRASNO_0101_00		(0)
#define TW_GIRASNO_MAX			(1)

#endif	//FLD_TORNWORLD_DEF_H
