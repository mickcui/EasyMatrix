#ifndef __DASHBOARD_HTML_H
#define __DASHBOARD_HTML_H

#include <Arduino.h>

const char DASHBOARD_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="zh-CN">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0, maximum-scale=1.0, user-scalable=no">
<title>EasyMatrix 像素时钟控制台</title>
<style>
:root {
  --bg: #0f172a;
  --card-bg: #1e293b;
  --border: #334155;
  --primary: #06b6d4;
  --primary-hover: #0891b2;
  --text: #f8fafc;
  --text-muted: #94a3b8;
  --success: #10b981;
  --danger: #ef4444;
  --radius: 12px;
}
* { box-sizing: border-box; margin: 0; padding: 0; }
body {
  background-color: var(--bg);
  color: var(--text);
  font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, Helvetica, Arial, sans-serif;
  padding: 16px;
  line-height: 1.5;
}
.container { max-width: 800px; margin: 0 auto; }
header {
  display: flex;
  justify-content: space-between;
  align-items: center;
  margin-bottom: 20px;
  padding-bottom: 12px;
  border-bottom: 1px solid var(--border);
}
.brand { font-size: 20px; font-weight: 700; color: var(--primary); display: flex; align-items: center; gap: 8px; }
.status-pill {
  display: flex;
  align-items: center;
  gap: 6px;
  font-size: 13px;
  padding: 4px 10px;
  border-radius: 20px;
  background: rgba(16, 185, 129, 0.15);
  color: var(--success);
}
.dot { width: 8px; height: 8px; border-radius: 50%; background: var(--success); animation: pulse 2s infinite; }
@keyframes pulse { 0% { opacity: 1; } 50% { opacity: 0.4; } 100% { opacity: 1; } }

.grid-kpi {
  display: grid;
  grid-template-columns: repeat(auto-fit, minmax(130px, 1fr));
  gap: 12px;
  margin-bottom: 16px;
}
.kpi-card {
  background: var(--card-bg);
  border: 1px solid var(--border);
  border-radius: var(--radius);
  padding: 12px;
}
.kpi-label { font-size: 12px; color: var(--text-muted); margin-bottom: 4px; }
.kpi-val { font-size: 16px; font-weight: 600; color: var(--text); }

