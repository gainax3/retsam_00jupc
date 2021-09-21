using System;
using System.Reflection;

namespace AdminTool
{
    public class DpwUtil
    {
        // これをビルドするにはdpwutilの登録が必要。
        // regsvr32 dpwutil.dll
        // DPWUTILLibの参照設定で分離をtrueにしているので、配布先で登録をする必要はない
        public static DPWUTILLib.Tools Tools = new DPWUTILLib.Tools();
        public static DPWUTILLib.PokemonChecker PokemonChecker = new DPWUTILLib.PokemonChecker();
        public static DPWUTILLib.Base64CoDec Base64CoDec = new DPWUTILLib.Base64CoDec();
        public static DPWUTILLib.BinaryCoDec BinaryCoDec = new DPWUTILLib.BinaryCoDec();
        public static DPWUTILLib.Encryptor Encryptor = new DPWUTILLib.Encryptor();
    }
}