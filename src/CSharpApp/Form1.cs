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

        private const string YourAppId = "69316b6861328938223cc124";
        private const string YourSecretKey = "MIGTAgEAMBMGByqGSM49AgEGCCqGSM49AwEHBHkwdwIBAQQgZgULOuiIDYZyGiUyYdGr3odHVN6ebZ1uDwXx7PXiHh2gCgYIKoZIzj0DAQehRANCAASf1FWCfsSn/tXFVRt04C7JkpRG12KSC3wnaJRWb5QWin9dsBk1OR31BCsELMYtWsFhA7e6Q6Fi4Mi6+ub24O5a";
        private const string SplashScreenUnitId = "b871f83c5e8845f1b43325561bcdd6c7";     //开屏:1920 x 1080
        private const string ExitScreenUnitId = "5076eab6ae1042b6b92f73ea01981475";       //退屏:1920 x 1080
        private const string BannerUnitId = "cb7d9688a2d9499992febb6b642b3625";           //横幅:728 x 90
        private const string InterstitialUnitId = "2cb66a1301404561881a3f26b6ce5ba7";     //插屏:1024 x 768
        private const string CoupletUnitId = "b502f6e6281c43e4b28ea22503471039";          //对联:300 x 600
        private const string RewardUnitId = "2ae60936ba664fbfb7d92ce3a19c2915";           //激励广告:1024x768
        private const string InformationFlowUnitId = "f152f6caf7a8440f8510bc31534baf4e";  //信息流，由开发者维护广告控件
        private const string EmbeddedUnitId = "4192966a9db343f48dd2f6308ea9ec30";         //嵌入式，由开发者维护广告控件

        private void AdvertManager_AdCloseEvent(object sender, string e)
        {
            ShowMessage("广告被关闭 " + e);

            //普通广告 {"unitId":"6bf68881673540788d096b9ea4a3cedb","advertStatus":1,"resourceId":"68d20656bd9558abfdf43465","materialId":"d235efa86ccf44acbe7053af760031b6"}
            //激励视频广告 {"unitId":"5c1e9aae78014be7908bf035848d7075","advertStatus":1,"completeStatus":1,"resourceId":"68ecb9eb20f045c603867874","materialId":"b0817d87ee2544629bac1933a60238d2","comment":"id123%2Cabc%2C%249.99","rewardId":"D1E593C16BBD412CA880FD89F0450A14"}

            JObject jsonObject = JObject.Parse(e);
            string unitId = (string)jsonObject["unitId"];

            if (unitId == RewardUnitId)//激励视频，根据返回结果发奖励道具
            {
                int completeStatus = (int)jsonObject["completeStatus"];
                string resourceId = (string)jsonObject["resourceId"];
                string materialId = (string)jsonObject["materialId"];
                string rewardId = (string)jsonObject["rewardId"];
                if (completeStatus == 1)
                {
                    //视频播放完毕，下发奖励道具 
                    //...

                    Task.Run(async () =>
                    {
                        _ = await AdvertManager.ReportAdRewardFulfillment(unitId, resourceId, materialId, rewardId);//向MG报告
                    });
                }
            }
        }

        private void AdvertManager_AdClickEvent(object sender, string e)
        {
            ShowMessage("广告被点击 " + e);
        }

        private void ApplicationManager_CmpClosedEvent(object sender, string e)
        {
            ShowMessage("CMP授权结果 " + e);
        }

        private async void Form1_Load(object sender, EventArgs e)
        {
            ApplicationManager.SetAppId(YourAppId, YourSecretKey);
            ApplicationManager.OpenCmp(this);
            var result = await ApplicationManager.Initialize();
            if (result.ReturnValue)
            {
                //初始化成功... 
                ShowMessage($"初始化完成:Token={ApplicationManager.AccessToken.Token}, ExpiresIn={ApplicationManager.AccessToken.ExpiresIn}");

                //退屏广告Step1.初始化成功之后，加载退屏广告资源
                AdvertManager.SetupExitAd(ExitScreenUnitId);

                //...


                AdvertManager.ShowAd(this, SplashScreenUnitId, AdType.FullScreen);//开屏广告
            }
            else
            {
                ShowMessage("初始化失败");
            }
        }

        /// <summary>
        /// 开屏(全屏)广告
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
        /// 插屏广告  image,video,web
        /// 1024*768
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
        /// 横幅广告
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
        /// 对联广告
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
        /// 激励视频
        /// 768*432
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void btnAd6_Click(object sender, EventArgs e)
        {
            string comment = "id123,abc,$9.99";//透传参数,需url编码
            comment = Uri.EscapeDataString(comment);
            AdvertManager.ShowAd(this, "{\"unitId\": \"" + RewardUnitId + "\",\"comment\":\"" + comment + "\"}", AdType.Reward);
        }
        private void btnAd6_2_Click(object sender, EventArgs e)
        {
            string comment = "id123,abc,$9.99";//透传参数,需url编码
            comment = Uri.EscapeDataString(comment);
            AdvertManager.ShowAd(this, "{\"unitId\": \"" + RewardUnitId + "\",\"comment\":\"" + comment + "\",\"media\":\"web\"}", AdType.Reward);
        }

        //信息流
        private void btnAd7_Click(object sender, EventArgs e)
        {
            dynamic jsonObj = new
            {
                unitId = InformationFlowUnitId,//尺寸自定义，MG后台设置400*50
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
                unitId = InformationFlowUnitId,//尺寸自定义，MG后台设置400*50
                media = "web",
                width = panelAd6_2.Width,
                height = panelAd6_2.Height
            };
            string json = JsonConvert.SerializeObject(jsonObj);
            AdvertManager.ShowAd(this.panelAd6_2, json, AdType.InformationFlow);
        }

        //嵌入式
        private void btnAd8_Click(object sender, EventArgs e)
        {
            dynamic jsonObj = new
            {
                unitId = EmbeddedUnitId,//尺寸自定义，MG后台设置200*200
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
                unitId = EmbeddedUnitId,//尺寸自定义，MG后台设置200*200
                media = "web",
                width = panelAd8_2.Width,
                height = panelAd8_2.Height
            };
            string json = JsonConvert.SerializeObject(jsonObj);
            AdvertManager.ShowAd(this.panelAd8_2, json, AdType.Embedded);
        }

        /// <summary>
        /// 删除广告位
        /// 前端功能，由开发者负责删除指定控件;
        /// 此处不会触发SDK的广告关闭事件
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void btnDeleteAd_Click(object sender, EventArgs e)
        {
            for (int i = panelAd.Controls.Count; i > 0; i--)//删除指定容器内的广告
            {
                Control item = panelAd.Controls[0];
                item.Dispose();
                panelAd.Controls.Remove(item);
            }
            for (int i = panelAd6.Controls.Count; i > 0; i--)//删除指定容器内的广告
            {
                Control item = panelAd6.Controls[0];
                item.Dispose();
                panelAd.Controls.Remove(item);
            }
            for (int i = this.Controls.Count; i > 0; i--)//清除页面中指定的某个广告
            {
                Control item = this.Controls[0];
                if (item.Tag != null && item.Tag.ToString().Contains("MGADKEY_"))
                {
                    item.Dispose();
                    this.Controls.Remove(item);
                }
            }

            for (int i = this.Controls.Count; i > 0; i--)//删除页面上所有MG Ads的广告
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
        /// 退屏广告
        /// Step2.在程序关闭时，弹出展示退屏广告
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
