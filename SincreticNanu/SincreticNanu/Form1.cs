using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.IO.Ports;
using System.Linq;
using System.Net;
using System.Net.Mail;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace SincreticNanu
{
    public partial class Form1 : Form
    {
        SerialPort mySerialPort = new SerialPort("COM3", 9600);

        NetworkCredential login;
        SmtpClient client;
        MailMessage msg;

        public Form1()
        {
            InitializeComponent();
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            mySerialPort.Open();
            timer1.Start();
        }

        private void button1_Click_1(object sender, EventArgs e)
        {
            string s = "[b].";
            mySerialPort.Write(s);
        }

        private void timer1_Tick(object sender, EventArgs e)
        {
            string s= mySerialPort.ReadExisting();

            if (s.Contains("Temp:"))
            {
                String[] temperatura = s.Split(':');
                if (temperatura[1].Length > 0)
                {
                    textBox1.Text = temperatura[1];
                }
            }
        }

        private void button2_Click(object sender, EventArgs e)
        {
            if(textBox2.Text != null)
            {
                mySerialPort.Write(textBox2.Text);
            }
        }
    }
}
