//============================================================================================
/**
 * @file	field_camera.h
 * @bfief	フィールドカメラ
 * @author	GAME FREAK inc.
 */
//============================================================================================

extern void FieldCameraInit(const VecFx32 *inTarget,
							FIELDSYS_WORK * repw,
							const int AreaCamera,
							const BOOL inDelayFlg);

extern void FieldCameraEnd(FIELDSYS_WORK * repw);

#ifdef PM_DEBUG	//++++++++++++++++++++++++++++++++++++++++++++
extern int Debug_FieldCameraTblNumGet(void);
extern void Debug_FieldCameraTblPtrGet(const VecFx32 *inTarget, GF_CAMERA_PTR camera_ptr, int AreaCamera);
#endif	//PM_DEBUG	//+++++++++++++++++++++++++++++++++++++++++++++++

