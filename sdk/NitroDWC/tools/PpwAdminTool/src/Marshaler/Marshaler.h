// Marshaler.h

#pragma once

using namespace System;
using namespace System::Text;
using namespace System::Collections::Generic;
using namespace System::Windows::Forms;
using namespace System::Runtime::InteropServices;
#pragma warning( disable:4691 )

using namespace std;

#pragma pack(push, 4)
#include "WFLBY_USER_PROFILE.h"
#include "lobby.h"
#pragma pack(pop)

namespace AdminTool {

    template<class T>
    array<Byte>^ NativeTypeToCliByteArray(const T& native)
    {
        array<Byte>^ arr = gcnew array<Byte>(sizeof(native));
        // nativeへは書き込まないのでconst外し
        Marshal::Copy(IntPtr(const_cast<T*>(&native)), arr, 0, sizeof(native)); 
        return arr;
    }

    template<class T>
    vector<T> ListToNativeArray(List<T>^ list)
    {
        vector<T> v;
        if(list->Count == 0)
        {
            return v;
        }
        array<T>^ arr = list->ToArray();
        pin_ptr<T> nativeName = &arr[0];
        v.assign((T*)nativeName, (T*)(nativeName + arr->Length));
        return v;
    }
	
    template<class TargetT, class SourceT>
    vector<TargetT> ListToNativeArray(List<SourceT^>^ list)
    {
        vector<TargetT> v;
		for each(SourceT^ elem in list)
		{
			v.push_back(elem->ToNative());
		}
        return v;
    }

	// ネイティブな配列から値型のリストへの変換
    template<class TargetT, class SourceT>
    List<TargetT>^ NativeArrayToList(const SourceT* ary, int num)
    {
        // 良い方法が見つからなかったので一個ずつ追加
        List<TargetT>^ list = gcnew List<TargetT>();
        for(int i=0; i<num; i++)
        {
            list->Add(TargetT(ary[i]));
        }
        return list;
    }

	// ネイティブな配列からハンドルのリストへの変換
    template<class TargetT, class SourceT>
    List<TargetT^>^ NativeArrayToManageList(const SourceT* ary, int num)
    {
        // 良い方法が見つからなかったので一個ずつ追加
        List<TargetT^>^ list = gcnew List<TargetT^>();
        for(int i=0; i<num; i++)
        {
            list->Add(gcnew TargetT(ary[i]));
        }
        return list;
    }

	// 配列の要素を連結する
	template<class T>
	String^ Join(List<T>^ list, String^ token)
	{
		String^ result = "";
		for(int i=0; i<list->Count; i++)
		{
			if(i != 0)
			{
				result += token;
			}
			result += list[i];
		}
		return result;
	}

	// 配列の要素を個数を指定して連結する
	template<class T>
	String^ Join(List<T>^ list, String^ token, int num)
	{
		String^ result = "";
		for(int i=0; i<list->Count && i<num; i++)
		{
			if(i != 0)
			{
				result += token;
			}
			result += list[i];
		}
		return result;
	}

