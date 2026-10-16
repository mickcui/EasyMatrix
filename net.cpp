#include <HTTPClient.h>
#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>
#include <Update.h>
#include "common.h"
#include "preferencesUtil.h"
#include "light.h"
#include "task.h"
#include "dashboardHtml.h"

void sendNTPpacket(IPAddress &address);
void connectWiFi(int timeOut_s);
void startAP();
void startServer();
void scanWiFi();
void getHTML();
void handleNotFound();
void handleRoot();
void handleWifiPage();
void handleConfigWifi();
void handleApiStatus();
void handleApiControl();
void handleApiMessage();
void handleUpdatePage();
void handleUpdateUpload();
void handleUpdateEnd();

// Wifi相关
String ssid;  //WIFI名称
String pass;  //WIFI密码
String WifiNames; // 根据搜索到的wifi生成的option字符串
String PassHTML; // 根据密码生成的HTML字符串
String RGBColors; // 根据查询到的NVS中的值生成的RGBinput字符串
bool apConfig; // 系统启动时是否需要配网
// SoftAP相关
const char *APssid = "EasyMatrix";
IPAddress staticIP(192, 168, 1, 1);
IPAddress gateway(192, 168, 1, 254);
IPAddress subnet(255, 255, 255, 0);
WebServer server(80);
// 是否顺利连接上wifi
bool wifiConnected = false;
// 是否NTP对时成功
bool RTCSuccess = false;
// 是否正在进行OTA固件升级
bool isUpdating = false;
// 标记服务器是否已启动
static bool serverStarted = false;

// 开启SoftAP进行配网
void wifiConfigBySoftAP(){
  startAP();
  scanWiFi();
  getHTML();
  startServer();
  safeDeleteTask(showTextTask);
  createShowIpTask();
}

// 处理服务器请求
void doClient(){
  server.handleClient();
}

// 处理404情况
void handleNotFound(){
  handleRoot();
}

// 访问根目录
void handleRoot(){
  if(currentPage == SETTING || WiFi.status() != WL_CONNECTED){
    // AP配网或未联网模式：展示配网页面
    String page;
    page.reserve(4096);
    page += ROOT_HTML_PAGE1;
    page += WifiNames;
    page += ROOT_HTML_PAGE2;
    page += PassHTML;
    page += ROOT_HTML_PAGE3;
    page += RGBColors;
    page += ROOT_HTML_PAGE4;
    server.send(200, "text/html", page);
  }else{
    // 局域网常驻模式：展示完整 Web 控制台
    server.send_P(200, "text/html", DASHBOARD_HTML);
  }
}

// 供局域网主动访问的配网页面
void handleWifiPage(){
  scanWiFi();
  getHTML();
  String page;
  page.reserve(4096);
  page += ROOT_HTML_PAGE1;
  page += WifiNames;
  page += ROOT_HTML_PAGE2;
  page += PassHTML;
  page += ROOT_HTML_PAGE3;
  page += RGBColors;
  page += ROOT_HTML_PAGE4;
  server.send(200, "text/html", page);
}

// 提交配网数据
void handleConfigWifi(){
  if (server.hasArg("ssid")){
    ssid = server.arg("ssid");
  }else{
    server.send(200, "text/html", "<meta charset='UTF-8'>错误, 没有发现WiFi名称");
    return;
  }
  if (server.hasArg("pass")){
    pass = server.arg("pass");
  }else{
    server.send(200, "text/html", "<meta charset='UTF-8'>错误, 没有发现WiFi密码");
    return;
  }
  if (server.hasArg("red") && server.hasArg("green") && server.hasArg("blue")){
    clockColor[0] = server.arg("red").toInt();
    clockColor[1] = server.arg("green").toInt();
    clockColor[2] = server.arg("blue").toInt();
  }else{
    server.send(200, "text/html", "<meta charset='UTF-8'>错误, 没有发现RGB颜色数据");
    return;
  }
  recordInfos(ssid, pass, clockColor[0], clockColor[1], clockColor[2], false);
  server.send(200, "text/html", "<meta charset='UTF-8'><style type='text/css'>body {font-size: 2rem;}</style><br/><br/>WiFi: " + ssid + "<br/>密码: " + pass + "<br/>RGB颜色: " + clockColor[0] + " " + clockColor[1] + " " + clockColor[2] + " " + "<br/>已保存信息，设备正在重启尝试连接...");
  delay(1000);
  ESP.restart();
}

