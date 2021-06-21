/*---------------------------------------------------------------------------*
  Project:  PpwAdminTool

  Copyright 2008 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

 *---------------------------------------------------------------------------*/
using System;
using System.Collections.Generic;
using System.Text;
using System.Net;
using System.IO;
using System.Runtime.InteropServices;
using System.Windows.Forms;
using System.Reflection;
using NWC;
using System.Collections;

namespace AdminTool
{
    /// <summary>
    /// 共通インタフェースクラス
    /// </summary>
    public partial class RequestInterface
    {
        /// <summary>
        /// 接続先パス
        /// </summary>
        // スケジュール
        public const string URL_ADMIN_SET_LOBBY_SCHEDULE = "/web/enc/lobby/adminSetSchedule.asp";
        public const string URL_ADMIN_APPEND_LOBBY_SCHEDULE = "/web/enc/lobby/adminAppendSchedule.asp";
        public const string URL_ADMIN_GET_LOBBY_SCHEDULE = "/web/enc/lobby/adminGetSchedule.asp";
        public const string URL_GET_LOBBY_SCHEDULE = "/web/enc/lobby/getSchedule.asp";

        // VIP
        public const string URL_ADMIN_SET_LOBBY_VIP = "/web/enc/lobby/adminSetVip.asp";
        public const string URL_ADMIN_GET_LOBBY_VIP = "/web/enc/lobby/adminGetVip.asp";
        public const string URL_GET_LOBBY_VIP = "/web/enc/lobby/getVip.asp";
        
        // 不正チェック
        public const string URL_ADMIN_CHECK_PROFILE = "/web/enc/lobby/checkProfile.asp";
        
        // アンケート
        public const string URL_GET_QUESTIONNAIRE = "/web/enc/lobby/getQuestionnaire.asp";
        public const string URL_SUBMIT_QUESTIONNAIRE = "/web/enc/lobby/submitQuestionnaire.asp";
        public const string URL_ADMIN_GET_SPECIAL_WEEK = "/web/enc/lobby/adminGetSpecialWeek.asp";
        public const string URL_ADMIN_SET_SPECIAL_WEEK = "/web/enc/lobby/adminSetSpecialWeek.asp";
        public const string URL_ADMIN_GET_FREE_QUESTION = "/web/enc/lobby/adminGetFreeQuestion.asp";
        public const string URL_ADMIN_SET_FREE_QUESTION = "/web/enc/lobby/adminSetFreeQuestion.asp";
        public const string URL_ADMIN_UPDATE_QUESTIONNAIRE = "/web/enc/lobby/adminUpdateQuestionnaire.asp";
        public const string URL_ADMIN_SET_QUESTION_SERIAL_NO = "/web/enc/lobby/adminSetQuestionSerialNo.asp";
        public const string URL_ADMIN_SET_NEXT_QUESTION_NO = "/web/enc/lobby/adminSetNextQuestionNo.asp";
        public const string URL_ADMIN_SET_QUESTION_NO = "/web/enc/lobby/adminSetQuestionNo.asp";
        public const string URL_ADMIN_SET_SPECIAL_QUESTION_THRESHOLD = "/web/enc/lobby/adminSetSpecialQuestionThreshold.asp";
        public const string URL_ADMIN_SET_DEFAULT_SUMMARIZE_FLAG = "/web/enc/lobby/adminSetDefaultSummarizeFlag.asp";
        public const string URL_ADMIN_INITIALIZE_QUESTIONNAIRE = "/web/enc/lobby/adminInitializeQuestionnaire.asp";

        private LobbySchedules currentSchedules = new LobbySchedules();
        public LobbySchedules CurrentSchedules
        {
            get { return currentSchedules; }
            set
            {
                currentSchedules = value;
                currentSchedules.Sort();
            }
        }


        /// <summary>
        /// ロビースケジュール取得リクエスト
        /// </summary>
        [StructLayout(LayoutKind.Sequential, Pack = 4)]
        internal class LobbyGetScheduleRequest : RequestContainer
        {
            public LobbyGetScheduleRequest() { }
            public LobbyGetScheduleRequest(int _id)
                : base(_id)
            {
            }
        }

        public enum PPW_LOBBY_TIME_EVENT : int
        {
            LOCK,              ///< 部屋のロック。
            NEON_A0,           ///< 室内ネオン諧調0。
            NEON_A1,           ///< 室内ネオン諧調1。
            NEON_A2,           ///< 室内ネオン諧調2。
            NEON_A3,           ///< 室内ネオン諧調3。
            NEON_A4,           ///< 室内ネオン諧調4。
            NEON_A5,           ///< 室内ネオン諧調5。
            NEON_B0,           ///< 床ネオン諧調0。
            NEON_B1,           ///< 床ネオン諧調1。
            NEON_B2,           ///< 床ネオン諧調2。
            NEON_B3,           ///< 床ネオン諧調3。
            NEON_C0,           ///< モニュメント諧調0。
            NEON_C1,           ///< モニュメント諧調1。
            NEON_C2,           ///< モニュメント諧調2。
            NEON_C3,           ///< モニュメント諧調3。
            MINIGAME_END,      ///< ミニゲーム終了。
            FIRE_WORKS_START,  ///< ファイアーワークス開始。
            FIRE_WORKS_END,    ///< ファイアーワークス終了。
            PARADE,            ///< パレード。
            CLOSE              ///< 終了。
        }

        public enum PPW_LOBBY_SEASON : int
        {
            NORMAL,
            SPRING,
            SUMMER,
            AUTUMN,
            WINTER
        }

        /// <summary>
        /// ロビースケジュールレコード
        /// </summary>
        [StructLayout(LayoutKind.Sequential, Pack = 4)]
        [Serializable()]
        public class LobbyScheduleRecord : IComparable<LobbyScheduleRecord>
        {
            ///< イベントが起動する時刻(メインチャンネルをロックしたときからの時間)。
            private int time;
            public int Time
            {
                get { return time; }
                set { time = value; }
            }
                           
            ///< 起動するイベント。
            private PPW_LOBBY_TIME_EVENT timeEvent;
	        public PPW_LOBBY_TIME_EVENT TimeEvent
	        {
		        get { return timeEvent;}
                set
                {
                    if (!Enum.IsDefined(typeof(PPW_LOBBY_TIME_EVENT), value) || PPW_LOBBY_TIME_EVENT.LOCK == value)
                    {
                        throw new InvalidDataException("" + (int)value + " はイベントに変換できません。");
                    }
                    timeEvent = value;
                }
	        }


            public LobbyScheduleRecord()
            {
                time = 0;
                timeEvent = PPW_LOBBY_TIME_EVENT.NEON_A0;
            }

            internal ListViewItem[] ToListViewItems()
            {
                List<ListViewItem> list = new List<ListViewItem>();
                list.Add(new ListViewItem(new String[] { "- time", time.ToString() }));
                list.Add(new ListViewItem(new String[] { "  event", timeEvent.ToString() }));
                return list.ToArray();
            }

            internal List<byte> ToByteArray()
            {
                List<byte> bytes = new List<byte>();
                bytes.AddRange(BitConverter.GetBytes(time));
                bytes.AddRange(BitConverter.GetBytes((int)timeEvent));
                return bytes;
            }

            override public string ToString()
            {
                return "- time: " + time.ToString() + "\n  event: " + timeEvent.ToString();
            }

            #region IComparable メンバ

            public int CompareTo(LobbyScheduleRecord rhs)
            {
                return (time - rhs.time) * 0xffff + ((int)timeEvent - (int)rhs.timeEvent);
            }

            #endregion
        }

        /// <summary>
        /// ロビースケジュール取得レスポンス
        /// </summary>
        [StructLayout(LayoutKind.Sequential, Pack = 4)]
        internal class LobbyGetScheduleResponse : ResponseContainer
        {
            public uint lockTime;                           ///< メインチャンネルをオープンしてからロックするまでの時間(秒)。
            public uint random;                             ///< サーバで生成される32bit範囲のランダム値。
            public uint roomFlag;                           ///< PPW_LOBBY_ROOM_FLAGに対応する各種bitフラグ
            public byte roomType;                           ///< 部屋の種類。
            public byte season;                             ///< 季節番号。
            public ushort scheduleRecordNum;                ///< スケジュールレコードの数。
            public List<LobbyScheduleRecord> scheduleRecords = new List<LobbyScheduleRecord>(); ///< スケジュールレコード

            public LobbyGetScheduleResponse() { }

            internal ListViewItem[] ToListViewItems()
            {
                List<ListViewItem> list = new List<ListViewItem>();
                list.Add(new ListViewItem(new String[] { "lockTime", lockTime.ToString() }));
                list.Add(new ListViewItem(new String[] { "random", random.ToString() }));
                list.Add(new ListViewItem(new String[] { "roomFlag", roomFlag.ToString() }));
                list.Add(new ListViewItem(new String[] { "roomType", roomType.ToString() }));
                list.Add(new ListViewItem(new String[] { "season", season.ToString() }));
                list.Add(new ListViewItem(new String[] { "scheduleRecordNum", scheduleRecordNum.ToString() }));
                foreach (LobbyScheduleRecord scheduleRecord in scheduleRecords)
                {
                    list.AddRange(new List<ListViewItem>(scheduleRecord.ToListViewItems()));
                }
                return list.ToArray();
            }
        }

