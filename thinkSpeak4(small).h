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

WebServer server ( 80 ); 
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
         
     //Serial.print("[Leitura] ");
     //Serial.print(leitura);
     //Serial.println("A");
 
     return leitura;
}

void setup()
{  
    Serial.begin(115200);
    lastConnectionTime = 0; 
    FazConexaoWiFi();
   

 //Configura o GPIO0 como output, ou seja, como saÃ­da para podermos alterar o seu valor
  pinMode(0, OUTPUT);
  
  //Deixa o GPIO0 e o GPIO2 com saÃ­da LOW
  digitalWrite(0, HIGH);
   
   
  
  //Configurações do IP fixo. Você pode alterar conforme a sua rede
 IPAddress ip(192, 168, 1, 106);
 IPAddress gateway(192, 168, 1, 1);
 //IPAddress dnServer(192, 168, 1, 1);
 IPAddress subnet(255, 255, 255, 0);
 Serial.print("Configurando IP fixo para : ");
 Serial.println(ip);
 
 //Envia a configuração
 /** O ERRO ESTA AQUI **/
 //WiFi.config(ip, dnServer, gateway, subnet);
 
//Mostramos no Monitor Serial o ip com o qual o esp8266 se conectou para ver se está de acordo com o que configuramos
 Serial.println("");
 Serial.println("Connectado!");
 Serial.print("Local IP address = ");
    Serial.println(WiFi.localIP());
    Serial.print("Local Gateway address= ");
    Serial.println(WiFi.gatewayIP());
    Serial.print("Local DNS server address = ");
    Serial.println(WiFi.dnsIP(0));
    Serial.print("Local Subnet mask = ");
    Serial.println(WiFi.subnetMask());

  //Nesse outo caso quando houver uma requisição http do tipo GET no caminho http://192.168.1.106/leitura
  //(pode ser outro ip dependendo da sua configuração) a função getleitura será executada
  server.on("/leitura", HTTP_GET, getleitura);
  
  //Nesse outo caso quando houver uma requisição http do tipo GET no caminho http://192.168.1.106/releon
  //(pode ser outro ip dependendo da sua configuração) a função getreleon será executada
  server.on("/releon", HTTP_GET, getreleon);
  
   //Nesse outo caso quando houver uma requisição http do tipo GET no caminho http://192.168.1.106/releoff
  //(pode ser outro ip dependendo da sua configuração) a função getreleoff será executada
  server.on("/releoff", HTTP_GET, getreleoff);
  
    //Nesse outo caso quando houver uma requisição http do tipo GET no caminho http://192.168.1.106/tensao
  //(pode ser outro ip dependendo da sua configuração) a função gettensao será executada
  server.on("/tensao", HTTP_GET, gettensao);
  
      //Nesse outo caso quando houver uma requisição http do tipo GET no caminho http://192.168.1.106/pot
  //(pode ser outro ip dependendo da sua configuração) a função getpot será executada
  server.on("/pot", HTTP_GET, getpot);

        //Nesse outo caso quando houver uma requisição http do tipo GET no caminho http://192.168.1.106/kwh
  //(pode ser outro ip dependendo da sua configuração) a função getkwh será executada
  server.on("/kwh", HTTP_GET, getkwh);

  //Nesse caso quando houver uma requisição http do tipo GET no caminho http://192.168.1.106/monitor
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
 
 
 float last(void)
 {

   
    float LeituraLida;
    int LeituraTruncada;
    char FieldLeitura[11];
     
    LeituraLida = FazLeitura();
    LeituraTruncada = (int)LeituraLida;
  LeituraTruncada = 78U;
    
        sprintf(FieldLeitura,"field1=%d",LeituraTruncada);
    EnviaInformacoesThingspeak(FieldLeitura);
    Serial.print("Dado enviado"); //
    
 }
 
//loop principal
void loop()
{
    //Verifica se há alguma requisição de algum cliente
  server.handleClient(); 
  delay(8000);
  
  last(); 
  delay(500);
 
     
}

void gettensao()
{

  int leitura =78U;
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

  int leitura = 78U;
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

  int leitura = 78U;
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
//do tipo GET no caminho http://192.168.1.106/leitura (pode ser outro ip dependendo da sua configuração) 
void getleitura()
{
  //Fazemos a leitura 

  int leitura = 78U;
  float h = leitura;
  //Cria um json com os dados da umidade
  String json = "{\"leitura\":"+String(h)+"}";
  //Envia o json para o cliente com o código 200, que é o código quando a requisição foi realizada com sucesso
  server.send(200, "application/json", json);
}


//Função que definimos que será executada quando o cliente fizer uma requisição
//do tipo GET no caminho http://192.168.1.106/releon (pode ser outro ip dependendo da sua configuração) 
void getreleon()
{
 digitalWrite(0, LOW);
 //ligado = false;
   /* String json = "{\"releon\"}";
  //Envia o json para o cliente com o código 200, que é o código quando a requisição foi realizada com sucesso
  server.send(200, "application/json", json); */
   showMonitor();
}

//Função que definimos que será executada quando o cliente fizer uma requisição
//do tipo GET no caminho http://192.168.2.8/ humidity (pode ser outro ip dependendo da sua configuração) 
void getreleoff()
{
digitalWrite(0, HIGH);
//ligado = true;
 /*  String json = "{\"releoff\"}";
  //Envia o json para o cliente com o código 200, que é o código quando a requisição foi realizada com sucesso
  server.send(200, "application/json", json);*/
  showMonitor();
}

//Função que definimos que será executada quando o cliente fizer uma requisição
//do tipo GET no caminho http://192.168.1.106/monitor (pode ser outro ip dependendo da sua configuração) 
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
  "<h3>Data - Hora</h3>"
  
  "</header>"
  
  "<div id='menu-equip'> <h2>Banheiro - Lampada</h2>"
     
     "<div id='tela_lamp'>"
     "<script>AlteraRele1()</script>"
     "<div id='rele'></div><div id='estado' style='visibility: hidden;'>"
   "ligado"
   "</div>"
      "<div id='botao2'><a href='/releon'>ON</a></div>"
    "<div id='botao'><a href='/releoff'>OFF</a></div>"
    
  "<p id='leitura'>Corrente: </p>"
        "<p id='leitura2'>Tensao: </p>"
      "<p id='leitura3'>Potencia: </p>"
      "<p id='leitura4'>Consumo(Kw): </p>"
    "</div>"
  
  "<div id='grafico'> <h3>Grafico</h3>"
  "<iframe width='480' height='220' style='border: 1px solid #cccccc;' src='https://thingspeak.com/channels/880282/charts/1?bgcolor=%23ffffff&color=%23d62020&dynamic=true&results=60&type=line&update=15'></iframe>"
    "</div>"
    
    "<div id='consumo'> <h3>Consumo</h3><br>"
    "<p id='vparcial'>Valor Parcial: </p>"
    "<p id='valor1'>R$ </p><br>"
    "<p id='vprevisto'>Valor Previsto: </p>"
    "<p id='valor2'>R$ </p>"
    "</div>"

  "</div> "
  "<div id='botao_voltar'><a href='https://raw.githack.com/luizeletrica/tccv1/master/_Equip/Banheiro/Banheiro.html'>VOLTAR</a></div>"
  "<div id='botao_MENU'><a href='http://192.162.1.103'>MENU PRINCIPAL</a></div>"

"</div>"
  
  
"<footer id='rodape'>"
"<p>Copyright &copy; 2019 - by Luiz Filipe <br/>"
"<a href='https://www.facebook.com/luizfilipe.96' target='_blank'> Facebook</a> | Twitter</p>"
"</footer>"

"</body>"



"<script type='text/javascript'>"
"function AlteraRele1(){"
    "var estado = document.getElementById('estado').innerHTML;"
    "if(estado === '0'){"
        "document.getElementById('estado').innerHTML='1';"
        "document.getElementById('rele').innerHTML='<div class='+'rele1_ligado'+'></div>';"
    
    "} else {"
        "document.getElementById('estado').innerHTML='0';"
        "document.getElementById('rele').innerHTML='<div class='+'rele1_desligado'+'></div>';"
        
    "}"
"}"
"</script>"


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