// 获取系统状态 JSON 接口
void handleApiStatus(){
  String pageName = "TIME";
  if(currentPage == RHYTHM) pageName = "RHYTHM";
  else if(currentPage == ANIM) pageName = "ANIM";
  else if(currentPage == BRIGHT) pageName = "BRIGHT";
  else if(currentPage == SETTING) pageName = "SETTING";

  String json = "{";
  json += "\"page\":\"" + pageName + "\",";
  json += "\"pageId\":" + String((int)currentPage) + ",";
  json += "\"timePage\":" + String(timePage) + ",";
  json += "\"timeModel\":" + String(timeModel) + ",";
  json += "\"rhythmPage\":" + String(rhythmPage) + ",";
  json += "\"bandsModel\":" + String(rhythmBandsModel) + ",";
  json += "\"animPage\":" + String(animPage) + ",";
  json += "\"brightModel\":" + String(brightModel) + ",";
  json += "\"brightness\":" + String(brightness) + ",";
  json += "\"color\":[" + String(clockColor[0]) + "," + String(clockColor[1]) + "," + String(clockColor[2]) + "],";
  json += "\"ip\":\"" + WiFi.localIP().toString() + "\",";
  json += "\"rssi\":" + String(WiFi.RSSI()) + ",";
  json += "\"heap\":" + String(ESP.getFreeHeap()) + ",";
  json += "\"uptime\":" + String(millis() / 1000);
  json += "}";

  server.send(200, "application/json", json);
}

// 控制命令接收接口
void handleApiControl(){
  bool needSaveExt = false;

  if (server.hasArg("page")){
    String p = server.arg("page");
    if(p.equalsIgnoreCase("TIME")){
      currentPage = TIME;
      drawTimeFirstTime = true;
      clearMatrix();
      drawTime();
    }else if(p.equalsIgnoreCase("RHYTHM")){
      currentPage = RHYTHM;
      memset(matrixArray,0,sizeof(matrixArray));
      lightedCount = 0;
      clearMatrix();
    }else if(p.equalsIgnoreCase("ANIM")){
      currentPage = ANIM;
      memset(matrixArray,0,sizeof(matrixArray));
      lightedCount = 0;
      clearMatrix();
    }else if(p.equalsIgnoreCase("BRIGHT")){
      currentPage = BRIGHT;
      drawBright();
    }
  }

  if (server.hasArg("timePage")){
    timePage = server.arg("timePage").toInt();
    clearMatrix();
    drawTimeFirstTime = true;
    needSaveExt = true;
    if(currentPage == TIME) drawTime();
  }

  if (server.hasArg("timeModel")){
    timeModel = server.arg("timeModel").toInt();
    timeIndex = 0;
    needSaveExt = true;
  }

  if (server.hasArg("rhythmPage")){
    rhythmPage = server.arg("rhythmPage").toInt();
    needSaveExt = true;
  }

  if (server.hasArg("bandsModel")){
    rhythmBandsModel = server.arg("bandsModel").toInt();
    memset(peak,0,sizeof(peak));
    memset(oldBarHeights,0,sizeof(oldBarHeights));
    needSaveExt = true;
  }

  if (server.hasArg("animPage")){
    animPage = server.arg("animPage").toInt();
    lightedCount = 0;
    memset(matrixArray,0,sizeof(matrixArray));
    clearMatrix();
    needSaveExt = true;
  }

  if (server.hasArg("brightModel")){
    brightModel = server.arg("brightModel").toInt();
    if(brightModel == BRIGHT_MODEL_AUTO){
      clearBrightSampling();
    }else{
      getBrightness();
    }
    needSaveExt = true;
    if(currentPage == BRIGHT) drawBright();
  }

  if (server.hasArg("brightness")){
    int b = server.arg("brightness").toInt();
    if(b >= 5 && b <= 145){
      brightness = b;
      brightModel = BRIGHT_MODEL_MANUAL;
      recordBrightness();
      needSaveExt = true;
      matrix.setBrightness(brightness);
      if(currentPage == BRIGHT) drawBright();
    }
  }

  if (server.hasArg("red") && server.hasArg("green") && server.hasArg("blue")){
    clockColor[0] = server.arg("red").toInt();
    clockColor[1] = server.arg("green").toInt();
    clockColor[2] = server.arg("blue").toInt();
    mainColor = matrix.Color(clockColor[0], clockColor[1], clockColor[2]);
    int maxRGB = max(clockColor[0], max(clockColor[1], clockColor[2]));
    bool findMax = false;
    int weekR, weekG, weekB;
    if(clockColor[0] == maxRGB){ findMax = true; weekR = clockColor[0]; } else { weekR = 255 - clockColor[0]; }
    if(clockColor[1] == maxRGB && !findMax){ findMax = true; weekG = clockColor[1]; } else { weekG = 255 - clockColor[1]; }
    if(clockColor[2] == maxRGB && !findMax){ findMax = true; weekB = clockColor[2]; } else { weekB = 255 - clockColor[2]; }
    weekColor = matrix.Color(weekR, weekG, weekB);
    recordInfos(ssid, pass, clockColor[0], clockColor[1], clockColor[2], false);
    if(currentPage == TIME){
      drawTimeFirstTime = true;
      drawTime();
    }
  }

  if(needSaveExt){
    recordExtensionPage();
  }

  if (server.hasArg("reboot")){
    server.send(200, "application/json", "{\"success\":true,\"msg\":\"rebooting\"}");
    delay(500);
    ESP.restart();
    return;
  }

  if (server.hasArg("resetWifi")){
    server.send(200, "application/json", "{\"success\":true,\"msg\":\"resetting_wifi\"}");
    delay(500);
    setApConfigWhenStart(true);
    ESP.restart();
    return;
  }

  handleApiStatus();
}

