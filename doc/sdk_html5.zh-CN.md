# Miracle Games HTML5 SDK

**其他语言版本: [English](sdk_html5.en.md), [中文](sdk_html5.zh-CN.md).**

**SDK名称：** Miracle Games HTML5 SDK

**SDK提供方的公司名称：**  北京奇游灵动科技有限公司

**处理的个人信息类型：** 您主动上传的数据、设备信息（设备型号、操作系统版本号）、网络信息（网络类型）、应用信息

**使用目的及功能场景：** 提供广告等相关功能

**SDK隐私政策：** [Miracle Games 隐私政策](https://www.mguwp.net/developer_privacy.html)

**处理方式：** 1、采用SSL协议加密及HTTPS传输加密技术保障安全； 2、采取加密、去标识化等安全措施脱敏处理

**合规使用说明：** [Miracle Games SDK 合规使用指南](https://www.mguwp.net/developer_compliance.html)

---
# 集成和功能说明
### [1.简介](#1简介)
### [2.引入javascript文件](#2在调用广告的页面引入我们的javascript文件)
### [3.广告](#3插屏全屏插播横幅对联激励视频广告)
### [4.广告点击事件](#4广告点击事件-1)
### [5.广告关闭事件](#5广告关闭事件-1)
### [联系方式](#联系方式-1)
---

# 1.简介

      Miracle Games 广告支持【开屏1920\*1080】【横幅728\*90】【插屏640\*640】【对联300\*600】【全屏插播768\*432】【激励视频1024\*768】【退屏】

# 2.在调用广告的页面，引入我们的javascript文件

```html
<script language="javascript" src="http://mdc.mguwp.net/ads/h5adsdk.js"></script>
```

# 3.插屏、全屏插播、横幅、对联、激励视频广告

      调用Miracle Games提供的JavaScript方法ShowAd(jsonParam)可以调用广告。参数如下：

| 参数名 | 参数描述 |
| --- | --- |
| jsonParam | 广告位参数，格式为json字符串。<br>*   unitId = 广告位ID<br>    <br>*   adType = 广告位类型：1.插屏 2.Banner 3.对联 4.全屏插播 5.开屏 6.退屏 7.激励视频<br>    <br>*   media = 指定素材类型：image,video,web；可不传。<br>    <br>*   comment = 仅激励视频广告支持，透传参数，传递前进行urlencode。在广告关闭事件中会得到此参数。 |

```javascript
function clientShowMgAd(unitId, adType, media, comment) {
    var jsonParam = new Object();
    jsonParam.unitId = unitId;
    jsonParam.adType = adType;  
    if (media != undefined && media != "") {
        jsonParam.media = media;//指定素材类型：image,video,web
    } 
    if (comment != undefined && comment != "") {
        jsonParam.comment = encodeURIComponent(JSON.stringify(comment));
    } 
    ShowAd(JSON.stringify(jsonParam));//调用MG的广告接口
} 
```

# 4.广告点击事件

       MG 退屏广告的点击事件无法在客户端捕获，以下事件内容不涉及退屏广告。
       玩家点击广告后，会调用JavaScript方法function AdClickEvent(param)。该方法需要开发者自已实现内部逻辑。demo中提供了该方法的示例代码。参数如下:

| 参数名 | 参数描述 | 示例 |
| --- | --- | --- |
| unitId | 开发者传入的广告位ID | e333abaf22404c4a8d382c1e7ba42076 |
| resourceId | 资源Id | String |
| materialId | 素材 Id | String |

```javascript
function AdClickEvent(param) {
    var jsonObj = JSON.parse(param);
    var unitId = jsonObj.unitId;
  
    document.getElementById("txtResultMsg").value += ("AdClickEvent广告点击事件：广告位=" + unitId + ",其它:" + param + "\r\n");
}
```

# 5.广告关闭事件

       MG 退屏广告的关闭事件无法在客户端捕获，以下事件内容不涉及退屏广告。
       玩家关闭广告后，会调用JavaScript方法function AdCloseEvent(param)。该方法需要开发者自已实现内部逻辑。demo中提供了该方法的示例代码。参数如下:

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

```javascript
function AdCloseEvent(param) {
    var jsonObj = JSON.parse(param);
    var unitId = jsonObj.unitId; 

    //激励视频广告
    if (jsonObj.hasOwnProperty("completeStatus")) {
        var completeStatus = jsonObj.completeStatus; 
        var comment = jsonObj.comment;
        var rewardId = jsonObj.rewardId;
        var resourceId = jsonObj.resourceId;
        var materialId = jsonObj.materialId;
        if (completeStatus == 1) {//激励视频播放完成
            //下放道具
            alert("广告位：" + unitId + ",奖励Id：" + rewardId + "已经下放道具");

            //资产核销，游戏道具下放成功后调用 
            var jsonParam = new Object(); 
            jsonParam.unitId = unitId;
            jsonParam.rewardId = rewardId;
            jsonParam.resourceId = resourceId;
            jsonParam.materialId = materialId;
            ReportAdRewardFulfillment(JSON.stringify(jsonParam));//向MG报告
        }
    }
    document.getElementById("txtResultMsg").value += ("AdCloseEvent广告关闭事件：广告位=" + unitId + ",其它:" + param + "\r\n");
} 
```

# 联系方式

| 类型       | 详情                 |
|------------|----------------------|
| 📞 服务电话 | 86-010-85895781      |
| 📧 技术邮箱 | Technic@mguwp.com    |
| 🕒 服务时间 | 10:00 - 19:00      |