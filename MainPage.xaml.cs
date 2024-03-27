using uPLibrary.Networking.M2Mqtt.Messages;
using uPLibrary.Networking.M2Mqtt;
using System.Text;

namespace PersonalAssignemnt
{
    public partial class MainPage : ContentPage
    {
        MqttClient client = new MqttClient("80.115.248.174");
        bool LedStatus = false;

        public MainPage()
        {
            InitializeComponent();
        }

        protected override void OnAppearing()
        {
            SetupBroker();
        }

        async void SetupBroker()
        {
            string clientId = Guid.NewGuid().ToString();
            client.MqttMsgPublishReceived += client_MqttMsgPublishReceived;
            client.Connect(clientId);
            client.Subscribe(new string[] { "Light/Chip" }, new byte[] { MqttMsgBase.QOS_LEVEL_AT_MOST_ONCE });
            client.Publish("Light/New", Encoding.Default.GetBytes("Send"));
        }

        async void LightsSwitched(object sender, EventArgs args)
        {
            if (LedStatus) { client.Publish("Light/Control", Encoding.Default.GetBytes("OFF")); }
            else { client.Publish("Light/Control", Encoding.Default.GetBytes("ON")); }
        }

        void updateGUI()
        {

                Button button = ((Button)FindByName("Controller"));
                if (LedStatus)
                {
                MainThread.BeginInvokeOnMainThread(() =>
                    {
                        button.Text = "ON";
                        button.BackgroundColor = Colors.Green;
                    });
                }
                else
                {
                MainThread.BeginInvokeOnMainThread(() =>
                    {
                        button.Text = "OFF";
                        button.BackgroundColor = Colors.Red;
                    });
                }
        }

        async void client_MqttMsgPublishReceived(object sender, MqttMsgPublishEventArgs e)
        {
            string message = Encoding.UTF8.GetString(e.Message);
            bool.TryParse(message, out LedStatus);
            updateGUI();
        }

    }



}
