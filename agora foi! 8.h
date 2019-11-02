//Programa : Tomada monitorada wifi
//Autores: Luiz Filipe e Tiago Soares

/** Incluindo Bibliotecas **/
#include <WiFi.h> //Wi-Fi
#include <WiFiClient.h>
#include <WebServer.h>  //https://github.com/bbx10/WebServer_tng

#include <Wire.h> //LCD
#include <LiquidCrystal_I2C.h>  //LCD
#include "EmonLib.h" //INCLUSÃO DE BIBLIOTECA

#define CURRENT_CAL 1.5 //VALOR DE CALIBRAÇÃO (DEVE SER AJUSTADO EM PARALELO COM UM MULTÍMETRO MEDINDO A CORRENTE DA CARGA)
const int pinoSensor = 34; //PINO ANALÓGICO EM QUE O SENSOR ESTÁ CONECTADO

EnergyMonitor emon1; //CRIA UMA INSTÂNCIA

//Define o LCD (Pino I2C-SCL 22 E I2C-SDA 21)
LiquidCrystal_I2C lcd(0x3F,16,2);

//1) The first section initializes variables. Be sure to edit the read API key and write API key, channel numbers, and wireless network connection information.
#include <ThingSpeak.h>

//defines
#define SSID_REDE     "IOT_TREE_BLUE"  //coloque aqui o nome da rede que se deseja conectar
#define SENHA_REDE    "98550526"  //coloque aqui a senha da rede que se deseja conectar


// ThingSpeak information
char thingSpeakAddress[] = "api.thingspeak.com";
unsigned long channelID = 880282;
char* readAPIKey = "8843JULUCZO27FVH";
char* writeAPIKey = "1798Z00U2SZ8IFPV";
unsigned int dataFieldOne = 1;                            // Field to write temperature data
unsigned int dataFieldTwo = 2;  
unsigned int dataFieldThree = 3;  

unsigned long lastConnectionTime = 0;
long lastUpdateTime = 0; 
WiFiClient client;


//Criamos uma variável do tipo ESP32(ESP8266WebServer) que já possui funções
//que auxiliam na criação das rotas que o ESP32/ESP8266 vai responder
WebServer server ( 80 );

// definir porta de leitura do sensor de ldr
const int portaLDR = GPIO_NUM_35;

void setup()
{

  //pinMode(portaLDR, INPUT);
  //int leitura = analogRead(portaLDR);
  int leitura = 2;
  int tensao = 127;
  int potencia = (leitura*tensao);
  int kwatt = (potencia/1000);
  
       //Configura o GPIO0 como output, ou seja, como saÃ­da para podermos alterar o seu valor
  pinMode(19, OUTPUT);
  
  //Deixa o GPIO0 e o GPIO2 com saÃ­da LOW
  digitalWrite(19, HIGH);
  
  Serial.begin(115200);
  
   emon1.current(pinoSensor, CURRENT_CAL); //PASSA PARA A FUNÇÃO OS PARÂMETROS (PINO ANALÓGIO / VALOR DE CALIBRAÇÃO)
   //Iniciando o LCD
  lcd.init();
  // Print a message to the LCD.
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.begin(16, 2);
  lcd.clear();
  

  
  Serial.println("Start");
connectWiFi();

  //Instrução para o ESP8266 se conectar à rede. 
  //No nosso caso o nome da rede é TesteESP e a senha é 87654321. 
  //Você deve alterar com as informações da sua rede
  //WiFi.begin("IOT_TREE_BLUE", "98550526" );
  /* const char* ssid     = "yourSSID";
  const char* password = "yourPassword"; */

/*   //Feedback caso esteja usando o Monitor Serial 
  Serial.println("");
  Serial.print("Conectando");
  
  //Esperamos até que o módulo se conecte à rede
 while (WiFi.status() != WL_CONNECTED)
 {
   delay(500);
   Serial.print(".");
 }
  
 //Configurações do IP fixo. Você pode alterar conforme a sua rede
 IPAddress ip(192, 168, 1, 106);
 IPAddress gateway(192, 168, 1, 1);
 IPAddress subnet(255, 255, 255, 0);
 Serial.print("Configurando IP fixo para : ");
 Serial.println(ip);
 
 //Envia a configuração
 //WiFi.config(ip, gateway, subnet);
 
 //Mostramos no Monitor Serial o ip com o qual o esp8266 se conectou para ver se está de acordo com o que configuramos
 Serial.println("");
 Serial.println("Connectado!");
 Serial.print ("IP: ");
 Serial.println(WiFi.localIP()); */

  //Nesse outo caso quando houver uma requisição http do tipo GET no caminho http://192.168.0.106/leitura
  //(pode ser outro ip dependendo da sua configuração) a função getHumidity será executada
  server.on("/leitura", HTTP_GET, getleitura);
  
  //Nesse outo caso quando houver uma requisição http do tipo GET no caminho http://192.168.0.106/releon
  //(pode ser outro ip dependendo da sua configuração) a função getHumidity será executada
  server.on("/releon", HTTP_GET, getreleon);
  
   //Nesse outo caso quando houver uma requisição http do tipo GET no caminho http://192.168.2.8/humidity
  //(pode ser outro ip dependendo da sua configuração) a função getHumidity será executada
  server.on("/releoff", HTTP_GET, getreleoff);
  
    //Nesse outo caso quando houver uma requisição http do tipo GET no caminho http://192.168.0.106/leitura
  //(pode ser outro ip dependendo da sua configuração) a função getHumidity será executada
  server.on("/tensao", HTTP_GET, gettensao);
  
      //Nesse outo caso quando houver uma requisição http do tipo GET no caminho http://192.168.0.106/leitura
  //(pode ser outro ip dependendo da sua configuração) a função getHumidity será executada
  server.on("/pot", HTTP_GET, getpot);

        //Nesse outo caso quando houver uma requisição http do tipo GET no caminho http://192.168.0.106/leitura
  //(pode ser outro ip dependendo da sua configuração) a função getHumidity será executada
  server.on("/kwh", HTTP_GET, getkwh);

  //Nesse caso quando houver uma requisição http do tipo GET no caminho http://192.168.0.106/monitor
  //(pode ser outro ip dependendo da sua configuração) a função showMonitor será executada. 
  //Esta função retornará a página principal que mostrará os valores
  //da corrente e recarregará essas informações de tempos em tempos
  server.on("/monitor", HTTP_GET, showMonitor);
  
  //Aqui definimos qual função será executada caso o caminho que o cliente requisitou não tenha sido registrado
  server.onNotFound(onNotFound);
  
  //Inicializamos o server que criamos na porta 80
  server.begin();
  Serial.println("Servidor HTTP iniciado");
}