        /// <summary>
        /// ロビースケジュール設定取得リクエスト
        /// </summary>
        [StructLayout(LayoutKind.Sequential, Pack = 4)]
        internal class LobbyAdminGetScheduleRequest : RequestContainer
        {
            public LobbyAdminGetScheduleRequest() { }
            public LobbyAdminGetScheduleRequest(int _id)
                : base(_id)
            {
            }
        }

        /// <summary>
        /// ロビースケジュール設定ベース
        /// </summary>
        [StructLayout(LayoutKind.Sequential, Pack = 4)]
        [Serializable()]
        public class LobbyAdminScheduleBase
        {
            public byte startMonth;
            public byte startDay;
            public ushort startYear;
            public uint lockTime;                           ///< メインチャンネルをオープンしてからロックするまでの時間(秒)。
            public uint roomFlag;                           ///< PPW_LOBBY_ROOM_FLAGに対応する各種bitフラグ
            [System.Xml.Serialization.XmlElement("roomRateFire")]   // XML要素名変更
            public byte roomRate1;                        ///< 部屋の種類。
            [System.Xml.Serialization.XmlElement("roomRateWater")]
            public byte roomRate2;                        ///< 部屋の種類。
            [System.Xml.Serialization.XmlElement("roomRateThunder")]
            public byte roomRate3;                        ///< 部屋の種類。
            [System.Xml.Serialization.XmlElement("roomRateGrass")]
            public byte roomRate4;                        ///< 部屋の種類。
            [System.Xml.Serialization.XmlElement("roomRateSecret")]
            public byte roomRate5;                        ///< 部屋の種類。
            public byte season;                             ///< 季節番号。
            [System.Xml.Serialization.XmlIgnoreAttribute]   // XMLシリアライズしない
            public ushort scheduleRecordNum;                ///< スケジュールレコードの数。

            public LobbyAdminScheduleBase()
            {
                DateTime now = DateTime.Now;
                startMonth = (byte)now.Month;
                startDay = (byte)now.Day;
                startYear = (ushort)now.Year;
                roomRate1 = 20;
                roomRate2 = 20;
                roomRate3 = 20;
                roomRate4 = 20;
                roomRate5 = 20;
            }

            internal List<byte> ToByteArray()
            {
                List<byte> bytes = new List<byte>();
                bytes.Add((byte)(startMonth - 1));  // サーバでは0-11を要求
                bytes.Add(startDay);
                bytes.AddRange(BitConverter.GetBytes(startYear));
                bytes.AddRange(BitConverter.GetBytes(lockTime));
                bytes.AddRange(BitConverter.GetBytes(roomFlag));
                bytes.Add(roomRate1);
                bytes.Add(roomRate2);
                bytes.Add(roomRate3);
                bytes.Add(roomRate4);
                bytes.Add(roomRate5);
                bytes.Add(season);
                bytes.AddRange(BitConverter.GetBytes(scheduleRecordNum));
                return bytes;
            }

            internal List<string> ToStringArray()
            {
                List<string> list = new List<string>();
                list.Add(startYear.ToString());
                list.Add(startMonth.ToString());
                list.Add(startDay.ToString());
                list.Add(lockTime.ToString());
                list.Add(roomFlag.ToString());
                list.Add(roomRate1.ToString());
                list.Add(roomRate2.ToString());
                list.Add(roomRate3.ToString());
                list.Add(roomRate4.ToString());
                list.Add(roomRate5.ToString());
                list.Add(season.ToString());
                // レコード数は実際のListの数を使う
                //list.Add(scheduleRecordNum.ToString());
                return list;
            }

            internal bool ChangeData(string tag, string data)
            {
                try
                {
                    Type t = this.GetType();
                    FieldInfo info = t.GetField(tag);
                    // dataをセットしたい型にキャストしてセット
                    info.SetValue(this, Convert.ChangeType(Int32.Parse(data), info.FieldType));
                }
                catch (Exception e)
                {
                    MessageBox.Show(e.Message);
                    return false;
                }
                return true;
            }

            internal bool IsConsistent()
            {
                return roomRate1 + roomRate2 + roomRate3 + roomRate4 + roomRate5 == 100 &&
                    startYear >= 2000 && startYear <= 2200 && startMonth >= 1 && startMonth <= 12 && startDay >= 1 && startDay <= 31;
            }
        }


        /// <summary>
        /// ロビースケジュール設定取得レスポンス
        /// </summary>
        [StructLayout(LayoutKind.Sequential, Pack = 4)]
        internal class LobbyAdminGetScheduleResponse : ResponseContainer
        {
            public LobbyAdminScheduleBase scheduleBase;
            public List<LobbyScheduleRecord> scheduleRecords = new List<LobbyScheduleRecord>(); ///< スケジュールレコード

            public LobbyAdminGetScheduleResponse() { }
        }

        /// <summary>
        /// ロビースケジュール設定セットリクエストの一部、高速化のためまとめて送るのでリクエストオブジェクトからは継承しない
        /// </summary>
        [StructLayout(LayoutKind.Sequential, Pack = 4)]
        [System.Xml.Serialization.XmlRoot("LobbySchedule")]
        [Serializable()]
        public class LobbyAdminSetScheduleRequestPart : ICloneable, IComparable<LobbyAdminSetScheduleRequestPart>
        {
            public const int OBJECT_SIZE = 340;
            public LobbyAdminScheduleBase scheduleBase = new LobbyAdminScheduleBase();

            [System.Xml.Serialization.XmlElement("LobbyScheduleRecord", Type = typeof(LobbyScheduleRecord))]
            public List<LobbyScheduleRecord> scheduleRecords = new List<LobbyScheduleRecord>(); ///< スケジュールレコード

            public DateTime StartDate
            {
                get { return new DateTime(scheduleBase.startYear, scheduleBase.startMonth, scheduleBase.startDay); }
                set
                {
                    scheduleBase.startYear = (ushort)value.Year;
                    scheduleBase.startMonth = (byte)value.Month;
                    scheduleBase.startDay = (byte)value.Day;
                }
            }

            public uint LockTime
            {
                get { return scheduleBase.lockTime; }
                set { scheduleBase.lockTime = value; }
            }

            public uint RoomFlag
            {
                get { return scheduleBase.roomFlag; }
                set { scheduleBase.roomFlag = value; }
            }

            public byte RoomRate1
            {
                get { return scheduleBase.roomRate1; }
                set { scheduleBase.roomRate1 = value; }
            }

            public byte RoomRate2
            {
                get { return scheduleBase.roomRate2; }
                set { scheduleBase.roomRate2 = value; }
            }

            public byte RoomRate3
            {
                get { return scheduleBase.roomRate3; }
                set { scheduleBase.roomRate3 = value; }
            }

            public byte RoomRate4
            {
                get { return scheduleBase.roomRate4; }
                set { scheduleBase.roomRate4 = value; }
            }

            public byte RoomRate5
            {
                get { return scheduleBase.roomRate5; }
                set { scheduleBase.roomRate5 = value; }
            }

            public byte Season
            {
                get { return scheduleBase.season; }
                set { scheduleBase.season = value; }
            }

            public PPW_LOBBY_SEASON EnumSeason
            {
                get { return (PPW_LOBBY_SEASON)scheduleBase.season; }
                set
                {
                    if (!Enum.IsDefined(typeof(PPW_LOBBY_SEASON), value))
                    {
                        throw new InvalidDataException("" + value + " は季節に変換できません。");
                    }
                    scheduleBase.season = (byte)value;
                }
            }


            public ushort ScheduleRecordNum
            {
                get { return (ushort)scheduleRecords.Count; }
                set { }
            }

            // scheduleRecordsをコピー用(TSV)にシリアライズ
            [System.Xml.Serialization.XmlIgnoreAttribute]   // XMLシリアライズしない
            public string ScheduleRecordsText
            {
                get
                {
                    System.Xml.Serialization.XmlSerializer serializer = new System.Xml.Serialization.XmlSerializer(scheduleRecords.GetType());
                    using (System.IO.MemoryStream ms = new System.IO.MemoryStream())
                    {
                        serializer.Serialize(ms, scheduleRecords);

                        // 巻き戻し
                        ms.Seek(0, SeekOrigin.Begin);

                        using (StreamReader reader = new StreamReader(ms))
                        {
                            // Just read to the end.
                            string result;
                            result = reader.ReadToEnd();
                            result = result.Replace("\r", "");
                            result = result.Replace("\n", "");
                            return result;
                        }
                    }
                }
                set
                {
                    System.Xml.Serialization.XmlSerializer serializer = new System.Xml.Serialization.XmlSerializer(scheduleRecords.GetType());
                    using (System.IO.MemoryStream ms = new System.IO.MemoryStream(Encoding.Unicode.GetBytes(value)))
                    {
                        this.scheduleRecords = (List<LobbyScheduleRecord>)serializer.Deserialize(ms);
                    }
                }
            }

