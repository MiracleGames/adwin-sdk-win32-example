# Miracle Games HTML5 SDK

**Other language versions: [English](sdk_html5.en.md), [中文](sdk_html5.zh-CN.md).**

**SDK Name:** Miracle Games HTML5 SDK

**Company Name of SDK Provider:** Beijing Qiyou Lingdong Technology Co., Ltd.

**Types of Personal Information Processed:** Data actively uploaded by you, device information (device model, operating system version number), network information (network type), application information.

**Purpose of Use and Functional Scenarios:** Provides login, payment, and other related functions.

**SDK Privacy Policy:** [Miracle Games Privacy Policy](https://www.mguwp.net/developer_privacy.html)

**Processing Method:** 1. Ensures security through SSL protocol encryption and HTTPS transmission encryption technology; 2. Processes data with de-identification measures such as encryption and anonymization.

**Compliance Usage Instructions:** [Miracle Games SDK Compliance Usage Guide](https://www.mguwp.net/developer_compliance.html)

---
---
# Integration and Function Description
### [1. Introduction](#1-introduction-1)
### [2. Import JavaScript File](#2-import-javascript-file-1)
### [3. Advertisements](#3advertisements)
### [4. Ad Click Event](#4ad-click-event)
### [5. Ad Close Event](#5ad-close-event)
### [Contact Information](#contact-information-1)
---

# 1. Introduction

      Miracle Games advertisements support 【Splash Screen 1920*1080】【Banner 728*90】【Interstitial 1024*768】【Couplet 300*600】【Rewarded Video 1024*768】【Exit Screen】

# 2. Import JavaScript File

```html
<script language="javascript" src="http://mdc.mguwp.net/ads/h5adsdk.js"></script>
```


# 3.Advertisements

      Call the JavaScript method ShowAd(jsonParam) provided by Miracle Games to display advertisements. Parameters are as follows:

| Parameter Name | Parameter Description |
| --- | --- |
| jsonParam | Ad slot parameters in JSON string format.<br>* unitId = Ad Slot ID<br>    <br>* adType = Ad Type:1.Splash Screen 2.Exit Screen 3.Banner 4.Interstitial 5.Couplet 6.Rewarded Video <br>    <br>* media = Specify material type: image,video,web; optional.<br>    <br>* comment = Only supported for rewarded video ads, pass-through parameter, should be URL encoded before passing. This parameter will be returned in the ad close event. |

```javascript
function clientShowMgAd(unitId, adType, media, comment) {
    var jsonParam = new Object();
    jsonParam.unitId = unitId;
    jsonParam.adType = adType;  
    if (media != undefined && media != "") {
        jsonParam.media = media;//Specify material type: image,video,web
    } 
    if (comment != undefined && comment != "") {
        jsonParam.comment = encodeURIComponent(JSON.stringify(comment));
    } 
    ShowAd(JSON.stringify(jsonParam));//Call MG's ad interface
} 
```

# 4.Ad Click Event

       The click event for MG Exit Screen ads cannot be captured on the client side. The following content does not apply to Exit Screen ads.
       When a player clicks an ad, the JavaScript method function AdClickEvent(param) will be called. Developers need to implement the internal logic of this method themselves. The demo provides sample code for this method. Parameters are as follows:

| Parameter Name | Parameter Description | Example |
| --- | --- | --- |
| unitId | Developer-provided Ad Slot ID | e333abaf22404c4a8d382c1e7ba42076 |
| resourceId | Resource Id | String |
| materialId | Material Id | String |

```javascript
function AdClickEvent(param) {
    var jsonObj = JSON.parse(param);
    var unitId = jsonObj.unitId;
  
    document.getElementById("txtResultMsg").value += ("AdClickEvent ad click event: Ad Slot=" + unitId + ", Others:" + param + "\r\n");
}
```

# 5.Ad Close Event

       The close event for MG Exit Screen ads cannot be captured on the client side. The following content does not apply to Exit Screen ads.
       When a player closes an ad, the JavaScript method function AdCloseEvent(param) will be called. Developers need to implement the internal logic of this method themselves. The demo provides sample code for this method. Parameters are as follows:

| Parameter Name | Parameter Description | Example |
| --- | --- | --- |
| unitId | Developer-provided Ad Slot ID | e333abaf22404c4a8d382c1e7ba42076 |
| advertStatus | Ad Slot Status | 1:Ad normal; 2:Ad closed by backend; 3:No ad material |
| The following parameters are only available for rewarded video ads |  |  |
| completeStatus | Ad playback status | 1:Ad playback completed, reward can be issued; 0:Ad playback not completed |
| comment | Pass-through parameter provided by the developer, URL encoded | abc%2c123 |
| rewardId | MG Order ID for the reward, used when the game reports verification after issuing the | String |
| resourceId | Resource Id | String |
| materialId | Material Id | String |

```javascript
function AdCloseEvent(param) {
    var jsonObj = JSON.parse(param);
    var unitId = jsonObj.unitId; 

    //Rewarded video ad
    if (jsonObj.hasOwnProperty("completeStatus")) {
        var completeStatus = jsonObj.completeStatus; 
        var comment = jsonObj.comment;
        var rewardId = jsonObj.rewardId;
        var resourceId = jsonObj.resourceId;
        var materialId = jsonObj.materialId;
        if (completeStatus == 1) {//Rewarded video playback completed
            //Issue item
            alert("Ad Slot: " + unitId + ", Reward ID: " + rewardId + " item has been issued");

            //Asset verification, called after successful item issuance 
            var jsonParam = new Object(); 
            jsonParam.unitId = unitId;
            jsonParam.rewardId = rewardId;
            jsonParam.resourceId = resourceId;
            jsonParam.materialId = materialId;
            ReportAdRewardFulfillment(JSON.stringify(jsonParam));//Report to MG
        }
    }
    document.getElementById("txtResultMsg").value += ("AdCloseEvent ad close event: Ad Slot=" + unitId + ", Others:" + param + "\r\n");
} 
```

# Contact Information

| Type              | Details                      |
|-------------------|------------------------------|
| 📞 Service Phone  | 86-010-85895781              |
| 📧 Technical Email | Technic@mguwp.com            |
| 🕒 Service Hours   | 10:00 - 19:00                |