void loop()
{
emon1.calcVI(20,100); //FUNÇÃO DE CÁLCULO (20 SEMICICLOS / TEMPO LIMITE PARA FAZER A MEDIÇÃO)
  double currentDraw = emon1.Irms; //VARIÁVEL RECEBE O VALOR DE CORRENTE RMS OBTIDO

    Serial.print("Corrente medida: "); //IMPRIME O TEXTO NA SERIAL
    Serial.print(currentDraw); //IMPRIME NA SERIAL O VALOR DE CORRENTE MEDIDA
    Serial.println("A"); //IMPRIME O TEXTO NA SERIAL

  lcd.clear();
  lcd.setCursor(0,0); //Mostra no LCD
  lcd.print(currentDraw);
  lcd.setCursor(0,1);
  lcd.println("A");

  //int leitura = analogRead(portaLDR);
  //Serial.println("Leitura do Sensor de LDR:");
  //Serial.println(leitura);
  //int leitura = 0U;
   float leitura = (currentDraw);
  int tensao = 127;
  float potencia = ((leitura) * (tensao));
  int kwatt = ((potencia) / (1000));
  
  Serial.print("Tesão: "); //IMPRIME O TEXTO NA SERIAL
  Serial.println(127);
  
   Serial.print("Potencia calculada: "); //IMPRIME O TEXTO NA SERIAL
  Serial.println(potencia);
 //Verifica se há alguma requisição de algum cliente
  server.handleClient();
  
  
  //write2TSData( channelID , dataFieldOne , leitura  );
  write2TSData( channelID , dataFieldOne , leitura , dataFieldTwo , potencia , dataFieldThree , kwatt ); 

}

int connectWiFi(){
    
  Serial.println("Conectando-se à rede WiFi...");
    Serial.println();  
    delay(1000);
    WiFi.begin(SSID_REDE, SENHA_REDE);
 
    while (WiFi.status() != WL_CONNECTED) 
    {
        delay(500);
        Serial.print(".");
    }

    
    Serial.println( "Connected" );
  Serial.println("IP obtido: ");
    Serial.println(WiFi.localIP());
  
    ThingSpeak.begin( client );
  
}

// Use this function if you want to write multiple fields simultaneously.
int write2TSData( long TSChannel, unsigned int TSField1, float field1Data, unsigned int TSField2, long field2Data, unsigned int TSField3, long field3Data ){

  ThingSpeak.setField( TSField1, field1Data );
  ThingSpeak.setField( TSField2, field2Data );
  ThingSpeak.setField( TSField3, field3Data );
   
  int writeSuccess = ThingSpeak.writeFields( TSChannel, writeAPIKey );
  return writeSuccess;
}

