using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Net.Sockets;
using System.Text;
using System.Threading.Tasks;
using System.Threading;

namespace Client
{
    // COMMUNICATION CLASS
    public abstract class Communication
    {
        #region Constructeur + Attributs
        protected Socket socket;
        protected string message;
        public static string QUIT = "QUIT";

        public Socket Socket
        {
            get { return socket; }
            set { socket = value; }
        }
        public string Message
        {
            get { return message; }
            set { message = value; }
        }
        public Communication(Socket sender)
        {
            this.Socket = sender;
        }

        #endregion

        public static string getIpAdressFromHost(string host)
        {
            try
            {

                Console.WriteLine("Tentative de connexion à '{0}' ...", host);
                // Retrouve les informations de l'hôte à partir du nom
                System.Net.IPHostEntry he = System.Net.Dns.GetHostEntry(host);
                // Retourner l'adresse IP
                if (he.AddressList != null && he.AddressList.Length == 2)
                    return he.AddressList[1].ToString(); // Aussi tester avec la case 0 du tableau

                return string.Empty;
            }
            catch (ArgumentNullException)
            {
                Console.Error.WriteLine("La chaine 'host' est une référence null.");
                return string.Empty;
            }
            catch (SocketException e)
            {
                Console.Error.WriteLine("Une erreur s'est produite lors de la connexion à '{0}'.\r\n{1}", host, e.Message);
                return string.Empty;
            }
            catch (System.Security.SecurityException)
            {
                Console.Error.WriteLine("L'appelant n'est pas autorisé à accéder aux informations DNS.");
                return string.Empty;
            }
        }

        public static String getFileName(String message)
        {
            int index = 0;
            foreach (char character in message)
            {
                if (character.Equals('('))
                {
                    ++index;
                    break;
                }
                ++index;
            }

            return message.Substring(index, (message.Length - index - 1));
        }

        public String getReceivedFileName(String message)
        {
            int index = 0;
            int position1 = 0;
            int position2 = 0;

            foreach (var character in message)
            {
                if (character.Equals('('))
                    position1 = index + 1;
                if (character.Equals(')'))
                    position2 = index + 1;

                ++index;
            }

            return message.Substring(position1, (position2 - position1 - 1));
        }

        public String getContentMessage(String message)
        {
            int index = 0;
            int position1 = 0;
            foreach (char character in message)
            {
                if (character.Equals(')'))
                    position1 = index + 2;
                ++index;
            }

            return message.Substring(position1, message.Length - position1 - 1);
        }

        public void createFile(String contenu, String filename)
        {
            var streamWriter = new StreamWriter(filename);
            using (streamWriter)
            {
                streamWriter.WriteLine(contenu);
            }
            streamWriter.Close();
        }
    }

    // RECV CLASS
    public class Recv : Communication
    {
        #region Constructor and attributes
        private byte[] bytes = new byte[1024];
        public Recv(Socket handler) : base(handler)
        {
            this.Message = "";
        }

        public byte[] Bytes
        {
            get { return this.bytes; }
            set { this.bytes = value; }
        }
        #endregion

        public void receiveMessage()
        {
            do
            {
                // Recevoir la réponse du serveur.
                int bytesRec = socket.Receive(bytes);
                string msgReceived = Encoding.ASCII.GetString(bytes, 0, bytesRec);
                StringBuilder decodedMessage = new StringBuilder();
                decodedMessage.Append(msgReceived);
                EncodeMethods.encodeMessage(7, decodedMessage, true);

                if (decodedMessage.ToString().Equals(QUIT))
                {
                    Console.ForegroundColor = ConsoleColor.Red;
                    Console.Error.WriteLine("Connection closed");
                    Environment.Exit(Environment.ExitCode);
                    break;
                }

                Console.ForegroundColor = ConsoleColor.Blue;
                Console.WriteLine(decodedMessage.ToString());
                Console.ForegroundColor = ConsoleColor.Green;

                if (decodedMessage.ToString().Trim().Contains("EnvoiFichier"))
                    this.createFile(this.getContentMessage(decodedMessage.ToString()), this.getReceivedFileName(decodedMessage.ToString()));
            } while (Socket.Connected && !Message.Equals(QUIT));
        }
    }

