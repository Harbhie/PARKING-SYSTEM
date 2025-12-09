// index_html.h
// Place this file in your Arduino sketch (New Tab -> index_html.h)
// Serves the dashboard and connects to ws://192.168.4.1:81/
// Messages expected from ESP32 (plain text):
//   open
//   close
//   occupied:<distance>   e.g. occupied:5.23
//   available
//   buzzer_off

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
  --bg1:#021025;--bg2:#04102b;--panel:rgba(255,255,255,0.04);
  --accent:#56f0ff;--accent2:#8a57ff;--glass:rgba(255,255,255,0.03);
  --danger:#ff6b6b;--muted:rgba(215,238,247,0.7);
  font-family:Inter,system-ui,-apple-system,Segoe UI,Roboto,Arial;
}
*{box-sizing:border-box}
html,body{height:100%;margin:0;background:linear-gradient(160deg,var(--bg1),var(--bg2));color:#d7eef7}
.wrap{display:flex;gap:20px;align-items:flex-start;justify-content:center;padding:28px}
.card{background:linear-gradient(180deg,rgba(255,255,255,0.02),rgba(255,255,255,0.01));
  border:1px solid rgba(255,255,255,0.04);border-radius:12px;padding:16px;min-width:280px}
.controls{width:340px;display:flex;flex-direction:column;gap:12px}
.header{display:flex;gap:12px;align-items:center}
.logo{width:48px;height:48px;border-radius:10px;background:linear-gradient(135deg,var(--accent),var(--accent2));display:flex;align-items:center;justify-content:center;color:#002;font-weight:700}
h1{font-size:18px;margin:0}
.sub{font-size:12px;color:var(--muted)}
.panel{padding:12px;border-radius:10px;background:var(--glass);border:1px solid rgba(255,255,255,0.02)}
.gate-visual{height:140px;display:flex;align-items:center;justify-content:center;position:relative;overflow:hidden}
.gate-base{width:220px;height:10px;background:linear-gradient(90deg,#0b2a3f,#082033);border-radius:6px;position:relative}
.gate-arm{position:absolute;left:110px;bottom:40px;width:6px;height:120px;background:linear-gradient(180deg,var(--accent),var(--accent2));transform-origin:bottom center;border-radius:3px;transition:transform .28s}
.status-row{display:flex;align-items:center;justify-content:space-between;gap:8px}
.big-count{font-size:32px;font-weight:700}
.small{font-size:12px;color:var(--muted)}
.toggle{display:flex;gap:8px;align-items:center}
.btn{background:linear-gradient(90deg,var(--accent),var(--accent2));color:#002;border:none;padding:9px 12px;border-radius:10px;cursor:pointer;font-weight:700}
.btn.ghost{background:transparent;border:1px solid rgba(255,255,255,0.04);color:inherit}
.grid-card{flex:1;min-width:420px}
.grid{display:grid;grid-template-columns:1fr;gap:12px}
.slot{height:84px;border-radius:10px;background:linear-gradient(180deg,rgba(255,255,255,0.016),rgba(255,255,255,0.01));display:flex;flex-direction:column;align-items:center;justify-content:center;border:1px solid rgba(255,255,255,0.02)}
.slot.open{box-shadow:0 6px 20px rgba(86,240,255,0.06);border-color:rgba(86,240,255,0.18)}
.slot.occupied{background:linear-gradient(180deg,rgba(255,107,107,0.06),rgba(255,107,107,0.02));border-color:rgba(255,107,107,0.12)}
.footer{font-size:12px;color:var(--muted);text-align:center;margin-top:8px}
.kv{display:flex;align-items:center;gap:6px}
.kv .val{font-weight:700}
@media(max-width:880px){.wrap{flex-direction:column;align-items:stretch;padding:12px}.controls{width:100%}}
</style>
</head>
<body>
  <div class="wrap">
    <div class="controls card">
      <div class="header">
        <div class="logo">P</div>
        <div>
          <h1>NeonPark</h1>
          <div class="sub">Real-time parking dashboard</div>
        </div>
      </div>

      <div class="panel">
        <div class="gate-visual">
          <div class="gate-base"></div>
          <div id="gateArm" class="gate-arm"></div>
        </div>
        <div style="display:flex;justify-content:space-between;align-items:center;margin-top:10px">
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

      <div class="panel" style="margin-top:10px">
        <div style="display:flex;justify-content:space-between;align-items:center">
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

      <div class="footer">Connect to WiFi <strong>Parking-System</strong> (pw: 12345678) and open <strong>http://192.168.4.1</strong></div>
    </div>

    <div class="grid-card card">
      <div style="display:flex;justify-content:space-between;align-items:center;margin-bottom:12px">
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
/* Minimal, robust WS dashboard (expects plain-text messages) */
const WS_URL = "ws://192.168.4.1:81/"; // ws endpoint used by WebSocketsServer on ESP32

// UI references
const gateArm = document.getElementById('gateArm');
const gateStatus = document.getElementById('gateStatus');
const lastAction = document.getElementById('lastAction');
const slotStatus = document.getElementById('slotStatus');
const distanceRead = document.getElementById('distanceRead');
const slotStateText = document.getElementById('slotStateText');
const singleSlot = document.getElementById('singleSlot');
const totalSlots = document.getElementById('totalSlots');

const btnOpen = document.getElementById('btnOpen');
const btnClose = document.getElementById('btnClose');

let ws = null;
let reconnectTimer = null;

function setGate(open){
  gateArm.style.transform = open ? 'rotate(-65deg)' : 'rotate(0deg)';
  gateStatus.textContent = open ? 'Open' : 'Closed';
  lastAction.textContent = open ? 'Gate opened' : 'Gate closed';
}
function setSlotOccupied(distance){
  singleSlot.classList.remove('open'); singleSlot.classList.add('occupied');
  slotStateText.textContent = 'Occupied';
  slotStatus.textContent = 'Occupied';
  distanceRead.textContent = (typeof distance === 'number') ? distance.toFixed(1) + ' cm' : (distance||'—') + ' cm';
}
function setSlotAvailable(){
  singleSlot.classList.remove('occupied'); singleSlot.classList.add('open');
  slotStateText.textContent = 'Empty';
  slotStatus.textContent = 'Available';
  distanceRead.textContent = '— cm';
}
function setBuzzerOff(){ lastAction.textContent = 'Buzzer off'; }

// WebSocket connect with auto-reconnect
function connectWS(){
  if(ws && (ws.readyState === WebSocket.OPEN || ws.readyState === WebSocket.CONNECTING)) return;
  try{
    ws = new WebSocket(WS_URL);
  }catch(e){
    console.error("WS init failed", e);
    scheduleReconnect();
    return;
  }

  ws.onopen = () => {
    console.log("WS connected");
    lastAction.textContent = 'Connected';
    if(reconnectTimer){ clearTimeout(reconnectTimer); reconnectTimer = null; }
  };

  ws.onclose = () => {
    console.log("WS closed");
    lastAction.textContent = 'Disconnected';
    scheduleReconnect();
  };

  ws.onerror = (e) => {
    console.error("WS error", e);
    lastAction.textContent = 'WS Error';
  };

  ws.onmessage = (m) => {
    const msg = (typeof m.data === 'string') ? m.data.trim() : '';
    console.log("WS recv:", msg);

    // plain messages:
    // open
    if(msg === 'open'){ setGate(true); return; }
    // close
    if(msg === 'close'){ setGate(false); return; }
    // available
    if(msg === 'available'){ setSlotAvailable(); return; }
    // buzzer_off
    if(msg === 'buzzer_off'){ setBuzzerOff(); return; }
    // occupied:<distance>
    if(msg.startsWith('occupied')){
      // formats accepted: "occupied" or "occupied:12.34"
      const parts = msg.split(':');
      if(parts.length >= 2){
        const d = parseFloat(parts[1]);
        if(!isNaN(d)) { setSlotOccupied(d); return; }
      }
      // fallback if distance not present
      setSlotOccupied(null);
      return;
    }
    // distance-only message (optional): "distance:12.34"
    if(msg.startsWith('distance:')){
      const d = parseFloat(msg.split(':')[1]);
      if(!isNaN(d)) distanceRead.textContent = d.toFixed(1) + ' cm';
      return;
    }
  };
}

function scheduleReconnect(){
  if(reconnectTimer) return;
  reconnectTimer = setTimeout(()=>{ reconnectTimer = null; connectWS(); }, 1500);
}

// Optional: let web UI request open/close to ESP32 (sends simple text request)
// ESP32 must handle these incoming messages if you want manual control
btnOpen.addEventListener('click', ()=>{
  if(ws && ws.readyState === WebSocket.OPEN){ ws.send('open_request'); lastAction.textContent = 'Requested open'; }
  else lastAction.textContent = 'Not connected';
});
btnClose.addEventListener('click', ()=>{
  if(ws && ws.readyState === WebSocket.OPEN){ ws.send('close_request'); lastAction.textContent = 'Requested close'; }
  else lastAction.textContent = 'Not connected';
});

// initialize
setGate(false);
setSlotAvailable();
connectWS();
</script>
</body>
</html>
)rawliteral";
