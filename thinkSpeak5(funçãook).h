//ESP8266
//1) The first section initializes variables. Be sure to edit the read API key and write API key, channel numbers, and wireless network connection information.
#include <ThingSpeak.h>
#include <WiFi.h>
#include <WebServer.h>

// Network parameters
const char* ssid     = "IOT_TREE_BLUE";
const char* password = "98550526";

//defines
#define SSID_REDE     "IOT_TREE_BLUE"  //coloque aqui o nome da rede que se deseja conectar
#define SENHA_REDE    "98550526"  //coloque aqui a senha da rede que se deseja conectar


// ThingSpeak information
char thingSpeakAddress[] = "api.thingspeak.com";
unsigned long channelID = 880282;
char* readAPIKey = "8843JULUCZO27FVH";
char* writeAPIKey = "1798Z00U2SZ8IFPV";
//const unsigned long postingInterval = 120L * 1000L;
unsigned int dataFieldOne = 1;                            // Field to write temperature data

//unsigned int aField = 6;                             //Field to hold first constant of the thermistor calibration                

// Global variables
// These constants are device specific.  You need to get them from the manufacturer or determine them yourself.
//float aConst = 2.25E-02;   


unsigned long lastConnectionTime = 0;
long lastUpdateTime = 0; 
WiFiClient client;

//2) In the setup function, start the serial monitor and read the calibration constants.
void setup() {

Serial.begin(9600);
Serial.println("Start");
connectWiFi();

// Read the constants at startup.
//aConst = readTSData( channelID, aField );
int leitura = 120U;

}

//3) The main loop continuously checks to see how long it has been since the temperature was read. If the set time period has elapsed, the device is read, the temperature is calculated, and the output is written to your channel.

void loop() {
    
  
 // Update only if the posting time is exceeded
    //if (millis() - lastUpdateTime >=  postingInterval) {
        
       // float fahrenheitTemperature, celsiusTemperature;
        
       // lastUpdateTime = millis();
        
        //float readValue = analogRead(A0);
		
        //float logR = log( 10000 * ( 1024 / readValue - 1 ));                 // Separate the calculation for simplicity and debugging
        
		//const int portaLDR = GPIO_NUM_35;
	//int leitura = analogRead(portaLDR);
		int leitura = 120U;
        //celsiusTemperature = 1 / ( aConst + logR + pow(logR,3) ) - 273.15;   // Calculate the temperature in Celsius
       // fahrenheitTemperature = celsiusTemperature * 9 / 5 + 32;
        Serial.println("ADC =  " + String( leitura ));
        write2TSData( channelID , dataFieldOne , leitura  );      // Write the temperature in F, C, and time since starting.
    //}
}


//4) Connect your device to the wireless network using the connectWiFi function.

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
   /*  while (WiFi.status() != WL_CONNECTED) {
      
	      WiFi.begin(SSID_REDE, SENHA_REDE);

        //WiFi.begin( ssid, password );
        delay(2500);
        Serial.println("Connecting to WiFi");
    } */
    
    Serial.println( "Connected" );
	Serial.println("IP obtido: ");
    Serial.println(WiFi.localIP());
	
    ThingSpeak.begin( client );
	
}

//5. Read data from a single field on a channel with readTSData. You can write a single value to ThingSpeak using writeTSDataand and write multiple values simultaneously with write2TSdata.

/* float readTSData( long TSChannel,unsigned int TSField ){
    
  float data =  ThingSpeak.readFloatField( TSChannel, TSField, readAPIKey );
  Serial.println( " Data read from ThingSpeak: " + String( data, 9 ) );
  return data;
  
} */


// Use this function if you want to write multiple fields simultaneously.
int write2TSData( long TSChannel, unsigned int TSField1, float field1Data){

  ThingSpeak.setField( TSField1, field1Data );
 
   
  int writeSuccess = ThingSpeak.writeFields( TSChannel, writeAPIKey );
  return writeSuccess;
}
