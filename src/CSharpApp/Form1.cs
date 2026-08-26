using MiracleGamesAd;
using MiracleGamesAd.Models;
using MiracleGamesAd.Services.Core.Common;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;
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

namespace CSharpApp
{
    public partial class Form1 : Form
    {
        /*
         * 1.Downlodad the SDK  https://doc.mg-ads.com/en/docs/AdSdkService/CSharpSdk/SdkDownload
         * 2.Reference the SDK  https://doc.mg-ads.com/en/docs/AdSdkService/CSharpSdk/AdIntegrationGuide
         */
        public Form1()
        {
            InitializeComponent();
            AdvertManager.AdClickEvent += AdvertManager_AdClickEvent;
            AdvertManager.AdCloseEvent += AdvertManager_AdCloseEvent;

            adUnitMap = new Dictionary<string, string>
            {
                ["FullScreenAdUnitId"] = FullScreenAdUnitId,
                ["BannerUnitId"] = BannerUnitId,
                ["InterstitialUnitId"] = InterstitialUnitId,
                ["CoupletUnitId"] = CoupletUnitId,
                ["RewardedUnitId"] = RewardedUnitId
            };
        }
         
        //Production environment
        private const string YourAppId = "69316b6861328938223cc124";
        private const string YourSecretKey = "MIGTAgEAMBMGByqGSM49AgEGCCqGSM49AwEHBHkwdwIBAQQgZgULOuiIDYZyGiUyYdGr3odHVN6ebZ1uDwXx7PXiHh2gCgYIKoZIzj0DAQehRANCAASf1FWCfsSn/tXFVRt04C7JkpRG12KSC3wnaJRWb5QWin9dsBk1OR31BCsELMYtWsFhA7e6Q6Fi4Mi6+ub24O5a";
        private const string FullScreenAdUnitId = "b871f83c5e8845f1b43325561bcdd6c7";             //Fullscreen Ad:1920 x 1080
        private const string ExitAdUnitId = "5076eab6ae1042b6b92f73ea01981475";                 //Exit Ad:1920 x 1080
        private const string BannerUnitId = "cb7d9688a2d9499992febb6b642b3625";               //Banner Ad:728 x 90
        private const string InterstitialUnitId = "2cb66a1301404561881a3f26b6ce5ba7";           //Interstitial Ad:1024 x 768
        private const string CoupletUnitId = "b502f6e6281c43e4b28ea22503471039";              //Couple Ad:300 x 600
        private const string RewardedUnitId = "2ae60936ba664fbfb7d92ce3a19c2915";           //Rewarded Ad:1024x768
        private const string FeedUnitId = "f152f6caf7a8440f8510bc31534baf4e";                      //Feed，Developers need to maintain the advertising control.
        private const string EmbeddedUnitId = "4192966a9db343f48dd2f6308ea9ec30";         //Embedded，Developers need to maintain the advertising control.
  
        private void AdvertManager_AdClickEvent(object sender, string e)
        {
            ShowMessage("Ad clicked " + e);
        }

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


        private async void Form1_Load(object sender, EventArgs e)
        {
            try
            {
                // ========= Step 1: Call SetAppId (set parameters) =========
                ApplicationManager.SetAppId(YourAppId, YourSecretKey);

                // ========= Step 2: Call OpenCmp (Core Interface) =========
                CmpParameters cmpOptions = new CmpParameters();
                // false (Recommended): Popup only appears once after the user's first selection, compliant with GDPR
                // true: Popup appears every time the app starts, suitable for testing environments
                cmpOptions.IgnoreExpiredCheck = false;
                var cmpresult = await ApplicationManager.OpenCmp(cmpOptions);
                if (cmpresult.ReturnValue)
                {
                    // CMP popup displayed successfully
                    ShowMessage($"CMP is required in this region, CMP result={cmpresult.Data?.ToString()}");
                }

                // ========= Optional: Get Region CMP Requirement (Additional Interface, Used in Specific Scenarios) =========
                //You can only determine whether “CMP is required in the current user's region” after the OpenCmp interface has been executed.
                /*
                bool isUserRegionCmpRequired = ApplicationManager.UserRegionCmpRequirement;
                ShowMessage($"CMP is required in this region, value={isUserRegionCmpRequired.ToString()}");
                */

                // ========= Step 3: Call Initialize (Core Interface) =========
                var result = await ApplicationManager.Initialize();
                if (result.ReturnValue)
                {
                    //Initialization successful... 
                    ShowMessage($"Initialization complete:Token={ApplicationManager.AccessToken.Token}, ExpiresIn={ApplicationManager.AccessToken.ExpiresIn}");

                    //Step 1: After successful initialization, load the Exit ad resources.
                    AdvertManager.SetupExitAd(ExitAdUnitId);

                    //...

                   await AdvertManager.ShowAd(this, FullScreenAdUnitId, AdType.FullScreen);//Fullscreen Ad
                }
                else
                {
                    ShowMessage("Initialization failed");
                }
            }
            catch (Exception)
            {
            }
        }

