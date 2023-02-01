using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace RobotInterface_BENGUET
{
    public class Robot
    {

        public string receivedText = "";
        public float distanceTelemetreDroit;
        public float distanceTelemetreCentre;
        public float distanceTelemetreGauche;
        public Queue<byte> byteListReceived;
        public Robot()
        {
            byteListReceived = new Queue<byte>();
        } 
    }
}