    // WFLBY_USER_PROFILEのマネージ表現
    // いらないパラメータは外す
    public ref struct REF_WFLBY_USER_PROFILE
    {
        s32				userid;						// ロビー内ユーザID
        u32				trainerid;					// トレーナーID					
        List<STRCODE>^	name;	                    // ユーザ名
        u8				sex;						// 性別
        u8				region_code;				// 言語コード LANG_JAPANなどなど
        u16				tr_type;					// トレーナの見た目
        u16				nation;						// 国コード
        u8				area;						// 地域コード
        u8				rom_code;					// ロムバージョン	VERSION_PLATINUMとか

        REF_WFLBY_USER_PROFILE(s32 userId_, u32 trainerid_, List<STRCODE>^ name_, u8 sex_, u8 region_code_, u16 tr_type_, u16 nation_, u8 area_, u8 rom_code_)
        {
            userid = userId_;
            trainerid = trainerid_;
            name = name_;
            sex = sex_;
            region_code = region_code_;
            tr_type = tr_type_;
            nation = nation_;
            area = area_;
            rom_code = rom_code_;
        }
        
        REF_WFLBY_USER_PROFILE(IntPtr ptr, int size)
        {
            WFLBY_USER_PROFILE nativeProfile;
            if(size != sizeof(nativeProfile))
            {
                throw gcnew System::Exception("サイズが一致しません");
            }
            memcpy(&nativeProfile, (void*)ptr, sizeof(nativeProfile));

            userid = nativeProfile.userid;
            trainerid = nativeProfile.trainerid;
            name = NativeArrayToList<STRCODE, STRCODE>(nativeProfile.name, PERSON_NAME_SIZE + EOM_SIZE);
            sex = nativeProfile.sex;
            region_code = nativeProfile.region_code;
            tr_type = nativeProfile.tr_type;
            nation = nativeProfile.nation;
            area = nativeProfile.area;
            rom_code = nativeProfile.rom_code;
        }
/*
        REF_WFLBY_USER_PROFILE(List<Byte>^ bytes)
        {
            WFLBY_USER_PROFILE* nativeProfile = (WFLBY_USER_PROFILE*)DWCi_GetVectorBuffer(ListToNativeArray(bytes));

            userid = nativeProfile->userid;
            trainerid = nativeProfile->trainerid;
            name = NativeArrayToList(nativeProfile->name, PERSON_NAME_SIZE + EOM_SIZE);
            sex = nativeProfile->sex;
            region_code = nativeProfile->region_code;
            tr_type = nativeProfile->tr_type;
            nation = nativeProfile->nation;
            area = nativeProfile->area;
            rom_code = nativeProfile->rom_code;
        }
*/
        array<Byte>^ ToByteArray()
        {
            vector<STRCODE> nativeName = ListToNativeArray(name);
            if(nativeName.empty())
            {
                throw gcnew System::Exception("名前が空です。");
            }

            WFLBY_USER_PROFILE userProfile = {
                userid, // ユーザID
                trainerid,   // トレーナーID
                {0},   // ユーザ名、後で代入
                0,      // 入室時間
                0,      // 自分の国のGMT時間
                {1, 2, 3, 4, 5, 6},    // 手持ちのポケモン
                {0},    // ポケモンのフォルムデータ
                {0},    // 卵フラグ
                sex,      // 性別
                region_code,      // 言語コード
                tr_type,      // トレーナータイプ
                nation,    // 国コード
                area,      // 地域コード
                0,      // 全国図鑑保持フラグ
                0,      // ゲームクリアフラグ
                0,      // タッチトイ
                rom_code,     // ROMバージョン
                0,      // プレイヤーステータス
                0,      // 冒険を始めた日時
                {0},    // 最後に行ったこと
                {0}     // 最後に行ったことに関連する人
            };
            memcpy(&userProfile.name, DWCi_GetVectorBuffer(nativeName),
                sizeof(userProfile.name[0]) * std::min(nativeName.size(), (std::size_t)(PERSON_NAME_SIZE + EOM_SIZE)));

            return NativeTypeToCliByteArray(userProfile);
        }
    };

	bool IsFreeQuestion(int questionNo)
	{
		return questionNo >= PPW_LOBBY_FREE_QUESTION_START_NO && questionNo < PPW_LOBBY_FREE_QUESTION_START_NO + PPW_LOBBY_FREE_QUESTION_NUM;
	}

	/// アンケート内容
	public ref struct REF_PPW_LobbyQuestionnaireRecord
	{
		s32 questionSerialNo;                   ///< 質問通し番号。0からスタート。
		s32 questionNo;                         ///< 質問番号。ROM内質問:0～59 任意質問:PPW_LOBBY_FREE_QUESTION_START_NO～PPW_LOBBY_FREE_QUESTION_START_NO+PPW_LOBBY_FREE_QUESTION_NUM
		List<u16>^ questionSentence;   ///< 任意質問。質問番号が任意質問の範囲だったときのみ格納されます。
		List<List<u16>^>^ answer;    ///< 任意質問の回答。質問番号が任意質問の範囲だったときのみ格納されます。
		List<u8>^ multiLanguageSummarizeFlags;     ///< 集計言語
		s32 isSpecialWeek;

		REF_PPW_LobbyQuestionnaireRecord(const PPW_LobbyQuestionnaireRecord& native)
		{
			questionSerialNo = native.questionSerialNo;
			questionNo = native.questionNo;
			questionSentence = NativeArrayToList<u16, u16>(native.questionSentence, PPW_LOBBY_MAX_QUESTION_SENTENCE_LENGTH);
			answer = gcnew List<List<u16>^>();
			for(int i=0; i<PPW_LOBBY_QUESTIONNAIRE_ANSWER_NUM; i++)
			{
				answer->Add(NativeArrayToList<u16, u16>(native.answer[i], PPW_LOBBY_MAX_ANSWER_LENGTH));
			}
			multiLanguageSummarizeFlags = NativeArrayToList<u8, u8>(native.multiLanguageSummarizeFlags, 12);
			isSpecialWeek = native.isSpecialWeek;
		}

		virtual String^ ToString() new
		{
			List<String^>^ answerStrs = gcnew List<String^>();
			for each(List<u16>^ ans in answer)
			{
				answerStrs->Add(IsFreeQuestion(questionNo) ? Util::UShortListToUnicodeString(ans) : "<なし>");
			}

			return "質問通し番号：" + questionSerialNo + Environment::NewLine +
				"質問番号：" + questionNo + Environment::NewLine +
				"質問文：" + (IsFreeQuestion(questionNo) ? Util::UShortListToUnicodeString(questionSentence) : "<なし>") + Environment::NewLine +
				"回答：" + Join(answerStrs, ", ") + Environment::NewLine +
				"集計言語：" + Join(multiLanguageSummarizeFlags, ", ", LANGUAGE_MAX) + Environment::NewLine +
				"スペシャルウィーク：" + (isSpecialWeek ? "はい" : "いいえ");
		}
	};

