using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Collections.Specialized;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace LogWindowUI
{
    public partial class MainWindow : Window
    {
        /*
        After some real usage we found out that having a ListCollectionView
        with no filter performed as expected but just having a Filter (no
        matter whether it was just a "return true" or more complicated) made
        it freeze after some ten thousand messages.
        So, we decided to have two separate collections: the one with all of
        the log entries, unfiltered, and then the one that's filtered which
        is shown to the user.
        This means we need extra control on Add and filter refreshes.
        */
        private List<LogEntry> LogEntries;
        private ObservableCollection<LogEntry> FilteredLogEntries;

        public ObservableCollection<LogSystem> LogSystems;
        public ObservableCollection<LogLevel> LogLevels;

        public int CurrentLogLevelSeverity { get; set; }

        public bool IsAutoScrollEnabled { get; set; }

        public MainWindow()
        {
            InitializeComponent();

            DataContext = this;

            LogEntries = new List<LogEntry>();
            LogSystems = new ObservableCollection<LogSystem>();
            LogLevels = new ObservableCollection<LogLevel>();
            FilteredLogEntries = new ObservableCollection<LogEntry>();

            FilteredLogEntries.CollectionChanged += OnLogEntriesChangedScrollToBottom;

            CurrentLogLevelSeverity = 0;
            IsAutoScrollEnabled = true;

            // tie View with ViewModel
            LogEntryList.ItemsSource = FilteredLogEntries;
            Systems.ItemsSource = LogSystems;
            Levels.ItemsSource = LogLevels;
        }

        protected override void OnClosing(CancelEventArgs e)
        {
            base.OnClosing(e);

            // TODO: re-think about this, closing window causes host program to end as well
            Environment.Exit(0);
        }

        public void ConfigureSystems(List<string> systems)
        {
            foreach(string system in systems)
            {
                LogSystem entry = new LogSystem
                {
                    Name = system,
                    Enabled = true
                };

                entry.PropertyChanged += OnSystemEnableChanged;

                LogSystems.Add(entry);
            };
        }

        public void SetSystemEnabled(string system, bool enabled)
        {
            LogSystem logSystem = LogSystems.FirstOrDefault((LogSystem s) =>
            {
                return s.Name == system;
            });

            if(logSystem != null)
            {
                logSystem.Enabled = enabled;
            }
        }

        public void ConfigureLevels(List<Tuple<string, string>> levels, string defaultLevelName)
        {
            // create log levels
            for(int i = 0; i < levels.Count; ++i)
            {
                LogLevel entry = new LogLevel
                {
                    Severity = i,
                    Name = levels[i].Item1,
                    Color = (Brush)new BrushConverter().ConvertFromString(levels[i].Item2),
                    Selected = levels[i].Item1 == defaultLevelName
                };

                if(entry.Selected)
                {
                    CurrentLogLevelSeverity = entry.Severity;
                }

                entry.PropertyChanged += OnLevelSelectedChanged;

                LogLevels.Add(entry);
            }

            // ensure one is Selected
            if(LogLevels.Count > 0 && LogLevels.All(l => !l.Selected))
            {
                LogLevels[0].Selected = true;
            }

            // style ListView based on the data from the log levels
            Style logListStyle = new Style();
            logListStyle.TargetType = typeof(ListViewItem);
            foreach(LogLevel level in LogLevels)
            {
                DataTrigger trigger = new DataTrigger();
                trigger.Binding = new Binding("Level");
                trigger.Value = level.Name;
                trigger.Setters.Add(new Setter(ListViewItem.ForegroundProperty, level.Color));

                logListStyle.Triggers.Add(trigger);
            }

            LogEntryList.ItemContainerStyle = logListStyle;
        }

        public void AddLogEntry(float timestamp, string level, string system, string message)
        {
            LogEntry entry = new LogEntry
            {
                Timestamp = timestamp,
                Level = level,
                System = system,
                Message = message
            };

            LogEntries.Add(entry);
            if (LogEntriesFilterPredicate(entry))
            {
                FilteredLogEntries.Add(entry);
            }
        }

        private void OnLogEntriesChangedScrollToBottom(object sender, NotifyCollectionChangedEventArgs e)
        {
            if (!IsAutoScrollEnabled)
            {
                return;
            }

            if (VisualTreeHelper.GetChildrenCount(LogEntryList) > 0)
            {
                Decorator border = VisualTreeHelper.GetChild(LogEntryList, 0) as Decorator;
                ScrollViewer scrollViewer = (ScrollViewer)VisualTreeHelper.GetChild(border, 0);
                scrollViewer.ScrollToBottom();
            }
        }

        private bool LogEntriesFilterPredicate(object item)
        {
            LogEntry entry = item as LogEntry;

            // filter out systems
            if(LogSystems.Any(s => s.Name == entry.System && !s.Enabled))
            {
                return false;
            }

            // filter out levels
            LogLevel level = LogLevels.FirstOrDefault(l => l.Name == entry.Level);
            if(level != null && level.Severity < CurrentLogLevelSeverity)
            {
                return false;
            }

            return true;
        }

        private void OnSystemEnableChanged(object sender, PropertyChangedEventArgs args)
        {
            RefreshFilter();
        }

        private void OnLevelSelectedChanged(object sender, PropertyChangedEventArgs args)
        {
            LogLevel level = sender as LogLevel;

            if(level.Selected)
            {
                CurrentLogLevelSeverity = level.Severity;

                RefreshFilter();
            }
        }

        private void RefreshFilter()
        {
            FilteredLogEntries.Clear();
            LogEntries.Where(LogEntriesFilterPredicate).ToList().ForEach(FilteredLogEntries.Add);
        }
    }
}
