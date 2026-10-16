#include <OneButton.h>
#include "Ticker.h"
#include "net.h"
#include "common.h"
#include "light.h"
#include "preferencesUtil.h"

enum CurrentPage currentPage = SETTING;
bool isCheckingTime = false; // 是否正在NTP对时中

void startShowText(void *param);
void startShowIp(void *param);
void btn1click();
void btn2click();
void btn3click();
void btn1LongClick();
void btn2LongClick();
void btn3LongClick();

///////////////////////////////////Freertos区域///////////////////////////////////////
// 按钮
OneButton button1(BTN1, true);
OneButton button2(BTN2, true);
OneButton button3(BTN3, true);
//系统启动动画的任务句柄
TaskHandle_t showTextTask = NULL;
//循环播放配网ip地址的任务句柄
TaskHandle_t showIpTask = NULL;

// 安全删除任务
void safeDeleteTask(TaskHandle_t &handle){
  if(handle != NULL){
    TaskHandle_t taskToDelete = handle;
    handle = NULL;
    vTaskDelete(taskToDelete);
    delay(50);
  }
}

//创建文字显示任务
void createShowTextTask(char *text){
  safeDeleteTask(showTextTask);
  xTaskCreate(startShowText, "startShowText", 2048, (void *)text, 1, &showTextTask);
}
//创建轮播配网的ip地址的任务
void createShowIpTask(){
  safeDeleteTask(showIpTask);
  xTaskCreate(startShowIp, "startShowIp", 2048, NULL, 1, &showIpTask);
}
// 启动文字显示，CONFIG不用了，改为采用显示ip地址的形式
void startShowText(void *param){
  String text = (char *)param;
  int x = 0;
  if(text.equals("START")){
    x = 2;
  }else if(text.equals("CONFIG")){
    x = 0;
  }
  int index = 0;
  while (true) {
    if(index % 3 == 0){
      drawText(x, 6, text + ".");
    }else if(index % 3 == 1){
      drawText(x, 6, text + "..");
    }else if(index % 3 == 2){
      drawText(x, 6, text + "...");
    }
    vTaskDelay(pdMS_TO_TICKS(1000));
    index++;
  }
  Serial.println("退出启动文字显示");
}
// 轮播IP
void startShowIp(void *param){
  while (true) {
    showIp();
  }
  Serial.println("退出轮播配网IP动画");
}
//////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////定时器区域///////////////////////////////////////
Ticker tickerCheckTime;
// NTP对时定时触发回调
void triggerCheckTime(){
  // 只要将对时标志置为true即可，在主循环中进行对时操作
  isCheckingTime = true;
}