/* // Use this function if you want to write multiple fields simultaneously.
int write2TSData( long TSChannel, unsigned int TSField1, float field1Data){

  ThingSpeak.setField( TSField1, field1Data );
    
  int writeSuccess = ThingSpeak.writeFields( TSChannel, writeAPIKey );
  return writeSuccess;
}
 */

void gettensao()
{
  int leitura = analogRead(portaLDR);
  int tensao = 0;
  if (leitura > 0)
  {
  tensao = 127;
  }
    float h = tensao;
    String json = "{\"tensao\":"+String(h)+"}";
  //Envia o json para o cliente com o código 200, que é o código quando a requisição foi realizada com sucesso
  server.send(200, "application/json", json);
    
}

void getpot()
{
  int leitura = analogRead(portaLDR);
  int tensao = 0;
  int pot = 0;
  if (leitura > 0)
  {
  tensao = 127;
  pot = ((leitura)*(tensao));
  }
    float h = pot;
    String json = "{\"pot\":"+String(h)+"}";
  //Envia o json para o cliente com o código 200, que é o código quando a requisição foi realizada com sucesso
  server.send(200, "application/json", json);
    
}

void getkwh()
{
  int leitura = analogRead(portaLDR);
  int tensao = 0;
  int pot = 0;
  int kwh = 0;
  if (leitura > 0)
  {
  tensao = 127;
  pot = ((leitura)*(tensao));
  kwh = ((pot)/1000);
  }
    float h = kwh;
    String json = "{\"kwh\":"+String(h)+"}";
  //Envia o json para o cliente com o código 200, que é o código quando a requisição foi realizada com sucesso
  server.send(200, "application/json", json);
    
}

//Função que definimos para ser chamada quando o caminho requisitado não foi registrado 
void onNotFound() 
{
//  server.send(404, "text/plain", "Not Found" );
Serial.print("Deu Ruim!");
}

//Função que definimos que será executada quando o cliente fizer uma requisição
//do tipo GET no caminho http://192.168.0.106/leitura (pode ser outro ip dependendo da sua configuração) 
void getleitura()
{
  //Fazemos a leitura 
  int leitura = analogRead(portaLDR);
  float h = leitura;
  //Cria um json com os dados da umidade
  String json = "{\"leitura\":"+String(h)+"}";
  //Envia o json para o cliente com o código 200, que é o código quando a requisição foi realizada com sucesso
  server.send(200, "application/json", json);
}


//Função que definimos que será executada quando o cliente fizer uma requisição
//do tipo GET no caminho http://192.168.0.106/releon (pode ser outro ip dependendo da sua configuração) 
void getreleon()
{
 digitalWrite(19, LOW);
   /* String json = "{\"releon\"}";
  //Envia o json para o cliente com o código 200, que é o código quando a requisição foi realizada com sucesso
  server.send(200, "application/json", json); */
   showMonitor();
}

//Função que definimos que será executada quando o cliente fizer uma requisição
//do tipo GET no caminho http://192.168.2.8/ humidity (pode ser outro ip dependendo da sua configuração) 
void getreleoff()
{
digitalWrite(19, HIGH);
 /*  String json = "{\"releoff\"}";
  //Envia o json para o cliente com o código 200, que é o código quando a requisição foi realizada com sucesso
  server.send(200, "application/json", json);*/
  showMonitor();
}


