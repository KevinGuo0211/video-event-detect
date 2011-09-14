using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;

namespace 烟火检测CSHARP
{
    class FileOperation
    {
        static public float[] readFromFile(string filePath)
        {
            StreamReader fileReader = null;
            try
            {
                fileReader = new StreamReader(filePath);
            }
            catch(Exception e)
            {
                return null;
            }
            string line = fileReader.ReadLine();
            if (line == null)
            {
                fileReader.Close();
                return null;
            }
            int mark = 0;
            string[] data = line.Split(',');
            if (data[data.Length - 1].Equals(""))
            {
                mark = 1;
            }
            float []dataF = new float[data.Length-mark];
            for (int i = 0; i < data.Length-mark; i++)
            {
                dataF[i] = (float)Convert.ToDouble(data[i]);
            }
            fileReader.Close();
            return dataF;
        }
    }
}