            public LobbyAdminSetScheduleRequestPart()
            {
            }

            public void StoreXML(string filePath)
            {
                System.Xml.Serialization.XmlSerializer serializer = new System.Xml.Serialization.XmlSerializer(typeof(LobbyAdminSetScheduleRequestPart));
                using (System.IO.FileStream fs = new System.IO.FileStream(filePath, System.IO.FileMode.Create))
                {
                    serializer.Serialize(fs, this);
                }
            }

            public static LobbyAdminSetScheduleRequestPart LoadXML(string filePath)
            {
                System.Xml.Serialization.XmlSerializer serializer = new System.Xml.Serialization.XmlSerializer(typeof(LobbyAdminSetScheduleRequestPart));
                LobbyAdminSetScheduleRequestPart loadedObj;
                using (System.IO.FileStream fs = new System.IO.FileStream(filePath, System.IO.FileMode.Open))
                {
                    loadedObj = (LobbyAdminSetScheduleRequestPart)serializer.Deserialize(fs);
                }
                return loadedObj;
            }

            public static LobbyAdminSetScheduleRequestPart PtrToStructure(IntPtr ptr)
            {
                LobbyAdminSetScheduleRequestPart result = new LobbyAdminSetScheduleRequestPart();

                // 手動で構造体構築
                int baseAddr = ptr.ToInt32();
                result.scheduleBase = (LobbyAdminScheduleBase)Marshal.PtrToStructure(ptr, typeof(LobbyAdminScheduleBase));
                result.scheduleBase.startMonth++;   // サーバは0-11を返してくるので修正

                LobbyScheduleRecord data;

                for (int i = 0; i < result.scheduleBase.scheduleRecordNum; i++)
                {
                    data = (LobbyScheduleRecord)Marshal.PtrToStructure(
                        new IntPtr(baseAddr + Marshal.SizeOf(typeof(LobbyAdminScheduleBase)) + (i * Marshal.SizeOf(typeof(LobbyScheduleRecord)))),
                        typeof(LobbyScheduleRecord));
                    //data.ntoh();
                    result.scheduleRecords.Add(data);
                }
                return result;
            }

            internal List<byte> ToByteArray()
            {
                List<byte> bytes = new List<byte>();
                scheduleBase.scheduleRecordNum = (ushort)scheduleRecords.Count;
                bytes.AddRange(scheduleBase.ToByteArray());
                for (int i = 0; i < 40; i++)
                {
                    if (i < scheduleBase.scheduleRecordNum)
                    {
                        bytes.AddRange(scheduleRecords[i].ToByteArray());
                    }
                    else
                    {
                        // あまりは空データをつっこむ
                        bytes.AddRange(BitConverter.GetBytes((int)0));
                        bytes.AddRange(BitConverter.GetBytes((int)0));
                    }
                }
                return bytes;
            }

            public string[] ToStringArray()
            {
                List<string> list = scheduleBase.ToStringArray();
                list.Add("" + scheduleRecords.Count);
                return list.ToArray();
            }

            internal string ToToolTipText()
            {
                string result = "";
                foreach (LobbyScheduleRecord record in scheduleRecords)
                {
                    result += record.ToString() + "\n";
                }
                return result;
            }

            internal bool ChangeData(string tag, string data)
            {
                return scheduleBase.ChangeData(tag, data);
            }

            internal bool IsConsistent()
            {
                return scheduleBase.IsConsistent() && scheduleRecords.Count <= 40;
            }
            
            #region ICloneable メンバ

            public object Clone()
            {
                return DeepCopy(this);
            }

            #endregion

            #region IComparable メンバ

            public int CompareTo(LobbyAdminSetScheduleRequestPart rhs)
            {
                // 日付でソート
                int score = (this.scheduleBase.startYear - rhs.scheduleBase.startYear) * 10000
                    + (this.scheduleBase.startMonth - rhs.scheduleBase.startMonth) * 100
                    + (this.scheduleBase.startDay - rhs.scheduleBase.startDay);
                if (score == 0)
                {
                    // せめて起動中だけは安定ソートになるように
                    return this.GetHashCode() - rhs.GetHashCode();
                }
                return score;
            }

            #endregion
        }

        /// <summary>
        /// ロビースケジュール設定セットリクエスト
        /// </summary>
        [StructLayout(LayoutKind.Sequential, Pack = 4)]
        [Serializable()]
        public class LobbySchedules : ICloneable

        {
            [System.Xml.Serialization.XmlElement("LobbySchedule", Type = typeof(LobbyAdminSetScheduleRequestPart))]
            public List<LobbyAdminSetScheduleRequestPart> lobbySchedules = new List<LobbyAdminSetScheduleRequestPart>();

            public LobbySchedules()
            {
            }

            public LobbySchedules(LobbyAdminSetScheduleRequestPart[] ary)
            {
                lobbySchedules = new List<LobbyAdminSetScheduleRequestPart>(ary);
            }

            public void Sort()
            {
                lobbySchedules.Sort();
            }

            public static LobbyAdminSetScheduleRequestPart[] PtrToStructure(IntPtr ptr, uint outSize)
            {
                if (outSize == 0 || outSize % LobbyAdminSetScheduleRequestPart.OBJECT_SIZE != 0)
                {
                    throw new Exception("invalid size: " + outSize);
                }
                uint itemNum = (uint)(outSize / LobbyAdminSetScheduleRequestPart.OBJECT_SIZE);

                List<LobbyAdminSetScheduleRequestPart> list = new List<LobbyAdminSetScheduleRequestPart>();
                for (uint i = 0; i < itemNum; i++)
                {
                    list.Add(LobbyAdminSetScheduleRequestPart.PtrToStructure(ptr));
                    ptr = new IntPtr(ptr.ToInt32() + LobbyAdminSetScheduleRequestPart.OBJECT_SIZE);
                }
                return list.ToArray();
            }

            // スケジュールごとのbyteのリストにして返す
            internal List<List<byte>> ToByteArrays()
            {
                List<List<byte>> result = new List<List<byte>>();
                foreach (LobbyAdminSetScheduleRequestPart item in lobbySchedules)
                {
                    List<byte> bytes = new List<byte>();
                    bytes.AddRange(item.ToByteArray());
                    result.Add(bytes);
                }
                return result;
            }

            internal bool ChangeData(int row, string tag, string data)
            {
                if (row >= lobbySchedules.Count)
                {
                    return false;
                }
                return lobbySchedules[row].ChangeData(tag, data);
            }

            internal void AddNewSchedule()
            {
                lobbySchedules.Add(new LobbyAdminSetScheduleRequestPart());
            }

            internal int GetScheduleHashCode(int index)
            {
                return lobbySchedules[index].GetHashCode();
            }

            internal int FindSchedule(int hash)
            {
                return lobbySchedules.FindIndex(delegate(LobbyAdminSetScheduleRequestPart item)
                {
                    return item.GetHashCode() == hash;
                });
            }

            internal void DeleteSchedule(int index)
            {
                lobbySchedules.RemoveAt(index);
            }

            internal void CopySchedule(int index)
            {
                LobbyAdminSetScheduleRequestPart schedule = lobbySchedules[index];
                lobbySchedules.Insert(index, (LobbyAdminSetScheduleRequestPart)DeepCopy(schedule));
            }

            internal bool IsConsistent()
            {
                if (lobbySchedules.Count == 0)
                {
                    return false;
                }

                // 最初のスケジュールが未来のものなら不正
                LobbyAdminScheduleBase scheduleBase = lobbySchedules[0].scheduleBase;
                DateTime scheduleTime;
                try
                {
                    scheduleTime = new DateTime(scheduleBase.startYear, scheduleBase.startMonth, scheduleBase.startDay);
                }
                catch (ArgumentOutOfRangeException)
                {
                    // 不正な日付
                    return false;
                }

                DateTime currentTime = DateTime.Now;
                if (currentTime.CompareTo(scheduleTime) < 0)
                {
                    return false;
                }

                foreach (LobbyAdminSetScheduleRequestPart schedule in lobbySchedules)
                {
                    if (!schedule.IsConsistent())
                    {
                        return false;
                    }
                }
                return true;
            }

            #region ICloneable メンバ

            public object Clone()
            {
                return DeepCopy(this);
            }

            #endregion
        }

        /// <summary>
        /// ロビースケジュール設定セットレスポンス
        /// </summary>
        [StructLayout(LayoutKind.Sequential, Pack = 4)]
        internal class LobbyAdminSetScheduleResponse : ResponseContainer
        {
            public LobbyAdminSetScheduleResponse() { }
        }

