using System;
using System.Collections.Generic;
using System.Linq;

namespace StubClass
{
    public class Output
    {
        private readonly Random _r = new Random();
        
        public int ReturnInt()
        {
            return _r.Next(1, 100);
        }

        public string ReturnString()
        {
            return typeof (Output).Name + ReturnInt();
        }

        public int[] ReturnArray()
        {
            int len = _r.Next(2, 6);
            var result = new int[len];
            for(int i=0; i < len; i++)
            {
                result[i] = ReturnInt();
            }

            return result;
        }

        public List<int> ReturnList()
        {
            return ReturnArray().ToList();
        }

        public Object ReturnObject()
        {
            return new ReturnValue {IntValue = ReturnInt(), StringValue = ReturnString()};
        }
    }

    public class ReturnValue
    {
        public int IntValue;
        public string StringValue;

        public override string ToString()
        {
            return String.Format("[{0}, '{1}']", IntValue, StringValue);
        }
    }
}
