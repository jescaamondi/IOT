
#include <WiFi.h>
#include <WebServer.h>

WebServer Jess(80);

const char* SSID = "ANITAB LAB";
const char* Pass = "Akirachix@2011";
int led = 2;

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
         "body {"
         "font-family: 'Segoe UI', Arial, sans-serif;"
         "line-height: 1.6;"
         "color: #6e6868;"
         "background-color: #000000;"
         "margin: 0;"
         "padding: 20px;"
         "}"
         ".container {"
         "max-width: 900px;"
         "margin: 40px auto;"
         "display: grid;"
         "grid-template-columns: 1fr 1fr;"
         "gap: 20px;"
         "}"
         "@media (max-width: 768px) {"
         ".container {"
         "grid-template-columns: 1fr;"
         "}"
         "}"
         ".card {"
         "background: white;"
         "padding: 30px;"
         "border-radius: 12px;"
         "box-shadow: 0 4px 15px rgba(143, 22, 22, 0.05);"
         "border-left: 5px solid #f905d5;"
         "}"
         "h2 {"
         "color: #b30000;"
         "margin-top: 0;"
         "margin-bottom: 20px;"
         "}"
         "button {"
         "background-color: #1f2022;"
         "color: white;"
         "border: none;"
         "padding: 12px 20px;"
         "font-size: 1rem;"
         "border-radius: 6px;"
         "cursor: pointer;"
         "width: 100%;"
         "margin-bottom: 10px;"
         "}"
         "button:hover {"
         "background-color: #004085;"
         "}"
         "input[type='number'], input[type='password'] {"
         "width: 100%;"
         "padding: 10px;"
         "margin: 10px 0 20px 0;"
         "border: 1px solid #ccc;"
         "border-radius: 6px;"
         "box-sizing: border-box;"
         "font-size: 1rem;"
         "}"
         ".wifi-item {"
         "display: flex;"
         "justify-content: space-between;"
         "align-items: center;"
         "padding: 10px;"
         "margin: 8px 0;"
         "background: #4b545d;"
         "border-radius: 6px;"
         "border: 1px solid #b1bdca;"
         "cursor: pointer;"
         "}"
         ".wifi-item:hover {"
         "background: #e9ecef;"
         "}"
         ".signal {"
         "font-weight: bold;"
         "color: #5d590c;"
         "}"
         ".modal {"
         "display: none;"
         "position: fixed;"
         "top: 0;"
         "left: 0;"
         "width: 100%;"
         "height: 100%;"
         "background: rgba(0,0,0,0.5);"
         "justify-content: center;"
         "align-items: center;"
         "}"
         ".modal-content {"
         "background: white;"
         "padding: 25px;"
         "border-radius: 12px;"
         "width: 90%;"
         "max-width: 400px;"
         "}"
         ".display {"
         "font-size: 2rem;"
         "font-weight: bold;"
         "text-align: center;"
         "margin: 20px 0;"
         "color: #222;"
         "font-family: monospace;"
         "}"
         ".btn-group {"
         "display: flex;"
         "gap: 10px;"
         "}"
         "</style>"
         "</head>"
         "<body>"
         "<h1 style='text-align:center; color:#0056b3; margin-bottom:10px;'>ESP32 Control Dashboard</h1>"
         "<div class='container'>"
         "<div class='card'>"
         "<h2>Clock & Stopwatch</h2>"
         "<div class='display' id='clockDisplay'>00:00:00</div>"
         "<hr style='border:0; border-top:1px solid #eee; margin:20px 0;'>"
         "<div class='display' id='stopwatchDisplay'>00:00:00.00</div>"
         "<div class='btn-group'>"
         "<button onclick='startStopwatch()'>Start</button>"
         "<button onclick='stopStopwatch()' style='background-color:#6c757d;'>Stop</button>"
         "<button onclick='resetStopwatch()' style='background-color:#dc3545;'>Reset</button>"
         "</div>"
         "</div>"
         "<div class='card'>"
         "<h2>LED Delay Control</h2>"
         "<label for='delayInput'>Blink Delay (milliseconds):</label>"
         "<input type='number' id='delayInput' min='0' placeholder='e.g., 500' value='" + String(blinkDelay) + "'>"
         "<button onclick='updateDelay()'>Update Delay</button>"
         "<p id='status-msg' style='margin-top:15px; font-weight:bold; color:#0056b3;'>Status: Running</p>"
         "<hr style='border:0; border-top:1px solid #eee; margin:20px 0;'>"
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
         "<button onclick='connectWifi()' style='margin-bottom:10px;'>Connect</button>"
         "<button onclick='closeModal()' style='background-color:#6c757d;'>Cancel</button>"
         "</div>"
         "</div>"
         "<script>"
         "setInterval(function() {"
         "  var d = new Date();"
         "  document.getElementById('clockDisplay').innerText = d.toLocaleTimeString();"
         "}, 1000);"
         "var startTime = 0;"
         "var elapsedTime = 0;"
         "var timerInterval;"
         "function startStopwatch() {"
         "  if (!timerInterval) {"
         "    startTime = Date.now() - elapsedTime;"
         "    timerInterval = setInterval(function() {"
         "      elapsedTime = Date.now() - startTime;"
         "      var diff = elapsedTime;"
         "      var ms = Math.floor((diff % 1000) / 10);"
         "      diff = Math.floor(diff / 1000);"
         "      var s = diff % 60;"
         "      diff = Math.floor(diff / 60);"
         "      var m = diff % 60;"
         "      var h = Math.floor(diff / 60);"
         "      document.getElementById('stopwatchDisplay').innerText = "
         "        (h < 10 ? '0' + h : h) + ':' + "
         "        (m < 10 ? '0' + m : m) + ':' + "
         "        (s < 10 ? '0' + s : s) + '.' + "
         "        (ms < 10 ? '0' + ms : ms);"
         "    }, 10);"
         "  }"
         "}"
         "function stopStopwatch() {"
         "  clearInterval(timerInterval);"
         "  timerInterval = null;"
         "}"
         "function resetStopwatch() {"
         "  clearInterval(timerInterval);"
         "  timerInterval = null;"
         "  elapsedTime = 0;"
         "  document.getElementById('stopwatchDisplay').innerText = '00:00:00.00';"
         "}"
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
         "  document.getElementById('wifi-list').innerHTML = 'Attempting connection to ' + ssid + '... Check serial monitor.';"
         "  fetch('/connect?ssid=' + encodeURIComponent(ssid) + '&pass=' + encodeURIComponent(pass));"
         "}"
         "</script>"
         "</body>"
         "</html>";
}

