#include <ESP8266WiFi.h> 
#include <PubSubClient.h>

#define pinLED1 2  //GP2 ESP-01
#define pinBotao1 12  //D6

//WiFi
const char* SSID = "ANTONIO";                // SSID / nome da rede WiFi que deseja se conectar
const char* PASSWORD = "antoniosilvawll";   // Senha da rede WiFi que deseja se conectar
WiFiClient wifiClient;                        
 
//MQTT Server
const char* BROKER_MQTT = "192.168.0.122"; //URL do broker MQTT que se deseja utilizar
int BROKER_PORT = 1883;                      // Porta do Broker MQTT

#define ID_MQTT  "BCI03"             //Informe um ID unico e seu. Caso sejam usados IDs repetidos a ultima conexão irá sobrepor a anterior. 
#define TOPIC_SUBSCRIBE "Leitura_analogica"   //Informe um Tópico único. Caso sejam usados tópicos em duplicidade, o último irá eliminar o anterior.
#define TOPIC_PUBLISH "Leitura_analogica"    //Informe um Tópico único. Caso sejam usados tópicos em duplicidade, o último irá eliminar o anterior.
PubSubClient MQTT(wifiClient);        // Instancia o Cliente MQTT passando o objeto espClient

//Declaração das Funções
void mantemConexoes();  //Garante que as conexoes com WiFi e MQTT Broker se mantenham ativas
void conectaWiFi();     //Faz conexão com WiFi
void conectaMQTT();     //Faz conexão com Broker MQTT
void recebePacote(char* topic, byte* payload, unsigned int length);
void enviaPacote();     //

void setup() {
  pinMode(2, OUTPUT);       
  pinMode(pinBotao1, INPUT_PULLUP);   

  Serial.begin(115200);

  conectaWiFi();
  MQTT.setServer(BROKER_MQTT, BROKER_PORT);  
  MQTT.setCallback(recebePacote); 
}

void loop() {
  mantemConexoes();
  enviaValores();
  MQTT.loop();
}

void mantemConexoes() {
    if (!MQTT.connected()) {
       conectaMQTT(); 
    }
    
    conectaWiFi(); //se não há conexão com o WiFI, a conexão é refeita
}

void conectaWiFi() {

  if (WiFi.status() == WL_CONNECTED) {
     return;
  }
        
  Serial.print("Conectando-se na rede: ");
  Serial.print(SSID);
  Serial.println("  Aguarde!");

  WiFi.begin(SSID, PASSWORD); // Conecta na rede WI-FI  
  while (WiFi.status() != WL_CONNECTED) {
      delay(100);
      Serial.print(".");
  }
  
  Serial.println();
  Serial.print("Conectado com sucesso, na rede: ");
  Serial.print(SSID);  
  Serial.print("  IP obtido: ");
  Serial.println(WiFi.localIP()); 
}

void conectaMQTT() { 
    while (!MQTT.connected()) {
        Serial.print("Conectando ao Broker MQTT: ");
        Serial.println(BROKER_MQTT);
        if (MQTT.connect(ID_MQTT)) {
            Serial.println("Conectado ao Broker com sucesso!");
            MQTT.subscribe(TOPIC_SUBSCRIBE);
        } 
        else {
            Serial.println("Noo foi possivel se conectar ao broker.");
            Serial.println("Nova tentatica de conexao em 10s");
            delay(10000);
        }
    }
}

void enviaValores() {
  const int analogInPin = A0;  // Entrada analogica do potenciometro
  const int pinled2 = 4;    //D1
  float Valor_sensor = 0;        // leitura do potenciometro
  int valor_anterior = 0;
  char Guarda_valor [4];
  char* Valor_convertido;

  // faz a leitura da entrada analogica:
  Valor_sensor = analogRead(analogInPin);
  if (abs(Valor_sensor - valor_anterior)>20){
  // dtostrf(a, b, c, d)
    // a = variável float; b = tamanho da string que será criada; c = número de dígitos após a vírgula; d = array que será guardado o resultado
    Valor_convertido = dtostrf(Valor_sensor, 1, 1, Guarda_valor);   
  
   MQTT.publish(TOPIC_PUBLISH, Valor_convertido, 1024);
   Serial.println("o Valor do sensor é:");
   Serial.println(Valor_convertido);
  }
}

void recebePacote(char* topic, byte* payload, unsigned int length) 
{
    String msg;

    //obtem a string do payload recebido
    for(int i = 0; i < length; i++) 
    {
       char c = (char)payload[i];
       msg += c;
    }
       
   Serial.println("o Valor do sensor é:");
   Serial.println(msg);
    
}