void startTickerCheckTime(){
  // 每隔一段时间进行一次NTP对时
  tickerCheckTime.attach(TIME_CHECK_INTERVAL, triggerCheckTime);
}
//////////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////// 按键区////////////////////////////////////////
// 初始化各按键
void btnInit(){
  button1.attachClick(btn1click);
  button1.setDebounceMs(10); //设置消抖时长 
  button2.attachClick(btn2click);
  button2.setDebounceMs(10); //设置消抖时长 
  button3.attachClick(btn3click);
  button3.setDebounceMs(10); //设置消抖时长 
  button1.attachLongPressStart(btn1LongClick);
  button1.setPressMs(1200); //设置长按时间
  button2.attachLongPressStart(btn2LongClick);
  button2.setPressMs(1200); //设置长按时间
  button3.attachLongPressStart(btn3LongClick);
  button3.setPressMs(1200); //设置长按时间
}
// 监控按键
void watchBtn(){
  button1.tick();
  button2.tick();
  button3.tick();
}
// 按键方法
// >按键
void btn1click(){
  if(isShowingMarquee){
    isShowingMarquee = false;
    clearMatrix();
    return;
  }
  switch(currentPage){
    case TIME:
      if(timePage == TIME_H_M_S){
        timePage = TIME_H_M;
      }else if(timePage == TIME_H_M){
        timePage = TIME_DATE;
      }else{
        timePage = TIME_H_M_S;
      }
      // 清屏
      clearMatrix();
      // 保存设置
      recordExtensionPage();
      // 绘制时间
      drawTimeFirstTime = true;
      drawTime();
      break;
    case RHYTHM:
      if(rhythmPage == RHYTHM_MODEL1){
        rhythmPage = RHYTHM_MODEL2;
      }else if(rhythmPage == RHYTHM_MODEL2){
        rhythmPage = RHYTHM_MODEL3;
      }else if(rhythmPage == RHYTHM_MODEL3){
        rhythmPage = RHYTHM_MODEL4;
      }else if(rhythmPage == RHYTHM_MODEL4){
        rhythmPage = RHYTHM_MODEL1;
      }
      // 保存设置
      recordExtensionPage();
      break;
    case ANIM:
      if(animPage == ANIM_MODEL1){
        animPage = ANIM_MODEL2;
      }else if(animPage == ANIM_MODEL2){
        // 将已点亮灯的个数置零
        lightedCount = 0;
        animPage = ANIM_MODEL3;
      }else if(animPage == ANIM_MODEL3){       
        animPage = ANIM_MODEL1;
      }
      // 将二维数组重置为0
      memset(matrixArray,0,sizeof(matrixArray));
      // 保存设置
      recordExtensionPage();
      // 清屏
      clearMatrix();
      break;
    case BRIGHT:
      if(brightModel == BRIGHT_MODEL_AUTO) return;
      if(brightness >= 145){
        Serial.println("已达最大亮度");
        return;
      }
      brightness+=BRIGHTNESS_SPACING;
      Serial.print("当前亮度：");Serial.println(brightness);
      recordBrightness();
      drawBright();
      break;
    default:
      break;
  }
}
// <按键
void btn2click(){
  if(isShowingMarquee){
    isShowingMarquee = false;
    clearMatrix();
    return;
  }
  switch(currentPage){
    case TIME:     
      if(timePage == TIME_H_M_S){
        timePage = TIME_DATE;
      }else if(timePage == TIME_H_M){
        timePage = TIME_H_M_S;
      }else{
        timePage = TIME_H_M;
      }
      // 清屏
      clearMatrix();
      // 保存设置
      recordExtensionPage();
      // 绘制时间
      drawTimeFirstTime = true;
      drawTime();
      break;
    case RHYTHM:
      if(rhythmPage == RHYTHM_MODEL1){
        rhythmPage = RHYTHM_MODEL4;
      }else if(rhythmPage == RHYTHM_MODEL2){
        rhythmPage = RHYTHM_MODEL1;
      }else if(rhythmPage == RHYTHM_MODEL3){
        rhythmPage = RHYTHM_MODEL2;
      }else if(rhythmPage == RHYTHM_MODEL4){
        rhythmPage = RHYTHM_MODEL3;
      }
      // 保存设置
      recordExtensionPage();
      break;
    case ANIM:
      if(animPage == ANIM_MODEL1){
        // 将已点亮灯的个数置零
        lightedCount = 0;
        animPage = ANIM_MODEL3;
      }else if(animPage == ANIM_MODEL2){
        animPage = ANIM_MODEL1;
      }else if(animPage == ANIM_MODEL3){
        animPage = ANIM_MODEL2;
      }
      // 将二维数组重置为0
      memset(matrixArray,0,sizeof(matrixArray));
      // 保存设置
      recordExtensionPage();
      // 清屏
      clearMatrix();
      break;
    case BRIGHT:
      if(brightModel == BRIGHT_MODEL_AUTO) return;
      if(brightness <= 5){
        Serial.println("已达最小亮度");
        return;
      }
      brightness-=BRIGHTNESS_SPACING;
      Serial.print("当前亮度：");Serial.println(brightness);
      recordBrightness();
      drawBright();
      break;
    default:
      break;
  }
}
void btn3click(){
  if(isShowingMarquee){
    isShowingMarquee = false;
    clearMatrix();
    return;
  }
  switch(currentPage){
    case SETTING:
      // 关闭显示IP的动画任务
      safeDeleteTask(showIpTask);
      // 清空屏幕
      clearMatrix();
      // 查看是否曾经配置过网络
      if(ssid.equals("")){ // 没有配置过，进入节奏灯页面
        // 至节奏灯页面
        currentPage = RHYTHM;
      }else{  //有配置过，尝试进行连接
        // 将页面置为时间页面
        currentPage = TIME;
        drawTimeFirstTime = true; 
        // 将重新配网标志位置为false
        setApConfigWhenStart(false);
        // 连接WiFi,30秒超时后显示wifi连接失败的图案
        connectWiFi(30); 
        // 如果连接上了wifi,就进行NTP对时,超过30秒对时失败，就进入节奏灯页面
        if(wifiConnected){
          checkTime(30);
          startServer();
        }
      }
      break;
    case TIME:
      // 清屏
      clearMatrix();
      currentPage = RHYTHM;
      break;
    case RHYTHM:
      // 将二维数组重置为0
      memset(matrixArray,0,sizeof(matrixArray));
      // 将已点亮灯的个数置零
      lightedCount = 0;
      // 清屏
      clearMatrix();
      currentPage = ANIM;
      break;
    case ANIM:
      currentPage = BRIGHT;
      drawBright();
      break;
    case BRIGHT:
      if (!RTCSuccess){
        // 至节奏灯页面
        currentPage = RHYTHM;
      }else{
        currentPage = TIME;
        drawTimeFirstTime = true;
      }
      break; 
    default:
      break;
  }
}
void btn1LongClick(){
}
void btn2LongClick(){
}
void btn3LongClick(){
  if(currentPage == SETTING){
    return;
  }
  if(currentPage == RHYTHM){ // 节奏灯页面长按，切换频段模式
    if(rhythmBandsModel == RHYTHM_BANDS_MODEL1){
      rhythmBandsModel = RHYTHM_BANDS_MODEL2;
    }else{
      rhythmBandsModel = RHYTHM_BANDS_MODEL1;
    }
    // 将相关数组重置为0
    memset(peak,0,sizeof(peak));
    memset(oldBarHeights,0,sizeof(oldBarHeights));
    // 保存新模式
    recordExtensionPage();
  }else if(currentPage == BRIGHT){ // 亮度页面长按，切换亮度调节模式
    if(brightModel == BRIGHT_MODEL_MANUAL){
      brightModel = BRIGHT_MODEL_AUTO;
      // 重置采样值
      clearBrightSampling();
    }else{
      brightModel = BRIGHT_MODEL_MANUAL;
    }
    // 保存新模式
    recordExtensionPage();
    // 重新读取NVS中的亮度值
    getBrightness();
    // 刷新页面
    drawBright();
  }else if(currentPage == TIME){ // 时间页面长按，切换时间跳变模式
    if(timeModel == TIME_MODEL_DIRECT){
      timeModel = TIME_MODEL_ANIM;
    }else{
      timeModel = TIME_MODEL_DIRECT;
    }
    // 保存新模式
    recordExtensionPage();
    // 重置时间动画索引
    timeIndex = 0;
  }else if(currentPage == ANIM){ // 动画页面长按，重启并配网
    Serial.println("重启并配网");
    setApConfigWhenStart(true);
    ESP.restart();
  }
}
///////////////////////////////////////////////////////////////

