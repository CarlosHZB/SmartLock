#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// Configuração da rede Wi-Fi
const char* ssid = "carlos_net";
const char* password = "braganet";

// Configuração do broker MQTT
const char* mqttServer = "ec2-52-67-169-181.sa-east-1.compute.amazonaws.com";
const int mqttPort = 1883; // Porta MQTT padrão
const char* mqttTopic = "open"; // Tópico MQTT para escutar

// Configuração do LED
const int ledPin = D8; // Pino do LED no ESP8266

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  pinMode(ledPin, OUTPUT);
  Serial.begin(9600);
  setup_wifi();
  client.setServer(mqttServer, 1883);
  client.setCallback(callback);
}

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Conectando-se a ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("Conectado à rede Wi-Fi");
  Serial.println("Endereço IP: " + WiFi.localIP().toString());
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Conectando ao broker MQTT...");

    if (client.connect("ESP8266Client")) {
      Serial.println("Conectado");
      client.subscribe(mqttTopic);
    } else {
      Serial.print("Falha, rc=");
      Serial.print(client.state());
      Serial.println(" Retentando em 5 segundos");
      delay(5000);
    }
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Mensagem recebida no tópico: ");
  Serial.println(topic);

  String payloadStr = "";
  for (int i = 0; i < length; i++) {
    payloadStr += (char)payload[i];
  }
  Serial.println("Mensagem: " + payloadStr);

  // Comparar a mensagem com o ID esperado
  String expectedId = "7f7bf473-d8f2-4be0-af41-25648bc12b26"; // Substitua pelo ID esperado
  if (payloadStr == expectedId) {
    Serial.println("Deu certo, porra");
    digitalWrite(ledPin, HIGH); // Acender o LED
    delay(1000); // Manter o LED aceso por 1 segundo
    digitalWrite(ledPin, LOW); // Desligar o LED
  }
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}
