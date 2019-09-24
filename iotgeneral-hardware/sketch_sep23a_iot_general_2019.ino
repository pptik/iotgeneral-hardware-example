/*
    Develop by      : Fahmi Nurfadilah
    Email           : fahmi.nurfadilah1412@gmail.com
    Project         : Agriculture IOT
    Version         : 1.0
*/

#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <SoftwareSerial.h>

// Update these with values suitable for your network.

const char* ssid = "LSKK_AP2.1";
const char* password = "lskk@ppejuang";
const char* mqtt_server = "rmq1.pptik.id";
const char* mqtt_user = "/iotgeneral:iotgeneral";
const char* mqtt_pass = "iotgeneral2019";
const char* CL = "iot-general-01";

char msg[100];

WiFiClient espClient;
PubSubClient client(espClient);

byte mac[6];
String MACAddress;

int X;
int Y;
float TIME = 0;
float FREQUENCY = 0;
float WATER = 0;
float TOTAL = 0;
float LS = 0;
const int input = D4;
const int LED = 13;

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

String mac2String(byte ar[]) {
  String s;
  for (byte i = 0; i < 6; ++i)
  {
    char buf[3];
    sprintf(buf, "%2X", ar[i]);
    s += buf;
    if (i < 5) s += ':';
  }
  return s;
}

void printMACAddress() {
  WiFi.macAddress(mac);
  MACAddress = mac2String(mac);
  Serial.println(MACAddress);
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '1') {
    digitalWrite(LED, HIGH);   // Turn the LED on (Note that HIGH is the voltage level
  } else {
    digitalWrite(LED, LOW);  // Turn the LED off by making the voltage LOW
  }
}

void reconnect() {
  // Loop until we're reconnected
  printMACAddress();
  const char* CL;
  CL = MACAddress.c_str();
  Serial.println(CL);
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(CL, mqtt_user, mqtt_pass)) {
      Serial.println("connected");
      digitalWrite(LED, HIGH);
    } else {
      digitalWrite(LED, LOW);
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      ESP.restart();
      delay(5000);

    }
  }
}

void setup()
{
  pinMode(input, INPUT);
  pinMode(LED, OUTPUT);
  Serial.begin(115200);
  setup_wifi();
  printMACAddress();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}
void loop() {
  String pubmsg = "";
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  X = pulseIn(input, HIGH);
  Y = pulseIn(input, LOW);
  TIME = X + Y;
  FREQUENCY = 1000000 / TIME;
  WATER = FREQUENCY / 7.5;
  LS = WATER / 60;
  if (FREQUENCY >= 0)
  {
    if (isinf(FREQUENCY))
    {

      Serial.print("VOL. :0.00");
      Serial.println("TOTAL:");
      Serial.print( TOTAL);
      Serial.print(" L");
    }
    else
    {
      TOTAL = TOTAL + LS;
      Serial.println(FREQUENCY);
      Serial.print("VOL.: ");
      Serial.print(WATER);
      Serial.print(" L/M");
      Serial.println("TOTAL:");
      Serial.print( TOTAL);
      Serial.print(" L");
    }
  }
  StaticJsonBuffer<300> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  root["jenis_iot"] = ("WF01"); //device id Sensor Water Flow
  root["long"] = 0;
  root["lat"] = 0;
  root["data_1"] = (WATER);
  root["time_stamp"] = ("24-09-2019");
  root["kode_device"] = ("648490E332");
  Serial.print("Publish message: ");
  root.printTo(pubmsg);
  Serial.println(pubmsg);
  client.publish("general", pubmsg.c_str());
  delay(1000);
}
