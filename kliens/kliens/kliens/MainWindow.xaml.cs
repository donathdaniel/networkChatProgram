using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Net.Sockets;
using System.Text;
using System.Threading;
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

public enum ActionType
{
    CONNECT = 0,
    SEND_PUBLIC = 1,
    SEND_PRIVATE = 2,
    CREATE_GROUP = 3,
    ADD_TO_GROUP = 4,
    SEND_TO_GROUP = 5,
    REMOVE_FROM_GROUP = 6,
    DELETE_GROUP = 7,
    SEND_FILE = 8,
    SEND_IMAGE = 9
};


namespace kliens
{

    public partial class MainWindow : Window
    {

        private readonly TcpClient client = new TcpClient();
        private NetworkStream networkStream = default;
        private string userName;
        private readonly string baseOutputString = ">> ";

        private ListBox outputTextBox;
        private TextBox nameTextBox;
        private Button connectButton;

        private TextBox sendTextBox;

        public MainWindow()
        {
            InitializeComponent();
            InitializeUI();
        }


        //UI 
        private void InitializeUI()
        {
            outputTextBox = (ListBox)FindName("OutputListBox");
            nameTextBox = (TextBox)FindName("NameTextBox");
            connectButton = (Button)FindName("ConnectButton");


            sendTextBox = (TextBox)FindName("SendTextBox");
        }

        private void AfterConnectSetup()
        {
            userName = nameTextBox.Text;
            outputTextBox.Items.Add(baseOutputString + "**YOU ARE CONNECTED** " + userName);

            connectButton.IsEnabled = false;
            nameTextBox.IsEnabled = false;

            Button sendTextButton = (Button)FindName("SendTextButton");
            Button createGroupButton = (Button)FindName("CreateGroupButton");

            sendTextButton.IsEnabled = true;
            createGroupButton.IsEnabled = true;
        }

        private void AfterGroupSetup(string outputMessage)
        {
            //UI changes after added to the group or removed from the group
            Button addToGroupButton = (Button)FindName("AddToGroupButton");
            Button removeFromGroupButton = (Button)FindName("RemoveFromGroupButton");
            Button deleteGroupButton = (Button)FindName("DeleteGroupButton");

            if (outputMessage.Contains("ADDED TO THE GROUP"))
            {
                addToGroupButton.IsEnabled = true;
                removeFromGroupButton.IsEnabled = true;
                deleteGroupButton.IsEnabled = true;
            }
            else if (outputMessage.Contains("REMOVED FROM THE GROUP") || outputMessage.Contains("DELETE GROUP"))
            {
                addToGroupButton.IsEnabled = false;
                removeFromGroupButton.IsEnabled = false;
                deleteGroupButton.IsEnabled = false;
            }
        }

        //Helper methods
        private string createMessage(ActionType actionType, string sender, string receiver, string message)
        {
            return ((int)actionType).ToString() + "#" + sender + "#" + receiver + "#" + message + "#";
        }


        //Click Listeners


        //---------------------------------------Connect-----------------------------------------
        private void ConnectButton_Click(object sender, RoutedEventArgs e)
        {
            if (!string.IsNullOrWhiteSpace(nameTextBox.Text))
            {
                client.Connect("127.0.0.1", 27015);
                AfterConnectSetup();

                NetworkStream serverStream = client.GetStream();
                byte[] writeStream = Encoding.ASCII.GetBytes(createMessage(ActionType.CONNECT, userName, " ", " "));

               /* byte[] writeStreamLenght = Encoding.ASCII.GetBytes(writeStream.Length.ToString());


                serverStream.Write(writeStreamLenght, 0, writeStreamLenght.Length);
                serverStream.Flush();*/

                serverStream.Write(writeStream, 0, writeStream.Length);
                serverStream.Flush();

                Thread thread = new Thread(ThreadHelper);
                thread.Start();

            }
        }
        private void ThreadHelper()
        {

            while (true)
            {
                byte[] readStream = new byte[client.ReceiveBufferSize];

                networkStream = client.GetStream();
                networkStream.Read(readStream, 0, client.ReceiveBufferSize);
                string outputMessage = Encoding.ASCII.GetString(readStream);


                this.Dispatcher.Invoke(() =>
                {
                    if (outputMessage[0] == '#')
                    {
                        outputMessage = outputMessage.Remove(0, 1);

                        string[] connectedNamesList = outputMessage.Split("#");
                        ListBox connectedNamesListBox = (ListBox)FindName("ConnectedNamesListBox");
                        connectedNamesListBox.Items.Clear();

                        foreach (string s in connectedNamesList)
                        {
                            connectedNamesListBox.Items.Add(s);
                        }
                    }

                    else
                    {
                        OutputListBox.Items.Add(baseOutputString + outputMessage);

                        AfterGroupSetup(outputMessage);
                    }
                });
            }
        }


