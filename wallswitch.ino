// wallswitch

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

/* edit to your network:
const char* ssid = "..";
const char* password = "...";
const char* udp_recipient = "192.168.0.111";
const int udp_port = 8266;
*/
#include "creds.h"

ESP8266WebServer server(80);

#define INPUT_ON_PACKET  "\x00"
#define INPUT_OFF_PACKET "\xff"
#define INPUT_TOGGLE_PACKET "\x80"

// if defined, always toggle, otherwise send on/off
#define TOGGLE

#define OPTOCOUPLER_PIN D2

static int analog_value = 0;
static int last_input = 0;
static int input = 0;

static const int vdiv = (47.0 + 10.0) / 10;

void handleRoot() {
  String html = "<!DOCTYPE html>\n"
"<html>\n"
"<head>\n"
"<title>Wallswitch</title>\n"
"</head>\n"
"<meta charset=\"UTF-8\">\n"
"<body>\n"
"<h1>wallswitch</h1>\n"
"<p>Optocoupler: " + String(!input ? "ON" : "off") + "\n" +
"<p>Voltage: " + String(analog_value / 1024.0 * vdiv) + " V" +
"</body>\n"
"</html>\n";

  server.send(200, "text/html", html);
}

void handleNotFound() {
  String message = "Not found: " + server.uri();

  server.send(404, "text/plain", message);
}

void setup() {
  pinMode(BUILTIN_LED, OUTPUT);
  digitalWrite(BUILTIN_LED, 0);
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.println("\nWaiting to connect to Wi-Fi...");

  // Wait for connection
  int toggle = 1;
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
    digitalWrite(BUILTIN_LED, toggle);
    toggle = !toggle;
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  digitalWrite(BUILTIN_LED, 1);

  pinMode(OPTOCOUPLER_PIN, INPUT);

  if (MDNS.begin("wallswitch")) {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

static WiFiUDP udp;

void loop() {
  server.handleClient();

  if (millis() % 50 == 0) {
      analog_value = analogRead(A0);
  }

  input = digitalRead(OPTOCOUPLER_PIN);

  if (input != last_input) {
    Serial.println("Input changed: " + String(last_input) + " -> " + String(input));

    digitalWrite(BUILTIN_LED, 0);
    udp.beginPacket(udp_recipient, udp_port);
#ifdef TOGGLE
    udp.write(INPUT_TOGGLE_PACKET, sizeof(INPUT_TOGGLE_PACKET) - 1);
#else
    if (!input) { // active low
      udp.write(INPUT_ON_PACKET, sizeof(INPUT_ON_PACKET) - 1);
    } else {
      udp.write(INPUT_OFF_PACKET, sizeof(INPUT_OFF_PACKET) - 1);
    }
#endif
    udp.endPacket();
    digitalWrite(BUILTIN_LED, 1);
  }

  last_input = input;
}