	/// アンケート情報
	public ref struct REF_PPW_LobbyQuestionnaire
	{
		REF_PPW_LobbyQuestionnaireRecord^ currentQuestionnaireRecord;    ///< 現在のアンケート情報
		REF_PPW_LobbyQuestionnaireRecord^ lastQuestionnaireRecord;       ///< 前回のアンケート情報
		List<s32>^ lastResult;         ///< 前回の自分の言語の結果
		List<s32>^ lastMultiResult;    ///< 前回の複数集計言語での集計結果。::lastQuestionnaireRecord構造体のmultiLanguageSummarizeFlagsメンバで表される言語での集計結果です。

        REF_PPW_LobbyQuestionnaire(IntPtr ptr, int size)
        {
            PPW_LobbyQuestionnaire native;
            if(size != sizeof(native))
            {
                throw gcnew System::Exception("サイズが一致しません");
            }
            memcpy(&native, (void*)ptr, sizeof(native));

			currentQuestionnaireRecord = gcnew REF_PPW_LobbyQuestionnaireRecord(native.currentQuestionnaireRecord);
			lastQuestionnaireRecord = gcnew REF_PPW_LobbyQuestionnaireRecord(native.lastQuestionnaireRecord);
			lastResult = NativeArrayToList<s32, s32>(native.lastResult, PPW_LOBBY_QUESTIONNAIRE_ANSWER_NUM);
			lastMultiResult = NativeArrayToList<s32, s32>(native.lastMultiResult, PPW_LOBBY_QUESTIONNAIRE_ANSWER_NUM);
        }

		virtual System::String^ ToString() new
		{
			return "今週のアンケート内容：" + Environment::NewLine +
				currentQuestionnaireRecord->ToString() + Environment::NewLine + Environment::NewLine +
				"先週のアンケート内容：" + Environment::NewLine +
				lastQuestionnaireRecord->ToString() + Environment::NewLine + Environment::NewLine +
				"先週の結果(自分の言語)：" + Join(lastResult, ", ") + Environment::NewLine +
				"先週の結果(複数言語)：" + Join(lastMultiResult, ", ");

		}
	};



	/// 質問条件
    [Serializable()]
	public ref struct REF_PPW_LobbyQuestionCondition
	{
		s32 questionNo;
		s32 multiLanguageSummarizeId;

		REF_PPW_LobbyQuestionCondition()
		{
		}

		REF_PPW_LobbyQuestionCondition(const PPW_LobbyQuestionCondition& native)
		{
			questionNo = native.questionNo;
			multiLanguageSummarizeId = native.multiLanguageSummarizeId;
		}

		PPW_LobbyQuestionCondition ToNative()
		{
			PPW_LobbyQuestionCondition native = { questionNo, multiLanguageSummarizeId };
			return native;
		}

		bool IsConsistent()
		{
			return (questionNo >= 0 && questionNo < 60) ||
				IsFreeQuestion(questionNo);
		}
	};

