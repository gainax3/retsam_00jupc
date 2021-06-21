using System;
using System.Collections.Generic;
using System.Text;

namespace AdminTool
{
    /// <summary>
    /// リストボックスのメンバに値を持たせるためのクラス
    /// </summary>
    class ListBoxMember
    {
        public const string DISPLAYMEMBER = "Name";
        public const string VALUEMEMBER = "Value";
        private string name;
        private object value;
        private ListBoxMember()
        {
            name = null;
            value = null;
        }
        public ListBoxMember(string _name, object _value)
            : this()
        {
            name = _name;
            value = _value;
        }
        public string Name
        {
            get { return name; }
        }
        public object Value
        {
            get { return value; }
        }
    }
}