        //---------------------------------Send public and private and group-------------------------------
        private void SendTextButton_Click(object sender, RoutedEventArgs e)
        {
            ComboBox SendTypeComboBox = (ComboBox)FindName("SendTypeComboBox");
            ComboBoxItem SendTypeComboBoItem = (ComboBoxItem)SendTypeComboBox.SelectedItem;

            if (SendTypeComboBoItem != null)
            {
                if (!string.IsNullOrWhiteSpace(sendTextBox.Text))
                {
                    if (SendTypeComboBoItem.Content.ToString() == "Public") SendPublic();
                    else if (SendTypeComboBoItem.Content.ToString() == "Private") SendPrivate();
                    else if (SendTypeComboBoItem.Content.ToString() == "Group") SendGroup();
                }
                else if (SendTypeComboBoItem.Content.ToString() == "File") SendFile();
                else if (SendTypeComboBoItem.Content.ToString() == "Image") SendImage();
            }
        }


        void SendPublic()
        {

            NetworkStream serverStream = client.GetStream();
            byte[] writeStream = Encoding.ASCII.GetBytes(createMessage(ActionType.SEND_PUBLIC, userName, " ", sendTextBox.Text));
           /* byte[] writeStreamLenght = Encoding.ASCII.GetBytes(writeStream.Length.ToString());


            serverStream.Write(writeStreamLenght, 0, writeStreamLenght.Length);
            serverStream.Flush();*/

            serverStream.Write(writeStream, 0, writeStream.Length);
            serverStream.Flush();
        }

        bool ConnectedNamesListBoxItemIsSelected(ref string receiver)
        {
            ListBox connectedNamesListBox = (ListBox)FindName("ConnectedNamesListBox");
            if (connectedNamesListBox.SelectedIndex == -1)
            {
                return false;
            }

            string connectedNamesListBoxItemString = connectedNamesListBox.SelectedItem.ToString();
            if (connectedNamesListBoxItemString == null || connectedNamesListBoxItemString == userName)
            {
                return false;
            }

            receiver = connectedNamesListBoxItemString;
            return true;
        }

        void SendPrivate()
        {
            string receiver = "";
            if (ConnectedNamesListBoxItemIsSelected(ref receiver))
            {
                NetworkStream serverStream = client.GetStream();
                byte[] writeStream = Encoding.ASCII.GetBytes(createMessage(ActionType.SEND_PRIVATE, userName, receiver, sendTextBox.Text));

                serverStream.Write(writeStream, 0, writeStream.Length);
                serverStream.Flush();
            }
        }

        void SendGroup()
        {
            Button addToGroupButton = (Button)FindName("AddToGroupButton");

            if (addToGroupButton.IsEnabled == false)
                return;


            NetworkStream serverStream = client.GetStream();
            byte[] writeStream = Encoding.ASCII.GetBytes(createMessage(ActionType.SEND_TO_GROUP, userName, " ", sendTextBox.Text));

            serverStream.Write(writeStream, 0, writeStream.Length);
            serverStream.Flush();

        }

        public byte[] ReadAllBytes(string fileName)
        {
            byte[] buffer = null;
            using (FileStream fs = new FileStream(fileName, FileMode.Open, FileAccess.Read))
            {
                buffer = new byte[fs.Length];
                fs.Read(buffer, 0, (int)fs.Length);
            }
            return buffer;
        }