        /// <summary>
        /// VIPレコード
        /// </summary>
        [StructLayout(LayoutKind.Sequential, Pack = 4)]
        internal class LobbyVipRecord
        {
            private int profileId;
            public int ProfileId
            {
                get { return profileId; }
                set { profileId = value; }
            }

            private int key;
            public int Key
            {
                get { return key; }
                set { key = value; }
            }


            public LobbyVipRecord() { }
        }

        /// <summary>
        /// VIPレコード
        /// </summary>
        [Serializable]
        [StructLayout(LayoutKind.Sequential, Pack = 4)]
        public class LobbyVipSettingRecord : IComparable<LobbyVipSettingRecord>, ICloneable
        {
            private byte startMonth;                         ///< 
            public byte StartMonth
            {
                get { return startMonth; }
                set { startMonth = value; }
            }

            private byte startDay;                           ///< 
            public byte StartDay
            {
                get { return startDay; }
                set { startDay = value; }
            }

            private ushort startYear;                        ///< 
            public ushort StartYear
            {
                get { return startYear; }
                set { startYear = value; }
            }

            public DateTime StartDate
            {
                get { return new DateTime(startYear, startMonth, startDay); }
                set
                {
                    startYear = (ushort)value.Year;
                    startMonth = (byte)value.Month;
                    startDay = (byte)value.Day;
                }
            }

            private byte endMonth;                           ///< 
            public byte EndMonth
            {
                get { return endMonth; }
                set { endMonth = value; }
            }

            private byte endDay;
            public byte EndDay
            {
                get { return endDay; }
                set { endDay = value; }
            }

            private ushort endYear;                          ///< 
            public ushort EndYear
            {
                get { return endYear; }
                set { endYear = value; }
            }

            public DateTime EndDate
            {
                get { return new DateTime(endYear, endMonth, endDay); }
                set
                {
                    endYear = (ushort)value.Year;
                    endMonth = (byte)value.Month;
                    endDay = (byte)value.Day;
                }
            }

            private int profileId;
            public int ProfileId
            {
                get { return profileId; }
                set { profileId = value; }
            }

            private int eventId;
            public int EventId
            {
                get { return eventId; }
                set { eventId = value; }
            }

            public LobbyVipSettingRecord()
            {
                // DGV使用時のデフォルトデータを設定
                DateTime now = DateTime.Now;
                startMonth = (byte)now.Month;
                startDay = (byte)now.Day;
                startYear = (ushort)now.Year;
                now = now.AddDays(1);
                endMonth = (byte)now.Month;
                endDay = (byte)now.Day;
                endYear = (ushort)now.Year;
                profileId = 100000;
                eventId = 0;
            }

            internal List<byte> ToByteArray()
            {
                List<byte> bytes = new List<byte>();
                bytes.Add((byte)(startMonth - 1));  // サーバでは0-11を要求
                bytes.Add(startDay);
                bytes.AddRange(BitConverter.GetBytes(startYear));
                bytes.Add((byte)(endMonth - 1));  // サーバでは0-11を要求
                bytes.Add(endDay);
                bytes.AddRange(BitConverter.GetBytes(endYear));
                bytes.AddRange(BitConverter.GetBytes(profileId));
                bytes.AddRange(BitConverter.GetBytes(eventId));
                return bytes;
            }

            internal bool IsConsistent()
            {
                DateTime startTime;
                DateTime endTime;
                try
                {
                    startTime = new DateTime(startYear, startMonth, startDay);
                    endTime = new DateTime(endYear, endMonth, endDay);
                }
                catch (ArgumentOutOfRangeException)
                {
                    // 不正な日付
                    return false;
                }
                if (startTime >= endTime)
                {
                    return false;
                }

                return startYear >= 2000 && startYear <= 2200 && startMonth >= 1 && startMonth <= 12 && startDay >= 1 && startDay <= 31
                    && endYear >= 2000 && endYear <= 2200 && endMonth >= 1 && endMonth <= 12 && endDay >= 1 && endDay <= 31
                    && profileId >= 100000;
            }

            #region IComparable<LobbyVipSettingRecord> メンバ

            public int CompareTo(LobbyVipSettingRecord other)
            {
                // 開始日順にする
                return (this.startYear * 10000 + this.startMonth * 100 + this.startDay)
                     - (other.startYear * 10000 + other.startMonth * 100 + other.startDay);
            }

            #endregion

            #region ICloneable メンバ

            public object Clone()
            {
                return DeepCopy(this);
            }

            #endregion
        }

        /// <summary>
        /// VIP取得リクエスト
        /// </summary>
        [StructLayout(LayoutKind.Sequential, Pack = 4)]
        internal class LobbyGetVipRequest : RequestContainer
        {
            uint trainerId;
            public LobbyGetVipRequest() { }
            public LobbyGetVipRequest(int _id, uint _trainerId)
                : base(_id)
            {
                trainerId = _trainerId;
            }
        }

        /// <summary>
        /// VIP取得レスポンス
        /// </summary>
        [StructLayout(LayoutKind.Sequential, Pack = 4)]
        internal class LobbyGetVipResponse : ResponseContainer
        {
            public List<LobbyVipRecord> vipRecords = new List<LobbyVipRecord>();

            public LobbyGetVipResponse() { }
        }

        /// <summary>
        /// VIP設定取得リクエスト
        /// </summary>
        [StructLayout(LayoutKind.Sequential, Pack = 4)]
        internal class LobbyAdminGetVipSettingRequest : RequestContainer
        {
            public LobbyAdminGetVipSettingRequest() { }
            public LobbyAdminGetVipSettingRequest(int _id)
                : base(_id)
            {
            }
        }

        /// <summary>
        /// VIP設定取得レスポンス
        /// </summary>
        [StructLayout(LayoutKind.Sequential, Pack = 4)]
        internal class LobbyAdminGetVipSettingResponse : ResponseContainer
        {
            public List<LobbyVipSettingRecord> vipSettingRecords = new List<LobbyVipSettingRecord>();

            public LobbyAdminGetVipSettingResponse() { }
        }

        /// <summary>
        /// VIP設定登録リクエスト
        /// </summary>
        [System.Xml.Serialization.XmlRoot("VipSetting")]
        [StructLayout(LayoutKind.Sequential, Pack = 4)]
        public class LobbyAdminSetVipSettingRequest : RequestContainer
        {
            public List<LobbyVipSettingRecord> vipSettingRecords = new List<LobbyVipSettingRecord>();

            public LobbyAdminSetVipSettingRequest() { }
            public LobbyAdminSetVipSettingRequest(int _id, List<LobbyVipSettingRecord> src)
                : base(_id)
            {
                vipSettingRecords = src;
            }

            internal override List<byte> ToByteArray()
            {
                // 先頭に共通ヘッダを付ける
                List<byte> result = base.ToByteArray();

                foreach (LobbyVipSettingRecord item in vipSettingRecords)
                {
                    result.AddRange(item.ToByteArray());
                }
                return result;
            }

            internal bool IsConsistent()
            {
                foreach (LobbyVipSettingRecord item in vipSettingRecords)
                {
                    if (!item.IsConsistent())
                    {
                        return false;
                    }
                }
                return true;
            }

        }

        /// <summary>
        /// VIP設定登録レスポンス
        /// </summary>
        [StructLayout(LayoutKind.Sequential, Pack = 4)]
        internal class LobbyAdminSetVipSettingResponse : ResponseContainer
        {
            public LobbyAdminSetVipSettingResponse() { }
        }



