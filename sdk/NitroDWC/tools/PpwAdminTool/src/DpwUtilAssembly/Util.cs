using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;

namespace AdminTool
{
    public class Util
    {
        public static int FriendCodeToProfileId(UInt64 friendCode)
        {
            return (int)friendCode;
        }

        public static ushort ProfileIdToUserId(int profileId)
        {
            return (ushort)profileId;
        }

        public enum Language
        {
            NONE,
            JP,
            EN,
            FR,
            IT,
            DE,
            RESERVE,
            SP,
            KR
        }

        public static Language IndexToLanguageCode(int index)
        {
            switch(index)
            {
                case 0:
                    return Language.JP;
                case 1:
                    return Language.EN;
                case 2:
                    return Language.FR;
                case 3:
                    return Language.IT;
                case 4:
                    return Language.DE;
                case 5:
                    return Language.SP;
                case 6:
                    return Language.KR;
                default:
                    throw new Exception("言語コードが不正です");
            }
        }

        public static int LanguageCodeToIndex(Language language)
        {
            switch (language)
            {
                case Language.JP:
                    return 0;
                case Language.EN:
                    return 1;
                case Language.FR:
                    return 2;
                case Language.IT:
                    return 3;
                case Language.DE:
                    return 4;
                case Language.SP:
                    return 5;
                case Language.KR:
                    return 6;
                default:
                    throw new Exception("不正な言語コードのインデックスです。");
            }
        }


        public enum RomVersion
        {
            PLATINUM
        }

        public static List<List<string>> TsvTextToArray(string text)
        {
            List<List<string>> result = new List<List<string>>();
            if (string.IsNullOrEmpty(text))
            {
                return result;
            }

            text = text.Replace("\r\n", "\n");
            text = text.Replace('\r', '\n');
            text.TrimEnd(new char[] { '\n' });
            string[] lines = text.Split('\n');

            foreach (string line in lines)
            {
                //タブで分割
                result.Add(new List<string>(line.Split('\t')));
            }
            return result;
        }

        // byteのListをエンディアン依存でshortにする。システムのエンディアンと逆にするときはreverseをtrueにする。
        public static List<ushort> BytesToUShorts(List<byte> list, bool reverse)
        {
            if(list.Count % 2 == 1)
            {
                throw new Exception("要素数が奇数です。");
            }

            List<ushort> result = new List<ushort>();

            ushort s;
            for (int i = 0; i < list.Count; i += 2)
            {
                if (reverse ^ !BitConverter.IsLittleEndian)
                {
                    // 反転しない
                    // !reverseかつビッグエンディアン
                    // reverseかつリトルエンディアン のとき
                    s = (ushort)(list[i] << 8 | list[i + 1]);
                }
                else
                {
                    // 反転する
                    s = (ushort)(list[i+1] << 8 | list[i]);
                }
                result.Add(s);
            }
            return result;
        }

        public static List<byte> UShortsToBytes(List<ushort> list, bool reverse)
        {
            List<byte> result = new List<byte>();
            foreach (ushort s in list)
            {
                byte[] bytes = BitConverter.GetBytes(s);    // GetBytesはシステムエンディアン依存
                if (reverse)
                {
                    Array.Reverse(bytes);
                }
                result.AddRange(bytes);
            }
            return result;
        }

        public static List<ushort> UnicodeStringToUShortList(string str)
        {
            return Util.BytesToUShorts(new List<byte>(Encoding.Unicode.GetBytes(str)), false);
        }

        public static string Join<T>(List<T> list, string token)
        {
            string result = "";
            for (int i = 0; i < list.Count; i++)
            {
                if (i != 0)
                {
                    result += token;
                }
                result += list[i];
            }
            return result;
        }

        // NULL文字とその後の文字を削除したものを返す
        public static string TrimNullString(string str)
        {
            int index = str.IndexOf('\0');
            return str.Remove(index);
        }

        // ポケコードのリストからトリムされたUnicode文字列に変換する。
        public static string UShortListToUnicodeString(List<ushort> pokeList)
        {
            List<byte> bytes = Util.UShortsToBytes(pokeList, false);
            return Util.TrimNullString(Util.ConvLFToMacro(DpwUtil.Tools.PokecodeToUnicodeWithoutCheckLF(Encoding.Unicode.GetString(bytes.ToArray()))));
        }

        public const string LFString = "<LF>";
	    // <LF>から改行コードに変換する
	    public static string ConvMacroToLF(string src)
	    {
		    return src.Replace(LFString, "\n");
	    }

	    // 改行コードから<LF>に変換する
	    public static string ConvLFToMacro(string src)
	    {
		    return src.Replace("\n", LFString);
	    }
    }
}
