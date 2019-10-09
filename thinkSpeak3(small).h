#include <WiFi.h>
#include <WebServer.h>
#include "esp_deep_sleep.h" //Library needed for ESP32 Sleep Functions
 
//defines
#define SSID_REDE     "IOT_TREE_BLUE"  //coloque aqui o nome da rede que se deseja conectar
#define SENHA_REDE    "98550526"  //coloque aqui a senha da rede que se deseja conectar
#define INTERVALO_ENVIO_THINGSPEAK  3000  //intervalo entre envios de dados ao ThingSpeak (em ms)
 
//constantes e variáveis globais
char EnderecoAPIThingSpeak[] = "api.thingspeak.com";
String ChaveEscritaThingSpeak = "1798Z00U2SZ8IFPV";
long lastConnectionTime; 
WiFiClient client;
 
//prototypes
void EnviaInformacoesThingspeak(String StringDados);
void FazConexaoWiFi(void);
float FazLeitura(void);
float last(void);
 
/*
 * Implementações
 */
 
//Função: envia informações ao ThingSpeak
//Parâmetros: String com a  informação a ser enviada
//Retorno: nenhum
void EnviaInformacoesThingspeak(String StringDados)
{
    if (client.connect(EnderecoAPIThingSpeak, 80))
    {         
        //faz a requisição HTTP ao ThingSpeak
        client.print("POST /update HTTP/1.1\n");
        client.print("Host: api.thingspeak.com\n");
        client.print("Connection: close\n");
        client.print("X-THINGSPEAKAPIKEY: "+ChaveEscritaThingSpeak+"\n");
        client.print("Content-Type: application/x-www-form-urlencoded\n");
        client.print("Content-Length: ");
        client.print(StringDados.length());
        client.print("\n\n");
        client.print(StringDados);
   
        lastConnectionTime = millis();
        Serial.println("- Informações enviadas ao ThingSpeak!");
     }   
}
 
//Função: faz a conexão WiFI
//Parâmetros: nenhum
//Retorno: nenhum
void FazConexaoWiFi(void)
{
    client.stop();
    Serial.println("Conectando-se à rede WiFi...");
    Serial.println();  
    delay(1000);
    WiFi.begin(SSID_REDE, SENHA_REDE);
 
    while (WiFi.status() != WL_CONNECTED) 
    {
        delay(500);
        Serial.print(".");
    }
 
    Serial.println("");
    Serial.println("WiFi connectado com sucesso!");  
    Serial.println("IP obtido: ");
    Serial.println(WiFi.localIP());
 
    delay(1000);
}
 
//Função: faz a leitura do nível de umidade
//Parâmetros: nenhum
//Retorno: umidade percentual (0-100)
//Observação: o ADC do NodeMCU permite até, no máximo, 3.3V. Dessa forma,
//            para 3.3V, obtem-se (empiricamente) 978 como leitura de ADC
float FazLeitura(void)
{
    const int portaLDR = GPIO_NUM_35;
	int leitura = analogRead(portaLDR);
         
     Serial.print("[Leitura] ");
     Serial.print(leitura);
     Serial.println("A");
 
     return leitura;
}
void setup()
{  
    Serial.begin(115200);
    lastConnectionTime = 0; 
    FazConexaoWiFi();
    Serial.println("Planta IoT com ESP8266 NodeMCU");
}
 
 
 float last(void)
 {
	 
	  float LeituraLida;
    int LeituraTruncada;
    char FieldLeitura[11];
     
    LeituraLida = FazLeitura();
    LeituraTruncada = (int)LeituraLida;
    
        sprintf(FieldLeitura,"field1=%d",LeituraTruncada);
        EnviaInformacoesThingspeak(FieldLeitura);
    
 }
//loop principal
void loop()
{
   
  last(); 
     
}