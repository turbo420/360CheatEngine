using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Server
{
    class Utils
    {
        public static string BytesToHexString(byte[] buffer)
        {
            string str = "";
            for (int i = 0; i < buffer.Length; i++) str += buffer[i].ToString("X2");
            return str;
        }
    }
}
