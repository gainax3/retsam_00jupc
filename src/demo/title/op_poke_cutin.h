//============================================================================================
/**
 * @file	op_poke_cutin.h
 * @brief	オープニングポケモンカットイン
 * @author	Nozomu Saito
 * @date	2006.06.03
 */
//============================================================================================
#ifndef __OP_POKE_CUTIN_H__
#define __OP_POKE_CUTIN_H__

typedef struct OPD_SSM_tag * OPD_SSM_PTR;

extern const int OPD_Poke_GetSSMSize(void);
extern void OPD_Poke_Init(OPD_SSM_PTR opd_ssm_ptr);
extern void OPD_Poke_End(OPD_SSM_PTR opd_ssm_ptr);
extern void OPD_Poke_VBlank(OPD_SSM_PTR opd_ssm_ptr);
extern void OPD_Poke_Main(OPD_SSM_PTR opd_ssm_ptr);

extern BOOL OPD_Poke_CutIn(OPD_SSM_PTR opd_ssm_ptr, const u8 inMonster, const int inCounter);

extern PTC_PTR OPD_Poke_PtcGet(OPD_SSM_PTR opd_ssm_ptr);
extern EMIT_PTR OPD_EmitterSet(OPD_SSM_PTR opd_ssm_ptr, int spr_no);


typedef enum {
	CUT_IN_POKE_0 = 0,
	CUT_IN_POKE_1,
	CUT_IN_POKE_2,
};

#endif //__OP_POKE_CUTIN_H__