//Função que definimos que será executada quando o cliente fizer uma requisição
//do tipo GET no caminho http://192.168.2.8/monitor (pode ser outro ip dependendo da sua configuração) 
void showMonitor()
{
  String html =
    "<html>"
  "<head>"
    "<meta name='viewport' content='width=device-width, initial-scale=1, user-scalable=no'/>"
    "<title>Tomada Blue</title>"
   "<link rel='stylesheet' type='text/css' media='screen and (min-width: 0px)' href='https://raw.githack.com/luizeletrica/tccv1/master/_css/tcc/small.css' />"
  "<link rel='stylesheet' type='text/css' media='screen and (min-width: 800px)' href='https://raw.githubusercontent.com/luizeletrica/tccv1/master/_css/tcc/medium.css' />"
  "<link rel='stylesheet' type='text/css' media='screen and (min-width: 1000px)' href='https://raw.githack.com/luizeletrica/tccv1/master/_css/tcc/large.css' />"
  

"<body>"
"<div id='interface'>"
  
  "<header id='cabecalho'>"
  
  "<h1>TOMADA BLUE</h1></br>"
  "<h2>Seja bem-vindo USUARIO!</h2>"  
 
  
  "</header>"
  
   " <div id='menu-equip'> <h2>Banheiro - Lampada</h2>"
     
    " <div id='tela_lamp'>"
   
    "  <iframe width='220' height='200'  src='https://thingspeak.com/channels/880282/widgets/114885'></iframe>"
   "   <div id='botoes'>"
    "  <div id='botao2'><a href='/releon'>ON</a></div>"
   " <div id='botao'><a href='/releoff'>OFF</a></div>"
" </div>"
  "<div id='medcorrente'>"
    " <iframe width='440' height='250' src='https://thingspeak.com/channels/880282/widgets/114884'></iframe>"
   " </div>"
  "</div>"
  
 " <div id='grafico'> <h3>Grafico</h3>"
"<iframe width='450' height='260' style='border: 1px solid #cccccc;' src='https://thingspeak.com/channels/880282/charts/2?bgcolor=%23ffffff&color=%23d62020&dynamic=true&results=60&title=Potencia&type=line'></iframe>"
   
" </div>"
    
   " <div id='consumo'> <h3>Consumido R$</h3>"
 " <iframe width='450' height='120' style='border: 1px solid #cccccc;' src='https://thingspeak.com/channels/880282/widgets/114560'></iframe>"
    "</div>"

 " </div> "
 " <div id='botao_voltar'><a href='https://raw.githack.com/luizeletrica/tccv1/master/_Equip/Banheiro/Banheiro.html'>VOLTAR</a></div>"
  "<div id='botao_MENU'><a href='http://192.162.1.103'>MENU PRINCIPAL</a></div>"

"</div>"
  
  
"<footer id='rodape'>"
"<p>Copyright &copy; 2019 - by Luiz Filipe <br/>"
"<a href='https://www.facebook.com/luizfilipe.96' target='_blank'> Facebook</a> | Twitter</p>"
"</footer>"

"</body>"

 "<script type='text/javascript'>"
   "refresh();" 
   "setInterval(refresh, 100);"
   "function refresh()"
   "{"
     
     "refreshleitura();"
   "}"
   
   "function refreshleitura()"
    "{"
      "var xmlhttp = new XMLHttpRequest();"
      "xmlhttp.onreadystatechange = function() {"
        "if (xmlhttp.readyState == XMLHttpRequest.DONE && xmlhttp.status == 200){"
          "document.getElementById('leitura').innerHTML = 'Corrente: ' + JSON.parse(xmlhttp.responseText).leitura + 'A';"
        "}"
      "};"
      "xmlhttp.open('GET', 'http://192.168.1.2/leitura', true);"
    
      "xmlhttp.send();"
    "}"
  
  "</script>"
  
  "<script type='text/javascript'>"
   "refresh();" 
   "setInterval(refresh, 100);"
   "function refresh()"
   "{"
     
     "refreshtensao();"
   "}"
   
   "function refreshtensao()"
    "{"
      "var xmlhttp = new XMLHttpRequest();"
      "xmlhttp.onreadystatechange = function() {"
        "if (xmlhttp.readyState == XMLHttpRequest.DONE && xmlhttp.status == 200){"
          "document.getElementById('leitura2').innerHTML = 'Tensao: ' + JSON.parse(xmlhttp.responseText).tensao + 'V';"
        "}"
      "};"
      "xmlhttp.open('GET', 'http://192.168.1.2/tensao', true);"
    
      "xmlhttp.send();"
    "}"
  
  "</script>"
  
   "<script type='text/javascript'>"
   "refresh();" 
   "setInterval(refresh, 100);"
   "function refresh()"
   "{"
     
     "refreshpot();"
   "}"
   
   "function refreshpot()"
    "{"
      "var xmlhttp = new XMLHttpRequest();"
      "xmlhttp.onreadystatechange = function() {"
        "if (xmlhttp.readyState == XMLHttpRequest.DONE && xmlhttp.status == 200){"
          "document.getElementById('leitura3').innerHTML = 'Potencia: ' + JSON.parse(xmlhttp.responseText).pot + 'W';"
        "}"
      "};"
      "xmlhttp.open('GET', 'http://192.168.1.2/pot', true);"
    
      "xmlhttp.send();"
    "}"
  
  "</script>"
  
  "</script>"
  
   "<script type='text/javascript'>"
   "refresh();" 
   "setInterval(refresh, 100);"
   "function refresh()"
   "{"
     
     "refreshkwh();"
   "}"
   
   "function refreshkwh()"
    "{"
      "var xmlhttp = new XMLHttpRequest();"
      "xmlhttp.onreadystatechange = function() {"
        "if (xmlhttp.readyState == XMLHttpRequest.DONE && xmlhttp.status == 200){"
          "document.getElementById('leitura4').innerHTML = 'Consumo(Kw): ' + JSON.parse(xmlhttp.responseText).kwh + 'Kw/h';"
        "}"
      "};"
      "xmlhttp.open('GET', 'http://192.168.1.2/kwh', true);"
    
      "xmlhttp.send();"
    "}"
  
  "</script>"
  
  "</html>";
  
  //Envia o html para o cliente com o código 200, que é o código quando a requisição foi realizada com sucesso
  server.send(200, "text/html", html);
}