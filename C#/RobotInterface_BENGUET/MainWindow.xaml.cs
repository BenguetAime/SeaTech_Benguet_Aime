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
using System.Windows.Threading;

namespace RobotInterface_BENGUET
{
    /// <summary>
    /// Logique d'interaction pour MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window

    {
        ReliableSerialPort serialPort1;
        DispatcherTimer timerAffichage;
        //string receivedText;

        Robot robot = new Robot();


        public MainWindow()
        {
            InitializeComponent();
            serialPort1 = new ReliableSerialPort("COM15", 115200, Parity.None, 8, StopBits.One);
            serialPort1.DataReceived += SerialPort1_DataReceived;
            serialPort1.Open();

            timerAffichage = new DispatcherTimer();
            timerAffichage.Interval = new TimeSpan(0, 0, 0, 0, 100);
            timerAffichage.Tick += TimerAffichage_Tick;
            timerAffichage.Start();

        }

        private void TimerAffichage_Tick(object sender, EventArgs e)
        {
            if (robot.receivedText != "")
            {
                textBoxReception.Text += robot.receivedText;
                robot.receivedText = "";

                while (robot.byteListReceived.Count > 0)
                {
                        byte val=robot.byteListReceived.Dequeue();
                        textBoxReception.Text += val.ToString();
                }
            }
        }

        private void SerialPort1_DataReceived(object sender, DataReceivedArgs e)
        {
           

            robot.receivedText += Encoding.UTF8.GetString(e.Data, 0, e.Data.Length);
            for(int i=1;i<e.Data.Length;i++)
                 robot.byteListReceived.Enqueue(e.Data[i]) ;
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

        private void SendMessage()
        {
            //serialPort1.WriteLine(textBoxEmission.Text);
            //byte[] DataSend = Encoding.ASCII.GetBytes(textBoxEmission.Text);
            //for (int i=0;Lengt(DataSend);i++)

            //serialPort1.WriteLine(DataSend);
            //textBoxEmission.Text = "";

        }


        private void TextBox_KeyUp(object sender, KeyEventArgs e)
        {
            if (e.Key == Key.Enter)
            {
                SendMessage();
            }
        }

        private void button_Click(object sender, RoutedEventArgs e)
        {
            textBoxReception.Text = "";
        }

        byte[] byteList= new byte [20];
        private object unsigned;

        private void Test_Click(object sender, RoutedEventArgs e)
        {
            // int i;
            //for (int i= 0; i < 20; i++)
            //{
            //    byteList[i] = (byte)(2*i);
            //}
            //serialPort1.Write(byteList, 0, byteList.Length);
            byte[] array = Encoding.ASCII.GetBytes("Bonjour");
            UartEncodeAndSendMessage(0x0080, array.Length, array);
        }



        /////////////////////////////////////////////////////////////////  TRAME
       
        byte CalculateChecksum(int msgFunction, int msgPayloadLength, byte[] msgPayload)
        { 
            byte checksum = 0;
            checksum ^= 0xFE;
            checksum ^= (byte)(msgFunction>>8);
            checksum ^= (byte)(msgFunction >>0);
            checksum ^= (byte)(msgPayloadLength>>8);
            checksum ^= (byte)(msgPayloadLength >>0);
            for (int i=0;i<msgPayloadLength;i++){
                checksum ^= msgPayload[i];
              
            }
            return checksum;

        }

        void UartEncodeAndSendMessage(int msgFunction, int msgPayloadLength, byte[] msgPayload) 
        {
            byte[] msg = new byte[msgPayloadLength+6];
            msg[0] = 0xFE;
            msg[1]=(byte)(msgFunction >> 8);
            msg[2] =(byte)(msgFunction >> 0);
            msg[3]= (byte)(msgPayloadLength >> 8);
            msg[4] = (byte)(msgPayloadLength >> 0);
            for(int i=0; i< msgPayloadLength; i++)
            {
                msg[5 + i] = msgPayload[i];
            }
            msg[5+msgPayloadLength] = CalculateChecksum(msgFunction, msgPayloadLength, msgPayload);

            serialPort1.Write(msg, 0, msg.Length);
        }






    }
}
