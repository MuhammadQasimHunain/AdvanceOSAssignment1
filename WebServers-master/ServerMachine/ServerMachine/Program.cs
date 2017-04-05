using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using System.Net.Sockets;
using System.Threading;
using ServerMachine.Threading;

namespace ServerMachine
{
    class Program
    {
        static void Main(string[] args)
        {
            int workerThreads = 0;
            int completeionPortThreads = 0;
            string responceFromUser = string.Empty;
            // THis should be asked at the start of the Program
            Console.WriteLine("Enter Number of worker threads?");
            responceFromUser = Console.ReadLine();
            workerThreads = int.Parse(responceFromUser);

            Console.WriteLine("Enter Number of completion worker threads?");
            responceFromUser = Console.ReadLine();
            completeionPortThreads = int.Parse(responceFromUser);

            RestartApplication:
            Console.Clear();

            // Limiting Thread Pool
            ThreadPool.SetMaxThreads(workerThreads, completeionPortThreads);

            TcpListener serverSocket = new TcpListener(8885);

            int requestCount = 0;

            TcpClient clientSocket = default(TcpClient);

            serverSocket.Start();

            Console.WriteLine(" >> Server Started");

            clientSocket = serverSocket.AcceptTcpClient();

            Console.WriteLine(" >> Accept connection from client");

            requestCount = 0;
            while ((true))

            {

                try

                {

                    requestCount = requestCount + 1;

                    NetworkStream networkStream = clientSocket.GetStream();

                    byte[] bytesFrom = new byte[(int)clientSocket.ReceiveBufferSize];

                    networkStream.Read(bytesFrom, 0, (int)clientSocket.ReceiveBufferSize);

                    if (bytesFrom.Any(i => i > 0))
                    {
                        // Passing Value to Thread Pool execution and the Boss will resume it's execution.
                        ThreadPoolExtended.Execute(bytesFrom, networkStream);
                    }
                    throw new Exception();
                }

                catch (Exception ex)

                {

                    //Console.WriteLine(ex.ToString());
                    Console.WriteLine("Refreshing Server");
                    serverSocket.Stop();
                    Thread.Sleep(1500);
                    goto RestartApplication;
                }

            }



            clientSocket.Close();

            serverSocket.Stop();

            Console.WriteLine(" >> exit");

            Console.ReadLine();


        }
    }
}