        /// <summary>
        /// FullScreen Ad
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private async void btnAd1_Click(object sender, EventArgs e)
        {
            try
            {
                AsyncProcessResult result = await AdvertManager.ShowAd(this, FullScreenAdUnitId, AdType.FullScreen);
                if (result.ReturnValue)
                {

                }
            }
            catch (Exception)
            {
            }
        }
        private async void btnAd1_2_Click(object sender, EventArgs e)
        {
            try
            {
                //media：Supported types: web, video; Generally, developers do not need to configure this.;If no value is provided, a random selection is made based on the MG backend configuration.
                AsyncProcessResult result = await AdvertManager.ShowAd(this, "{\"unitId\": \"" + FullScreenAdUnitId + "\",\"media\":\"video\"}", AdType.FullScreen);
                if (result.ReturnValue)
                { 
                }
            }
            catch (Exception)
            {
            }
        }

        private async void btnAd1_3_Click(object sender, EventArgs e)
        {
            try
            {
                AsyncProcessResult result = await AdvertManager.ShowAd(this, "{\"unitId\": \"" + FullScreenAdUnitId + "\",\"media\":\"web\"}", AdType.FullScreen);
                if (result.ReturnValue)
                {
                }
            }
            catch (Exception)
            {
            }
        }
        private async void btnAd1_4_Click(object sender, EventArgs e)
        {
            try
            {
                AsyncProcessResult result = await AdvertManager.ShowAd(this, "{\"unitId\": \"" + FullScreenAdUnitId + "\",\"media\":\"image\"}", AdType.FullScreen);
                if (result.ReturnValue)
                {
                }
            }
            catch (Exception)
            {
            }
        }

        /// <summary>
        /// Interstitial Ad  
        /// media support:image,video,web
        /// size:1024*768
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private async void btnAd4_Click(object sender, EventArgs e)
        {
            try
            {
                AsyncProcessResult result = await AdvertManager.ShowAd(this, InterstitialUnitId, AdType.Interstitial);
                if (result.ReturnValue)
                {
                }
            }
            catch (Exception)
            {
            }
        }
        private async void btn4_2_Click(object sender, EventArgs e)
        {
            try
            {
                AsyncProcessResult result = await AdvertManager.ShowAd(this, "{\"unitId\": \"" + InterstitialUnitId + "\",\"media\":\"video\"}", AdType.Interstitial);
                if (result.ReturnValue)
                {
                }
            }
            catch (Exception)
            {
            }
        }
        private async void btn4_3_Click(object sender, EventArgs e)
        {
            try
            {
                AsyncProcessResult result = await AdvertManager.ShowAd(this, "{\"unitId\": \"" + InterstitialUnitId + "\",\"media\":\"web\"}", AdType.Interstitial);
                if (result.ReturnValue)
                {
                }
            }
            catch (Exception)
            {
            }
        }
        private async void btn4_4_Click(object sender, EventArgs e)
        {
            try
            {
                AsyncProcessResult result = await AdvertManager.ShowAd(this, "{\"unitId\": \"" + InterstitialUnitId + "\",\"media\":\"image\"}", AdType.Interstitial);
                if (result.ReturnValue)
                {
                }
            }
            catch (Exception)
            {
            }
        }
        /// <summary>
        /// Banner Ad 
        /// 728*90
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private async void btnAd3_Click(object sender, EventArgs e)
        {
            try
            {
                AsyncProcessResult result = await AdvertManager.ShowAd(this, BannerUnitId, AdType.Banner);
                if (result.ReturnValue)
                {
                }
            }
            catch (Exception)
            {
            }
        }