        public static byte[] Combine(byte[] first, byte[] second)
        {
            byte[] ret = new byte[first.Length + second.Length];
            Buffer.BlockCopy(first, 0, ret, 0, first.Length);
            Buffer.BlockCopy(second, 0, ret, first.Length, second.Length);
            return ret;
        }


        void SendFile()
        {
            string receiver = "";
            if (ConnectedNamesListBoxItemIsSelected(ref receiver))
            {
                NetworkStream serverStream = client.GetStream();

                string fileName = @"text.txt";
                byte[] fileData = ReadAllBytes(fileName);
                
                byte[] writeStream = Combine(Encoding.ASCII.GetBytes(createMessage(ActionType.SEND_FILE, userName, receiver, "")), fileData);

                for (int i = 0; i < writeStream.Length; i += 1024)
                {
                    serverStream.Write(writeStream, i, writeStream.Length);
                    serverStream.Flush();
                }

            }
        }

        void SendImage()
        {
            string receiver = "";
            if (ConnectedNamesListBoxItemIsSelected(ref receiver))
            {
                NetworkStream serverStream = client.GetStream();

                string fileName = @"eper2.jpg";
                byte[] fileData = ReadAllBytes(fileName);

                byte[] writeStream = Combine(Encoding.ASCII.GetBytes(createMessage(ActionType.SEND_IMAGE, userName, receiver, "")), fileData);

                serverStream.Write(writeStream, 0, writeStream.Length);
                /*for (int i = 0; i < writeStream.Length; i += 1024)
                {
                    serverStream.Write(writeStream, i, writeStream.Length);
                    serverStream.Flush();
                }*/

            }
        }


        //---------------------------------Create Group-----------------------------------


        private void CreateGroupButton_Click(object sender, RoutedEventArgs e)
        {
            TextBox groupNameTextBox = (TextBox)FindName("GroupNameTextBox");

            if (!string.IsNullOrWhiteSpace(groupNameTextBox.Text))
            {
                NetworkStream serverStream = client.GetStream();
                byte[] writeStream = Encoding.ASCII.GetBytes(createMessage(ActionType.CREATE_GROUP, userName, " ", groupNameTextBox.Text));

                serverStream.Write(writeStream, 0, writeStream.Length);
                serverStream.Flush();

                //UI changes after creating the group
                Button addToGroupButton = (Button)FindName("AddToGroupButton");
                Button removeFromGroupButton = (Button)FindName("RemoveFromGroupButton");
                Button deleteGroupButton = (Button)FindName("DeleteGroupButton");

                addToGroupButton.IsEnabled = true;
                removeFromGroupButton.IsEnabled = true;
                deleteGroupButton.IsEnabled = true;
            }

        }

        private void AddToGroupButton_Click(object sender, RoutedEventArgs e)
        {
            string receiver = "";
            if (ConnectedNamesListBoxItemIsSelected(ref receiver) == true)
            {
                NetworkStream serverStream = client.GetStream();
                byte[] writeStream = Encoding.ASCII.GetBytes(createMessage(ActionType.ADD_TO_GROUP, userName, receiver, " "));

                serverStream.Write(writeStream, 0, writeStream.Length);
                serverStream.Flush();
            }
        }

        private void RemoveFromGroupButton_Click(object sender, RoutedEventArgs e)
        {
            string receiver = "";
            if (ConnectedNamesListBoxItemIsSelected(ref receiver) == true)
            {
                NetworkStream serverStream = client.GetStream();
                byte[] writeStream = Encoding.ASCII.GetBytes(createMessage(ActionType.REMOVE_FROM_GROUP, userName, receiver, " "));

                serverStream.Write(writeStream, 0, writeStream.Length);
                serverStream.Flush();
            }
        }

        private void DeleteGroupButton_Click(object sender, RoutedEventArgs e)
        {
            NetworkStream serverStream = client.GetStream();
            byte[] writeStream = Encoding.ASCII.GetBytes(createMessage(ActionType.DELETE_GROUP, userName, " ", " "));

            serverStream.Write(writeStream, 0, writeStream.Length);
            serverStream.Flush();


        }
    }
}
