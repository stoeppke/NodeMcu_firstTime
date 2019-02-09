/*
    This sketch demonstrates how to set up a simple HTTP-like server.
    The server will set a GPIO pin depending on the request
      http://server_ip/gpio/0 will set the GPIO2 low,
      http://server_ip/gpio/1 will set the GPIO2 high
    server_ip is the IP address of the ESP8266 module, will be
    printed to Serial when the module is connected.
*/

#include <ESP8266WiFi.h>
#define TriggerTime 400
// #define DEBUG

const char *ssid = "MP_Flur";
const char *password = "****";

// Create an instance of the server
// specify the port to listen on as an argument
WiFiServer server(80);

void setup()
{
  Serial.begin(115200);

  delay(10);

  // heardbeat LED setup
  pinMode(LED_BUILTIN, OUTPUT);

  // prepare GPIO2 - D4 output - ment as relai port
  pinMode(2, OUTPUT);
  digitalWrite(2, 0);

  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.hostname("JPRF_DoorBell");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  // Start the server
  server.begin();
  Serial.println("Server started");

  // Print the IP address
  Serial.println(WiFi.localIP());
}

void loop()
{
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client)
  {
    return;
  }

// Wait until the client sends some data
#ifdef DEBUG
  Serial.println("new client");
#endif
  while (!client.available())
  {
    delay(1);
  }

  // Read the first line of the request
  String req = client.readStringUntil('\r');
#ifdef DEBUG
  Serial.println(req);
#endif
  client.flush();

  // Match the request
  int val;
  if (req.indexOf("/gpio/0") != -1)
  {
    val = 0;
  }
  else if (req.indexOf("/gpio/1") != -1)
  {
    val = 1;
  }
  else if (req.indexOf("/gpio/2") != -1)
  {
    val = 2;
  }
  else
  {
#ifdef DEBUG
    Serial.println("invalid request");
#endif
    client.stop();
    return;
  }

  String s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html>\r\nGPIO is now ";

  if (val <= 1)
  {
    // Set GPIO2 according to the request
    digitalWrite(2, val);
    client.flush();

    // Prepare the response
    s += (val) ? "high" : "low";
    // Send the response to the client
  }
  else
  { //TODO
    digitalWrite(2, !digitalRead(2));
    delay(TriggerTime);
    digitalWrite(2, !digitalRead(2));

    client.flush();

    s += "trigered";
  }

  s += "</html>\n";
  client.print(s);
  delay(1);
#ifdef DEBUG
  Serial.println("Client disonnected");
#endif

  // The client will actually be disconnected
  // when the function returns and 'client' object is detroyed
  digitalWrite(LED_BUILTIN, 1);
#ifdef DEBUG
  Serial.println(255.0 / (-1.0 * WiFi.RSSI() - 16));
#endif
  // analogWrite(LED_BUILTIN, 255 / (-1 * WiFi.RSSI()));
}