        private async void btnAd3_2_Click(object sender, EventArgs e)
        {
            try
            {
                AsyncProcessResult result = await AdvertManager.ShowAd(this, "{\"unitId\": \"" + BannerUnitId + "\",\"media\":\"web\"}", AdType.Banner);
                if (result.ReturnValue)
                {
                }
            }
            catch (Exception)
            {
            }
        }
        private async void btnAd3_3_Click(object sender, EventArgs e)
        {
            try
            {
                AsyncProcessResult result = await AdvertManager.ShowAd(this, "{\"unitId\": \"" + BannerUnitId + "\",\"media\":\"image\"}", AdType.Banner);
                if (result.ReturnValue)
                {
                }
            }
            catch (Exception)
            {
            }
        }
        /// <summary>
        /// Couple Ad
        /// 300*600
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private async void btnAd5_Click(object sender, EventArgs e)
        {
            try
            {
                AsyncProcessResult result = await AdvertManager.ShowAd(this, "{\"unitId\": \"" + CoupletUnitId + "\",\"tag\":\"MGADKEY_COUPLET_LEFT\",\"tag2\":\"MGADKEY_COUPLET_RIGHT\"}", AdType.Couplet);
                if (result.ReturnValue)
                {
                }
            }
            catch (Exception)
            {
            }
        }
        private async void btnAd5_2_Click(object sender, EventArgs e)
        {
            try
            {
                AsyncProcessResult result = await AdvertManager.ShowAd(this, "{\"unitId\": \"" + CoupletUnitId + "\",\"tag\":\"MGADKEY_COUPLET_LEFT\",\"tag2\":\"MGADKEY_COUPLET_RIGHT\",\"media\":\"web\"}", AdType.Couplet);
                if (result.ReturnValue)
                {
                }
            }
            catch (Exception)
            {
            }
        }
        private async void btnAd5_3_Click(object sender, EventArgs e)
        {
            try
            {
                AsyncProcessResult result = await AdvertManager.ShowAd(this, "{\"unitId\": \"" + CoupletUnitId + "\",\"tag\":\"MGADKEY_COUPLET_LEFT\",\"tag2\":\"MGADKEY_COUPLET_RIGHT\",\"media\":\"image\"}", AdType.Couplet);
                if (result.ReturnValue)
                {
                }
            }
            catch (Exception)
            {
            }
        }

        /// <summary>
        /// Rewarded Ad
        /// 768*432
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private async void btnAd6_Click(object sender, EventArgs e)
        {
            try
            {
                string comment = "id123,abc,$9.99";//Transparent parameters require URL encoding.
                comment = Uri.EscapeDataString(comment);
                AsyncProcessResult result = await AdvertManager.ShowAd(this, "{\"unitId\": \"" + RewardedUnitId + "\",\"comment\":\"" + comment + "\",\"isFullScreen\":true}", AdType.Rewarded);
                if (result.ReturnValue)
                {
                }
            }
            catch (Exception)
            {
            }
        }
        private async void btnAd6_2_Click(object sender, EventArgs e)
        {
            try
            {
                string comment = "id123,abc,$9.99";
                comment = Uri.EscapeDataString(comment);
                AsyncProcessResult result = await AdvertManager.ShowAd(this, "{\"unitId\": \"" + RewardedUnitId + "\",\"comment\":\"" + comment + "\",\"media\":\"web\"}", AdType.Rewarded);
                if (result.ReturnValue)
                {
                }
            }
            catch (Exception)
            {
            }
        }
        private async void btnAd6_3_Click(object sender, EventArgs e)
        {
            try
            {
                string comment = "id123,abc,$9.99";
                comment = Uri.EscapeDataString(comment);
                AsyncProcessResult result = await AdvertManager.ShowAd(this, "{\"unitId\": \"" + RewardedUnitId + "\",\"comment\":\"" + comment + "\",\"media\":\"video\"}", AdType.Rewarded);
                if (result.ReturnValue)
                {
                }
            }
            catch (Exception)
            {
            }
        }
        //Feed
        private async void btnAd7_Click(object sender, EventArgs e)
        {
            try
            {
                dynamic jsonObj = new
                {
                    unitId = FeedUnitId,//Customizable dimensions, example: MG backend settings 400×50
                    width = panelAd6.Width,
                    height = panelAd6.Height
                };
                string json = JsonConvert.SerializeObject(jsonObj);
                AsyncProcessResult result = await AdvertManager.ShowAd(this.panelAd6, json, AdType.Feed);
                if (result.ReturnValue)
                {
                }
            }
            catch (Exception)
            {
            }
        }
        private async void btnAd7_2_Click(object sender, EventArgs e)
        {
            try
            {
                dynamic jsonObj = new
                {
                    unitId = FeedUnitId,
                    media = "web",
                    width = panelAd6_2.Width,
                    height = panelAd6_2.Height,
                    cmpLocationX = 50,
                    cmpLocationY = 200
                };
                string json = JsonConvert.SerializeObject(jsonObj);
                AsyncProcessResult result = await AdvertManager.ShowAd(this.panelAd6_2, json, AdType.Feed);
                if (result.ReturnValue)
                {
                }
            }
            catch (Exception)
            {
            }
        }
        private async void btnAd7_3_Click(object sender, EventArgs e)
        {
            try
            {
                dynamic jsonObj = new
                {
                    unitId = FeedUnitId,
                    media = "image",
                    width = panelAd6_2.Width,
                    height = panelAd6_2.Height
                };
                string json = JsonConvert.SerializeObject(jsonObj);
                AsyncProcessResult result = await AdvertManager.ShowAd(this.panelAd6_2, json, AdType.Feed);
                if (result.ReturnValue)
                {
                }
            }
            catch (Exception)
            {
            }
        }
        //Embedded
        private async void btnAd8_Click(object sender, EventArgs e)
        {
            try
            {
                dynamic jsonObj = new
                {
                    unitId = EmbeddedUnitId,//Customizable dimensions, example: MG backend settings 200x200
                    width = panelAd.Width,
                    height = panelAd.Height
                };
                string json = JsonConvert.SerializeObject(jsonObj);
                AsyncProcessResult result = await AdvertManager.ShowAd(this.panelAd, json, AdType.Embedded);
                if (result.ReturnValue)
                {
                }
            }
            catch (Exception)
            {
            }
        }

