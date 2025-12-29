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
        //Parameters are created in the developer backend. https://developer.ad.mguwp.net
        private const string YourAppId = "692e5d6a207c9dd383ba56f7";
        private const string YourSecretKey = "MIGTAgEAMBMGByqGSM49AgEGCCqGSM49AwEHBHkwdwIBAQQg9pm4A6JkgQr7Xx5UUmX/NT+ZKM+ZF/2btAIBsdrJF76gCgYIKoZIzj0DAQehRANCAARrjJmtngZzxhRAa0Wn99ZN7QGf9ozmvghuvaicqFmA3j35XDkfXBgIqMTABogfpd+1LrAADeXkgOPzqw6b12my";
        private const string SplashAdUnitId = "9ad41e3410084523a4f2a13ca65df395";         //Splash Ad:1920 x 1080
        private const string ExitAdUnitId = "a0e4a92613674feab0e3eaa36d1c17b8";           //Exit Ad:1920 x 1080
        private const string BannerUnitId = "948ba7ccdfa34a0b8e2f96a66244c494";           //Banner Ad:728 x 90
        private const string InterstitialUnitId = "10494292d2d9431691c3bebf0f35815c";     //Interstitial Ad:1024 x 768
        private const string CoupletUnitId = "ef5f566b0eb14132987efbd57d6af60f";          //Couple Ad:300 x 600
        private const string RewardedUnitId = "a9bd7d57faef4f8cb016979284c86102";         //Rewarded Ad:1024x768
        private const string FeedUnitId = "e13d9a6a4dbd42c2bd50561773dbda40";             //Feed，Developers need to maintain the advertising control.
        private const string EmbeddedUnitId = "eb60d1e936c044adb696d9fa8147d590";         //Embedded，Developers need to maintain the advertising control.

        private void AdvertManager_AdCloseEvent(object sender, string e)
        {
            ShowMessage("The ad has been disabled. " + e);

            //Standard Ad {"unitId":"6bf68881673540788d096b9ea4a3cedb","advertStatus":1,"resourceId":"68d20656bd9558abfdf43465","materialId":"d235efa86ccf44acbe7053af760031b6"}
            //Rewarded Ad {"unitId":"5c1e9aae78014be7908bf035848d7075","advertStatus":1,"completeStatus":1,"resourceId":"68ecb9eb20f045c603867874","materialId":"b0817d87ee2544629bac1933a60238d2","comment":"id123%2Cabc%2C%249.99","rewardId":"D1E593C16BBD412CA880FD89F0450A14"}

            JObject jsonObject = JObject.Parse(e);
            string unitId = (string)jsonObject["unitId"];

            if (unitId == RewardedUnitId)//Rewarded Ad, with rewards distributed based on the results returned.
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
                AdvertManager.SetupExitAd(ExitAdUnitId);

                //...


                AdvertManager.ShowAd(this, SplashAdUnitId, AdType.FullScreen);//Splash Ad
            }
            else
            {
                ShowMessage("Initialization failed");
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

        /// <summary>
        /// Splash Ad
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void btnAd1_Click(object sender, EventArgs e)
        {
            AdvertManager.ShowAd(this, SplashAdUnitId, AdType.FullScreen);
        }
        private void btnAd1_2_Click(object sender, EventArgs e)
        { 
            AdvertManager.ShowAd(this, "{\"unitId\": \"" + SplashAdUnitId + "\",\"media\":\"video\"}", AdType.FullScreen);
        }

        private void btnAd1_3_Click(object sender, EventArgs e)
        {
            AdvertManager.ShowAd(this, "{\"unitId\": \"" + SplashAdUnitId + "\",\"media\":\"web\"}", AdType.FullScreen);
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
            AdvertManager.ShowAd(this, "{\"unitId\": \"" + RewardedUnitId + "\",\"comment\":\"" + comment + "\"}", AdType.Reward);
        }
        private void btnAd6_2_Click(object sender, EventArgs e)
        {
            string comment = "id123,abc,$9.99";
            comment = Uri.EscapeDataString(comment);
            AdvertManager.ShowAd(this, "{\"unitId\": \"" + RewardedUnitId + "\",\"comment\":\"" + comment + "\",\"media\":\"web\"}", AdType.Reward);
        }

        //Feed
        private void btnAd7_Click(object sender, EventArgs e)
        {
            dynamic jsonObj = new
            {
                unitId = FeedUnitId,//Customizable dimensions, example: MG backend settings 400×50
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
                unitId = FeedUnitId,
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