// 接收弹幕消息
void handleApiMessage(){
  if(!server.hasArg("text")){
    server.send(400, "application/json", "{\"error\":\"missing text parameter\"}");
    return;
  }
  String text = server.arg("text");
  int repeat = 2;
  if(server.hasArg("repeat")){
    repeat = server.arg("repeat").toInt();
    if(repeat < 1) repeat = 1;
    if(repeat > 5) repeat = 5;
  }
  startMarquee(text, repeat);
  server.send(200, "application/json", "{\"success\":true,\"text\":\"" + text + "\"}");
}

// 独立 OTA 页面
void handleUpdatePage(){
  server.send_P(200, "text/html", OTA_PAGE);
}

// OTA 固件上传处理
void handleUpdateUpload(){
  HTTPUpload& upload = server.upload();
  if (upload.status == UPLOAD_FILE_START) {
    Serial.printf("[OTA] 开始上传固件: %s\n", upload.filename.c_str());
    isUpdating = true;
    drawText(2, 6, "OTA..");
    if (!Update.begin(UPDATE_SIZE_UNKNOWN)) {
      Update.printError(Serial);
    }
  } else if (upload.status == UPLOAD_FILE_WRITE) {
    if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
      Update.printError(Serial);
    }
  } else if (upload.status == UPLOAD_FILE_END) {
    if (Update.end(true)) {
      Serial.printf("[OTA] 固件写入成功, 大小: %u 字节\n", upload.totalSize);
      drawText(2, 6, "OTA OK");
    } else {
      Serial.println("[OTA] 固件写入失败!");
      Update.printError(Serial);
      drawText(2, 6, "OTA ERR");
    }
  } else if (upload.status == UPLOAD_FILE_ABORTED) {
    Update.end();
    isUpdating = false;
    Serial.println("[OTA] 上传中止");
    drawText(2, 6, "ABORT");
  }
}

// OTA 固件升级结果响应
void handleUpdateEnd(){
  server.sendHeader("Connection", "close");
  if (Update.hasError()) {
    isUpdating = false;
    server.send(200, "text/html", "<meta charset='UTF-8'><style>body{background:#0f172a;color:#ef4444;font-family:sans-serif;text-align:center;padding-top:50px;}</style><h2>❌ 固件升级失败！</h2><p>请检查文件格式后重试。</p><br><a href='/' style='color:#06b6d4;'>返回控制台</a>");
  } else {
    server.send(200, "text/html", "<meta charset='UTF-8'><style>body{background:#0f172a;color:#10b981;font-family:sans-serif;text-align:center;padding-top:50px;}</style><h2>✅ 固件升级成功！</h2><p>设备正在重启，将在 10 秒后自动刷新跳转...</p><script>setTimeout(function(){location.href='/';}, 10000);</script>");
    delay(1000);
    ESP.restart();
  }
}

// 连接WiFi
void connectWiFi(int timeOut_s){
  int connectTime = 0;
  Serial.print("正在连接网络: ");
  Serial.println(ssid);
  WiFi.setAutoReconnect(true);
  WiFi.persistent(true);
  WiFi.begin(ssid.c_str(), pass.c_str());
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
    connectTime++;
    if (connectTime > 2 * timeOut_s){
      Serial.println("\n网络连接失败...");
      safeDeleteTask(showTextTask);
      drawFailed(4, 24, "WIFI");
      delay(2000);
      clearMatrix();
      currentPage = RHYTHM;
      return;
    }
  }
  wifiConnected = true;
  Serial.println("\n网络连接成功");
  Serial.print("局域网 Web 控制台 IP： http://");
  Serial.println(WiFi.localIP());
}