        private async void btnAd8_2_Click(object sender, EventArgs e)
        {
            try
            {
                dynamic jsonObj = new
                {
                    unitId = EmbeddedUnitId,
                    media = "web",
                    width = panelAd8_2.Width,
                    height = panelAd8_2.Height
                };
                string json = JsonConvert.SerializeObject(jsonObj);
                AsyncProcessResult result = await AdvertManager.ShowAd(this.panelAd8_2, json, AdType.Embedded);
                if (result.ReturnValue)
                {
                }
            }
            catch (Exception)
            {
            }
        }
        private async void btnAd8_3_Click(object sender, EventArgs e)
        {
            try
            {
                dynamic jsonObj = new
                {
                    unitId = EmbeddedUnitId,
                    media = "image",
                    width = panelAd8_2.Width,
                    height = panelAd8_2.Height
                };
                string json = JsonConvert.SerializeObject(jsonObj);
                AsyncProcessResult result = await AdvertManager.ShowAd(this.panelAd8_2, json, AdType.Embedded);
                if (result.ReturnValue)
                {
                }
            }
            catch (Exception)
            {
            }
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
            try
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
            catch (Exception)
            {
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
            try
            {
                //_ = AdvertManager.ShowExitAdBlocking();
            }
            catch (Exception)
            {
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

        private Dictionary<string, string> adUnitMap;
        private async void btn_getAd1_Click(object sender, EventArgs e)
        {
            try
            {
                Button btnGetAd = (Button)sender;
                string btnTag = btnGetAd.Tag.ToString();
                string adUnitId = adUnitMap[btnTag.Split(',')[0]];
                AdType adType = (AdType)Convert.ToInt32(btnTag.Split(',')[1]);

                ShowMessage($"Ad unit[{adUnitId}] [{DateTime.Now.ToString("yyyy-MM-dd HH:mm:ss")}] Start Preloading...");
                dynamic jsonObj = new
                {
                    unitId = adUnitId
                };
                string json = JsonConvert.SerializeObject(jsonObj);
                var result = await AdvertManager.PreloadAd(json, adType);
                if (result.ReturnValue)
                {
                    EnableAdBtn(btnTag.Split(',')[0]);
                    ShowMessage($"Ad unit[{adUnitId}] [{DateTime.Now.ToString("yyyy-MM-dd HH:mm:ss")}] materials are ready.");
                }
                else
                {
                    ShowMessage($"Ad unit[{adUnitId}] [{DateTime.Now.ToString("yyyy-MM-dd HH:mm:ss")}] Preload failed：[{result.ErrorCode}]");
                }
            }
            catch (Exception)
            {
            }
        }
        private void EnableAdBtn(string adKey)
        {
            foreach (var item in groupBox1.Controls)
            {
                if (item is Button button)
                {
                    if (button.Tag.ToString().Contains($"SHOW_{adKey}"))
                    {
                        button.Enabled = true;
                        return;
                    }
                }
            }
        }
        private void btn_showAd1_Click(object sender, EventArgs e)
        {
            try
            {
                Button btn = (Button)sender;
                string btnTag = btn.Tag.ToString();
                string adUnitId = adUnitMap[btnTag.Split(',')[0].Replace("SHOW_", "")];
                AdType adType = (AdType)Convert.ToInt32(btnTag.Split(',')[1]);
                dynamic jsonObj = new
                {
                    unitId = adUnitId
                };
                string json = JsonConvert.SerializeObject(jsonObj);
                var result = AdvertManager.ShowPreloadAd(this, json, adType);
                if (result.ReturnValue)
                    btn.Enabled = false;
            }
            catch (Exception)
            {
            }
        }

        private async void btn_getGoogleAd1_Click(object sender, EventArgs e)
        {
            try
            {
                Button btnGetAd = (Button)sender;
                string btnTag = btnGetAd.Tag.ToString();
                string adUnitId = adUnitMap[btnTag.Split(',')[0]];
                AdType adType = (AdType)Convert.ToInt32(btnTag.Split(',')[1]);

                ShowMessage($"GoogleAd unit[{adUnitId}] [{DateTime.Now.ToString("yyyy-MM-dd HH:mm:ss")}] Start Preloading...");
                dynamic jsonObj = new
                {
                    unitId = adUnitId,
                    media = "web",//Supported types: web, video; Generally, developers do not need to configure this.;If no value is provided, a random selection is made based on the MG backend configuration.
                    isFullScreen = true
                };
                string json = JsonConvert.SerializeObject(jsonObj);
                var result = await AdvertManager.PreloadAd(json, adType);
                if (result.ReturnValue)
                {
                    foreach (var item in groupBox2.Controls)
                    {
                        if (item is Button button)
                        {
                            if (button.Tag.ToString().Contains($"SHOW_{btnTag.Split(',')[0]}"))
                            {
                                button.Enabled = true;
                                break;
                            }
                        }
                    }

                    ShowMessage($"GoogleAd unit[{adUnitId}] [{DateTime.Now.ToString("yyyy-MM-dd HH:mm:ss")}] materials are ready.");
                }
                else
                {
                    ShowMessage($"GoogleAd unit[{adUnitId}] [{DateTime.Now.ToString("yyyy-MM-dd HH:mm:ss")}] Preload failed：[{result.ErrorCode}]");
                }
            }
            catch (Exception)
            {
            }
        }

        private void btn_showGoogleAd1_Click(object sender, EventArgs e)
        {
            try
            {
                Button btn = (Button)sender;
                string btnTag = btn.Tag.ToString();
                string adUnitId = adUnitMap[btnTag.Split(',')[0].Replace("SHOW_", "")];
                AdType adType = (AdType)Convert.ToInt32(btnTag.Split(',')[1]);
                dynamic jsonObj = new
                {
                    unitId = adUnitId,
                    media = "web",
                    isFullScreen = true
                };
                string json = JsonConvert.SerializeObject(jsonObj);
                var result = AdvertManager.ShowPreloadAd(this, json, adType);
                if (result.ReturnValue)
                    btn.Enabled = false;
            }
            catch (Exception)
            {
            }
        }

        private async void btnOpenCmp_Click(object sender, EventArgs e)
        {
            try
            {
                bool isUserRegionCmpRequired = ApplicationManager.UserRegionCmpRequirement;
                ShowMessage($"CMP is required in this region, value={isUserRegionCmpRequired.ToString()}");

                if (isUserRegionCmpRequired) // CMP is required for this region.
                {
                    var cmpresult = await ApplicationManager.OpenCmp(new CmpParameters { IgnoreExpiredCheck = true });
                    if (cmpresult.ReturnValue)
                    {
                        //CMP authorization is required in this region
                        ShowMessage($"This country needs CMP, CMP result={cmpresult.Data?.ToString()}");
                    }
                }
            }
            catch (Exception)
            {
            }
        }
    }
}
