using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;

namespace 烟火检测CSHARP
{
    class CPPDLL
    {
        [DllImport("FireSmoke_Detect_DLL.dll", CharSet = CharSet.Ansi)]
        public static extern int fireDetect(char[] filePath);
    }
}
