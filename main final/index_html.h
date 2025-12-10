// index_html.h (enhanced: Light/Dark Mode, Device Status, Mode Indicator, Version)
#pragma once
#include <pgmspace.h>

const char index_html[] PROGMEM = R"rawliteral(
<!doctype html>
<html lang="en">
<head>
<meta charset="utf-8" />
<meta name="viewport" content="width=device-width,initial-scale=1" />
<title>Parking Dashboard</title>

<style>
:root{
  /* Dark Mode Theme (existing) */
  --bg1:#021025;--bg2:#04102b;
  --panel:rgba(255,255,255,0.04);
  --accent:#56f0ff;--accent2:#8a57ff;--glass:rgba(255,255,255,0.03);
  --danger:#ff6b6b;--muted:rgba(215,238,247,0.7);

  /* Light Mode Theme */
  --bg1-light:#ffffff;
  --bg2-light:#ffffff;
  --panel-light:rgba(0,0,0,0.05);
  --glass-light:rgba(0,0,0,0.03);
  --muted-light:rgba(40,40,40,0.6);

  font-family:Inter,system-ui,-apple-system,Segoe UI,Roboto,Arial;
  transition: background-color .25s, color .25s;
}

*{box-sizing:border-box}
html,body{
  height:100%;margin:0;
  background:linear-gradient(160deg,var(--bg1),var(--bg2));
  color:#d7eef7;
}
body.light{
  background:linear-gradient(160deg,var(--bg1-light),var(--bg2-light));
  color:#111;
}

.wrap{display:flex;gap:20px;align-items:flex-start;justify-content:center;padding:28px}
.card{
  background:linear-gradient(180deg,rgba(255,255,255,0.02),rgba(255,255,255,0.01));
  border:1px solid rgba(255,255,255,0.04);
  border-radius:12px;padding:16px;min-width:280px
}
body.light .card{
  background:var(--panel-light);
  border:1px solid rgba(0,0,0,0.08);
}

.controls{width:340px;display:flex;flex-direction:column;gap:12px}

/* Header upgraded for Option A2 */
.header{
  display:flex;
  justify-content:space-between;
  align-items:center;
}

.header-left{display:flex;gap:12px;align-items:center}

.logo{
  width:48px;height:48px;border-radius:10px;
  background:linear-gradient(135deg,var(--accent),var(--accent2));
  display:flex;align-items:center;justify-content:center;
  color:#002;font-weight:700
}

.header-right{
  display:flex;
  gap:12px;
  align-items:center;
  font-size:12px;
}

#deviceStatus{font-weight:700;}
#modeIndicator{font-weight:700;}

#themeToggle{
  padding:6px 10px;
  border-radius:8px;
  border:none;
  cursor:pointer;
  font-size:16px;
  background:var(--panel);
  color:white;
}
body.light #themeToggle{
  background:var(--panel-light);
  color:#000;
}

/* Original styling preserved */
h1{font-size:18px;margin:0}
.sub{font-size:12px;color:var(--muted)}
body.light .sub{color:var(--muted-light)}

.panel{
  padding:12px;border-radius:10px;background:var(--glass);
  border:1px solid rgba(255,255,255,0.02)
}
body.light .panel{
  background:var(--glass-light);
  border:1px solid rgba(0,0,0,0.08)
}

