using Client;
using System;
using System.IO;
using System.Net;
using System.Net.Sockets;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading;

public class SocketClient
{

    public static int Main(String[] args)
    {
        // Demander le nom du host pour ensuite se connecer à ce dernier.
        string serverIP = string.Empty;
        do
        {
            Console.Write("[Veuillez entrer le nom du host] : ");
            serverIP = Communication.getIpAdressFromHost(Console.ReadLine());
        } while (serverIP.Equals(String.Empty));

        byte[] bytes = new byte[1024]; // Déclaration du buffer
        try
        {
            int port = 8080;
            IPEndPoint remoteEP = new IPEndPoint(IPAddress.Parse(serverIP), port);
            // Création de la ocket TCP/IP
            Socket sender = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
            try
            {
                // Connecter le socket.
                sender.Connect(remoteEP);
                Console.WriteLine("Socket connectée à {0}", sender.RemoteEndPoint.ToString());
                Console.WriteLine("[Entrez '" + Communication.QUIT + "' pour quitter le chat] :");

                // THREADING.
                Info.threads      = new Threading(sender);
                Thread threadRecv = new Thread(new ThreadStart(Handler.receive));
                Thread threadSend = new Thread(new ThreadStart(Handler.send));
                threadRecv.Start();
                threadSend.Start();

                //sender.Shutdown(SocketShutdown.Both);
                //sender.Close();
            }
            catch (ArgumentNullException ane) { Console.WriteLine("ArgumentNullException : {0}", ane.ToString()); }
            catch (SocketException se) { Console.WriteLine("SocketException : {0}", se.ToString()); }
            catch (Exception e) { Console.WriteLine("Unexpected exception : {0}", e.ToString()); }
        }
        catch (Exception e) { Console.WriteLine(e.ToString()); }
        return 0;
    }
}

public static class EncodeMethods
{
    [DllImport(@"C:\Users\guertz\Google Drive\Session 5\ClientServeur\Serveur\Serveur\Debug\DLL.dll")]
    public static extern void encodeMessage(int position, StringBuilder messageReceived, bool isChiffrer);
}
