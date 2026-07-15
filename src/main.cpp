#include <WiFi.h>
#include <WebServer.h>
#include "secret.h" 

WebServer server(80);

const int LED_PIN = 2;
unsigned long lastBlinkTime = 0;
int blinkDelay = 500; 
bool ledState = false;

String page() {
  return "<!DOCTYPE html>"
         "<html lang='en'>"
         "<head>"
         "<meta charset='UTF-8'>"
         "<meta name='viewport' content='width=device-width, initial-scale=1.0'>"
         "<style>"
         "body { font-family: 'Segoe UI', Arial, sans-serif; line-height: 1.6; color: #e0e0e0; background-color: #121212; margin: 0; padding: 20px; }"
         ".container { max-width: 900px; margin: 40px auto; display: grid; grid-template-columns: 1fr 1fr; gap: 20px; }"
         "@media (max-width: 768px) { .container { grid-template-columns: 1fr; } }"
         ".card { background: #1e1e1e; padding: 30px; border-radius: 12px; box-shadow: 0 4px 15px rgba(0, 0, 0, 0.5); border-left: 5px solid #f905d5; }"
         "h2 { color: #ff4d4d; margin-top: 0; margin-bottom: 20px; }"
         "button { background-color: #333; color: white; border: none; padding: 12px 20px; font-size: 1rem; border-radius: 6px; cursor: pointer; width: 100%; margin-bottom: 10px; }"
         "button:hover { background-color: #0056b3; }"
         "input[type='number'], input[type='password'] { width: 100%; padding: 10px; margin: 10px 0 20px 0; border: 1px solid #444; background: #2a2a2a; color: white; border-radius: 6px; box-sizing: border-box; font-size: 1rem; }"
         ".wifi-item { display: flex; justify-content: space-between; align-items: center; padding: 10px; margin: 8px 0; background: #2a2a2a; border-radius: 6px; border: 1px solid #444; cursor: pointer; }"
         ".wifi-item:hover { background: #3a3a3a; }"
         ".signal { font-weight: bold; color: #ffcc00; }"
         ".modal { display: none; position: fixed; top: 0; left: 0; width: 100%; height: 100%; background: rgba(0,0,0,0.7); justify-content: center; align-items: center; }"
         ".modal-content { background: #1e1e1e; padding: 25px; border-radius: 12px; width: 90%; max-width: 400px; border: 1px solid #444; }"
         ".display { font-size: 2rem; font-weight: bold; text-align: center; margin: 20px 0; color: #00ffcc; font-family: monospace; }"
         ".btn-group { display: flex; gap: 10px; }"
         "</style>"
         "</head>"
         "<body>"
         "<h1 style='text-align:center; color:#00ffff; margin-bottom:10px;'>ESP32 Control Dashboard</h1>"
         "<div class='container'>"
         "<div class='card'>"
         "<h2>Clock & Stopwatch</h2>"
         "<div class='display' id='clockDisplay'>00:00:00</div>"
         "<hr style='border:0; border-top:1px solid #333; margin:20px 0;'>"
         "<div class='display' id='stopwatchDisplay'>00:00:00.00</div>"
         "<div class='btn-group'>"
         "<button onclick='startStopwatch()'>Start</button>"
         "<button onclick='stopStopwatch()' style='background-color:#555;'>Stop</button>"
         "<button onclick='resetStopwatch()' style='background-color:#990000;'>Reset</button>"
         "</div>"
         "</div>"
         "<div class='card'>"
         "<h2>LED Delay Control</h2>"
         "<label for='delayInput'>Blink Delay (ms):</label>"
         "<input type='number' id='delayInput' min='0' placeholder='e.g., 500' value='" + String(blinkDelay) + "'>"
         "<button onclick='updateDelay()'>Update Delay</button>"
         "<p id='status-msg' style='margin-top:15px; font-weight:bold; color:#00ffff;'>Status: Running</p>"
         "<hr style='border:0; border-top:1px solid #333; margin:20px 0;'>"
         "<h2>Wi-Fi Networks</h2>"
         "<button onclick='scanWifi()'>Scan Networks</button>"
         "<div id='wifi-list' style='margin-top: 20px;'></div>"
         "</div>"
         "</div>"
         "<div id='wifiModal' class='modal'>"
         "<div class='modal-content'>"
         "<h3>Connect to <span id='modal-ssid'></span></h3>"
         "<input type='hidden' id='selected-ssid'>"
         "<label>Password:</label>"
         "<input type='password' id='wifi-pass' placeholder='Enter Wi-Fi password'>"
         "<button onclick='connectWifi()' style='margin-bottom:10px; background-color:#28a745;'>Connect</button>"
         "<button onclick='closeModal()' style='background-color:#555;'>Cancel</button>"
         "</div>"
         "</div>"
         "<script>"
         "setInterval(function() {"
         "  var d = new Date();"
         "  document.getElementById('clockDisplay').innerText = d.toLocaleTimeString();"
         "}, 1000);"
         "var startTime = 0; var elapsedTime = 0; var timerInterval;"
         "function startStopwatch() {"
         "  if (!timerInterval) {"
         "    startTime = Date.now() - elapsedTime;"
         "    timerInterval = setInterval(function() {"
         "      elapsedTime = Date.now() - startTime;"
         "      var diff = elapsedTime;"
         "      var ms = Math.floor((diff % 1000) / 10);"
         "      diff = Math.floor(diff / 1000);"
         "      var s = diff % 60; diff = Math.floor(diff / 60);"
         "      var m = diff % 60; var h = Math.floor(diff / 60);"
         "      document.getElementById('stopwatchDisplay').innerText = "
         "        (h < 10 ? '0' + h : h) + ':' + (m < 10 ? '0' + m : m) + ':' + "
         "        (s < 10 ? '0' + s : s) + '.' + (ms < 10 ? '0' + ms : ms);"
         "    }, 10);"
         "  }"
         "}"
         "function stopStopwatch() { clearInterval(timerInterval); timerInterval = null; }"
         "function resetStopwatch() { clearInterval(timerInterval); timerInterval = null; elapsedTime = 0; document.getElementById('stopwatchDisplay').innerText = '00:00:00.00'; }"
         "function scanWifi() {"
         "  var list = document.getElementById('wifi-list');"
         "  list.innerHTML = 'Scanning networks, please wait...';"
         "  fetch('/scan').then(function(res) { return res.json(); }).then(function(data) {"
         "    list.innerHTML = '';"
         "    if (data.length === 0) { list.innerHTML = 'No networks found.'; return; }"
         "    data.forEach(function(net) {"
         "      var div = document.createElement('div');"
         "      div.className = 'wifi-item';"
         "      div.onclick = function() { openModal(net.ssid); };"
         "      div.innerHTML = '<span>' + net.ssid + ' (' + net.crypto + ')</span><span class=\\'signal\\'>' + net.rssi + ' dBm</span>';"
         "      list.appendChild(div);"
         "    });"
         "  });"
         "}"
         "function updateDelay() {"
         "  var delayVal = document.getElementById('delayInput').value;"
         "  if (!delayVal || delayVal < 0) { alert('Please enter a valid positive number'); return; }"
         "  fetch('/set_delay?val=' + delayVal).then(function(res) { return res.text(); }).then(function(msg) {"
         "    document.getElementById('status-msg').innerText = 'Status: ' + msg;"
         "  });"
         "}"
         "function openModal(ssid) {"
         "  document.getElementById('selected-ssid').value = ssid;"
         "  document.getElementById('modal-ssid').innerText = ssid;"
         "  document.getElementById('wifiModal').style.display = 'flex';"
         "}"
         "function closeModal() {"
         "  document.getElementById('wifiModal').style.display = 'none';"
         "  document.getElementById('wifi-pass').value = '';"
         "}"
         "function connectWifi() {"
         "  var ssid = document.getElementById('selected-ssid').value;"
         "  var pass = document.getElementById('wifi-pass').value;"
         "  closeModal();"
         "  var list = document.getElementById('wifi-list');"
         "  list.innerHTML = 'Instructing ESP32 to connect to ' + ssid + '...';"
         "  fetch('/connect?ssid=' + encodeURIComponent(ssid) + '&pass=' + encodeURIComponent(pass))"
         "    .then(function(res) { return res.text(); })"
         "    .then(function(msg) { list.innerHTML = msg; });"
         "}"
         "</script>"
         "</body>"
         "</html>";
}