// 启动服务器
void startServer(){
  if(serverStarted) return;
  server.on("/", HTTP_GET, handleRoot);
  server.on("/configwifi", HTTP_POST, handleConfigWifi);
  server.on("/wifi", HTTP_GET, handleWifiPage);
  server.on("/api/status", HTTP_GET, handleApiStatus);
  server.on("/api/control", HTTP_POST, handleApiControl);
  server.on("/api/message", HTTP_POST, handleApiMessage);
  server.on("/update", HTTP_GET, handleUpdatePage);
  server.on("/update", HTTP_POST, handleUpdateEnd, handleUpdateUpload);
  server.onNotFound(handleNotFound);
  server.begin();
  serverStarted = true;
  Serial.println("Web 服务器启动成功 (端口 80)！");
}

// 开启AP模式，如果开启失败，重启系统
void startAP(){
  Serial.println("开启AP模式...");
  WiFi.enableAP(true);
  WiFi.softAPConfig(staticIP, gateway, subnet);
  if (!WiFi.softAP(APssid)) {
    Serial.println("AP模式启动失败");
    ESP.restart();
  }  
  Serial.println("AP模式启动成功");
  Serial.print("IP地址: ");
  Serial.println(WiFi.softAPIP());
}

// 扫描WiFi
void scanWiFi(){
  Serial.println("开始扫描WiFi");
  int n = WiFi.scanNetworks();
  if (n){
    Serial.print("扫描到 ");
    Serial.print(n);
    Serial.println(" 个WIFI");
    WifiNames = "";
    WifiNames.reserve(1024);
    bool hasSavedSSID = false;
    for (size_t i = 0; i < n; i++){
      int32_t rssi = WiFi.RSSI(i);
      String signalStrength;
      if(rssi >= -35){
        signalStrength = " (信号极强)";
      }else if(rssi >= -50){
        signalStrength = " (信号强)";
      }else if(rssi >= -70){
        signalStrength = " (信号中)";
      }else{
        signalStrength = " (信号弱)";
      }
      if(WiFi.SSID(i).equals(ssid) && !hasSavedSSID){
        hasSavedSSID = true;
        WifiNames += "<option selected value='" + WiFi.SSID(i) + "'>" + WiFi.SSID(i) + signalStrength + "</option>";
      }else{
        WifiNames += "<option value='" + WiFi.SSID(i) + "'>" + WiFi.SSID(i) + signalStrength + "</option>";
      }
    }
    if(!hasSavedSSID && !ssid.equals("")){
      WifiNames += "<option selected value='" + ssid + "'>" + ssid + "</option>";
    }
  }else{
    Serial.println("没扫描到WIFI");
  }
}

// 组合HTML颜色及密码字段
void getHTML(){
  RGBColors = "";
  RGBColors.reserve(512);
  RGBColors += "R<input type='number' placeholder='0-255' name='red' id='red' class='rgb' value='" + String(clockColor[0]) + "'>";
  RGBColors += "G<input type='number' placeholder='0-255' name='green' id='green' class='rgb' value='" + String(clockColor[1]) + "'>";
  RGBColors += "B<input type='number' placeholder='0-255' name='blue' id='blue' class='rgb' value='" + String(clockColor[2]) + "'>";
  PassHTML = "<input type='text' placeholder='请输入WiFi密码' name='pass' id='pass' class='passAndCity commonWidth' value='" + pass + "'>";
  PassHTML.reserve(256);
}

// 获取NTP并同步RTC时间
void getNTPTime(){
  Serial.println("执行NTP对时");
  configTime(8 * 3600, 0, NTP1, NTP2, NTP3);
}

// 系统启动时进行NTP对时
void checkTime(int limitTime){
  time_t start;
  time(&start);
  getNTPTime();
  struct tm timeinfo;
  while (!getLocalTime(&timeinfo)){
    time_t end;
    time(&end);
    if((end - start) > limitTime){
      Serial.println("时钟对时超时...");
      safeDeleteTask(showTextTask);
      drawFailed(4, 24, "TIME");
      delay(2000);
      clearMatrix();
      currentPage = RHYTHM;
      return;
    }
    Serial.println("时钟对时失败...");
    delay(500);
    getNTPTime();
  }
  RTCSuccess = true;
}

// 关闭wifi
void disConnectWifi(){
  WiFi.disconnect();
}

// 定时执行 NTP 对时（保持局域网连接，不关闭 WiFi）
void checkTimeTicker(){
  Serial.println("定时执行 NTP 对时...");
  if(WiFi.status() == WL_CONNECTED){
    wifiConnected = true;
    getNTPTime();
  }else{
    Serial.println("网络未连接，尝试重连...");
    WiFi.begin(ssid.c_str(), pass.c_str());
    int connectTime = 0;
    while (WiFi.status() != WL_CONNECTED && connectTime < 20) {
      delay(500);
      connectTime++;
    }
    if(WiFi.status() == WL_CONNECTED){
      wifiConnected = true;
      getNTPTime();
    }else{
      wifiConnected = false;
    }
  }
}
