using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;

namespace 烟火检测CSHARP
{
    class CPPDLL
    {
        [DllImport("FireDetectDll3.5.dll", CharSet = CharSet.Ansi)]
        public static extern int fireDetect(char[] filePath);
    }
}
