using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Net.Sockets;

namespace ClientMachine
{
    public partial class Form1 : Form
    {
        TcpClient clientSocket = new TcpClient();

        NetworkStream serverStream;

        public Form1()
        {
            InitializeComponent();
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            msg(@"Client Started");

            clientSocket.Connect("127.0.0.1", 8888);

            label1.Text = "Client Socket Program - Server Connected ...";
        }


        public void msg(string mesg)

        {

            textBox1.Text = textBox1.Text + Environment.NewLine + " >> " + mesg;

        }

        private void button1_Click(object sender, EventArgs e)
        {
            NetworkStream serverStream = clientSocket.GetStream();

            byte[] outStream = System.Text.Encoding.ASCII.GetBytes(string.Format(@"{0}{1}",txtMessage.Text, @"$"));

            serverStream.Write(outStream, 0, outStream.Length);

            serverStream.Flush();



            byte[] inStream = new byte[(int)clientSocket.ReceiveBufferSize];

            serverStream.Read(inStream, 0, (int)clientSocket.ReceiveBufferSize);

            string returndata = System.Text.Encoding.ASCII.GetString(inStream);

            msg("Data from Server : " + returndata);
        }

        private void textBox1_TextChanged(object sender, EventArgs e)
        {

        }
    }
}
