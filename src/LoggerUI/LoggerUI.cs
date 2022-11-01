using System.Diagnostics;
using System;
using System.Threading;
using System.Windows;
using System.Collections.Generic;

namespace LogWindowUI
{
    public sealed class LoggerUI
    {
        private static LoggerUI m_instance = null;
        public static LoggerUI Instance
        {
            get
            {
                Debug.Assert(m_instance != null, "LoggerUI not initialized",
                    "Initialize must be called before using Instance");
                return m_instance;
            }
        }

        private App m_application = null;

        private LoggerUI(Rect dimensions)
        {
            // application and window need their own thread, so we create it
            AutoResetEvent windowCreatedEvent = new AutoResetEvent(false);
            Thread t = new Thread(() =>
            {
                m_application = new App();

                MainWindow window = new MainWindow();

                // set window dimensions
                window.WindowStartupLocation = WindowStartupLocation.Manual;
                window.Left = dimensions.Left;
                window.Top = dimensions.Top;
                window.Width = dimensions.Width;
                window.Height = dimensions.Height;

                window.Loaded += (object sender, RoutedEventArgs e) =>
                {
                    // notify we're done when the main window is loaded
                    windowCreatedEvent.Set();
                };

                m_application.Run(window);
            });
            t.SetApartmentState(ApartmentState.STA);
            t.Start();

            // wait until the application and window are created
            windowCreatedEvent.WaitOne();
        }

        public static void Initialize(int x, int y, int w, int h)
        {
            Debug.Assert(m_instance == null, "LoggerUI already initialized");
            m_instance = new LoggerUI(new Rect(x, y, w, h));
        }

        public static void Destroy()
        {
            AutoResetEvent applicationShutdownEvent = new AutoResetEvent(false);
            Debug.Assert(m_instance != null, "LoggerUI already destroyed!");

            m_instance.m_application.Dispatcher.BeginInvoke((Action)delegate
            {
                m_instance.m_application.Exit += (object sender, ExitEventArgs e) =>
                {
                    applicationShutdownEvent.Set();
                };
                m_instance.m_application.Shutdown();
            });
            applicationShutdownEvent.WaitOne();

            m_instance = null;
        }

        public void ConfigureSytems(List<string> systems)
        {
            Debug.Assert(m_application != null);

            m_application.Dispatcher.BeginInvoke((Action)delegate
            {
                Debug.Assert(m_application.MainWindow != null);

                (m_application.MainWindow as MainWindow).ConfigureSystems(systems);
            });
        }

        public void ConfigureLevels(List<Tuple<string, string>> levels, string defaultLevelName)
        {
            Debug.Assert(m_application != null);

            m_application.Dispatcher.BeginInvoke((Action)delegate
            {
                Debug.Assert(m_application.MainWindow != null);

                (m_application.MainWindow as MainWindow).ConfigureLevels(levels, defaultLevelName);
            });
        }

        public void SetSystemEnabled(string system, bool enabled)
        {
            Debug.Assert(m_application != null);

            m_application.Dispatcher.BeginInvoke((Action)delegate
            {
                if(m_application.MainWindow == null)
                {
                    return;
                }

                (m_application.MainWindow as MainWindow).SetSystemEnabled(system, enabled);
            });
        }

        public void Add(float timestamp, string level, string system, string message)
        {
            Debug.Assert(m_application != null);

            // add it to the window via UI thread
            m_application.Dispatcher.BeginInvoke((Action)delegate
            {
                // window can be closed so we don't show anything
                if (m_application.MainWindow == null)
                {
                    return;
                }

                (m_application.MainWindow as MainWindow).AddLogEntry(timestamp, level, system, message);
            });
        }
    }
}