        internal LobbyGetScheduleResponse LobbyGetSchedule(out string message)
        {
            int baseStructureOffset = 20;
            LobbyGetScheduleRequest request = new LobbyGetScheduleRequest(ADMIN_PROFILE_ID);
            LobbyGetScheduleResponse result = new LobbyGetScheduleResponse();
            IntPtr ptr;
            uint outSize;
            message = "";

            //request.hton();

            encSession.Session(URL_GET_LOBBY_SCHEDULE, ADMIN_PROFILE_ID, request, out ptr, out outSize);

            // レスポンスコードのチェック
            if (outSize < Marshal.SizeOf(result.code))
            {
                throw new Exception("レスポンスサイズが不正です。レスポンスコードが含まれていません。");
            }
            else
            {
                result.code = (uint)Marshal.ReadInt32(ptr);
                //result.ntoh();
                if (result.code != 0)
                {
                    // メッセージの取り出し
                    message = Marshal.PtrToStringAnsi(new IntPtr(ptr.ToInt32() + Marshal.SizeOf(result.code)), 128);
                    return result;
                }
            }

            if (outSize < baseStructureOffset)
            {
                throw new Exception("レスポンスサイズが不正です。LobbyGetScheduleResponse構造体の最低サイズを満たしていません。");
            }

            // 手動で構造体構築
            int baseAddr = ptr.ToInt32();
            result.lockTime = (uint)Marshal.ReadInt32(new IntPtr(baseAddr + 4));
            result.random = (uint)Marshal.ReadInt32(new IntPtr(baseAddr + 8));
            result.roomFlag = (uint)Marshal.ReadInt32(new IntPtr(baseAddr + 12));
            result.roomType = Marshal.ReadByte(new IntPtr(baseAddr + 16));
            result.season = Marshal.ReadByte(new IntPtr(baseAddr + 17));
            result.scheduleRecordNum = (ushort)Marshal.ReadInt16(new IntPtr(baseAddr + 18));
            //result.ntoh();

            LobbyScheduleRecord data;

            // サイズチェック
            if ((outSize - baseStructureOffset) % Marshal.SizeOf(typeof(LobbyScheduleRecord)) == 0 &&
                (outSize - baseStructureOffset) / Marshal.SizeOf(typeof(LobbyScheduleRecord)) == result.scheduleRecordNum)
            {
                for (int i = 0; i < result.scheduleRecordNum; i++)
                {
                    data = (LobbyScheduleRecord)Marshal.PtrToStructure(
                        new IntPtr(baseAddr + baseStructureOffset + (i * Marshal.SizeOf(typeof(LobbyScheduleRecord)))), typeof(LobbyScheduleRecord));
                    //data.ntoh();
                    result.scheduleRecords.Add(data);
                }
            }
            else
            {
                throw new Exception("レスポンスサイズが不正です。PPW_LobbyScheduleRecord構造体の倍数ではありません。");
            }
            return result;
        }

        /// <summary>
        /// 不正チェックリクエスト
        /// </summary>
        [StructLayout(LayoutKind.Sequential, Pack = 4)]
        internal class LobbyAdminCheckProfileRequest : RequestContainer
        {
            REF_WFLBY_USER_PROFILE profile;

            public LobbyAdminCheckProfileRequest() { }
            public LobbyAdminCheckProfileRequest(int _id, REF_WFLBY_USER_PROFILE profile_)
                : base(_id)
            {
                profile = profile_;
            }

            internal override List<byte> ToByteArray()
            {
                List<byte> result = base.ToByteArray();
                result.AddRange(profile.ToByteArray());
                return result;
            }
        }

        /// <summary>
        /// 不正チェックレスポンス
        /// </summary>
        [StructLayout(LayoutKind.Sequential, Pack = 4)]
        internal class LobbyAdminCheckProfileResponse : ResponseContainer
        {
            public REF_WFLBY_USER_PROFILE profile;
            public LobbyAdminCheckProfileResponse(uint code, IntPtr ptr, int size)
                : base(code)
            {
                profile = new REF_WFLBY_USER_PROFILE(ptr, size);
            }

            public static LobbyAdminCheckProfileResponse Result(IntPtr ptr, int size, out uint code, out string message)
            {
                code = 0;
                message = "";

                // レスポンスコードのチェック
                if (size < Marshal.SizeOf(code))
                {
                    throw new Exception("レスポンスサイズが不正です。レスポンスコードが含まれていません。");
                }
                code = (uint)Marshal.ReadInt32(ptr);
                if (code != 0)
                {
                    // メッセージの取り出し
                    message = Marshal.PtrToStringAnsi(new IntPtr(ptr.ToInt32() + Marshal.SizeOf(code)), 128);
                    return null;
                }

                return new LobbyAdminCheckProfileResponse(code, new IntPtr(ptr.ToInt32() + Marshal.SizeOf(code)), size - Marshal.SizeOf(code));
            }
        }

        /// <summary>
        /// アンケート情報取得リクエスト
        /// </summary>
        [StructLayout(LayoutKind.Sequential, Pack = 4)]
        internal class LobbyGetQuestionnaireRequest : RequestContainer
        {
            public LobbyGetQuestionnaireRequest() { }
            public LobbyGetQuestionnaireRequest(int _id)
                : base(_id)
            {
            }
        }

        /// <summary>
        /// アンケート情報取得レスポンス
        /// </summary>
        [StructLayout(LayoutKind.Sequential, Pack = 4)]
        internal class LobbyGetQuestionnaireResponse : ResponseContainer
        {
            public REF_PPW_LobbyQuestionnaire questionnaire;
            public LobbyGetQuestionnaireResponse(uint code, IntPtr ptr, int size)
                : base(code)
            {
                questionnaire = new REF_PPW_LobbyQuestionnaire(ptr, size);
            }

            public static LobbyGetQuestionnaireResponse Result(IntPtr ptr, int size, out uint code, out string message)
            {
                code = 0;
                message = "";

                // レスポンスコードのチェック
                if (size < Marshal.SizeOf(code))
                {
                    throw new Exception("レスポンスサイズが不正です。レスポンスコードが含まれていません。");
                }
                code = (uint)Marshal.ReadInt32(ptr);
                if (code != 0)
                {
                    // メッセージの取り出し
                    message = Marshal.PtrToStringAnsi(new IntPtr(ptr.ToInt32() + Marshal.SizeOf(code)), 128);
                    return null;
                }

                return new LobbyGetQuestionnaireResponse(code, new IntPtr(ptr.ToInt32() + Marshal.SizeOf(code)), size - Marshal.SizeOf(code));
            }
        }

        [StructLayout(LayoutKind.Sequential, Pack = 4)]
        public class LobbySpecialWeekSetting
        {
            public List<REF_PPW_LobbySpecialWeekRecord> records = new List<REF_PPW_LobbySpecialWeekRecord>();
            public LobbySpecialWeekSetting()
            {
            }
            public LobbySpecialWeekSetting(List<REF_PPW_LobbySpecialWeekRecord> records_)
            {
                records = records_;
            }

            internal IEnumerable<byte> ToByteArray()
            {
                List<byte> result = new List<byte>();
                foreach (REF_PPW_LobbySpecialWeekRecord record in records)
                {
                    result.AddRange(record.ToByteArray());
                }
                return result;
            }

            internal bool IsConsistent()
            {
                foreach (REF_PPW_LobbySpecialWeekRecord item in records)
                {
                    if (!item.IsConsistent())
                    {
                        return false;
                    }
                }
                return true;
            }
        }

        /// <summary>
        /// スペシャルウィーク取得リクエスト
        /// </summary>
        [StructLayout(LayoutKind.Sequential, Pack = 4)]
        internal class LobbyAdminGetSpecialWeekRequest : RequestContainer
        {
            public LobbyAdminGetSpecialWeekRequest() { }
            public LobbyAdminGetSpecialWeekRequest(int _id)
                : base(_id)
            {
            }
        }

        /// <summary>
        /// スペシャルウィーク取得レスポンス
        /// </summary>
        [StructLayout(LayoutKind.Sequential, Pack = 4)]
        internal class LobbyAdminGetSpecialWeekResponse : ResponseContainer
        {
            public LobbySpecialWeekSetting specialWeek = new LobbySpecialWeekSetting();
            public LobbyAdminGetSpecialWeekResponse(uint code, IntPtr ptr, int size)
                : base(code)
            {
                if (size % REF_PPW_LobbySpecialWeekRecord.SPECIAL_RECORD_SIZE != 0)
                {
                    throw new Exception("レスポンスサイズが不正です。PPW_LobbySpecialWeekRecord構造体の倍数ではありません。");
                }

                while (size != 0)
                {
                    specialWeek.records.Add(new REF_PPW_LobbySpecialWeekRecord(ptr, REF_PPW_LobbySpecialWeekRecord.SPECIAL_RECORD_SIZE));
                    ptr = new IntPtr(ptr.ToInt32() + REF_PPW_LobbySpecialWeekRecord.SPECIAL_RECORD_SIZE);
                    size -= REF_PPW_LobbySpecialWeekRecord.SPECIAL_RECORD_SIZE;
                }
            }

            public static LobbyAdminGetSpecialWeekResponse Result(IntPtr ptr, int size, out uint code, out string message)
            {
                code = 0;
                message = "";

                // レスポンスコードのチェック
                if (size < Marshal.SizeOf(code))
                {
                    throw new Exception("レスポンスサイズが不正です。レスポンスコードが含まれていません。");
                }
                code = (uint)Marshal.ReadInt32(ptr);
                if (code != 0)
                {
                    // メッセージの取り出し
                    message = Marshal.PtrToStringAnsi(new IntPtr(ptr.ToInt32() + Marshal.SizeOf(code)), 128);
                    return null;
                }

                return new LobbyAdminGetSpecialWeekResponse(code, new IntPtr(ptr.ToInt32() + Marshal.SizeOf(code)), size - Marshal.SizeOf(code));
            }
        }


        /// <summary>
        /// スペシャルウィーク登録リクエスト
        /// </summary>
        [StructLayout(LayoutKind.Sequential, Pack = 4)]
        public class LobbyAdminSetSpecialWeekRequest : RequestContainer
        {
            public LobbySpecialWeekSetting specialWeek = new LobbySpecialWeekSetting();