    // RECV CLASS
    public class Send : Communication
    {
        public Send(Socket handler) : base(handler) {}

        public void sendFile()
        {
            string filename = getFileName(Message);
            var file = Path.GetFileName(filename);
            bool error = false;
            do
            {
                if (error)
                {
                    // Change text color
                    Console.ForegroundColor = ConsoleColor.Blue;
                    Message = Console.ReadLine();
                    if (!Message.Contains("EnvoiFichier"))
                    {
                        sendMessage();
                        return;
                    }
                    else
                    {
                        filename = getFileName(Message);
                        file = Path.GetFileName(filename);
                    }
                }

                if (File.Exists(file))
                {
                    //Chaque ligne de texte du fichier
                    String lineOfText;
                    //Résultat final
                    String result = "";

                    //Parametre du filestream
                    var filestream = new System.IO.FileStream(file,
                              System.IO.FileMode.Open,
                              System.IO.FileAccess.Read,
                              System.IO.FileShare.ReadWrite);
                    //Prendre le contenu du fichier texte en UTF-8
                    var fileContent = new System.IO.StreamReader(filestream, System.Text.Encoding.UTF8, true, 128);

                    //Append chaque ligne au resultat final
                    while ((lineOfText = fileContent.ReadLine()) != null)
                    {
                        result += lineOfText;
                        result += " ";
                    }

                    //Building final massage
                    String myMessage = "EnvoiFichier(" + filename + ") " + result;

                    //FAIRE LE ENCODING
                    //Fonction ici
                    StringBuilder t = new StringBuilder();
                    t.Append(myMessage);
                    EncodeMethods.encodeMessage(7, t, false);

                    //Mettre le resultat final en bytes       
                    byte[] msg = Encoding.ASCII.GetBytes(t.ToString());
                    //Envoyer le message tirer du text file
                    int bytesSent = Socket.Send(msg);
                }
                else
                {
                    Console.WriteLine("Le fichier n'existe pas.");
                    error = true;
                }
            } while (!File.Exists(file));
        }

        public void sendMessage()
        {
            //Encoder le message
            StringBuilder x = new StringBuilder();
            x.Append(Message);
            EncodeMethods.encodeMessage(7, x, false);
            // Encoder le string en tableau de byte(octet)
            byte[] msg = Encoding.ASCII.GetBytes(x.ToString());
            // Envoyer le message au serveur.
            int bytesSent = Socket.Send(msg);
        }

        public void sendStuff()
        {
            do
            {
                Console.ForegroundColor = ConsoleColor.Green;
                this.Message = Console.ReadLine();
                Console.ForegroundColor = ConsoleColor.Green;
                Info.threads.ReceiveTool.Message = this.Message;
                if (!this.Message.Equals(QUIT))
                {
                    // Envoyer le message/fichier au serveur
                    if (message.Contains("EnvoiFichier")) this.sendFile();
                    else this.sendMessage();
                }
                else
                {
                    Console.ForegroundColor = ConsoleColor.Red;
                    Console.Error.WriteLine("Connection closed");
                    Environment.Exit(Environment.ExitCode);
                    break;
                }
            } while (Socket.Connected && !Message.Equals(QUIT));
        }
    }

    // THREADING CLASS
    public class Threading
    {
        private Recv receiveTool;
        private Send sendTool;

        public Threading(Socket handler)
        {
            ReceiveTool = new Recv(handler);
            SendTool    = new Send(handler);
        }

        public Recv ReceiveTool
        {
            get { return this.receiveTool; }
            set { this.receiveTool = value;  }
        }
        public Send SendTool
        {
            get { return this.sendTool; }
            set { this.sendTool = value; }
        }
    }

    // INFO CLASS
    public static class Info
    {
        public static Threading threads;
    }

    // HANDLER CLASS
    public static class Handler
    {
        public static void send()
        {
            Info.threads.SendTool.sendStuff();
        }

        public static void receive()
        {
            Info.threads.ReceiveTool.receiveMessage();
        }
    }
}
