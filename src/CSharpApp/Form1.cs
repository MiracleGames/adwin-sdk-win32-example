using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using MiracleGamesAd;
using MiracleGamesAd.Models;
using MiracleGamesAd.Services.Core.Common;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;

namespace CSharpApp
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
            ApplicationManager.CmpClosedEvent += ApplicationManager_CmpClosedEvent;
            AdvertManager.AdClickEvent += AdvertManager_AdClickEvent;
            AdvertManager.AdCloseEvent += AdvertManager_AdCloseEvent;
        }
        //Production environment
        private const string YourAppId = "69316b6861328938223cc124";
        private const string YourSecretKey = "MIGTAgEAMBMGByqGSM49AgEGCCqGSM49AwEHBHkwdwIBAQQgZgULOuiIDYZyGiUyYdGr3odHVN6ebZ1uDwXx7PXiHh2gCgYIKoZIzj0DAQehRANCAASf1FWCfsSn/tXFVRt04C7JkpRG12KSC3wnaJRWb5QWin9dsBk1OR31BCsELMYtWsFhA7e6Q6Fi4Mi6+ub24O5a";
        private const string SplashScreenUnitId = "b871f83c5e8845f1b43325561bcdd6c7";     //Splash Ad:1920 x 1080
        private const string ExitScreenUnitId = "5076eab6ae1042b6b92f73ea01981475";       //Exit Ad:1920 x 1080
        private const string BannerUnitId = "cb7d9688a2d9499992febb6b642b3625";           //Banner Ad:728 x 90
        private const string InterstitialUnitId = "2cb66a1301404561881a3f26b6ce5ba7";     //Interstitial Ad:1024 x 768
        private const string CoupletUnitId = "b502f6e6281c43e4b28ea22503471039";          //Couple Ad:300 x 600
        private const string RewardUnitId = "2ae60936ba664fbfb7d92ce3a19c2915";           //Rewarded Ad:1024x768
        private const string InformationFlowUnitId = "f152f6caf7a8440f8510bc31534baf4e";  //Feed，Developers need to maintain the advertising control.
        private const string EmbeddedUnitId = "4192966a9db343f48dd2f6308ea9ec30";         //Embedded，Developers need to maintain the advertising control.
        
        private void AdvertManager_AdCloseEvent(object sender, string e)
        {
            ShowMessage("The ad has been disabled. " + e);

            //Standard Ad {"unitId":"6bf68881673540788d096b9ea4a3cedb","advertStatus":1,"resourceId":"68d20656bd9558abfdf43465","materialId":"d235efa86ccf44acbe7053af760031b6"}
            //Rewarded Ad {"unitId":"5c1e9aae78014be7908bf035848d7075","advertStatus":1,"completeStatus":1,"resourceId":"68ecb9eb20f045c603867874","materialId":"b0817d87ee2544629bac1933a60238d2","comment":"id123%2Cabc%2C%249.99","rewardId":"D1E593C16BBD412CA880FD89F0450A14"}

            JObject jsonObject = JObject.Parse(e);
            string unitId = (string)jsonObject["unitId"];

            if (unitId == RewardUnitId)//Rewarded Ad, with rewards distributed based on the results returned.
            {
                int completeStatus = (int)jsonObject["completeStatus"];
                string resourceId = (string)jsonObject["resourceId"];
                string materialId = (string)jsonObject["materialId"];
                string rewardId = (string)jsonObject["rewardId"];
                if (completeStatus == 1)
                {
                    //Video playback complete. Reward items distributed. 
                    //...

                    Task.Run(async () =>
                    {
                        _ = await AdvertManager.ReportAdRewardFulfillment(unitId, resourceId, materialId, rewardId);//Report to MG
                    });
                }
            }
        }

        private void AdvertManager_AdClickEvent(object sender, string e)
        {
            ShowMessage("Ad clicked " + e);
        }

        private void ApplicationManager_CmpClosedEvent(object sender, string e)
        {
            ShowMessage("CMP Authorization results " + e);
        }

        private async void Form1_Load(object sender, EventArgs e)
        {
            ApplicationManager.SetAppId(YourAppId, YourSecretKey);
            ApplicationManager.OpenCmp(this);

            var result = await ApplicationManager.Initialize();
            if (result.ReturnValue)
            {
                //Initialization successful... 
                ShowMessage($"Initialization complete:Token={ApplicationManager.AccessToken.Token}, ExpiresIn={ApplicationManager.AccessToken.ExpiresIn}");

                //Step 1: After successful initialization, load the Exit ad resources.
                AdvertManager.SetupExitAd(ExitScreenUnitId);

                //...


                AdvertManager.ShowAd(this, SplashScreenUnitId, AdType.FullScreen);//Splash Ad
            }
            else
            {
                ShowMessage("Initialization failed");
            }
        }

        /// <summary>
        /// Splash Ad
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void btnAd1_Click(object sender, EventArgs e)
        {
            AdvertManager.ShowAd(this, SplashScreenUnitId, AdType.FullScreen);
        }
        private void btnAd1_2_Click(object sender, EventArgs e)
        { 
            AdvertManager.ShowAd(this, "{\"unitId\": \"" + SplashScreenUnitId + "\",\"media\":\"video\"}", AdType.FullScreen);
        }

        private void btnAd1_3_Click(object sender, EventArgs e)
        {
            AdvertManager.ShowAd(this, "{\"unitId\": \"" + SplashScreenUnitId + "\",\"media\":\"web\"}", AdType.FullScreen);
        }

        /// <summary>
        /// Interstitial Ad  
        /// media support:image,video,web
        /// size:1024*768
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void btnAd4_Click(object sender, EventArgs e)
        {
            AdvertManager.ShowAd(this, InterstitialUnitId, AdType.Interstitial);
        }  
        private void btn4_2_Click(object sender, EventArgs e)
        {
            AdvertManager.ShowAd(this, "{\"unitId\": \"" + InterstitialUnitId + "\",\"media\":\"video\"}", AdType.Interstitial);
        }
        private void btn4_3_Click(object sender, EventArgs e)
        {
            AdvertManager.ShowAd(this, "{\"unitId\": \"" + InterstitialUnitId + "\",\"media\":\"web\"}", AdType.Interstitial);
        }

        /// <summary>
        /// Banner Ad 
        /// 728*90
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void btnAd3_Click(object sender, EventArgs e)
        {
            AdvertManager.ShowAd(this, BannerUnitId, AdType.Banner);
        }

        private void btnAd3_2_Click(object sender, EventArgs e)
        {
            AdvertManager.ShowAd(this, "{\"unitId\": \"" + BannerUnitId + "\",\"media\":\"web\"}", AdType.Banner);
        }

        /// <summary>
        /// Couple Ad
        /// 300*600
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void btnAd5_Click(object sender, EventArgs e)
        {
            AdvertManager.ShowAd(this, "{\"unitId\": \"" + CoupletUnitId + "\",\"tag\":\"MGADKEY_COUPLET_LEFT\",\"tag2\":\"MGADKEY_COUPLET_RIGHT\"}", AdType.Couplet);
        }
        private void btnAd5_2_Click(object sender, EventArgs e)
        {
            AdvertManager.ShowAd(this, "{\"unitId\": \"" + CoupletUnitId + "\",\"tag\":\"MGADKEY_COUPLET_LEFT\",\"tag2\":\"MGADKEY_COUPLET_RIGHT\",\"media\":\"web\"}", AdType.Couplet);
        }

        /// <summary>
        /// Rewarded Ad
        /// 768*432
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void btnAd6_Click(object sender, EventArgs e)
        {
            string comment = "id123,abc,$9.99";//Transparent parameters require URL encoding.
            comment = Uri.EscapeDataString(comment);
            AdvertManager.ShowAd(this, "{\"unitId\": \"" + RewardUnitId + "\",\"comment\":\"" + comment + "\"}", AdType.Reward);
        }
        private void btnAd6_2_Click(object sender, EventArgs e)
        {
            string comment = "id123,abc,$9.99";
            comment = Uri.EscapeDataString(comment);
            AdvertManager.ShowAd(this, "{\"unitId\": \"" + RewardUnitId + "\",\"comment\":\"" + comment + "\",\"media\":\"web\"}", AdType.Reward);
        }

        //Feed
        private void btnAd7_Click(object sender, EventArgs e)
        {
            dynamic jsonObj = new
            {
                unitId = InformationFlowUnitId,//Customizable dimensions, example: MG backend settings 400×50
                media = "image",
                width = panelAd6.Width,
                height = panelAd6.Height
            };
            string json = JsonConvert.SerializeObject(jsonObj);
            AdvertManager.ShowAd(this.panelAd6, json, AdType.InformationFlow);
        }
        private void btnAd7_2_Click(object sender, EventArgs e)
        {
            dynamic jsonObj = new
            {
                unitId = InformationFlowUnitId,
                media = "web",
                width = panelAd6_2.Width,
                height = panelAd6_2.Height
            };
            string json = JsonConvert.SerializeObject(jsonObj);
            AdvertManager.ShowAd(this.panelAd6_2, json, AdType.InformationFlow);
        }

        //Embedded
        private void btnAd8_Click(object sender, EventArgs e)
        {
            dynamic jsonObj = new
            {
                unitId = EmbeddedUnitId,//Customizable dimensions, example: MG backend settings 200x200
                media = "image",
                width = panelAd.Width,
                height = panelAd.Height
            };
            string json = JsonConvert.SerializeObject(jsonObj);
            AdvertManager.ShowAd(this.panelAd, json, AdType.Embedded);
        }

        private void btnAd8_2_Click(object sender, EventArgs e)
        {
            dynamic jsonObj = new
            {
                unitId = EmbeddedUnitId,
                media = "web",
                width = panelAd8_2.Width,
                height = panelAd8_2.Height
            };
            string json = JsonConvert.SerializeObject(jsonObj);
            AdvertManager.ShowAd(this.panelAd8_2, json, AdType.Embedded);
        }

        /// <summary>
        /// Remove Ad Slot
        /// Frontend functionality; developers are responsible for removing specified controls;
        /// This action does not trigger the SDK's ad close event.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void btnDeleteAd_Click(object sender, EventArgs e)
        {
            for (int i = panelAd.Controls.Count; i > 0; i--)
            {
                Control item = panelAd.Controls[0];
                item.Dispose();
                panelAd.Controls.Remove(item);
            }
            for (int i = panelAd6.Controls.Count; i > 0; i--)
            {
                Control item = panelAd6.Controls[0];
                item.Dispose();
                panelAd.Controls.Remove(item);
            }
            for (int i = this.Controls.Count; i > 0; i--)
            {
                Control item = this.Controls[0];
                if (item.Tag != null && item.Tag.ToString().Contains("MGADKEY_"))
                {
                    item.Dispose();
                    this.Controls.Remove(item);
                }
            }

            for (int i = this.Controls.Count; i > 0; i--)//Remove all MG Ads ad from the page
            {
                Control item = this.Controls[0];
                if (item.CompanyName == "MiracleGamesAd.Controls")
                {
                    item.Dispose();
                    this.Controls.Remove(item);
                }
            }
        } 

        /// <summary>
        /// Exit ad
        /// Step 2. Display a Exit ad when the app closes.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void Form1_FormClosing(object sender, FormClosingEventArgs e)
        {
            _ = AdvertManager.ShowExitAdBlocking();
        }

        private void ShowMessage(string message)
        {
            if (this.InvokeRequired)
            {
                this.Invoke(new Action(() => ShowMessage(message)));
            }
            else
            {
                StringBuilder sb = new StringBuilder(textBox1.Text);
                sb.Append("\r\n" + message);
                textBox1.Text = sb.ToString();
            }
        }

        private void btnDeleteCmp_Click(object sender, EventArgs e)
        {
            for (int i = this.Controls.Count; i > 0; i--)
            {
                Control item = this.Controls[0];
                if (item.Name == "UCCmpView")
                {
                    item.Dispose();
                    this.Controls.Remove(item);
                    return;
                }
            }
        } 
    }
}