	/// スペシャルウィークレコード
    [Serializable()]
	public ref struct REF_PPW_LobbySpecialWeekRecord
	{
		static const int SPECIAL_RECORD_SIZE = sizeof(PPW_LobbySpecialWeekRecord);
		static const int LOBBY_FREE_QUESTION_START_NO = PPW_LOBBY_FREE_QUESTION_START_NO;
		static const int LOBBY_FREE_QUESTION_NUM = PPW_LOBBY_FREE_QUESTION_NUM;
	private:
		s32 questionSerialNo;
		List<REF_PPW_LobbyQuestionCondition^>^ questionConditions;
	public:
		property s32 QuestionSerialNo
		{
			s32 get()
			{
				return questionSerialNo;
			}
			void set(s32 value)
			{
				questionSerialNo = value;
			}
		}

		property s32 QuestionNoJp
		{
			s32 get()
			{
				return questionConditions[(int)Util::Language::JP]->questionNo;
			}
			void set(s32 value)
			{
				questionConditions[(int)Util::Language::JP]->questionNo = value;
			}
		}

		property s32 QuestionNoUs
		{
			s32 get()
			{
				return questionConditions[(int)Util::Language::EN]->questionNo;
			}
			void set(s32 value)
			{
				questionConditions[(int)Util::Language::EN]->questionNo = value;
			}
		}


		property s32 QuestionNoFr
		{
			s32 get()
			{
				return questionConditions[(int)Util::Language::FR]->questionNo;
			}
			void set(s32 value)
			{
				questionConditions[(int)Util::Language::FR]->questionNo = value;
			}
		}
		

		property s32 QuestionNoIt
		{
			s32 get()
			{
				return questionConditions[(int)Util::Language::IT]->questionNo;
			}
			void set(s32 value)
			{
				questionConditions[(int)Util::Language::IT]->questionNo = value;
			}
		}
		

		property s32 QuestionNoDe
		{
			s32 get()
			{
				return questionConditions[(int)Util::Language::IT]->questionNo;
			}
			void set(s32 value)
			{
				questionConditions[(int)Util::Language::IT]->questionNo = value;
			}
		}
		

		property s32 QuestionNoSp
		{
			s32 get()
			{
				return questionConditions[(int)Util::Language::SP]->questionNo;
			}
			void set(s32 value)
			{
				questionConditions[(int)Util::Language::SP]->questionNo = value;
			}
		}
		

		property s32 QuestionNoKr
		{
			s32 get()
			{
				return questionConditions[(int)Util::Language::KR]->questionNo;
			}
			void set(s32 value)
			{
				questionConditions[(int)Util::Language::KR]->questionNo = value;
			}
		}

		property s32 MultiLanguageSummarizeIdJp
		{
			s32 get()
			{
				return questionConditions[(int)Util::Language::JP]->multiLanguageSummarizeId;
			}
			void set(s32 value)
			{
				questionConditions[(int)Util::Language::JP]->multiLanguageSummarizeId = value;
			}
		}

		property s32 MultiLanguageSummarizeIdUs
		{
			s32 get()
			{
				return questionConditions[(int)Util::Language::EN]->multiLanguageSummarizeId;
			}
			void set(s32 value)
			{
				questionConditions[(int)Util::Language::EN]->multiLanguageSummarizeId = value;
			}
		}

		property s32 MultiLanguageSummarizeIdFr
		{
			s32 get()
			{
				return questionConditions[(int)Util::Language::FR]->multiLanguageSummarizeId;
			}
			void set(s32 value)
			{
				questionConditions[(int)Util::Language::FR]->multiLanguageSummarizeId = value;
			}
		}

		property s32 MultiLanguageSummarizeIdIt
		{
			s32 get()
			{
				return questionConditions[(int)Util::Language::IT]->multiLanguageSummarizeId;
			}
			void set(s32 value)
			{
				questionConditions[(int)Util::Language::IT]->multiLanguageSummarizeId = value;
			}
		}
		

		property s32 MultiLanguageSummarizeIdDe
		{
			s32 get()
			{
				return questionConditions[(int)Util::Language::DE]->multiLanguageSummarizeId;
			}
			void set(s32 value)
			{
				questionConditions[(int)Util::Language::DE]->multiLanguageSummarizeId = value;
			}
		}

		property s32 MultiLanguageSummarizeIdSp
		{
			s32 get()
			{
				return questionConditions[(int)Util::Language::SP]->multiLanguageSummarizeId;
			}
			void set(s32 value)
			{
				questionConditions[(int)Util::Language::SP]->multiLanguageSummarizeId = value;
			}
		}
		
		property s32 MultiLanguageSummarizeIdKr
		{
			s32 get()
			{
				return questionConditions[(int)Util::Language::KR]->multiLanguageSummarizeId;
			}
			void set(s32 value)
			{
				questionConditions[(int)Util::Language::KR]->multiLanguageSummarizeId = value;
			}
		}

        REF_PPW_LobbySpecialWeekRecord()
		{
			questionConditions = gcnew List<REF_PPW_LobbyQuestionCondition^>();
			for(int i=0; i<LANGUAGE_MAX; i++)
			{
				questionConditions->Add(gcnew REF_PPW_LobbyQuestionCondition());
			}
		}

        REF_PPW_LobbySpecialWeekRecord(IntPtr ptr, int size)
        {
            PPW_LobbySpecialWeekRecord native;
            if(size != sizeof(native))
            {
                throw gcnew System::Exception("サイズが一致しません");
            }
            memcpy(&native, (void*)ptr, sizeof(native));

			questionSerialNo = native.questionSerialNo;
			questionConditions = NativeArrayToManageList<REF_PPW_LobbyQuestionCondition, PPW_LobbyQuestionCondition>(native.questionConditions, LANGUAGE_MAX);
        }
		
        array<Byte>^ ToByteArray()
        {
			PPW_LobbySpecialWeekRecord native = {
				questionSerialNo,
				{0}	// questionConditions 後で代入
			};
            memcpy(&native.questionConditions,
				DWCi_GetVectorBuffer(ListToNativeArray<PPW_LobbyQuestionCondition, REF_PPW_LobbyQuestionCondition>(questionConditions)),
                sizeof(native.questionConditions[0]) * LANGUAGE_MAX);


            return NativeTypeToCliByteArray(native);
        }

		bool IsConsistent()
		{
			for(int i = 0; i<LANGUAGE_MAX; i++)
			{
				if(!questionConditions[i]->IsConsistent())
				{
					return false;
				}
			}
			return true;
		}
	};

