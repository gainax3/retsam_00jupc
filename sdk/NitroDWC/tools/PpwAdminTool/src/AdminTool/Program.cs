using System;
using System.Collections.Generic;
using System.Windows.Forms;
using System.Reflection;

namespace AdminTool
{
    static class Program
    {
        /// <summary>
        /// アプリケーションのメイン エントリ ポイントです。
        /// </summary>
        [STAThread]
        static void Main()
        {
            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);
#if !DEBUG
            Application.ThreadException += new System.Threading.ThreadExceptionEventHandler(Application_ThreadException);
#endif
            Application.SetUnhandledExceptionMode(UnhandledExceptionMode.Automatic);
            Application.Run(new MainForm());
        }

        static void Application_ThreadException(object sender, System.Threading.ThreadExceptionEventArgs e)
        {
            if (e.Exception is InvalidOperationException && e.Exception.TargetSite.Name == "RemoveCurrent")
            {
                // DGVの削除ボタンが原因なので続行
                MessageBox.Show(e.Exception.Message, "エラー");
            }
            else
            {
                MessageBox.Show("内部エラーが発生したので終了します。\n" + e.Exception.Message + "\n" + e.Exception.StackTrace, "エラー");
                Application.Exit();
            }
        }
    }
}