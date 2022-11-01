using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Media;

namespace LogWindowUI
{
    public class LogLevel : INotifyPropertyChanged
    {
        public event PropertyChangedEventHandler PropertyChanged;

        public string Name     { get; set; }
        public Brush  Color    { get; set; }
        public int    Severity { get; set; }

        private bool m_selected;
        public  bool Selected
        {
            get
            {
                return m_selected;
            }
            set
            {
                m_selected = value;
                if (PropertyChanged != null)
                {
                    PropertyChanged(this, new PropertyChangedEventArgs("Selected"));
                }
            }
        }
    }
}