	/// 任意質問
	public ref struct REF_PPW_LobbyFreeQuestion
	{
		static const int LOBBY_MAX_QUESTION_SENTENCE_LENGTH = PPW_LOBBY_MAX_QUESTION_SENTENCE_LENGTH;
		static const int LOBBY_QUESTIONNAIRE_ANSWER_NUM = PPW_LOBBY_QUESTIONNAIRE_ANSWER_NUM;
		static const int LOBBY_MAX_ANSWER_LENGTH = PPW_LOBBY_MAX_ANSWER_LENGTH;
		static const int NATIVE_SIZE = sizeof(PPW_LobbyFreeQuestion);

	private:
		s32 language;                   ///< 登録する言語
		s32 questionNo;                 ///< 質問番号
		String^ questionSentence;	///< 任意質問。
		List<String^>^ answer;		///< 任意質問の回答。
	public:
		property s32 Language
		{
			s32 get()
			{
				return language;
			}
			void set(s32 value)
			{
				language = value;
			}
		}

		property s32 QuestionNo
		{
			s32 get()
			{
				return questionNo;
			}
			void set(s32 value)
			{
				questionNo = value;
			}
		}

		property String^ QuestionSentence
		{
			String^ get()
			{
				return questionSentence;
			}
			void set(String^ value)
			{
				questionSentence = value;
			}
		}
		property String^ Answer1
		{
			String^ get()
			{
				return answer[0];
			}
			void set(String^ value)
			{
				answer[0] = value;
			}
		}
		property String^ Answer2
		{
			String^ get()
			{
				return answer[1];
			}
			void set(String^ value)
			{
				answer[1] = value;
			}
		}
		property String^ Answer3
		{
			String^ get()
			{
				return answer[2];
			}
			void set(String^ value)
			{
				answer[2] = value;
			}
		}


		REF_PPW_LobbyFreeQuestion(s32 questionNo_, s32 language_, String^ questionSentence_, List<String^>^ answer_)
		{
			language = language_;
			questionNo = questionNo_;
			questionSentence = questionSentence_;
			answer = answer_;
		}

		// XMLシリアライズで必要
		REF_PPW_LobbyFreeQuestion()
		{
			answer = gcnew List<String^>();
			for(int i=0; i<PPW_LOBBY_QUESTIONNAIRE_ANSWER_NUM; i++)
			{
				answer->Add("");
			}
		}
		
        REF_PPW_LobbyFreeQuestion(IntPtr ptr, int size)
        {
            PPW_LobbyFreeQuestion native;
            if(size != sizeof(native))
            {
                throw gcnew Exception("サイズが一致しません");
            }
            memcpy(&native, (void*)ptr, sizeof(native));

			questionNo = native.questionNo;
			language = native.language;

			// 質問文のマーシャリング
			{
				List<u16>^ questionSentenceList = NativeArrayToList<u16, u16>(native.questionSentence, PPW_LOBBY_MAX_QUESTION_SENTENCE_LENGTH);
				List<u8>^ bytes = Util::UShortsToBytes(questionSentenceList, false);
				String^ str = Encoding::Unicode->GetString(bytes->ToArray());

				try
				{
					questionSentence = Util::ConvLFToMacro(Util::TrimNullString(
						DpwUtil::Tools->PokecodeToUnicodeWithoutCheckLF(Encoding::Unicode->GetString(bytes->ToArray()))));
				}
				catch(Exception^)
				{
					throw gcnew Exception("サーバに設定がセットされていないか、壊れています。初期値をセットしてください。");
				}
			}

			// 回答のマーシャリング
			answer = gcnew List<String^>();
			for(int i=0; i<PPW_LOBBY_QUESTIONNAIRE_ANSWER_NUM; i++)
			{
				List<u16>^ answerList = NativeArrayToList<u16, u16>(native.answer[i], PPW_LOBBY_MAX_ANSWER_LENGTH);
				List<u8>^ bytes = Util::UShortsToBytes(answerList, false);
				answer->Add(Util::TrimNullString(DpwUtil::Tools->PokecodeToUnicode(Encoding::Unicode->GetString(bytes->ToArray()))));
			}
        }

        array<Byte>^ ToByteArray()
        {
			PPW_LobbyFreeQuestion native = {
				language,
				questionNo,
				{0},	// questionSentence 後で代入
				{0}		// answer 後で代入
			};

			// StringからList<u16>へ変換
            String^ cur = "";
            String^ questionSentencePoke = "";
			List<String^>^ answerPokes = gcnew List<String^>();
            try
            {
				cur = questionSentence;
				cur = Util::ConvMacroToLF(cur);	// <LF>から改行コードに変換する
				questionSentencePoke = DpwUtil::Tools->UnicodeToPokecode(cur);
				for(int i=0; i<PPW_LOBBY_QUESTIONNAIRE_ANSWER_NUM; i++)
				{
					cur = answer[i];
					answerPokes->Add(DpwUtil::Tools->UnicodeToPokecode(cur));
				}
            }
            catch (Exception^ e)
            {
				MessageBox::Show("「" + cur + "」はゲーム内文字コードへ変換できません。");
                throw e;
            }
			List<u16>^ questionSentenceList = Util::UnicodeStringToUShortList(questionSentencePoke);
			List<List<u16>^>^ answerList = gcnew List<List<u16>^>();
			for(int i=0; i<PPW_LOBBY_QUESTIONNAIRE_ANSWER_NUM; i++)
			{
				answerList->Add(Util::UnicodeStringToUShortList(answerPokes[i]));
			}
			
			// List<u16>からネイティブ配列へ
            memcpy(&native.questionSentence,
				DWCi_GetVectorBuffer(ListToNativeArray(questionSentenceList)),
                sizeof(native.questionSentence[0]) * PPW_LOBBY_MAX_QUESTION_SENTENCE_LENGTH);

			for(int i=0; i<PPW_LOBBY_QUESTIONNAIRE_ANSWER_NUM; i++)
			{
				memcpy(&native.answer[i],
					DWCi_GetVectorBuffer(ListToNativeArray(answerList[i])),
					sizeof(native.answer[0]));
			}
			
            return NativeTypeToCliByteArray(native);
        }

		bool IsConsistent()
		{
			return questionNo >= PPW_LOBBY_FREE_QUESTION_START_NO &&
				questionNo < PPW_LOBBY_FREE_QUESTION_START_NO + PPW_LOBBY_FREE_QUESTION_NUM &&
				this->questionSentence->Length <= 108 &&
				answer[0]->Length <= 16 &&
				answer[1]->Length <= 16 &&
				answer[2]->Length <= 16;
		}
	};
}