.gate-visual{height:140px;display:flex;align-items:center;justify-content:center;position:relative;overflow:hidden}
.gate-base{width:220px;height:10px;background:linear-gradient(90deg,#0b2a3f,#082033);border-radius:6px;position:relative}
body.light .gate-base{background:linear-gradient(90deg,#e5e9ef,#f7f9fb)}

.gate-arm{position:absolute;left:110px;bottom:40px;width:6px;height:120px;background:linear-gradient(180deg,var(--accent),var(--accent2));transform-origin:bottom center;border-radius:3px;transition:transform .28s}

.big-count{font-size:32px;font-weight:700}
.small{font-size:12px;color:var(--muted)}
body.light .small{color:var(--muted-light)}

.btn{
  background:linear-gradient(90deg,var(--accent),var(--accent2));color:#002;
  border:none;padding:9px 12px;border-radius:10px;cursor:pointer;font-weight:700
}
.btn.ghost{background:transparent;border:1px solid rgba(255,255,255,0.04);color:inherit}
.btn.danger{background:var(--danger);color:#fff}

.grid-card{flex:1;min-width:420px}
.grid{display:grid;grid-template-columns:1fr;gap:12px}

.slot{
  height:84px;border-radius:10px;
  background:linear-gradient(180deg,rgba(255,255,255,0.016),rgba(255,255,255,0.01));
  display:flex;flex-direction:column;align-items:center;justify-content:center;
  border:1px solid rgba(255,255,255,0.02)
}
body.light .slot{
  background:linear-gradient(180deg,rgba(0,0,0,0.03),rgba(0,0,0,0.02));
  border:1px solid rgba(0,0,0,0.06)
}

.slot.open{box-shadow:0 6px 20px rgba(86,240,255,0.06);border-color:rgba(86,240,255,0.18)}
.slot.occupied{background:linear-gradient(180deg,rgba(255,107,107,0.06),rgba(255,107,107,0.02));border-color:rgba(255,107,107,0.12)}

.footer{font-size:12px;color:var(--muted);text-align:center;margin-top:8px}
body.light .footer{color:var(--muted-light)}

@media(max-width:880px){
  .wrap{flex-direction:column;align-items:stretch;padding:12px}
  .controls{width:100%}
}
</style>
</head>

<body>
<div class="wrap">

  <div class="controls card">

    <!-- HEADER WITH NEW RIGHT-SIDE BLOCK -->
    <div class="header">

      <div class="header-left">
        <div class="logo">P</div>
        <div>
          <h1>NeonPark</h1>
          <div class="sub">Real-time parking dashboard</div>
        </div>
      </div>

      <div class="header-right">
        <div id="deviceStatus">Connecting...</div>
        <div id="modeIndicator">Mode: AUTO</div>
        <button id="themeToggle">🌙</button>
        <div style="opacity:0.7;">v3.0.0</div>
      </div>

    </div>
    <!-- END HEADER -->

    <!-- Gate Visual Panel -->
    <div class="panel">
      <div class="gate-visual">
        <div class="gate-base"></div>
        <div id="gateArm" class="gate-arm"></div>
      </div>
      <div style="display:flex;justify-content:space-between;margin-top:10px">
        <div>
          <div class="small">Gate</div>
          <div id="gateStatus" class="big-count">Closed</div>
        </div>
        <div style="text-align:right">
          <div class="small">Last</div>
          <div id="lastAction" class="small">—</div>
        </div>
      </div>
    </div>

    <!-- Slot Panel -->
    <div class="panel" style="margin-top:10px">
      <div style="display:flex;justify-content:space-between">
        <div>
          <div class="small">Slot</div>
          <div id="slotStatus" class="big-count">—</div>
        </div>
        <div style="text-align:right">
          <div class="small">Distance</div>
          <div id="distanceRead" class="small">— cm</div>
        </div>
      </div>

      <div style="display:flex;gap:8px;margin-top:12px">
        <button id="btnOpen" class="btn">Open</button>
        <button id="btnClose" class="btn ghost">Close</button>
      </div>
    </div>

    <!-- Mode Panel -->
    <div class="panel" style="margin-top:10px">
      <div class="small">Mode</div>
      <div style="display:flex;gap:8px;margin-top:12px">
        <button id="btnAuto" class="btn">AUTO</button>
        <button id="btnMaint" class="btn ghost">MAINTENANCE</button>
        <button id="btnSleep" class="btn danger">SLEEP</button>
      </div>
    </div>

    <div class="footer">
      Connect to WiFi <strong>Parking-System</strong> (pw: 12345678)<br>
      Open <strong>http://192.168.4.1</strong>
    </div>

  </div>

  <!-- Right Side Slot Grid -->
  <div class="grid-card card">
    <div style="display:flex;justify-content:space-between;margin-bottom:12px">
      <div>
        <h2 style="margin:0">Parking Slot</h2>
        <div class="sub">Live status</div>
      </div>
      <div class="kv"><div class="small">Total</div><div id="totalSlots" class="val">1</div></div>
    </div>

    <div id="slotGrid" class="grid">
      <div id="singleSlot" class="slot open">
        <div style="font-weight:700">#1</div>
        <div id="slotStateText" class="small">Empty</div>
      </div>
    </div>
  </div>

</div>

<script>
// WebSocket Setup
const WS_URL = "ws://192.168.4.1:81/";
let ws = null;
let reconnectTimer = null;
let currentMode = 0; // 0=AUTO,1=MAINT,2=SLEEP

const gateArm = document.getElementById('gateArm');
const gateStatus = document.getElementById('gateStatus');
const lastAction = document.getElementById('lastAction');
const slotStatus = document.getElementById('slotStatus');
const distanceRead = document.getElementById('distanceRead');
const slotStateText = document.getElementById('slotStateText');
const singleSlot = document.getElementById('singleSlot');

const btnOpen = document.getElementById('btnOpen');
const btnClose = document.getElementById('btnClose');

const btnAuto = document.getElementById('btnAuto');
const btnMaint = document.getElementById('btnMaint');
const btnSleep = document.getElementById('btnSleep');

const deviceStatus = document.getElementById('deviceStatus');
const modeIndicator = document.getElementById('modeIndicator');
const themeToggle = document.getElementById('themeToggle');

// ----------- THEME HANDLING -----------
function applyTheme(light){
  if(light){
    document.body.classList.add('light');
    themeToggle.textContent = "☀";
  } else {
    document.body.classList.remove('light');
    themeToggle.textContent = "🌙";
  }
  localStorage.setItem("theme_light", light ? "1" : "0");
}

const saved = localStorage.getItem("theme_light");
applyTheme(saved === "1");

themeToggle.onclick = ()=>{
  const isLight = document.body.classList.contains("light");
  applyTheme(!isLight);
};

// ----------- UI Updates -----------
function setGate(open){
  gateArm.style.transform = open ? 'rotate(-65deg)' : 'rotate(0deg)';
  gateStatus.textContent = open ? 'Open' : 'Closed';
  lastAction.textContent = open ? 'Gate opened' : 'Gate closed';
}

function setSlotOccupied(distance){
  singleSlot.classList.remove('open');
  singleSlot.classList.add('occupied');
  slotStateText.textContent = 'Occupied';
  slotStatus.textContent = 'Occupied';
  distanceRead.textContent = (typeof distance === 'number') ?
    distance.toFixed(1) + ' cm' : '— cm';
}

function setSlotAvailable(){
  singleSlot.classList.remove('occupied');
  singleSlot.classList.add('open');
  slotStateText.textContent = 'Empty';
  slotStatus.textContent = 'Available';
  distanceRead.textContent = '— cm';
}

function setBuzzerOff(){
  lastAction.textContent = 'Buzzer off';
}

function updateModeIndicator(){
  const text = ["AUTO","MAINTENANCE","SLEEP"][currentMode];
  modeIndicator.textContent = "Mode: " + text;
}

// ----------- WEBSOCKET -----------
function connectWS(){
  try { ws = new WebSocket(WS_URL); }
  catch(e){ scheduleReconnect(); return; }

  deviceStatus.textContent = "Connecting...";

  ws.onopen = ()=>{
    deviceStatus.textContent = "Connected";
    if (reconnectTimer){ clearTimeout(reconnectTimer); reconnectTimer = null; }
  };

  ws.onclose = ()=>{
    deviceStatus.textContent = "Disconnected";
    scheduleReconnect();
  };

  ws.onerror = ()=>{
    deviceStatus.textContent = "Error";
    scheduleReconnect();
  };

  ws.onmessage = (m)=>{
    const msg = m.data.trim();

    if(msg === "open") return setGate(true);
    if(msg === "close") return setGate(false);
    if(msg === "available") return setSlotAvailable();
    if(msg === "buzzer_off") return setBuzzerOff();

    if(msg.startsWith("occupied")){
      const parts = msg.split(":");
      if(parts.length >= 2){
        const d = parseFloat(parts[1]);
        if(!isNaN(d)) return setSlotOccupied(d);
      }
      return setSlotOccupied(null);
    }
  };
}

function scheduleReconnect(){
  if(reconnectTimer) return;
  reconnectTimer = setTimeout(()=>{ reconnectTimer = null; connectWS(); }, 1500);
}

// ----------- BUTTONS -----------
btnOpen.onclick = ()=>{ if(ws.readyState===1) ws.send("open_request"); };
btnClose.onclick = ()=>{ if(ws.readyState===1) ws.send("close_request"); };

btnAuto.onclick = ()=>{
  if(ws.readyState===1) ws.send("mode_auto");
  currentMode = 0;
  updateModeIndicator();
};

btnMaint.onclick = ()=>{
  if(ws.readyState===1) ws.send("mode_maint");
  currentMode = 1;
  updateModeIndicator();
};

btnSleep.onclick = ()=>{
  if(ws.readyState===1) ws.send("mode_sleep");
  currentMode = 2;
  updateModeIndicator();
};

// ----------- INIT -----------
setGate(false);
setSlotAvailable();
updateModeIndicator();
connectWS();

</script>
</body>
</html>
)rawliteral";
