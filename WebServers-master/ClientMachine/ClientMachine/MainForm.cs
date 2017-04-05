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
using System.IO;

namespace ClientMachine
{
    public partial class MainForm : Form
    {
        TcpClient clientSocket = new TcpClient();

        public NetworkStream serverStream { get; set; }
        public byte[] FileStreamReader { get; set; }
        public int StreamSize { get; set; }
        List<string> lstMessages = new List<string>();

        public MainForm()
        {
            InitializeComponent();
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            msg(@"Client Started");
            timerSyncronizer.Enabled = true;
            clientSocket.Connect("127.0.0.1", 8885);

            label1.Text = "Client Socket Program - Server Connected ...";
        }


        public void msg(string mesg)

        {

            textBox1.Text = mesg;

        }

        private void button1_Click(object sender, EventArgs e)
        {

            var tasks = new List<Task>();
            int reqestsPerThread = 0;
            int ThreadCount = 0;
            string fileNameRequested = txtQueryString.Text;
            if (int.TryParse(txtRequestsPerThread.Text, out reqestsPerThread) &&
                int.TryParse(txtThreadCount.Text, out ThreadCount))
            {

                for (int i = 0; i < ThreadCount; i++)
                {
                    tasks.Add(Task.Factory.StartNew(() =>
                    {
                        RequestFilesFromServer(reqestsPerThread, fileNameRequested);
                    }));
                }
                //Task.WaitAll(tasks.ToArray());

            }
            else
            {
                MessageBox.Show("Integer Cant Be Parsed Please Enter Integers.");
            }
        }

        private void textBox1_TextChanged(object sender, EventArgs e)
        {

        }
        public void RequestFilesFromServer(int numberOfRequest, string fileName)
        {
            for (int i = 0; i < numberOfRequest; i++)
            {
                NetworkStream serverStream = clientSocket.GetStream();
                byte[] OutStream = Encoding.ASCII.GetBytes(string.Format(@"{0}$", fileName));
                serverStream.Write(OutStream, 0, OutStream.Length);

                serverStream.Flush();

                byte[] inStream = new byte[clientSocket.ReceiveBufferSize];
                if (inStream.Any(i => i > 0))
                {
                    string FileProduced = string.Format(@"D:/TestClient/{0}", fileName);
                    serverStream.Read(inStream, 0, clientSocket.ReceiveBufferSize);

                    using (FileStream fileStream = File.Create(FileProduced, (int)inStream.Length))
                    {
                        fileStream.Write(inStream, 0, inStream.Length);
                    }
                    lstMessages.Add("Request Responded.");
                    this.webBrowser1.Navigate(FileProduced);
                }
                else
                {
                    MessageBox.Show("Connection From Server is disconnected.");
                }
                //string returndata = System.Text.Encoding.ASCII.GetString(inStream);

            }
        }
        public string SendFileToServer(int numberOfRequests)
        {
            for (int i = 0; i < numberOfRequests; i++)
            {

                NetworkStream serverStream = clientSocket.GetStream();

                byte[] outStream = System.Text.Encoding.ASCII.GetBytes(string.Format(@"$"));

                StreamSize = clientSocket.ReceiveBufferSize;

                FileStreamReader = Combine(FileStreamReader, outStream);

                serverStream.Write(FileStreamReader, 0, FileStreamReader.Length);

                serverStream.Flush();



                byte[] inStream = new byte[clientSocket.ReceiveBufferSize];

                serverStream.Read(inStream, 0, clientSocket.ReceiveBufferSize);

                string returndata = System.Text.Encoding.ASCII.GetString(inStream);

                return string.Format("Data from Server : " + returndata);
            }
            return string.Empty;
        }

        byte[] Combine(byte[] a1, byte[] a2)
        {
            byte[] ret = new byte[a1.Length + a2.Length];
            Array.Copy(a1, 0, ret, 0, a1.Length);
            Array.Copy(a2, 0, ret, a1.Length, a2.Length);
            return ret;
        }
        private void label4_Click(object sender, EventArgs e)
        {

        }

        private void button2_Click(object sender, EventArgs e)
        {
            if (ofdFileToSend.ShowDialog() == DialogResult.OK)
            {
                FileStreamReader = ReadFully(ofdFileToSend.FileName);
                txtMessage.Text = Path.GetFileName(ofdFileToSend.FileName);
            }
        }
        public byte[] ReadFully(string filename)
        {
            FileStream fs = new FileStream(filename, FileMode.Open, FileAccess.Read);

            byte[] bytes = System.IO.File.ReadAllBytes(filename);

            //Close the File Stream
            fs.Close();
            return bytes; //return the byte data
        }

        private void timer1_Tick(object sender, EventArgs e)
        {
            msg(string.Join(Environment.NewLine + " >> ", lstMessages));
        }

        private void btnRefresh_Click(object sender, EventArgs e)
        {
            try
            {
                clientSocket.Connect("127.0.0.1", 8885);

                label1.Text = "Client Socket Program - Server Connected ...";
            }
            catch (Exception exp)
            {
                MessageBox.Show(exp.Message);
            }
        }
    }
}