void handleRoot() {
  server.send(200, "text/html", page());
}

void handleScan() {
  int n = WiFi.scanNetworks();
  String json = "[";
  for (int i = 0; i < n; ++i) {
    if (i > 0) json += ",";
    json += "{";
    json += "\"ssid\":\"" + WiFi.SSID(i) + "\",";
    json += "\"rssi\":" + String(WiFi.RSSI(i)) + ",";
    String crypto;
    switch (WiFi.encryptionType(i)) {
      case WIFI_AUTH_OPEN: crypto = "Open"; break;
      case WIFI_AUTH_WEP: crypto = "WEP"; break;
      case WIFI_AUTH_WPA_PSK: crypto = "WPA"; break;
      case WIFI_AUTH_WPA2_PSK: crypto = "WPA2"; break;
      case WIFI_AUTH_WPA_WPA2_PSK: crypto = "WPA/WPA2"; break;
      default: crypto = "Unknown";
    }
    json += "\"crypto\":\"" + crypto + "\"";
    json += "}";
  }
  json += "]";
  WiFi.scanDelete();
  server.send(200, "application/json", json);
}

void handleSetDelay() {
  if (server.hasArg("val")) {
    blinkDelay = server.arg("val").toInt();
    server.send(200, "text/plain", "Delay updated to " + String(blinkDelay) + "ms");
  } else {
    server.send(400, "text/plain", "Bad Request");
  }
}

void handleConnect() {
  if (server.hasArg("ssid") && server.hasArg("pass")) {
    String ssid = server.arg("ssid");
    String pass = server.arg("pass");
    
    server.send(200, "text/plain", "Switching networks... Dashboard connection will drop.");
    
    Serial.println("Switching Station Wi-Fi connection to: " + ssid);
    WiFi.begin(ssid.c_str(), pass.c_str());
  } else {
    server.send(400, "text/plain", "Missing fields");
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);

  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID, PASSWORD);
  Serial.print("Connecting to Wi-Fi");

  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nConnected successfully!");
    Serial.print("Access dashboard at: http://");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\nConnection failed! Check your credentials in secret.h");
  }

  server.on("/", handleRoot);
  server.on("/scan", handleScan);
  server.on("/set_delay", handleSetDelay);
  server.on("/connect", handleConnect);
  
  server.begin();
  Serial.println("HTTP Server started");
}


void loop() {
  server.handleClient();

static bool connectedLastCheck = false;
if (WiFi.status() == WL_CONNECTED && !connectedLastCheck)
 {Serial.print("Connected to network! Access dashboard at http://");
  Serial.println(WiFi.localIP());connectedLastCheck = true;
} else if
 (WiFi.status() != WL_CONNECTED && connectedLastCheck) 
 {Serial.println("Disconnected from Wi-Fi.");
  connectedLastCheck = false;
}if (millis() - lastBlinkTime >= (unsigned long)blinkDelay) {lastBlinkTime = millis();
  ledState = !ledState;digitalWrite(LED_PIN, ledState);
}
}