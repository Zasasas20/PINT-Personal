using uPLibrary.Networking.M2Mqtt.Messages;
using uPLibrary.Networking.M2Mqtt;
using System.Text;

namespace PersonalAssignemnt
{
    public partial class MainPage : ContentPage
    {
        MqttClient client = new MqttClient("80.115.248.174");
        bool LedStatus = false;
        bool AutoStatus = false;

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
            client.Subscribe(new string[] { "Light/Chip", "Light/ChipA" }, new byte[] { MqttMsgBase.QOS_LEVEL_AT_MOST_ONCE, MqttMsgBase.QOS_LEVEL_AT_MOST_ONCE }) ;
            client.Publish("Light/New", Encoding.Default.GetBytes("SEND"));
            await Task.Yield();
        }

        async void LightsSwitched(object sender, EventArgs args)
        {
            client.Publish("Light/Control", Encoding.Default.GetBytes("LIGHT"));
            await Task.Yield();
        }

        async void AutoSwitched(object sender, EventArgs args)
        {
            client.Publish("Light/Control", Encoding.Default.GetBytes("AUTO")); 
            await Task.Yield();
        }

        void updateGUI()
        {

                Button Light = ((Button)FindByName("LightController"));
                Button Auto = ((Button)FindByName("AutoController"));
                if (LedStatus)
                {
                MainThread.BeginInvokeOnMainThread(() =>
                    {
                        Light.Text = "ON";
                        Light.BackgroundColor = Colors.Green;
                    });
                }
                else
                {
                MainThread.BeginInvokeOnMainThread(() =>
                    {
                        Light.Text = "OFF";
                        Light.BackgroundColor = Colors.Red;
                    });
                }

                if (AutoStatus)
                {
                    MainThread.BeginInvokeOnMainThread(() =>
                    {
                        AutoController.Text = "ON";
                        Auto.BackgroundColor = Colors.Green;
                    });
                }
                else
                {
                    MainThread.BeginInvokeOnMainThread(() =>
                    {
                        Auto.Text = "OFF";
                        Auto.BackgroundColor = Colors.Red;
                    });
                }

        }

        async void client_MqttMsgPublishReceived(object sender, MqttMsgPublishEventArgs e)
        {
            string message = Encoding.UTF8.GetString(e.Message);
            if (e.Topic == "Light/ChipA")
            {
                bool.TryParse(message, out AutoStatus);
            }
            else
            {
                bool.TryParse(message, out LedStatus);
            }
            updateGUI();
            await Task.Yield();
        }

    }



}
