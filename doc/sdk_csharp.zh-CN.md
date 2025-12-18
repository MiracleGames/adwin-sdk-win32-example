# Miracle Games CSharp SDK

**其他语言版本: [English](sdk_csharp.en.md), [中文](sdk_csharp.zh-CN.md).**

**SDK名称：** Miracle Games CSharp SDK

**SDK提供方的公司名称：**  北京奇游灵动科技有限公司

**处理的个人信息类型：** 您主动上传的数据、设备信息（设备型号、操作系统版本号）、网络信息（网络类型）、应用信息

**使用目的及功能场景：** 提供广告等相关功能

**SDK隐私政策：** [Miracle Games 隐私政策](https://www.mguwp.net/developer_privacy.html)

**处理方式：** 1、采用SSL协议加密及HTTPS传输加密技术保障安全； 2、采取加密、去标识化等安全措施脱敏处理

**合规使用说明：** [Miracle Games SDK 合规使用指南](https://www.mguwp.net/developer_compliance.html)

---
# 集成和功能说明
### [1.开发环境配置](#1开发环境配置-1)
### [2.SDK初始化](#2SDK初始化-1)
### [3.广告](#3广告-1)
### [联系方式](#联系方式-1)
---


# 1.开发环境配置

### 1.1.添加引用

      下载 MiracleGamesAdSDK，解压到项目文件夹中，如dll文件夹。在项目中添加引用"MiracleGamesAd.dll"。

![csharp_addreference.png](../images/csharp_addreference.png)

### 1.2.编译后事件

      在编译后事件中添加命令，将MiracleGamesAdSDK中的所有dll文件，生成到编译文件夹中。

```plaintext
xcopy /y "$(ProjectDir)dll" "$(OutDir)"
xcopy /yei "$(ProjectDir)dll\runtimes" "$(OutDir)runtimes"
```

![csharp_event.png](../images/csharp_event.png)


# 2.SDK初始化

### 2.1.简介

      在接入Miracle Games SDK之前，首先需要进行SDK的初始化，初始化完成后，才可以配合后台系统使用本SDK的全部功能，在进入游戏后即进行SDK初始化。


### 2.2.SDK初始化

```c#
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
```

### 2.3.没有初始化成功的错误可能如下

　　● 网络故障，没有正确的网络支持

　　● 本SDK不支持VPN，本机开启了VPN软件

　　● AppId错误，请登录开发者后台检查应用设置

　　● 服务器问题，请检查result的错误信息，及时[联系技术支持](contact.zh-CN.md)

 
# 3.广告

### 3.1.简介

      在接入广告之前，首先需要完成SDK的初始化。

      Miracle Games 广告支持【开屏1920\*1080】【横幅728\*90】【插屏1024\*768】【对联300\*600】【激励视频1024\*768】【信息流】【嵌入式】【退屏】

### 3.2.开屏、横幅、插屏、对联、激励视频、信息流、嵌入式广告

```c#
//1.开屏广告
AdvertManager.ShowAd(this, SplashScreenUnitId, AdType.FullScreen);

//2.横幅
AdvertManager.ShowAd(this, BannerUnitId, AdType.Banner);

//3.插屏广告
AdvertManager.ShowAd(this, InterstitialUnitId, AdType.Interstitial);

//4.对联
AdvertManager.ShowAd(this, CoupletUnitId, AdType.Couplet);

//5.激励视频
{
    string comment = "id123,abc,$9.99";//透传参数  
    dynamic jsonObj = new
    {
        unitId = RewardUnitId,
        comment = Uri.EscapeDataString(comment)//透传参数,需url编码
    };
    string json = JsonConvert.SerializeObject(jsonObj);
    AdvertManager.ShowAd(this, json, AdType.Reward);
}

//6.信息流
{
    //尺寸自定义(示例400*50)，在MG后台设置
    dynamic jsonObj = new
    {
        unitId = InformationFlowUnitId,
        media = "image",
        width = panelAd6.Width,
        height = panelAd6.Height
    };
    string json = JsonConvert.SerializeObject(jsonObj);
    //需要开发者负责维护广告容器
    AdvertManager.ShowAd(this.panelAd6, json, AdType.InformationFlow);
}

//7.嵌入式
{
    //尺寸自定义(示例200*200)，在MG后台设置
    dynamic jsonObj = new
    {
        unitId = EmbeddedUnitId,
        media = "image",
        width = panelAd.Width,
        height = panelAd.Height
    };
    string json = JsonConvert.SerializeObject(jsonObj);
    //需要开发者负责维护广告容器
    AdvertManager.ShowAd(this.panelAd, json, AdType.Embedded);
}
```

### 3.3.退屏广告

      弹屏广告是在退出游戏时触发，为了保证退出游戏时广告的弹出率，MG会分两步完成退屏广告的实现

      1.在初始化完成后，将退屏广告的信息加载到内存中

      2.在退出游戏时，直接打开退屏广告

```c#
//退屏广告
//Step1.初始化成功之后，加载退屏广告资源
AdvertManager.SetupExitAd(ExitScreenUnitId);


// 退屏广告
// Step2.在程序关闭时，弹出展示退屏广告
private void Form1_FormClosing(object sender, FormClosingEventArgs e)
{
    _ = AdvertManager.ShowExitAdBlocking();
}
```

### 3.4.广告关闭事件

       注册广告关闭的回调事件，一般在页面的构造函数中进行

       广告关闭事件参数说明

| 参数名 | 参数描述 | 示例 |
| --- | --- | --- |
| unitId | 开发者传入的广告位ID | e333abaf22404c4a8d382c1e7ba42076 |
| advertStatus | 广告位状态 | 1:广告正常；2:广告被后台关闭；3:没有广告素材 |
| 以下是仅激励视频广告拥有的参数 |  |  |
| completeStatus | 广告的播放状态 | 1:广告播放完毕，可以发奖励；0:广告未播放完毕 |
| comment | 由开发者传入的透传参数，经过 url 编码 | abc%2c123 |
| rewardId | 奖励的MG订单号，游戏发奖后向MG报告核销时使用 | String |
| resourceId | 资源Id | String |
| materialId | 素材 Id | String |

```c#
public Form1()
{
    InitializeComponent();
    AdvertManager.AdClickEvent += AdvertManager_AdClickEvent;
    AdvertManager.AdCloseEvent += AdvertManager_AdCloseEvent;
}

private void AdvertManager_AdCloseEvent(object sender, string e)
{
    ShowMessage("广告被关闭 " + e);

    //普通广告 {"unitId":"6bf68881673540788d096b9ea4a3cedb","advertStatus":1,"resourceId":"68d20656bd9558abfdf43465","materialId":"d235efa86ccf44acbe7053af760031b6"}
    //激励视频广告 {"unitId":"0f505442fac84f098e81d6f2ca04abe1","advertStatus":1,"completeStatus":1,"resourceId":"68ecb9eb20f045c603867874","materialId":"b0817d87ee2544629bac1933a60238d2","comment":"id123%2Cabc%2C%249.99","rewardId":"D1E593C16BBD412CA880FD89F0450A14"}

    JObject jsonObject = JObject.Parse(e);
    string unitId = (string)jsonObject["unitId"];

    if (unitId == RewardUnitId)//激励视频，根据返回结果发奖励道具
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
                _ = await AdvertManager.ReportAdRewardFulfillment(unitId, resourceId, materialId, rewardId);//向MG报告
            });
        }
    }
}

private void AdvertManager_AdClickEvent(object sender, string e)
{
    ShowMessage("广告被点击 " + e);
}
```


# 联系方式

| 类型       | 详情                 |
|------------|----------------------|
| 📞 服务电话 | 86-010-85895781      |
| 📧 技术邮箱 | Technic@mguwp.com    |
| 🕒 服务时间 | 10:00 - 19:00      |
