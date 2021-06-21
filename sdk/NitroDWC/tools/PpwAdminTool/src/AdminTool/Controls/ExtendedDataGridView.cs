using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;
using System.Drawing;

namespace AdminTool
{
    class ExtendedDataGridView : DataGridView
    {
        public ExtendedDataGridView()
            : base()
        {
            KeyDown +=new KeyEventHandler(ExtendedDataGridView_KeyDown);
            CellContextMenuStripNeeded += new DataGridViewCellContextMenuStripNeededEventHandler(ExtendedDataGridView_CellContextMenuStripNeeded);
            CellFormatting += new DataGridViewCellFormattingEventHandler(ExtendedDataGridView_CellFormatting);
        }

        void ExtendedDataGridView_CellFormatting(object sender, DataGridViewCellFormattingEventArgs e)
        {
            DataGridView dgv = (DataGridView)sender;

            // 新規行の前景色をグレーにする
            if (dgv.Rows[e.RowIndex].IsNewRow)
            {
                e.CellStyle.ForeColor = Color.Gray;
            }
        }

        void ExtendedDataGridView_KeyDown(object sender, KeyEventArgs e)
        {
            DataGridView dgv = (DataGridView)sender;
            if (dgv.CurrentCell == null)
            {
                return;
            }

            if (e.Control)
            {
                switch (e.KeyCode)
                {
                    // ペースト
                    case Keys.V:
                        List<List<string>> stringArray = Util.TsvTextToArray(Clipboard.GetText());
                        SetStringArray(stringArray);
                        this.Refresh();
                        break;
                }
            }
        }

        void ExtendedDataGridView_CellContextMenuStripNeeded(object sender, DataGridViewCellContextMenuStripNeededEventArgs e)
        {
            DataGridView dgv = (DataGridView)sender;

            if (e.ColumnIndex < 0 || e.RowIndex < 0)
            {
                return;
            }

            // 単に選択する
            dgv.ClearSelection();
            dgv[e.ColumnIndex, e.RowIndex].Selected = true;
        }

        public void SetStringArray(List<List<string>> stringArray)
        {
            int startRow = CurrentCell.RowIndex;
            int startColumn = CurrentCell.ColumnIndex;
            DataGridViewCell cell = SelectMostTopLeftCell(SelectedCells);
            startRow = cell.RowIndex;
            startColumn = cell.ColumnIndex;
            for (int i = 0; i < stringArray.Count && i + startRow < Rows.Count; i++)
            {
                if (Rows[startRow + i].IsNewRow)
                {
                    MessageBox.Show("新規行(*がついている行)にはペーストできません。");
                    continue;
                }

                for (int j = 0; j < stringArray[i].Count && j + startColumn < Rows[startRow + i].Cells.Count; j++)
                {
                    object orgValue = this[startColumn + j, startRow + i].Value;

                    if (this[startColumn + j, startRow + i].ReadOnly)
                    {
                        // リードオンリーに設定されている。
                        MessageBox.Show("読み取り専用のセルにはペーストできません。");
                        return;
                    }

                    if (stringArray[i][j] == "")
                    {
                        // 選択範囲が長方形ではない
                        MessageBox.Show("長方形でない選択範囲のコピー＆ペーストには対応していません。");
                        return;
                    }

                    this[startColumn + j, startRow + i].Value = stringArray[i][j];

                    // 入力データがおかしい等の理由でセットされていなかった場合は中断する。表示テキストを比較
                    if (this[startColumn + j, startRow + i].FormattedValue.ToString() != stringArray[i][j])
                    {
                        // 入力前の値に戻す
                        // DataErrorイベントハンドラでe.Cancel = falseをすると下記は必要ない
                        //this[startColumn + j, startRow + i].Value = orgValue;
                        return;
                    }
                }
            }
        }

        // 一番左上のセルを返す
        // 矩形選択に対応、Ctrl押しながらの複数選択は適当に対応
        public static DataGridViewCell SelectMostTopLeftCell(DataGridViewSelectedCellCollection dataGridViewSelectedCellCollection)
        {
            int? minRow = null;
            int? minColumn = null;
            DataGridViewCell resultCell = null;
            foreach (DataGridViewCell cell in dataGridViewSelectedCellCollection)
            {
                if (minRow == null || cell.RowIndex < minRow)
                {
                    minRow = cell.RowIndex;
                }
            }

            foreach (DataGridViewCell cell in dataGridViewSelectedCellCollection)
            {
                if (minRow == cell.RowIndex)
                {
                    if (minColumn == null || cell.ColumnIndex < minColumn)
                    {
                        minColumn = cell.ColumnIndex;
                        resultCell = cell;
                    }
                }
            }
            return resultCell;
        }

        public static void ShowDataError(object sender, DataGridViewDataErrorEventArgs e)
        {
            DataGridView dgv = (DataGridView)sender;
            MessageBox.Show("入力もしくはペーストされたデータ形式がそのセルの形式と一致していません。\n"
                + "セルの位置: " + (e.RowIndex + 1) + "行目、「" + dgv.Columns[e.ColumnIndex].Name + "」カラム\n"
                + "詳細情報: " + e.Exception.Message);
        }

    }
}