            public LobbyAdminSetSpecialWeekRequest() { }
            public LobbyAdminSetSpecialWeekRequest(int _id, LobbySpecialWeekSetting specialWeek_)
                : base(_id)
            {
                specialWeek = specialWeek_;
            }

            internal override List<byte> ToByteArray()
            {
                List<byte> result = base.ToByteArray();
                result.AddRange(specialWeek.ToByteArray());
                return result;
            }

            internal bool IsConsistent()
            {
                return specialWeek.IsConsistent();
            }

        }

        /// <summary>
        /// スペシャルウィーク登録レスポンス
        /// </summary>
        [StructLayout(LayoutKind.Sequential, Pack = 4)]
        internal class LobbyAdminSetSpecialWeekResponse : ResponseContainer
        {
            public LobbyAdminSetSpecialWeekResponse() { }

            public static void Result(IntPtr ptr, int size, out uint code, out string message)
            {
                code = 0;
                message = "";

                // レスポンスコードのチェック
                if (size < Marshal.SizeOf(code))
                {
                    throw new Exception("レスポンスサイズが不正です。レスポンスコードが含まれていません。");
                }
                code = (uint)Marshal.ReadInt32(ptr);
                if (code != 0)
                {
                    // メッセージの取り出し
                    message = Marshal.PtrToStringAnsi(new IntPtr(ptr.ToInt32() + Marshal.SizeOf(code)), 128);
                    return;
                }
            }
        }


        [StructLayout(LayoutKind.Sequential, Pack = 4)]
        public class LobbyFreeQuestionSetting
        {
            public List<REF_PPW_LobbyFreeQuestion> records = new List<REF_PPW_LobbyFreeQuestion>();
            public LobbyFreeQuestionSetting() { }
            public LobbyFreeQuestionSetting(int language)
            {
                for (int i = 0; i < REF_PPW_LobbySpecialWeekRecord.LOBBY_FREE_QUESTION_NUM; i++)
                {
                    List<string> answers = new List<string>();
                    answers.Add("かいとう1");
                    answers.Add("かいとう2");
                    answers.Add("かいとう3");

                    records.Add(new REF_PPW_LobbyFreeQuestion(REF_PPW_LobbySpecialWeekRecord.LOBBY_FREE_QUESTION_START_NO + i,
                        language,
                        "しつもんぶん",
                        answers));
                }
            }

            public LobbyFreeQuestionSetting(IntPtr ptr, int size)
            {
                while (size != 0)
                {
                    records.Add(new REF_PPW_LobbyFreeQuestion(ptr, REF_PPW_LobbyFreeQuestion.NATIVE_SIZE));
                    ptr = new IntPtr(ptr.ToInt32() + REF_PPW_LobbyFreeQuestion.NATIVE_SIZE);
                    size -= REF_PPW_LobbyFreeQuestion.NATIVE_SIZE;
                }
            }

            public LobbyFreeQuestionSetting(List<REF_PPW_LobbyFreeQuestion> records_)
            {
                records = records_;
            }

            /*
            public LobbyFreeQuestionSetting(int questionNo, int language, DataGridView dgv)
            {
                for (int i = 0; i < dgv.Rows.Count; i++)
                {
                    string cur = "";
                    string questionSentence = "";
                    string answer1 = "";
                    string answer2 = "";
                    string answer3 = "";
                    try
                    {
                        cur = dgv[0, i].Value.ToString();
                        questionSentence = DpwUtil.Tools.UnicodeToPokecode(cur);
                        cur = dgv[1, i].Value.ToString();
                        answer1 = DpwUtil.Tools.UnicodeToPokecode(cur);
                        cur = dgv[2, i].Value.ToString();
                        answer2 = DpwUtil.Tools.UnicodeToPokecode(cur);
                        cur = dgv[3, i].Value.ToString();
                        answer3 = DpwUtil.Tools.UnicodeToPokecode(cur);
                    }
                    catch (Exception e)
                    {
                        MessageBox.Show("「" + cur + "」はゲーム内文字コードへ変換できません。");
                        throw e;
                    }
                    List<ushort> questionSentenceList = Util.BytesToShorts(new List<byte>(Encoding.Unicode.GetBytes(questionSentence)), false);
                    List<ushort> answer1List = Util.BytesToShorts(new List<byte>(Encoding.Unicode.GetBytes(answer1)), false);
                    List<ushort> answer2List = Util.BytesToShorts(new List<byte>(Encoding.Unicode.GetBytes(answer2)), false);
                    List<ushort> answer3List = Util.BytesToShorts(new List<byte>(Encoding.Unicode.GetBytes(answer3)), false);
                    List<List<ushort>> answer = new List<List<ushort>>();
                    answer.Add(answer1List);
                    answer.Add(answer2List);
                    answer.Add(answer3List);
                    records.Add(new REF_PPW_LobbyFreeQuestion(questionNo, language, questionSentenceList, answer));
                }
            }
             * */

            internal IEnumerable<byte> ToByteArray()
            {
                List<byte> result = new List<byte>();
                foreach (REF_PPW_LobbyFreeQuestion record in records)
                {
                    result.AddRange(record.ToByteArray());
                }
                return result;
            }

            internal bool IsConsistent()
            {
                foreach (REF_PPW_LobbyFreeQuestion item in records)
                {
                    if (!item.IsConsistent())
                    {
                        return false;
                    }
                }
                return true;
            }
        }

        /// <summary>
        /// 任意質問登録リクエスト
        /// </summary>
        [StructLayout(LayoutKind.Sequential, Pack = 4)]
        public class LobbyAdminSetFreeQuestionRequest : RequestContainer
        {
            public LobbyFreeQuestionSetting freeQuestion;

            public LobbyAdminSetFreeQuestionRequest(int _id, LobbyFreeQuestionSetting freeQuestion_)
                : base(_id)
            {
                freeQuestion = freeQuestion_;
            }

            internal override List<byte> ToByteArray()
            {
                List<byte> result = base.ToByteArray();
                result.AddRange(freeQuestion.ToByteArray());
                return result;
            }

            internal bool IsConsistent()
            {
                return freeQuestion.IsConsistent();
            }

        }

        /// <summary>
        /// 任意質問登録レスポンス
        /// </summary>
        [StructLayout(LayoutKind.Sequential, Pack = 4)]
        internal class LobbyAdminSetFreeQuestionResponse : ResponseContainer
        {
            public LobbyAdminSetFreeQuestionResponse() { }

            public static void Result(IntPtr ptr, int size, out uint code, out string message)
            {
                code = 0;
                message = "";

                // レスポンスコードのチェック
                if (size < Marshal.SizeOf(code))
                {
                    throw new Exception("レスポンスサイズが不正です。レスポンスコードが含まれていません。");
                }
                code = (uint)Marshal.ReadInt32(ptr);
                if (code != 0)
                {
                    // メッセージの取り出し
                    message = Marshal.PtrToStringAnsi(new IntPtr(ptr.ToInt32() + Marshal.SizeOf(code)), 128);
                    return;
                }
            }
        }

        /// <summary>
        /// 任意質問取得リクエスト
        /// </summary>
        [StructLayout(LayoutKind.Sequential, Pack = 4)]
        internal class LobbyAdminGetFreeQuestionRequest : RequestContainer
        {
            int languageForGet;
            public LobbyAdminGetFreeQuestionRequest() { }
            public LobbyAdminGetFreeQuestionRequest(int _id, int languageForGet_)
                : base(_id)
            {
                languageForGet = languageForGet_;
            }
        }

        /// <summary>
        /// 任意質問取得レスポンス
        /// </summary>
        [StructLayout(LayoutKind.Sequential, Pack = 4)]
        internal class LobbyAdminGetFreeQuestionResponse : ResponseContainer
        {
            public LobbyFreeQuestionSetting freeQuestion;
            public LobbyAdminGetFreeQuestionResponse(uint code, IntPtr ptr, int size)
                : base(code)
            {
                freeQuestion = new LobbyFreeQuestionSetting(ptr, size);
            }

            public static LobbyAdminGetFreeQuestionResponse Result(IntPtr ptr, int size, out uint code, out string message)
            {
                code = 0;
                message = "";

                // レスポンスコードのチェック
                if (size < Marshal.SizeOf(code))
                {
                    throw new Exception("レスポンスサイズが不正です。レスポンスコードが含まれていません。");
                }
                code = (uint)Marshal.ReadInt32(ptr);
                if (code != 0)
                {
                    // メッセージの取り出し
                    message = Marshal.PtrToStringAnsi(new IntPtr(ptr.ToInt32() + Marshal.SizeOf(code)), 128);
                    return null;
                }

                return new LobbyAdminGetFreeQuestionResponse(code, new IntPtr(ptr.ToInt32() + Marshal.SizeOf(code)), size - Marshal.SizeOf(code));
            }
        }

