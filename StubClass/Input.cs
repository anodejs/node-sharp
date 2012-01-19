using System;
using System.Collections.Generic;
using System.Linq;

namespace StubClass
{
    public class Input
    {
        public int AcceptInt(int val)
        {
            return -val;
        }

        public string AcceptString(string val)
        {
            return new String(val.Reverse().ToArray());
        }

        public int[] AcceptArray(int[] val)
        {
            return val.Reverse().ToArray();
        }

        public List<int> AcceptList(List<int> val )
        {
            val.Reverse();
            return val;
        }

        public ReturnValue AcceptObject(ReturnValue val)
        {
            val.IntValue = AcceptInt(val.IntValue);
            val.StringValue = AcceptString(val.StringValue);
            
            return val;
        }
    }
}
