#include <string.h>
#include <nitro.h>

#include <base/dwc_common.h>


//----------------------------------------------------------------------------
// define
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
// variable
//----------------------------------------------------------------------------
static MATHRandContext32 stRandContext32 = { 0, 0, 0 };  // 乱数コンテキスト


//----------------------------------------------------------------------------
// prototype
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
// function - external
//----------------------------------------------------------------------------
/*---------------------------------------------------------------------------*
  汎用key/value型文字列作成関数
  引数　：key       設定したいkey文字列
          value     設定したいvalue文字列
          string    key/value型文字列格納先ポインタ
          separator 各文字列を分ける区切り文字
  戻り値：セットされたkey/value文字列長（NULL終端含まず）
  用途　：key/valueの文字列を指定し、DWC汎用key/value文字列を作成する
 *---------------------------------------------------------------------------*/
int  DWC_SetCommonKeyValueString(const char* key, const char* value, char* string, char separator)
{

    SDK_ASSERT(key && value);

    OS_SNPrintf(string, DWC_COMMONSTR_MAX_KEY_VALUE_LEN,
                "%c%s%c%s",
                separator, key, separator, value);

    return (int)strlen(string);
}


/*---------------------------------------------------------------------------*
  汎用key/value型文字列追加関数
  引数　：key       設定したいkey文字列
          value     設定したいvalue文字列
          string    key/value型文字列格納先ポインタ
          separator 各文字列を分ける区切り文字
  戻り値：key/value文字列が追加された後のkey/value文字列長（NULL終端含まず）
  用途　：key/valueの文字列を既存の文字列に追加する
 *---------------------------------------------------------------------------*/
int  DWC_AddCommonKeyValueString(const char* key, const char* value, char* string, char separator)
{

    (void)DWC_SetCommonKeyValueString(key, value, strchr(string, '\0'), separator);

    return (int)strlen(string);
}


/*---------------------------------------------------------------------------*
  汎用key/value型文字列value取得関数
  引数　：key       取り出したいkey文字列
          value     取り出したvalue文字列の格納先ポインタ。
          string    key/value型文字列
          separator 各文字列を分ける区切り文字
  戻り値：value文字列長（NULL終端含まず）。存在しないkeyを指定した場合は-1を返す
  用途　：指定した区切り文字で区切られたDWC汎用key/value型文字列から、
          指定されたkey文字列に対応するvalue文字列を取得する。
 *---------------------------------------------------------------------------*/
int  DWC_GetCommonValueString(const char* key, char* value, const char* string, char separator)
{
    const char* pSrcBegin;
    char* pSrcEnd;
    int len;

    SDK_ASSERT(key);

    if (!value) return -1;

    pSrcBegin = strchr(string, separator);  // key/value文字列の開始ポインタを取得
    if (!pSrcBegin) return -1;

    while (1){
        if (!strncmp(pSrcBegin+1, key, strlen(key)) &&
            (pSrcBegin[strlen(key)+1] == separator)){
            break;  // key発見
        }

        // 次のkeyを探す。
        pSrcBegin = strchr(pSrcBegin+1, separator);
        if (!pSrcBegin) return -1;
        pSrcBegin = strchr(pSrcBegin+1, separator);
        if (!pSrcBegin) return -1;
    }

    // valueの開始ポインタを取得
    pSrcBegin = strchr(pSrcBegin+1, separator);
    if (!pSrcBegin) return -1;
    pSrcBegin++;

    // valueの文字列長を取得
    pSrcEnd = strchr(pSrcBegin, separator);
    if (pSrcEnd) len = (int)(pSrcEnd-pSrcBegin);
    else len = (int)strlen(pSrcBegin);

    strncpy(value, pSrcBegin, (u32)len);  // value文字列をコピー
    value[len] = '\0';

    return len;
}


//----------------------------------------------------------------------------
// function - internal
//----------------------------------------------------------------------------
/*---------------------------------------------------------------------------*
  32bit乱数値取得関数
  引数　：max 取得する乱数の最大値。ここで設定した max-1 が得られる乱数の
              最大値となる。0を指定した場合にはすべての範囲の数が得られる。
  戻り値：32bit乱数値
  用途　：関数MATH_Rand32()で生成した32bit乱数値を取得する
 *---------------------------------------------------------------------------*/
u32  DWCi_GetMathRand32(u32 max)
{
    u64 seed;

    if (!stRandContext32.x && !stRandContext32.mul && !stRandContext32.add){
        // 乱数コンテキストが初期化されていない場合は初期化する
        OS_GetMacAddress((u8 *)&seed);
        seed = ((seed >> 24) & 0xffffff) | (OS_GetTick() << 24);
        MATH_InitRand32(&stRandContext32, seed);
    }

    return MATH_Rand32(&stRandContext32, max);
}


/*---------------------------------------------------------------------------*
  ワイド文字列の文字列長取得
  引数　：str ワイド文字列
  戻り値：文字数
 *---------------------------------------------------------------------------*/
u32 DWCi_WStrLen( const u16* str )
{
    u32 length = 0;

    while ( str[length] != 0 )
    {
        length++;
    }

    return length;
}


//----------------------------------------------------------------------------
// function - static
//----------------------------------------------------------------------------