.card {
  background: var(--card-bg);
  border: 1px solid var(--border);
  border-radius: var(--radius);
  padding: 18px;
  margin-bottom: 16px;
}
.card-title {
  font-size: 16px;
  font-weight: 600;
  margin-bottom: 14px;
  display: flex;
  align-items: center;
  gap: 8px;
}
.btn-group {
  display: grid;
  grid-template-columns: repeat(auto-fit, minmax(100px, 1fr));
  gap: 10px;
  margin-bottom: 12px;
}
button {
  background: #334155;
  color: var(--text);
  border: 1px solid transparent;
  padding: 10px 14px;
  border-radius: 8px;
  font-size: 14px;
  cursor: pointer;
  font-weight: 500;
  transition: all 0.2s;
  display: flex;
  align-items: center;
  justify-content: center;
  text-align: center;
}
button:hover { background: #475569; }
button.active {
  background: var(--primary);
  color: #0f172a;
  font-weight: 700;
  border-color: rgba(255,255,255,0.4);
  box-shadow: 0 0 12px rgba(6, 182, 212, 0.4);
}
button:disabled { opacity: 0.5; cursor: not-allowed; }

.sub-group { margin-top: 14px; padding-top: 14px; border-top: 1px dashed var(--border); }
.sub-title { font-size: 13px; color: var(--text-muted); margin-bottom: 8px; }

.slider-container { display: flex; align-items: center; gap: 12px; margin: 12px 0; }
input[type=range] { flex: 1; height: 6px; accent-color: var(--primary); border-radius: 4px; }
.color-picker-box { display: flex; align-items: center; gap: 14px; margin: 12px 0; }
input[type=color] {
  width: 44px;
  height: 44px;
  border: none;
  border-radius: 8px;
  cursor: pointer;
  background: transparent;
}
.input-row { display: flex; gap: 10px; }
input[type=text], input[type=file] {
  flex: 1;
  background: #0f172a;
  border: 1px solid var(--border);
  color: var(--text);
  padding: 10px 12px;
  border-radius: 8px;
  font-size: 14px;
}
.btn-primary { background: var(--primary); color: #0f172a; font-weight: 600; }
.btn-primary:hover { background: var(--primary-hover); }
.btn-danger { background: #dc2626; color: #fff; }
.btn-danger:hover { background: #b91c1c; }

progress {
  width: 100%;
  height: 12px;
  border-radius: 6px;
  margin-top: 10px;
  display: none;
  accent-color: var(--primary);
}
.tip-text { font-size: 12px; color: var(--text-muted); margin-top: 6px; }
footer { text-align: center; color: var(--text-muted); font-size: 12px; margin-top: 24px; padding-bottom: 20px; }
</style>
</head>
<body>
<div class="container">
  <header>
    <div class="brand">⚡ EasyMatrix 控制台</div>
    <div class="status-pill"><div class="dot"></div>在线</div>
  </header>

  <!-- 状态概览卡片 -->
  <div class="grid-kpi">
    <div class="kpi-card">
      <div class="kpi-label">当前页面</div>
      <div class="kpi-val" id="kpiPage">--</div>
    </div>
    <div class="kpi-card">
      <div class="kpi-label">设备 IP</div>
      <div class="kpi-val" id="kpiIp">--</div>
    </div>
    <div class="kpi-card">
      <div class="kpi-label">WiFi 信号</div>
      <div class="kpi-val" id="kpiRssi">--</div>
    </div>
    <div class="kpi-card">
      <div class="kpi-label">运行时间</div>
      <div class="kpi-val" id="kpiUptime">--</div>
    </div>
  </div>

  <!-- 主页面切换 -->
  <div class="card">
    <div class="card-title">🖥️ 显示页面切换</div>
    <div class="btn-group">
      <button id="btnPageTIME" onclick="sendCmd({page:'TIME'})">⏰ 时钟时间</button>
      <button id="btnPageRHYTHM" onclick="sendCmd({page:'RHYTHM'})">🎵 音乐律动</button>
      <button id="btnPageANIM" onclick="sendCmd({page:'ANIM'})">👾 像素动画</button>
      <button id="btnPageBRIGHT" onclick="sendCmd({page:'BRIGHT'})">💡 亮度设置</button>
    </div>

    <!-- 时间页面设置 -->
    <div id="subTIME" class="sub-group">
      <div class="sub-title">时间显示格式</div>
      <div class="btn-group">
        <button id="btnTimePage1" onclick="sendCmd({timePage:1})">时:分:秒</button>
        <button id="btnTimePage2" onclick="sendCmd({timePage:2})">时:分</button>
        <button id="btnTimePage3" onclick="sendCmd({timePage:3})">日期 + 星期</button>
      </div>
      <div class="sub-title" style="margin-top:10px;">数字过渡效果</div>
      <div class="btn-group">
        <button id="btnTimeModel2" onclick="sendCmd({timeModel:2})">滑动翻页动画</button>
        <button id="btnTimeModel1" onclick="sendCmd({timeModel:1})">直接数字跳变</button>
      </div>
    </div>

    <!-- 律动页面设置 -->
    <div id="subRHYTHM" class="sub-group" style="display:none;">
      <div class="sub-title">频谱柱状样式</div>
      <div class="btn-group">
        <button id="btnRhythmPage1" onclick="sendCmd({rhythmPage:1})">全彩渐变柱</button>
        <button id="btnRhythmPage2" onclick="sendCmd({rhythmPage:2})">经典落顶点</button>
        <button id="btnRhythmPage3" onclick="sendCmd({rhythmPage:3})">双色彩虹柱</button>
        <button id="btnRhythmPage4" onclick="sendCmd({rhythmPage:4})">流动彩虹条</button>
      </div>
      <div class="sub-title" style="margin-top:10px;">频段排布方式</div>
      <div class="btn-group">
        <button id="btnBands1" onclick="sendCmd({bandsModel:1})">单向 (左低 -> 右高)</button>
        <button id="btnBands2" onclick="sendCmd({bandsModel:2})">对称 (低频在中间)</button>
      </div>
    </div>

    <!-- 动画页面设置 -->
    <div id="subANIM" class="sub-group" style="display:none;">
      <div class="sub-title">像素动画选择</div>
      <div class="btn-group">
        <button id="btnAnimPage1" onclick="sendCmd({animPage:1})">🐬 跃水小海豚</button>
        <button id="btnAnimPage2" onclick="sendCmd({animPage:2})">💻 黑客代码雨</button>
        <button id="btnAnimPage3" onclick="sendCmd({animPage:3})">✨ 星光散落粒子</button>
      </div>
    </div>
  </div>

  <!-- 亮度与颜色 -->
  <div class="card">
    <div class="card-title">💡 亮度与颜色管理</div>
    <div class="sub-title">调光模式</div>
    <div class="btn-group">
      <button id="btnBrightModel1" onclick="sendCmd({brightModel:1})">手动调光</button>
      <button id="btnBrightModel2" onclick="sendCmd({brightModel:2})">光敏自动调光</button>
    </div>
    <div class="slider-container" id="sliderBox">
      <span style="font-size:13px;color:var(--text-muted);">微暗</span>
      <input type="range" id="brightRange" min="5" max="145" step="5" onchange="sendCmd({brightness:this.value})">
      <span id="brightValText" style="font-size:14px;font-weight:600;min-width:32px;">45</span>
    </div>
    <div class="sub-group">
      <div class="sub-title">时钟主体色彩 (RGB)</div>
      <div class="color-picker-box">
        <input type="color" id="clockColorPicker" onchange="onColorChange(this.value)">
        <span id="colorHexText" style="font-size:14px;font-family:monospace;">#00FAFA</span>
        <button class="btn-primary" onclick="saveColor()" style="margin-left:auto;">保存并应用色彩</button>
      </div>
    </div>
  </div>

  <!-- 弹幕文字发送 -->
  <div class="card">
    <div class="card-title">💬 屏幕弹幕 / 滚动消息</div>
    <div class="input-row">
      <input type="text" id="marqueeText" placeholder="输入英文/数字消息 (例如: Hello World!)" maxlength="40">
      <button class="btn-primary" onclick="sendMessage()">发送弹幕</button>
    </div>
    <div class="tip-text">发送后文字将立即在 8×32 屏幕上平滑滚动展示。</div>
  </div>

  <!-- OTA 在线固件升级 -->
  <div class="card">
    <div class="card-title">🚀 固件在线升级 (OTA)</div>
    <div class="input-row" style="flex-direction:column;">
      <input type="file" id="otaFileInput" accept=".bin">
      <button class="btn-primary" id="btnUploadOta" onclick="startOTA()" style="width:100%;margin-top:8px;">开始空中升级</button>
    </div>
    <progress id="otaProg" value="0" max="100"></progress>
    <div id="otaStatusText" class="tip-text">选择 Arduino 或 PlatformIO 编译导出的 <code>.bin</code> 固件即可在线无线烧录。</div>
  </div>

  <!-- 系统快捷操作 -->
  <div class="card">
    <div class="card-title">🔧 系统维护</div>
    <div class="btn-group">
      <button onclick="if(confirm('确定要进入热点重新配置 WiFi 吗？')) sendCmd({resetWifi:true})">📶 重新配网</button>
      <button class="btn-danger" onclick="if(confirm('确定要重启设备吗？')) sendCmd({reboot:true})">🔄 重启设备</button>
    </div>
  </div>

  <footer>EasyMatrix Pixel Clock · LAN Web Dashboard & OTA</footer>
</div>

<script>
let curR = 0, curG = 250, curB = 250;

function sendCmd(params) {
  const url = '/api/control?' + new URLSearchParams(params);
  fetch(url, { method: 'POST' })
    .then(r => r.json())
    .then(data => updateUI(data))
    .catch(err => console.error(err));
}

function updateUI(data) {
  if(!data) return;
  // 更新 KPI 卡片
  document.getElementById('kpiPage').innerText = data.page;
  document.getElementById('kpiIp').innerText = data.ip;
  document.getElementById('kpiRssi').innerText = data.rssi + ' dBm';
  
  const m = Math.floor(data.uptime / 60);
  const h = Math.floor(m / 60);
  document.getElementById('kpiUptime').innerText = h > 0 ? (h + '小时' + (m%60) + '分') : (m + '分钟');

  // 主页面按钮高亮
  ['TIME','RHYTHM','ANIM','BRIGHT'].forEach(p => {
    const el = document.getElementById('btnPage' + p);
    if(el) el.classList.toggle('active', data.page === p);
  });

  // 控制子面板显示
  document.getElementById('subTIME').style.display = (data.page === 'TIME') ? 'block' : 'none';
  document.getElementById('subRHYTHM').style.display = (data.page === 'RHYTHM') ? 'block' : 'none';
  document.getElementById('subANIM').style.display = (data.page === 'ANIM') ? 'block' : 'none';

  // 时间格式
  [1,2,3].forEach(n => {
    const el = document.getElementById('btnTimePage' + n);
    if(el) el.classList.toggle('active', data.timePage === n);
  });
  // 时间跳变
  [1,2].forEach(n => {
    const el = document.getElementById('btnTimeModel' + n);
    if(el) el.classList.toggle('active', data.timeModel === n);
  });

  // 律动模式
  [1,2,3,4].forEach(n => {
    const el = document.getElementById('btnRhythmPage' + n);
    if(el) el.classList.toggle('active', data.rhythmPage === n);
  });
  [1,2].forEach(n => {
    const el = document.getElementById('btnBands' + n);
    if(el) el.classList.toggle('active', data.bandsModel === n);
  });

  // 动画模式
  [1,2,3].forEach(n => {
    const el = document.getElementById('btnAnimPage' + n);
    if(el) el.classList.toggle('active', data.animPage === n);
  });

  // 亮度
  [1,2].forEach(n => {
    const el = document.getElementById('btnBrightModel' + n);
    if(el) el.classList.toggle('active', data.brightModel === n);
  });
  document.getElementById('sliderBox').style.opacity = (data.brightModel === 2) ? '0.4' : '1';
  document.getElementById('brightRange').disabled = (data.brightModel === 2);
  document.getElementById('brightRange').value = data.brightness;
  document.getElementById('brightValText').innerText = data.brightness;

  // 颜色
  if(data.color && data.color.length === 3) {
    curR = data.color[0]; curG = data.color[1]; curB = data.color[2];
    const hex = rgbToHex(curR, curG, curB);
    document.getElementById('clockColorPicker').value = hex;
    document.getElementById('colorHexText').innerText = hex.toUpperCase() + ` (${curR},${curG},${curB})`;
  }
}

function rgbToHex(r, g, b) {
  return "#" + [r, g, b].map(x => {
    const hex = x.toString(16);
    return hex.length === 1 ? "0" + hex : hex;
  }).join("");
}

function hexToRgb(hex) {
  const result = /^#?([a-f\d]{2})([a-f\d]{2})([a-f\d]{2})$/i.exec(hex);
  return result ? {
    r: parseInt(result[1], 16),
    g: parseInt(result[2], 16),
    b: parseInt(result[3], 16)
  } : null;
}

function onColorChange(hex) {
  const rgb = hexToRgb(hex);
  if(rgb) {
    curR = rgb.r; curG = rgb.g; curB = rgb.b;
    document.getElementById('colorHexText').innerText = hex.toUpperCase() + ` (${curR},${curG},${curB})`;
  }
}

function saveColor() {
  sendCmd({ red: curR, green: curG, blue: curB });
}

function sendMessage() {
  const input = document.getElementById('marqueeText');
  const txt = input.value.trim();
  if(!txt) { alert('请输入要发送的文字内容'); return; }
  fetch('/api/message?text=' + encodeURIComponent(txt) + '&repeat=2', { method: 'POST' })
    .then(r => r.json())
    .then(res => {
      alert('已成功发送到屏幕展示！');
      input.value = '';
    })
    .catch(() => alert('发送失败，请检查连接'));
}

function startOTA() {
  const fileInput = document.getElementById('otaFileInput');
  if(!fileInput.files.length) { alert('请先选择 .bin 固件文件'); return; }
  if(!confirm('确定开始固件升级吗？写入期间切勿断电！')) return;

  const file = fileInput.files[0];
  const formData = new FormData();
  formData.append('update', file);

  const xhr = new XMLHttpRequest();
  xhr.open('POST', '/update');

  const pbar = document.getElementById('otaProg');
  const ptext = document.getElementById('otaStatusText');
  const btn = document.getElementById('btnUploadOta');

  btn.disabled = true;
  btn.innerText = '正在上传中...';
  pbar.style.display = 'block';

  xhr.upload.onprogress = function(e) {
    if(e.lengthComputable) {
      const pct = Math.round((e.loaded / e.total) * 100);
      pbar.value = pct;
      ptext.innerText = '正在写入固件: ' + pct + '% (请保持供电)';
    }
  };

  xhr.onload = function() {
    if(xhr.status === 200) {
      ptext.innerHTML = '<b style="color:var(--success);">✅ 升级成功！设备正在自动重启，将在 12 秒后刷新页面...</b>';
      setTimeout(() => location.reload(), 12000);
    } else {
      ptext.innerHTML = '<b style="color:var(--danger);">❌ 升级失败: ' + xhr.responseText + '</b>';
      btn.disabled = false;
      btn.innerText = '重新升级';
    }
  };

  xhr.onerror = function() {
    ptext.innerHTML = '<b style="color:var(--danger);">网络连接中断，升级未完成</b>';
    btn.disabled = false;
    btn.innerText = '重试';
  };

  xhr.send(formData);
}

function fetchStatus() {
  fetch('/api/status')
    .then(r => r.json())
    .then(data => updateUI(data))
    .catch(() => {});
}

// 页面加载即抓取状态，随后每 3.5 秒周期轮询
fetchStatus();
setInterval(fetchStatus, 3500);
</script>
</body>
</html>
)rawliteral";

const char OTA_PAGE[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width,initial-scale=1.0">
<title>EasyMatrix 固件在线升级</title>
<style>
body{background:#0f172a;color:#f8fafc;font-family:-apple-system,BlinkMacSystemFont,"Segoe UI",Roboto,sans-serif;padding:20px;display:flex;justify-content:center;align-items:center;min-height:90vh;margin:0;}
.box{background:#1e293b;border:1px solid #334155;border-radius:12px;padding:24px;max-width:420px;width:100%;box-shadow:0 10px 25px rgba(0,0,0,0.4);text-align:center;}
h2{margin-top:0;color:#38bdf8;}
p{color:#94a3b8;font-size:14px;line-height:1.5;}
.input-file{margin:20px 0;padding:12px;border:2px dashed #475569;border-radius:8px;cursor:pointer;width:90%;background:#0f172a;color:#cbd5e1;}
.btn{background:#0284c7;color:#fff;border:none;padding:12px 24px;border-radius:8px;font-size:16px;cursor:pointer;width:100%;font-weight:bold;transition:0.2s;}
.btn:hover{background:#0369a1;}
.back{display:inline-block;margin-top:16px;color:#38bdf8;text-decoration:none;font-size:14px;}
progress{width:100%;height:16px;margin-top:16px;display:none;border-radius:8px;accent-color:#0284c7;}
</style>
</head>
<body>
<div class="box">
<h2>🚀 固件在线升级 (OTA)</h2>
<p>请选择编译生成的 <code>.bin</code> 固件文件上传。<br>升级过程中切勿断电！</p>
<form method="POST" action="/update" enctype="multipart/form-data" id="otaForm">
<input type="file" name="update" accept=".bin" class="input-file" required id="fileInput"><br>
<button type="submit" class="btn" id="uploadBtn">开始升级</button>
<progress id="pbar" value="0" max="100"></progress>
<div id="ptext" style="margin-top:10px;font-size:13px;color:#94a3b8;"></div>
</form>
<a href="/" class="back">← 返回主控制台</a>
</div>
<script>
document.getElementById('otaForm').onsubmit = function(e){
  var f = document.getElementById('fileInput').files[0];
  if(!f) return false;
  if(!confirm('确定开始固件升级吗？升级过程中请勿切断电源！')) { e.preventDefault(); return false; }
  e.preventDefault();
  var fd = new FormData();
  fd.append('update', f);
  var xhr = new XMLHttpRequest();
  xhr.open('POST', '/update');
  var pbar = document.getElementById('pbar');
  var ptext = document.getElementById('ptext');
  var btn = document.getElementById('uploadBtn');
  btn.disabled = true;
  btn.innerText = '正在升级...';
  pbar.style.display = 'block';
  xhr.upload.onprogress = function(evt){
    if(evt.lengthComputable){
      var pct = Math.round((evt.loaded / evt.total) * 100);
      pbar.value = pct;
      ptext.innerText = '正在写入固件: ' + pct + '%';
    }
  };
  xhr.onload = function(){
    if(xhr.status === 200){
      document.body.innerHTML = xhr.responseText;
    }else{
      ptext.innerHTML = '<span style="color:#ef4444">上传失败: ' + xhr.responseText + '</span>';
      btn.disabled = false;
      btn.innerText = '重新升级';
    }
  };
  xhr.onerror = function(){
    ptext.innerHTML = '<span style="color:#ef4444">网络连接中断</span>';
    btn.disabled = false;
    btn.innerText = '重试';
  };
  xhr.send(fd);
};
</script>
</body>
</html>
)rawliteral";

#endif