void handleRoot() {
  Jess.send(200, "text/html", page());
}

void handleScan() {
  int n = WiFi.scanNetworks();
  int indices[n];
  for (int i = 0; i < n; i++) indices[i] = i;
  for (int i = 0; i < n - 1; i++) {
    for (int j = i + 1; j < n; j++) {
      if (WiFi.RSSI(indices[j]) > WiFi.RSSI(indices[i])) {
        int temp = indices[i];
        indices[i] = indices[j];
        indices[j] = temp;
      }
    }
  }
  String json = "[";
  for (int i = 0; i < n; ++i) {
    if (i > 0) json += ",";
    json += "{";
    json += "\"ssid\":\"" + WiFi.SSID(indices[i]) + "\",";
    json += "\"rssi\":" + String(WiFi.RSSI(indices[i])) + ",";
    json += "\"crypto\":\"" + String((WiFi.encryptionType(indices[i]) == WIFI_AUTH_OPEN) ? "Open" : "Secured") + "\"";
    json += "}";
  }
  json += "]";
  Jess.send(200, "application/json", json);
}

void handleSetDelay() {
  if (Jess.hasArg("val")) {
    int val = Jess.arg("val").toInt();
    blinkDelay = val;
if (blinkDelay == 0) {digitalWrite(led, LOW);
  Jess.send(200, "text/plain", "LED Turned OFF (Delay 0)");} 
  else 
  {Jess.send(200, "text/plain", "Delay updated to " + String(blinkDelay) + "ms");}}
   else {Jess.send(400, "text/plain", "Bad Request");}}



void handleConnect() {
  if (Jess.hasArg("ssid") &&
     Jess.hasArg("pass")) {String req_ssid = Jess.arg("ssid");

     String req_pass = Jess.arg("pass");
      Jess.send(200, "text/plain", "Connecting...");
      delay(100);WiFi.disconnect();
      WiFi.begin(req_ssid.c_str(), req_pass.c_str());
      Serial.print("Connecting to target network: ");
      Serial.println(req_ssid);} 
      else {Jess.send(400, "text/plain", "Missing Parameters");}}



      void setup() {
        
        WiFi.mode(WIFI_AP_STA);
        WiFi.begin(SSID, Pass);
        Serial.begin(115200);
        while (WiFi.status() != WL_CONNECTED) {Serial.print("connecting");
          delay(1000);}
          pinMode(led, OUTPUT);
          Serial.print("connected");
          Serial.println(WiFi.localIP());
          Jess.on("/", handleRoot);
          Jess.on("/scan", handleScan);
          Jess.on("/set_delay", handleSetDelay);
          Jess.on("/connect", handleConnect);
          Jess.begin();
        }
          
          
          void loop() {
            Jess.handleClient();
            if (blinkDelay > 0) {unsigned long currentMillis = millis();
              if (currentMillis - lastBlinkTime >= blinkDelay) {lastBlinkTime = currentMillis;
                ledState = !ledState;
                digitalWrite(led, ledState);
              }
            }
          }