        /// <summary>
        /// アンケート更新リクエスト
        /// </summary>
        [StructLayout(LayoutKind.Sequential, Pack = 4)]
        internal class LobbyAdminUpdateQuestionnaireRequest : RequestContainer
        {
            public LobbyAdminUpdateQuestionnaireRequest(int _id)
                : base(_id)
            {
            }
        }

        /// <summary>
        /// アンケート更新レスポンス
        /// </summary>
        [StructLayout(LayoutKind.Sequential, Pack = 4)]
        internal class LobbyAdminUpdateQuestionnaireResponse : ResponseContainer
        {
        }

        /// <summary>
        /// アンケート提出リクエスト
        /// </summary>
        [StructLayout(LayoutKind.Sequential, Pack = 4)]
        internal class LobbySubmitQuestionnaireRequest : RequestContainer
        {
            int questionSerialNo;
            int questionNo;
            int answerNo;
            int trainerId;
            int sex;
            ushort country;
            ushort region;

            public LobbySubmitQuestionnaireRequest(int _id, int questionSerialNo_, byte language_, int answerNo_)
                : base(_id)
            {
                questionSerialNo = questionSerialNo_;
                answerNo = answerNo_;
                language = language_;

                // 残りは適当に設定
                trainerId = 0;
                sex = 0;
                country = 0;
                region = 0;
            }
        }

        /// <summary>
        /// アンケート提出レスポンス
        /// </summary>
        [StructLayout(LayoutKind.Sequential, Pack = 4)]
        internal class LobbySubmitQuestionnaireResponse : ResponseContainer
        {
        }

        /// <summary>
        /// 質問通し番号セットリクエスト
        /// </summary>
        [StructLayout(LayoutKind.Sequential, Pack = 4)]
        internal class LobbyAdminSetQuestionSerialNoRequest : RequestContainer
        {
            int questionSerialNo;
            public LobbyAdminSetQuestionSerialNoRequest(int _id, int questionSerialNo_)
                : base(_id)
            {
                questionSerialNo = questionSerialNo_;
            }
        }

        /// <summary>
        /// 質問通し番号セットレスポンス
        /// </summary>
        [StructLayout(LayoutKind.Sequential, Pack = 4)]
        internal class LobbyAdminSetQuestionSerialNoResponse : ResponseContainer
        {
        }

        /// <summary>
        /// 次の質問番号セットリクエスト
        /// </summary>
        [StructLayout(LayoutKind.Sequential, Pack = 4)]
        internal class LobbyAdminSetNextQuestionNoRequest : RequestContainer
        {
            int nextQuestionNo;
            public LobbyAdminSetNextQuestionNoRequest(int _id, int nextQuestionNo_)
                : base(_id)
            {
                nextQuestionNo = nextQuestionNo_;
            }
        }

        /// <summary>
        /// 次の質問番号セットレスポンス
        /// </summary>
        [StructLayout(LayoutKind.Sequential, Pack = 4)]
        internal class LobbyAdminSetNextQuestionNoResponse : ResponseContainer
        {
        }

        /// <summary>
        /// 質問番号セットリクエスト
        /// </summary>
        [StructLayout(LayoutKind.Sequential, Pack = 4)]
        internal class LobbyAdminSetQuestionNoRequest : RequestContainer
        {
            int questionNo;
            public LobbyAdminSetQuestionNoRequest(int _id, int questionNo_)
                : base(_id)
            {
                questionNo = questionNo_;
            }
        }

        /// <summary>
        /// 質問番号セットレスポンス
        /// </summary>
        [StructLayout(LayoutKind.Sequential, Pack = 4)]
        internal class LobbyAdminSetQuestionNoResponse : ResponseContainer
        {
        }

        /// <summary>
        /// アンケート初期化リクエスト
        /// </summary>
        [StructLayout(LayoutKind.Sequential, Pack = 4)]
        internal class LobbyAdminInitializeQuestionnaireRequest : RequestContainer
        {
            public LobbyAdminInitializeQuestionnaireRequest(int _id)
                : base(_id)
            {
            }
        }

        /// <summary>
        /// アンケート初期化レスポンス
        /// </summary>
        [StructLayout(LayoutKind.Sequential, Pack = 4)]
        internal class LobbyAdminInitializeQuestionnaireResponse : ResponseContainer
        {
        }

        /// <summary>
        /// 特別質問閾値セットリクエスト
        /// </summary>
        [StructLayout(LayoutKind.Sequential, Pack = 4)]
        internal class LobbyAdminSetSpecialQuestionThresholdRequest : RequestContainer
        {
            int specialQuestionThreshold;
            public LobbyAdminSetSpecialQuestionThresholdRequest(int _id, int specialQuestionThreshold_)
                : base(_id)
            {
                specialQuestionThreshold = specialQuestionThreshold_;
            }
        }

        /// <summary>
        /// 特別質問閾値セットレスポンス
        /// </summary>
        [StructLayout(LayoutKind.Sequential, Pack = 4)]
        internal class LobbyAdminSetSpecialQuestionThresholdResponse : ResponseContainer
        {
        }

        /// <summary>
        /// デフォルト集計フラグセットセットリクエスト
        /// </summary>
        [StructLayout(LayoutKind.Sequential, Pack = 4, CharSet = CharSet.Ansi)]
        internal class LobbyAdminSetDefaultSummarizeFlagRequest : RequestContainer
        {
            [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 12)]
            string defaultSummarizeFlag;
            public LobbyAdminSetDefaultSummarizeFlagRequest(int _id, string defaultSummarizeFlag_)
                : base(_id)
            {
                defaultSummarizeFlag = defaultSummarizeFlag_;
            }
        }

        /// <summary>
        /// デフォルト集計フラグセットレスポンス
        /// </summary>
        [StructLayout(LayoutKind.Sequential, Pack = 4)]
        internal class LobbyAdminSetDefaultSummarizeFlagResponse : ResponseContainer
        {
        }

        internal LobbySchedules LobbyAdminGetSchedule(out uint code, out string message)
        {
            LobbyAdminGetScheduleRequest request = new LobbyAdminGetScheduleRequest(ADMIN_PROFILE_ID);
            LobbyAdminSetScheduleRequestPart[] result;  // レスポンスでLobbyAdminSetScheduleRequestの配列を作る
            IntPtr ptr;
            uint outSize;
            message = "";
            const int resultCodeSize = 4;

            //request.hton();

            encSession.Session(URL_ADMIN_GET_LOBBY_SCHEDULE, ADMIN_PROFILE_ID, request, out ptr, out outSize);

            // サイズのチェック
            if (outSize % LobbyAdminSetScheduleRequestPart.OBJECT_SIZE != resultCodeSize)
            {
                throw new Exception("レスポンスサイズが不正です。size: " + outSize);
            }

            code = (uint)Marshal.ReadInt32(ptr);

            if (code != 0)
            {
                // メッセージの取り出し
                message = Marshal.PtrToStringAnsi(new IntPtr(ptr.ToInt32() + resultCodeSize), 128);
                return null;
            }

            result = LobbySchedules.PtrToStructure(new IntPtr(ptr.ToInt32() + resultCodeSize), outSize - resultCodeSize);

            return new LobbySchedules(result);
        }

        internal void LobbyAdminSetSchedule(LobbySchedules schedules, out uint code, out string message)
        {
            LobbyAdminSetScheduleResponse result = new LobbyAdminSetScheduleResponse();
            IntPtr ptr;
            uint outSize;
            code = 0;
            message = "";


            List<List<byte>> list = schedules.ToByteArrays();
            if (list.Count == 0)
            {
                throw new Exception("スケジュールが空です。");
            }

            // 一度に複数のスケジュールをセットするため、いくつかのスケジュールをまとめたリストを作る。
            List<List<byte>> appendedList = new List<List<byte>>();
            {
                foreach (List<byte> bytes in list)
                {
                    if (appendedList.Count == 0 || appendedList[appendedList.Count - 1].Count + bytes.Count > RequestInterface.MAX_REQUEST_SIZE)
                    {
                        // 最後の要素がいっぱいになったので新規リストを追加
                        // 共通ヘッダを付けて初期化する
                        appendedList.Add(new RequestContainer().ToByteArray());
                    }

                    // データを追加
                    appendedList[appendedList.Count - 1].AddRange(bytes);
                }
            }

            string url = URL_ADMIN_SET_LOBBY_SCHEDULE;
            foreach (List<byte> bytes in appendedList)
            {
                encSession.Session(url, ADMIN_PROFILE_ID, bytes.ToArray(), out ptr, out outSize);

                // レスポンスコードのチェック
                if (outSize < Marshal.SizeOf(result.code))
                {
                    throw new Exception("レスポンスサイズが不正です。レスポンスコードが含まれていません。");
                }

                code = (uint)Marshal.ReadInt32(ptr);
                //result.ntoh();
                if (code != 0)
                {
                    // メッセージの取り出し
                    message = Marshal.PtrToStringAnsi(new IntPtr(ptr.ToInt32() + Marshal.SizeOf(result.code)), 128);
                    return;
                }

                // 二回目以降はアペンド用URLに変更
                url = URL_ADMIN_APPEND_LOBBY_SCHEDULE;
            }
        }

