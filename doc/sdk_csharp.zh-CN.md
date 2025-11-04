# Miracle Games CSharp SDK

**其他语言版本: [English](sdk_csharp.en.md), [中文](sdk_csharp.zh-CN.md).**

**SDK名称：** Miracle Games CSharp SDK

**SDK提供方的公司名称：**  北京奇游灵动科技有限公司

**处理的个人信息类型：** 您主动上传的数据、设备信息（设备型号、操作系统版本号）、网络信息（网络类型）、应用信息

**使用目的及功能场景：** 提供登录、支付等相关功能

**SDK隐私政策：** [Miracle Games 隐私政策](https://www.mguwp.net/developer_privacy.html)

**处理方式：** 1、采用SSL协议加密及HTTPS传输加密技术保障安全； 2、采取加密、去标识化等安全措施脱敏处理

**合规使用说明：** [Miracle Games SDK 合规使用指南](https://www.mguwp.net/developer_compliance.html)

---
# 集成和功能说明
### [开发环境配置](#1开发环境配置)
### [SDK初始化](#2SDK初始化)
### [SDK初始化](doc/sdk_html5uwp.zh-CN.md)

---

# 1.开发环境配置

### 1.1.添加引用

下载 MiracleGamesAdSDK，解压到项目文件夹中，如dll文件夹。

在项目中添加引用"MiracleGamesAd.dll"。

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
private async void Form1_Load(object sender, EventArgs e)
{
    string message;

    var result = await ApplicationManager.SetupAsync("应用ID","秘钥");
    if (result.ReturnValue)
    {
        //初始化成功.
        message = $"初始化完成:Token={ApplicationManager.AccessToken.Token}, ExpiresIn={ApplicationManager.AccessToken.ExpiresIn}";

        //开屏广告
        AdvertManager.OpenAdvert(this, "768338453d614f3aad85eea7e3916e7e", AdType.FullScreen);

        //退屏广告；Step1.初始化成功之后，加载退屏广告资源
        AdvertManager.SetupExitAdUnitId("7cdc7614b69c4118933e2067e6e14d01");
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
```

### 2.3.没有初始化成功的错误可能如下

● 网络故障，没有正确的网络支持

　　● 本SDK不支持VPN，本机开启了VPN软件

　　● AppKey错误，请登录开发者后台检查应用设置

　　● 服务器问题，请检查result的错误信息，及时[联系技术支持](contact.zh-CN.md)

 



# 联系方式

| 类型       | 详情                 |
|------------|----------------------|
| 📞 服务电话 | 86-010-85895781      |
| 📧 技术邮箱 | Technic@mguwp.com    |
| 🕒 服务时间 | 10:00 - 19:00      |
