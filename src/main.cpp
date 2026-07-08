
#include <Wifi.h>
#include <WebServer.h>
WebServer Jess(80);

const char*SSID="ANITAB LAB";
const char*Pass="Akirachix@2011";
int led=2;

String page(){
 return
 "<!DOCTYPE html>" 
 "<html lang='en'>"
 "<head>"
 "<style>"
 "body {"
 "font-family: 'Segoe UI', Arial, sans-serif;"
 "line-height: 1.6;"
 "color: #333;"
 "max-width: 600px;"
 "margin: 40px auto;"
 "padding: 20px;"
 "background-color: #f4f7f6;"
 "}"
 ".card {"
 "background: white;"
  "padding: 30px;"
  "border-radius: 12px;"
  "box-shadow: 0 4px 15px rgba(0,0,0,0.05);"
  "border-left: 5px solid #0056b3;"
  "}"
  "h1 {"
  "color: #0056b3;"
  "margin-top: 0;"
  "}"
  "p {"
  "font-size: 1.1rem;"
  "margin-bottom: 0;"
  "}"
  "</style>"
"</head>"
"<body>"
"<div class='card'>"
"<a href='/me'>ON</a>"
"<h2> His Core Teachings</h2>"
"<p>At the center of His ministry was the radical message of the Kingdom of God. His teachings emphasized several key principles:</p>"
"<ul>"
"<li>Unconditional love, forgiveness, and compassion for outcasts and the marginalized.</li>"
"<li>An inward, ethical purity where thoughts and intentions matter as much as actions.</li>"
"<li>Humility, peace-making, and putting devotion to God above material pursuits.</li>"
"</div>"
"</body>"
"</html>";
  
};
void handleRoot(){
  Jess.send(200,"text/html", page());
}
void handleme(){
  Jess.sendHeader("location", "/homepage");
  Jess.send(303);
}
void setup()
{
  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID, Pass);
  Serial.begin(115200);
  while (
    WiFi.status()
    !=WL_CONNECTED)
    {
      Serial.print("connecting");
         delay(1000);
    }
    pinMode(led, OUTPUT);
    Serial.print("connected");
    Serial.println(WiFi.localIP());  
    Jess.on("/", handleRoot);//wait till you see the condition
    Jess.on("/me",handleme);
    Jess.begin();
}
void loop()
{
Jess.handleClient();
}

