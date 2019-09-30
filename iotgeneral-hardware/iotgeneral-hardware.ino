/*
    Develop by      : Fahmi Nurfadilah 
    Email           : fahmi.nurfadilah1412@gmail.com
    Updated by      : Vitradisa Pratama
    Email           : vitradisa@pptik.itb.ac.id
    Project         : IoT General
    Version         : 1.2
*/

#include <PubSubClient.h>
#include <ESP8266WiFi.h>

// Update these with values suitable for your network.

const char* ssid = "........";
const char* password = "........";
const char* mqtt_server = "rmq1.pptik.id";
const char* mqtt_user = "/iotgeneral:iotgeneral";
const char* mqtt_pass = "........";
const char* CL = "........";
const char* mqtt_pub_topic = "general";

char msg[100];
const int LED = 13;
WiFiClient espClient;
PubSubClient client(espClient);

byte mac[6];
String MACAddress;

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
  pinMode(LED, OUTPUT);
  //pinMode(input, INPUT);
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
  String jenis_iot = "........";
  String longitude = "0";
  String latitude = "0";
  String data_1 = "0";
  String data_2 = "0";
  String data_3 = "0";
  String data_4 = "0";
  String timestamp_device = "........";
  String kode_device = "........";
  Serial.print("Publish message: ");
  pubmsg = "#" + jenis_iot + "#" + longitude + "#" + latitude + "#" + data_1 + "#" + data_2 + "#" + data_3 + "#" + data_4 + "#" + timestamp_device + "#" + kode_device;
  Serial.println(pubmsg);
  client.publish(mqtt_pub_topic, pubmsg.c_str());
  delay(5000);
}
