#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>


const char* SSID = "FIAP-IOT";
const char* PASSWORD = "F!@p25.IOT";
const char* MQTT_BROKER = "54.227.182.115"; 

const int MQTT_PORT = 1883;
const char* MQTT_TOPIC_TROPA = "vinheria/Tropa/Tamandua";

#define DHTPIN 4     
#define DHTTYPE DHT11
#define LDRPIN 36   


DHT dht(DHTPIN, DHTTYPE);
WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  Serial.begin(115200);
  dht.begin();
  connectWifi();
  client.setServer(MQTT_BROKER, MQTT_PORT);
}

void loop() {
  if (!client.connected()) {
    reconnectMQTT();
  }
  client.loop();


  delay(10000); 

  float h = dht.readHumidity();
  float t = dht.readTemperature();
  int l = analogRead(LDRPIN);

  if (isnan(h) || isnan(t)) {
    Serial.println("Falha ao ler DHT!");
    return;
  }


  String jsonPayload = "{";
  jsonPayload += "\"temp\": " + String(t) + ",";
  jsonPayload += "\"hum\": " + String(h) + ",";
  jsonPayload += "\"light\": " + String(l);
  jsonPayload += "}";

  Serial.print("Enviando dados: ");
  Serial.println(jsonPayload);

  
  client.publish(MQTT_TOPIC_TROPA, jsonPayload.c_str());
}

void connectWifi() {
  delay(10);
  Serial.println();
  Serial.print("Conectando em ");
  Serial.println(SSID);
  WiFi.begin(SSID, PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi conectado!");
  Serial.println("IP: ");
  Serial.println(WiFi.localIP());
}

void reconnectMQTT() {
  while (!client.connected()) {
    Serial.print("Tentando conexão MQTT...");
    if (client.connect("ESP32_Vinheria")) {
      Serial.println("conectado!");
    } else {
      Serial.print("falhou, rc=");
      Serial.print(client.state());
      Serial.println(" tentando novamente em 5s");
      delay(5000);
    }
  }
}