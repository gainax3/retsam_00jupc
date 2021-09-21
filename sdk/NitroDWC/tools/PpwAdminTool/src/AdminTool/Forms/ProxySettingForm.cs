using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace AdminTool
{
    public partial class ProxySettingForm : Form
    {
        private bool endOk = false;

        public bool EndOk
        {
            get { return endOk; }
        }

        private string resultProxyServer;

        public string ResultProxyServer
        {
            get { return resultProxyServer; }
            set { resultProxyServer = value; }
        }

        public ProxySettingForm(string proxyServer, System.Collections.IEnumerable proxyServerHistory)
        {
            InitializeComponent();

            foreach(string history in proxyServerHistory)
            {
                comboBoxProxyServer.Items.Add(history);
            }
            comboBoxProxyServer.Text = proxyServer;
        }

        private void buttonOk_Click(object sender, EventArgs e)
        {
            endOk = true;
            resultProxyServer = comboBoxProxyServer.Text;
            Close();
        }

        private void buttonCancel_Click(object sender, EventArgs e)
        {
            resultProxyServer = comboBoxProxyServer.Text;
            Close();
        }

        private void comboBoxProxyServer_KeyDown(object sender, KeyEventArgs e)
        {
            switch (e.KeyCode)
            {
                case Keys.Enter:
                    buttonOk_Click(null, null);
                    break;
                case Keys.Escape:
                    buttonCancel_Click(null, null);
                    break;
            }
        }
    }
}