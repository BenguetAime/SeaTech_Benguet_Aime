using ExtendedSerialPort;
using System;
using System.Collections.Generic;
using System.IO.Ports;
using System.Linq;
using System.Text;
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

namespace RobotInterface_BENGUET
{
    /// <summary>
    /// Logique d'interaction pour MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
  
    {
        ReliableSerialPort serialPort1;



        public MainWindow()
        {
            InitializeComponent();
            serialPort1 = new ReliableSerialPort("COM4", 115200, Parity.None, 8, StopBits.One);
            serialPort1.DataReceived += SerialPort1_DataReceived;
            serialPort1.Open();
        }

        private void SerialPort1_DataReceived(object sender, DataReceivedArgs e)
        {
            textBoxReception.Text += Encoding.UTF8.GetString(e.Data, 0, e.Data.Length);
        }

        bool toggle;
        private void ButtonEnvoyer_Click_1(object sender, RoutedEventArgs e)
        {
            if (toggle)
            ButtonEnvoyer.Background = Brushes.RoyalBlue;
            else ButtonEnvoyer.Background = Brushes.Beige;
            toggle = !toggle;
            SendMessage();



        }

        private void SendMessage ()
        {
            //textBoxReception.Text += "\n"+ "Reçu: " + textBoxEmission.Text;
            //textBoxEmission.Text= "";
            serialPort1.WriteLine(textBoxEmission.Text);
            textBoxEmission.Text = "";

        }


        private void TextBox_KeyUp(object sender, KeyEventArgs e)
        {
            if (e.Key == Key.Enter)
            {
                SendMessage();
            }
        }

    }
}
