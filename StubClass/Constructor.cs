using System;
using System.Collections.Generic;

namespace StubClass
{
    public class Constructor
    {
        private readonly int _int;
        private readonly string _str;
        private readonly List<int> _array;
 
        public Constructor()
        {
            _int = 0;
            _str = "";
            _array = new List<int>();
        }

        public Constructor(int i, string s)
        {
            _int = i;
            _str = s;
            _array = new List<int>();
        }

        public Constructor(int i, string s, int[] values )
        {
            _int = i;
            _str = s;
            _array = new List<int>(values);
        }

        public Constructor(StubClass.ReturnValue value)
        {
            _int = value.IntValue;
            _str = value.StringValue;
            _array = new List<int>();
        }

        public override string ToString()
        {
            return String.Format("[{0}, {1}, {2}]", _int, _str, String.Join(",", _array));
        }
    }
}
