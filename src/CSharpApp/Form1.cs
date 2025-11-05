using MiracleGamesAd;
using MiracleGamesAd.Models;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;
using System;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace CSharpApp
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
            AdvertManager.AdClickEvent += AdvertManager_AdClickEvent;
            AdvertManager.AdCloseEvent += AdvertManager_AdCloseEvent;
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
        private void AdvertManager_AdCloseEvent(object sender, string e)
        {
            ShowMessage("广告被关闭 " + e);

            //普通广告 {"unitId":"6bf68881673540788d096b9ea4a3cedb","advertStatus":1,"resourceId":"68d20656bd9558abfdf43465","materialId":"d235efa86ccf44acbe7053af760031b6"}
            //激励视频广告 {"unitId":"0f505442fac84f098e81d6f2ca04abe1","advertStatus":1,"completeStatus":1,"resourceId":"68ecb9eb20f045c603867874","materialId":"b0817d87ee2544629bac1933a60238d2","comment":"id123%2Cabc%2C%249.99","rewardId":"D1E593C16BBD412CA880FD89F0450A14"}

            JObject jsonObject = JObject.Parse(e);
            string unitId = (string)jsonObject["unitId"];

            if (unitId == "0f505442fac84f098e81d6f2ca04abe1")//激励视频，根据返回结果发奖励道具
            {
                int completeStatus = (int)jsonObject["completeStatus"];
                string resourceId = (string)jsonObject["resourceId"];
                string materialId = (string)jsonObject["materialId"];
                string rewardId = (string)jsonObject["rewardId"];
                string comment = (string)jsonObject["comment"];//透传参数
                if (completeStatus == 1)
                {
                    //视频播放完毕，下发奖励道具 
                    //...

                    Task.Run(async () =>
                    {
                        var result = await AdvertManager.ReportAdRewardFulfillment(unitId, resourceId, materialId, rewardId);//向MG报告
                        if (result.ReturnValue)
                        {
                            ShowMessage("激励视频广告，奖励道具已向MG核销");
                        }
                    });
                }
            }
        }

        private void AdvertManager_AdClickEvent(object sender, string e)
        {
            ShowMessage("广告被点击 " + e);
        }

        private async void Form1_Load(object sender, EventArgs e)
        {
            string message;

            var result = await ApplicationManager.Initialize("9NC834S3H84L",
                "MIGTAgEAMBMGByqGSM49AgEGCCqGSM49AwEHBHkwdwIBAQQgiJm0JnjgpDjxEKKzH/7kc3N8r+nvmHko1EPV6My6WG6gCgYIKoZIzj0DAQehRANCAAR2z1Eih/EOFjBMbpgMdvfYjUqFEVaRbnEeYEYZrp4K3pGj1YoY0/dmRRQ58OaHfxKotbFDMwNDBpuHwtxTqGE6");
            if (result.ReturnValue)
            {
                //初始化成功.
                message = $"初始化完成:Token={ApplicationManager.AccessToken.Token}, ExpiresIn={ApplicationManager.AccessToken.ExpiresIn}";

                //开屏广告
                AdvertManager.ShowAd(this, "768338453d614f3aad85eea7e3916e7e", AdType.FullScreen);

                //退屏广告；Step1.初始化成功之后，加载退屏广告资源
                AdvertManager.SetupExitAd("7cdc7614b69c4118933e2067e6e14d01");
            }
            else
            {
                message = $"初始化失败";
            }

            this.Invoke(new Action(() =>
            {
                textBox1.Text = message;
            }));
        }

        /// <summary>
        /// 插屏广告 
        /// 640*640
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void btnAd1_Click(object sender, EventArgs e)
        {
            AdvertManager.ShowAd(this, "e333abaf22404c4a8d382c1e7ba42076", AdType.Interstitial);
        }

        private void btnAd1_2_Click(object sender, EventArgs e)
        {
            AdvertManager.ShowAd(this, "{\"unitId\": \"e333abaf22404c4a8d382c1e7ba42076\",\"media\":\"web\"}", AdType.Interstitial);
        }

        private void btnAd1_1_Click(object sender, EventArgs e)
        {
            //指定图片类型 和 广告容器
            dynamic jsonObj = new
            {
                unitId = "e333abaf22404c4a8d382c1e7ba42076",
                media = "image",
                width = panelAd.Width,//当容器大小与MG广告标准尺寸有差异时，需要开发者传入容器的尺寸
                height = panelAd.Height
            };
            string json = JsonConvert.SerializeObject(jsonObj);
            AdvertManager.ShowAd(this.panelAd, json, AdType.Interstitial);
        }

        /// <summary>
        /// 全屏插播广告
        /// 768*432
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void btnAd2_Click(object sender, EventArgs e)
        {
            AdvertManager.ShowAd(this, "d65b9c6612bd494fbd6844b490d536dc", AdType.FullScreenInterstitial);
        }

        /// <summary>
        /// 横幅广告
        /// 728*90
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void btnAd3_Click(object sender, EventArgs e)
        {
            AdvertManager.ShowAd(this, "e9b34829a2ad4a959874f9a180278bfe", AdType.Banner);
        }

        /// <summary>
        /// 对联广告
        /// 300*600
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void btnAd4_Click(object sender, EventArgs e)
        {
            AdvertManager.ShowAd(this, "{\"unitId\": \"c68cd45e8e374ccd98a704887e5b3582\",\"tag\":\"MGADKEY_COUPLET_LEFT\",\"tag2\":\"MGADKEY_COUPLET_RIGHT\"}", AdType.Couplet);
        }

        /// <summary>
        /// 激励视频
        /// 768*432
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void btnAd5_Click(object sender, EventArgs e)
        {
            string comment = "id123,abc,$9.99";//透传参数  
            dynamic jsonObj = new
            {
                unitId = "0f505442fac84f098e81d6f2ca04abe1",
                comment = Uri.EscapeDataString(comment)//透传参数,需url编码
            };
            string json = JsonConvert.SerializeObject(jsonObj);
            AdvertManager.ShowAd(this, json, AdType.Reward);
        }

        /// <summary>
        /// 删除广告位
        /// 前端功能，由开发者负责删除指定控件;
        /// 此处不会触发SDK的广告关闭事件
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void btnAd6_Click(object sender, EventArgs e)
        {
            for (int i = panelAd.Controls.Count; i > 0; i--)//删除指定容器内的广告
            {
                Control item = panelAd.Controls[0];
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

    }
}
