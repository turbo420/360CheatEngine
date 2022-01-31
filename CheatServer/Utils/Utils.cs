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

        public static long GetTimeStamp()
        {
            return DateTimeOffset.UtcNow.ToUnixTimeSeconds();
        }

        public static byte[] GenerateRandomData(int count)
        {
            byte[] RandData = new byte[count];
            new Random().NextBytes(RandData);

            return RandData;
        }

        public static void AddStringToArray(ref char[] array, string err)
        {
            Array.Copy(err.ToCharArray(), 0, array, 0, err.Length);
        }
    }
}
