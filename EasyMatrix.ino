#include "common.h"
#include "preferencesUtil.h"
#include "light.h"
#include "net.h"
#include "task.h"

/**
EasyMatrix 像素时钟 - 版本 1.6

主要特性与更新：
1. 局域网常驻全功能深色 Web 控制台与 RESTful API 控制
2. Web OTA 固件在线空中升级（带进度条与屏幕提示，免插线刷机）
3. 屏幕实时弹幕/跑马灯消息推送
4. 32 频段音频 FFT 音乐频谱律动（动态高度自适应、对称布局支持）
5. 多种时间显示格式与平滑滚动动画
6. 跃水海豚、黑客帝国代码雨、星光散落等艺术像素动画
7. 光敏电阻环境光二次 Gamma 曲线自适应感应调光
8. FreeRTOS 任务安全封装与 NVS 全局掉电持久化

注意：首次烧录前请将开发板设置中的 "Erase All Flash Before Sketch Upload" 选为 Enable。
*/

unsigned long prevDisplay = 0;
unsigned long prevSampling = 0;

void setup() {
  Serial.begin(115200);
  // 从NVS中获取信息
  getInfos();
  // 初始化点阵屏(包含拾音器)
  initMatrix();
  // 创建加载动画任务
  createShowTextTask("START");
  // 初始化按键
  btnInit();
  Serial.println("各外设初始化成功");
  // nvs中没有WiFi信息，进入配置页面
  if(apConfig){
    currentPage = SETTING; // 将页面置为配网页面
    wifiConfigBySoftAP(); // 开启SoftAP配置WiFi
  }else{
    // 连接WiFi,30秒超时后显示wifi连接失败的图案
    connectWiFi(30); 
    // 如果连接上了wifi,就进行NTP对时,超过30秒对时失败，就进入节奏灯页面
    if(wifiConnected){
      checkTime(30);
      if(RTCSuccess){
        // 开启对时任务
        startTickerCheckTime();
        // 将页面置为时间页面
        currentPage = TIME; 
        // 停止启动加载文字的动画
        safeDeleteTask(showTextTask);
        // 清屏
        clearMatrix();
      }
      // 启动局域网常驻 Web 控制台服务器
      startServer();
    }
  }
}

void loop() { 
  doClient(); // 局域网常驻 Web 控制台与 OTA 请求监听
  watchBtn();

  // 如果正在进行 OTA 固件更新，暂停屏幕绘制以保障闪存写入稳定性
  if(isUpdating){
    delay(10);
    return;
  }

  // 弹幕跑马灯播放
  if(isShowingMarquee){
    drawMarqueeFrame();
    return;
  }

  if(brightModel == BRIGHT_MODEL_AUTO && ((millis() - prevSampling) >= 1000 || prevSampling > millis())){
    brightSamplingValue+=analogRead(LIGHT_ADC);
    brightSamplingTime++;
    prevSampling = millis();
    if(brightSamplingTime >= BRIGHT_SAMPLING_TIMES){ // 每轮采样N次重新计算一次亮度值
      calculateBrightnessValue();
      clearBrightSampling();
    }
  }
  if(isCheckingTime){ // 对时中
    Serial.println("开始对时");
    long start = millis(); // 记录开始对时的时间
    // 绘制对时提示文字
    drawCheckTimeText();
    // 执行对时逻辑
    checkTimeTicker();
    // 将对时标志置为false
    isCheckingTime = false;
    // 让整个对时过程持续超过4秒，不然时间太短，提示文字一闪而过，让人感觉鬼畜了
    while((millis() - start) < 4000){
      watchBtn(); // 保持按键监控，避免阻塞
      delay(50);
    }
    // 清屏
    clearMatrix();
    Serial.println("结束对时");
    // 结束对时后，重新绘制之前的页面
    if(currentPage == BRIGHT){
      drawBright();
    }else if(currentPage == ANIM){
      lightedCount = 0;
      memset(matrixArray,0,sizeof(matrixArray));
    }
  }else{
    switch(currentPage){
      case SETTING:  // 配置页面
        doClient(); // 监听客户端配网请求
        break;
      case TIME: // 时钟页面       
        if((millis() - prevDisplay) >= 50 || prevDisplay > millis()){
          // 绘制时间
          drawTime();
          prevDisplay = millis();
        }
        break;
      case RHYTHM: // 节奏灯页面
        drawRHYTHM();
        break;
      case ANIM: // 动画页面
        drawAnim();
        break;
      case BRIGHT: // 亮度调节页面
        drawBright();
        break;  
      default:
        break;
    }
  } 
}


