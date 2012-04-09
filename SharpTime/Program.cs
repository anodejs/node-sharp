using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Reflection;

namespace SharpTime
{
    class Program
    {
        private static int repeatCount = 1;

        static void Main()
        {
            repeatCount = 10000;
            var l = new Loader();

            Time("Startup time", () => l.Load("StubClass.dll"));

            Time("Create", () => Iter(() => l.Create("StubClass.Output")));

            Time("Int", () => Iter(() => l.Call("ReturnInt")));
            Time("String", () => Iter(() => l.Call("ReturnString")));
            Time("Array", () => Iter(() => l.Call("ReturnArray")));
            Time("List", () => Iter(() => l.Call("ReturnList")));
            Time("Object", () => Iter(() => l.Call("ReturnObject")));

            repeatCount = 1000000;
            var intArr = new [] {1, 2, 3, 4};
            Time("Int static", () => Iter(() => 4));
            Time("String static", () => Iter(() => "ReturnString"));
            Time("Array static", () => Iter(() => new[] {1, 2, 3}));
            Time("List static", () => Iter(() => new List<int>(intArr)));
            Time("Object static", () => Iter(() => new RetVal(1,"str")));

        }

        [DebuggerStepThrough]
        static void Time(string name, Action method)
        {
            var s = new Stopwatch();
            s.Start();
            method();
            s.Stop();

            Console.WriteLine("{0}: {1}ms", name, s.ElapsedMilliseconds);
        }

        [DebuggerStepThrough]
        static void Iter(Action method)
        {
            for(int i=0; i < repeatCount; i++)
            {
                method();
            }
        }

        [DebuggerStepThrough]
        static void Iter<T>(Func<T> method)
        {
            for (int i = 0; i < repeatCount; i++)
            {
                method();
            }
        }

        private class RetVal
        {
            private readonly int intVal;
            private readonly string strVal;

            public RetVal(int i, string s)
            {
                intVal = i;
                strVal = s;
            }

            public override string ToString()
            {
                return String.Format("{0},{1}", strVal, intVal);
            }
        }
    }

    internal class Loader
    {
        private Assembly _assembly;
        private Object _instance;
        private Type _type;
        
        public Loader()
        {
            _assembly = null;
        }

        public void Load(string name)
        {
            string path = Directory.EnumerateFiles(Environment.CurrentDirectory, name).First();
            _assembly = Assembly.LoadFile(path);
        }

        public void Create(string type)
        {
            _instance = _assembly.CreateInstance(type);
            if (_instance == null) throw new ArgumentException("Type was not found", "type");
            _type = _instance.GetType();
        }

        public object Call(string method, params object[] args)
        {
            MethodInfo mi = _type.GetMethod(method);
            if (mi == null) throw new ArgumentException("method was not found", "method");
            return mi.Invoke(_instance, args);
        }
    }
}
