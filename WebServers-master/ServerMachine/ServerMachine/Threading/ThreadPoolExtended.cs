using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Net.Sockets;
using System.Reflection;
using System.Security.Permissions;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace ServerMachine.Threading
{
    public static class ThreadPoolExtended
    {
        //private static LocalDataStoreSlot _Slot = Thread.AllocateNamedDataSlot("webserver.data");

        public static void Execute(byte[] bytesFrom, NetworkStream networkStream)
        {
            ThreadPool.QueueUserWorkItem(o => ThreadProc(bytesFrom, networkStream));
            Console.WriteLine("Main thread Assigned task to worker thread then sleeps.");
            
        }

        private static Random random = new Random();

        static void ThreadProc(byte[] bytesFrom, NetworkStream networkStream)
        {
            try
            {

                //// No state object was passed to QueueUserWorkItem, so stateInfo is null.
                string dataFromClient = System.Text.Encoding.ASCII.GetString(bytesFrom);
                ////Thread.SetData(_Slot, data);

                dataFromClient = dataFromClient.Substring(0, dataFromClient.IndexOf("$"));

                //Console.WriteLine(" >> Data from client - " + dataFromClient);
                string Filename = string.Format(@"D:/TestServer/{0}", dataFromClient);
                byte[] ResponceToClient;
                // Find File from Server
                if (File.Exists(Filename))
                {
                    if (IsReadable(filename:Filename))
                    {
                        ResponceToClient = ReadFully(filename: Filename);
                        networkStream.Write(ResponceToClient, 0, ResponceToClient.Length);

                        networkStream.Flush();

                        Console.WriteLine(string.Format("Request File {0} is responded successfully.", dataFromClient));
                    }
                    else
                    {
                        string serverResponse = string.Format(@"Error Code : 403 {0}Request File {1} is forbidden.", Environment.NewLine, dataFromClient);
                        ResponceToClient = Encoding.ASCII.GetBytes(serverResponse);
                        networkStream.Write(ResponceToClient, 0, ResponceToClient.Length);

                        networkStream.Flush();

                        Console.WriteLine(serverResponse);
                    }
                }
                else
                {
                    // Convert File to byte stream and send back
                    string serverResponse = string.Format(@"Error Code : 404 {0}Request File {1} not Found", Environment.NewLine, dataFromClient);
                    ResponceToClient = Encoding.ASCII.GetBytes(serverResponse);
                    networkStream.Write(ResponceToClient, 0, ResponceToClient.Length);

                    networkStream.Flush();

                    Console.WriteLine(serverResponse);

                }

            }
            catch (Exception exp)
            {
                Console.WriteLine(exp.Message);
                throw;
            }

        }
        public static byte[] ReadFully(string filename)
        {
            FileStream fs = new FileStream(filename, FileMode.Open, FileAccess.Read);

            byte[] bytes = System.IO.File.ReadAllBytes(filename);

            //Close the File Stream
            fs.Close();
            return bytes; //return the byte data
        }

        public static bool IsReadable(string filename)
        {
            try
            {
                FileStream fs = new FileStream(filename, FileMode.Open, FileAccess.Read);

                byte[] bytes = System.IO.File.ReadAllBytes(filename);

                //Close the File Stream
                fs.Close();
                return true;

            }
            catch (Exception)
            {
                return false;
            }

            return false;
        }
    }
}
