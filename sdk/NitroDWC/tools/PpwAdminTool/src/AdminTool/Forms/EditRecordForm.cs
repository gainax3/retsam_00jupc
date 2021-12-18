using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace AdminTool
{
    public partial class EditRecordForm : Form
    {
        public List<RequestInterface.LobbyScheduleRecord> orgData;
        public bool endOk = false;
        public bool EndOk
        {
            get { return endOk; }
        }

        public EditRecordForm(List<RequestInterface.LobbyScheduleRecord> dataSource)
        {
            InitializeComponent();
            orgData = dataSource;

            BindingSource bindingSource = new BindingSource();
            bindingSource.AllowNew = true;
            bindingSource.DataSource = typeof(RequestInterface.LobbyScheduleRecord);    // このデータソースの規定の型を指定

            foreach (RequestInterface.LobbyScheduleRecord item in dataSource)
            {
                bindingSource.Add(item);
            }

            DataGridViewTextBoxColumn text = new DataGridViewTextBoxColumn();
            text.DataPropertyName = "Time"; // パブリックなプロパティ名を指定しないと表示されないことに注意
            text.Name = "時刻(秒)";
            text.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight;
            dataGridView.Columns.Add(text);
            
            DataGridViewComboBoxColumn combo = new DataGridViewComboBoxColumn();
            List<RequestInterface.PPW_LOBBY_TIME_EVENT> list =
                new List<RequestInterface.PPW_LOBBY_TIME_EVENT>
                ((RequestInterface.PPW_LOBBY_TIME_EVENT[])Enum.GetValues(typeof(RequestInterface.PPW_LOBBY_TIME_EVENT)));
            list.RemoveAt(0);   // Enum要素の内、最初のLockを除外する。
            combo.DataSource = list;
            combo.DataPropertyName = "TimeEvent";
            combo.Name = "イベント";
            combo.MaxDropDownItems = 20;
            dataGridView.Columns.Add(combo);

            dataGridView.AutoGenerateColumns = false;
            dataGridView.DataSource = bindingSource;
            bindingNavigator.BindingSource = bindingSource;
        }

        private void EditRecordForm_FormClosed(object sender, FormClosedEventArgs e)
        {
            if (endOk)
            {
                BindingSource bindingSource = (BindingSource)dataGridView.DataSource;
                orgData.Clear();
                foreach (RequestInterface.LobbyScheduleRecord item in bindingSource)
                {
                    orgData.Add(item);
                }
            }
        }

        private void buttonOK_Click(object sender, EventArgs e)
        {
            endOk = true;
            Close();
        }

        private void buttonCancel_Click(object sender, EventArgs e)
        {
            Close();
        }

        private void dataGridView_DataError(object sender, DataGridViewDataErrorEventArgs e)
        {
            ExtendedDataGridView.ShowDataError(sender, e);
            e.Cancel = false;
        }
    }
}