        internal LobbyGetVipResponse LobbyGetVip(uint trainerId, out string message)
        {
            int baseStructureOffset = 4;
            LobbyGetVipRequest request = new LobbyGetVipRequest(ADMIN_PROFILE_ID, trainerId);
            LobbyGetVipResponse result = new LobbyGetVipResponse();
            IntPtr ptr;
            uint outSize;
            message = "";

            encSession.Session(URL_GET_LOBBY_VIP, ADMIN_PROFILE_ID, request, out ptr, out outSize);

            // レスポンスコードのチェック
            if (outSize < Marshal.SizeOf(result.code))
            {
                throw new Exception("レスポンスサイズが不正です。レスポンスコードが含まれていません。");
            }
            else
            {
                result.code = (uint)Marshal.ReadInt32(ptr);
                if (result.code != 0)
                {
                    // メッセージの取り出し
                    message = Marshal.PtrToStringAnsi(new IntPtr(ptr.ToInt32() + Marshal.SizeOf(result.code)), 128);
                    return result;
                }
            }

            int baseAddr = ptr.ToInt32();

            long recordNum = (outSize - baseStructureOffset) / Marshal.SizeOf(typeof(LobbyVipRecord));
            if (recordNum == 0)
            {
                MessageBox.Show("現在VIPはいません。");
                return result;
            }

            // サイズチェック
            if ((outSize - baseStructureOffset) % Marshal.SizeOf(typeof(LobbyVipRecord)) == 0)
            {
                LobbyVipRecord data;
                for (int i = 0; i < recordNum; i++)
                {
                    data = (LobbyVipRecord)Marshal.PtrToStructure(
                        new IntPtr(baseAddr + baseStructureOffset + (i * Marshal.SizeOf(typeof(LobbyVipRecord)))), typeof(LobbyVipRecord));
                    result.vipRecords.Add(data);
                }
            }
            else
            {
                throw new Exception("レスポンスサイズが不正です。LobbyVipRecord構造体の倍数ではありません。");
            }
            return result;
        }

        internal LobbyAdminGetVipSettingResponse LobbyAdminGetVipSetting(out string message)
        {
            int baseStructureOffset = 4;
            LobbyAdminGetVipSettingRequest request = new LobbyAdminGetVipSettingRequest(ADMIN_PROFILE_ID);
            LobbyAdminGetVipSettingResponse result = new LobbyAdminGetVipSettingResponse();
            IntPtr ptr;
            uint outSize;
            message = "";

            encSession.Session(URL_ADMIN_GET_LOBBY_VIP, ADMIN_PROFILE_ID, request, out ptr, out outSize);

            // レスポンスコードのチェック
            if (outSize < Marshal.SizeOf(result.code))
            {
                throw new Exception("レスポンスサイズが不正です。レスポンスコードが含まれていません。");
            }
            else
            {
                result.code = (uint)Marshal.ReadInt32(ptr);
                if (result.code != 0)
                {
                    // メッセージの取り出し
                    message = Marshal.PtrToStringAnsi(new IntPtr(ptr.ToInt32() + Marshal.SizeOf(result.code)), 128);
                    return result;
                }
            }

            int baseAddr = ptr.ToInt32();

            long recordNum = (outSize - baseStructureOffset) / Marshal.SizeOf(typeof(LobbyVipSettingRecord));
            if (recordNum == 0)
            {
                MessageBox.Show("サーバに登録されているVIP設定リストは空です。");
                return result;
            }

            // サイズチェック
            if ((outSize - baseStructureOffset) % Marshal.SizeOf(typeof(LobbyVipSettingRecord)) == 0)
            {
                LobbyVipSettingRecord data;
                for (int i = 0; i < recordNum; i++)
                {
                    data = (LobbyVipSettingRecord)Marshal.PtrToStructure(
                        new IntPtr(baseAddr + baseStructureOffset + (i * Marshal.SizeOf(typeof(LobbyVipSettingRecord)))), typeof(LobbyVipSettingRecord));
                    data.StartMonth++;  // 月はサーバから0-11が返ってくるので修正
                    data.EndMonth++;
                    result.vipSettingRecords.Add(data);
                }
            }
            else
            {
                throw new Exception("レスポンスサイズが不正です。LobbyVipSettingRecord構造体の倍数ではありません。");
            }
            return result;
        }

        internal void LobbyAdminSetVipSettingRecord(LobbyAdminSetVipSettingRequest req, out uint code, out string message)
        {
            LobbyAdminSetScheduleResponse result = new LobbyAdminSetScheduleResponse();
            IntPtr ptr;
            uint outSize;
            code = 0;
            message = "";


            List<byte> bytes = req.ToByteArray();
            if (bytes.Count == 0)
            {
                if (MessageBox.Show("空のVIPリストを登録します。よろしいですか？", "確認", MessageBoxButtons.YesNo) != DialogResult.Yes)
                {
                    return;
                }
            }

            encSession.Session(URL_ADMIN_SET_LOBBY_VIP, ADMIN_PROFILE_ID, bytes.ToArray(), out ptr, out outSize);

            // レスポンスコードのチェック
            if (outSize < Marshal.SizeOf(result.code))
            {
                throw new Exception("レスポンスサイズが不正です。レスポンスコードが含まれていません。");
            }

            code = (uint)Marshal.ReadInt32(ptr);
            if (code != 0)
            {
                // メッセージの取り出し
                message = Marshal.PtrToStringAnsi(new IntPtr(ptr.ToInt32() + Marshal.SizeOf(result.code)), 128);
                return;
            }
        }

        internal LobbyAdminCheckProfileResponse LobbyAdminCheckProfile(LobbyAdminCheckProfileRequest req, out uint code, out string message)
        {
            IntPtr ptr;
            uint outSize;
            code = 0;
            message = "";

            List<byte> bytes = req.ToByteArray();
            encSession.Session(URL_ADMIN_CHECK_PROFILE, ADMIN_PROFILE_ID, bytes.ToArray(), out ptr, out outSize);

            return LobbyAdminCheckProfileResponse.Result(ptr, (int)outSize, out code, out message);
        }

        internal LobbyGetQuestionnaireResponse LobbyGetQuestionnaire(LobbyGetQuestionnaireRequest req, out uint code, out string message)
        {
            IntPtr ptr;
            uint outSize;
            code = 0;
            message = "";

            List<byte> bytes = req.ToByteArray();
            encSession.Session(URL_GET_QUESTIONNAIRE, ADMIN_PROFILE_ID, bytes.ToArray(), out ptr, out outSize);

            return LobbyGetQuestionnaireResponse.Result(ptr, (int)outSize, out code, out message);
        }

        internal LobbyAdminGetSpecialWeekResponse LobbyAdminGetSpecialWeek(LobbyAdminGetSpecialWeekRequest req, out uint code, out string message)
        {
            IntPtr ptr;
            uint outSize;
            code = 0;
            message = "";

            List<byte> bytes = req.ToByteArray();
            encSession.Session(URL_ADMIN_GET_SPECIAL_WEEK, ADMIN_PROFILE_ID, bytes.ToArray(), out ptr, out outSize);

            return LobbyAdminGetSpecialWeekResponse.Result(ptr, (int)outSize, out code, out message);
        }

        internal void LobbyAdminSetSpecialWeek(LobbyAdminSetSpecialWeekRequest req, out uint code, out string message)
        {
            IntPtr ptr;
            uint outSize;
            code = 0;
            message = "";

            List<byte> bytes = req.ToByteArray();
            encSession.Session(URL_ADMIN_SET_SPECIAL_WEEK, ADMIN_PROFILE_ID, bytes.ToArray(), out ptr, out outSize);

            LobbyAdminSetSpecialWeekResponse.Result(ptr, (int)outSize, out code, out message);
        }

        internal void LobbyAdminSetFreeQuestion(LobbyAdminSetFreeQuestionRequest req, out uint code, out string message)
        {
            IntPtr ptr;
            uint outSize;
            code = 0;
            message = "";

            List<byte> bytes = req.ToByteArray();
            encSession.Session(URL_ADMIN_SET_FREE_QUESTION, ADMIN_PROFILE_ID, bytes.ToArray(), out ptr, out outSize);

            LobbyAdminSetFreeQuestionResponse.Result(ptr, (int)outSize, out code, out message);
        }

        internal LobbyAdminGetFreeQuestionResponse LobbyAdminGetFreeQuestion(LobbyAdminGetFreeQuestionRequest req, out uint code, out string message)
        {
            IntPtr ptr;
            uint outSize;
            code = 0;
            message = "";

            encSession.Session(URL_ADMIN_GET_FREE_QUESTION, ADMIN_PROFILE_ID, req, out ptr, out outSize);

            return LobbyAdminGetFreeQuestionResponse.Result(ptr, (int)outSize, out code, out message);
        }
    }
}
