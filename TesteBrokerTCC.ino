#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "Stepper.h" 
#include <Servo.h>
#include <ESP8266HTTPClient.h>

Servo myservo;

int PassosPorVolta = 100; 
Stepper MotorStepper(PassosPorVolta, 2, 4, 3, 5); 

// Configuração da rede Wi-Fi
const char* ssid = "Carlos HZB";
const char* password = "12345678";

// Configuração do broker MQTT
const char* mqttServer = "broker.emqx.io";
const int mqttPort = 1883; // Porta MQTT padrão
const char* mqttTopic = "open"; // Tópico MQTT para escutar

// Configuração do LED
const int ledPin = D2; // Pino do LED no ESP8266
const int reedSwitchPin = D1; 
int openCorrect = 0;

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  pinMode(ledPin, OUTPUT);
  MotorStepper.setSpeed(300);
  myservo.attach(D3);
  pinMode(reedSwitchPin, INPUT_PULLUP);
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

void sendPostRequest() {
  HTTPClient http;

  std::unique_ptr<BearSSL::WiFiClientSecure>client(new BearSSL::WiFiClientSecure);

  client->setInsecure();
  
  // Endpoint para enviar o POST request
  const char* endpoint = "https://web-production-4b19.up.railway.app/alerts";

  // Body da requisição
  const char* requestBody = "{\"message\":\"A sala foi aberta sem a liberação da fechadura\",\"classroomId\":\"5696c38a-8b13-454b-ad7b-b3b40bd32730\"}";

  http.begin(*client, endpoint); // Inicia a conexão HTTP
  http.addHeader("Content-Type", "application/json"); // Define o tipo de conteúdo como JSON
  
  // Envia o POST request e obtém a resposta
  int httpResponseCode = http.POST(requestBody);

  if (httpResponseCode > 0) {
    Serial.print("POST request enviado com sucesso. Código de resposta: ");
    Serial.println(httpResponseCode);
  } else {
    Serial.print("Falha ao enviar o POST request. Código de erro: ");
    Serial.println(httpResponseCode);
  }

  http.end(); // Encerra a conexão HTTP
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Conectando ao broker MQTT...");

    if (client.connect("ESP8266Client2")) {
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
  int pos;
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
    openCorrect = 15;
    Serial.println("Deu certo");
    digitalWrite(ledPin, HIGH); // Acender o LED
      for (pos = 0; pos <= 90; pos += 1) { 
      myservo.write(pos);              
      delay(30);                       
    }
    //delay(2000);
    //for (pos = 90; pos >= 0; pos -= 1) { 
    //  myservo.write(pos);              
    //  delay(30);                       
    //}
    delay(1000); // Manter o LED aceso por 1 segundo
    digitalWrite(ledPin, LOW); // Desligar o LED
  }
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();


  if (digitalRead(reedSwitchPin) == LOW && openCorrect == 0) {
    sendPostRequest(); 
    delay(1000);

  }
}
