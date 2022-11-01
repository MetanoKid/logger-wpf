using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace LogWindowUI
{
    public class LogEntry
    {
        public float  Timestamp { get; set; }
        public string Level     { get; set; }
        public string System    { get; set; }
        public string Message   { get; set; }
    }
}
