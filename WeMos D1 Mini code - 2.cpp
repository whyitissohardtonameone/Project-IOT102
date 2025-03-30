#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

const char* ssid = "Your_SSID";       // Replace with your Wi-Fi SSID
const char* password = "Your_PASSWORD"; // Replace with your Wi-Fi password

ESP8266WebServer server(80);

String timeInput = "";
String alarmInput = "";

void setup() {
  Serial.begin(9600); // TX pin
  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi...");
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting...");
  }
  Serial.println("Connected!");

  // Homepage route
  server.on("/", HTTP_GET, []() {
    String html = R"=====(
      <!DOCTYPE html>
      <html>
      <body>
        <h2>Welcome to ESP8266 Control Panel</h2>
        <p><a href="/time">Set Time</a></p>
        <p><a href="/alarm">Set Alarm</a></p>
      </body>
      </html>
    )=====";
    server.send(200, "text/html", html);
  });

  // Route for time setup
  server.on("/time", HTTP_GET, []() {
    String html = R"=====(
      <!DOCTYPE html>
      <html>
      <body>
        <h2>Time Setup</h2>
        <form action="/set_time">
          Year (YYYY):<br>
          <input type="text" name="year"><br><br>
          Month (MM):<br>
          <input type="text" name="month"><br><br>
          Day (DD):<br>
          <input type="text" name="day"><br><br>
          Hour (HH):<br>
          <input type="text" name="hour"><br><br>
          Minute (MM):<br>
          <input type="text" name="minute"><br><br>
          Second (SS):<br>
          <input type="text" name="second"><br><br>
          <input type="submit" value="Set Time">
        </form>
      </body>
      </html>
    )=====";
    server.send(200, "text/html", html);
  });

  // Set time
  server.on("/set_time", HTTP_GET, []() {
    if (server.hasArg("year") && server.hasArg("month") && server.hasArg("day") &&
        server.hasArg("hour") && server.hasArg("minute") && server.hasArg("second")) {
      
      String year = server.arg("year");
      String month = server.arg("month");
      String day = server.arg("day");
      String hour = server.arg("hour");
      String minute = server.arg("minute");
      String second = server.arg("second");
      
      // Concatenate to form "YYYY-MM-DD HH:MM:SS"
      timeInput = year + "-" + month + "-" + day + " " + hour + ":" + minute + ":" + second;
      
      Serial.println(timeInput); // Send to TX pin
    }
    server.send(200, "text/plain", "Time Set Successfully!");
  });

  // Route for alarm setup
  server.on("/alarm", HTTP_GET, []() {
    String html = R"=====(
      <!DOCTYPE html>
      <html>
      <body>
        <h2>Alarm Setup</h2>
        <form action="/set_alarm">
          Alarm (HH:MM):<br>
          <input type="text" name="alarm"><br><br>
          <input type="submit" value="Set Alarm">
        </form>
      </body>
      </html>
    )=====";
    server.send(200, "text/html", html);
  });

  // Set alarm
  server.on("/set_alarm", HTTP_GET, []() {
    if (server.hasArg("alarm")) {
      alarmInput = server.arg("alarm");
      Serial.println("AL " + alarmInput); // Send to TX pin
    }
    server.send(200, "text/plain", "Alarm Set Successfully!");
  });

  server.begin();
  Serial.println("Web server started!");
}

void loop() {
  server.handleClient();

  // Clear inputs after processing
  if (timeInput != "") {
    Serial.println(timeInput);
    timeInput = ""; // Reset after sending
  }
  if (alarmInput != "") {
    Serial.println("AL " + alarmInput);
    alarmInput = ""; // Reset after sending
  }
}
