/*************************************************************
 IOT 3000 - Oppgave 1, WasteScan 
 Av IOT Drømmeteamet
 
 Dette er programkoden til en WasteScan. En device som forteller hvordan 
 en gitt type avfall skal sorteres basert på input fra en mobil strekkodeleser

  Devicen er en NodeMCU med LCD-Skjerm
  
  Koden er en miks av forskjellige tutorials. De fleste er nevnt i 
  litteraturlisten i dokumentet Prosjektrapport-wastescan.pdf
  *************************************************************/
  
  
// Legger til bibliotek for for å håndtere tid
#include <time.h> 


// Legger til bibliotek for Wi-Fi 
#include <ESP8266WiFi.h> 


//Legger til biblioteker for å håndtere kommunikasjon med Azure IoT Hub. 
#include <AzureIoTHub.h> 
#include <AzureIoTUtility.h> 

//Legger til bibliotek for å håndtere MQTT-meldinger 
#include <AzureIoTProtocol_MQTT.h> 


//Legger til bibliotek for å håndtere JSON data og parsing av JSON data
#include <ArduinoJson.h> 

//Legger til bibliotek for å håndtere LCD-Skjerm
#include <LiquidCrystal.h> 

 
//Pinner til Displayet
const int RS = D2, EN = D3, d4 = D5, d5 = D6, d6 = D7, d7 = D8;    

//Det lages en instans av LiquidCrystal 
LiquidCrystal lcd(RS, EN, d4, d5, d6, d7); 

 
// Setter max størrelse på meldinger
#define MESSAGE_MAX_LEN 256 

 
// Nettverk id defineres 
#define IOT_CONFIG_WIFI_SSID            "***********"  

// Passord til nettverk defineres
#define IOT_CONFIG_WIFI_PASSWORD        "************" 


//Connection-string Azure IoT Hub defineres 
#define IOT_CONFIG_CONNECTION_STRING    "HostName=IoTHUBUSN.azure-devices.net;DeviceId=wastescan-node1;SharedAccessKey=4VQQwKnY/m4HuHIIwGuhUfivbrFstz/1mxgEJJB3WxE=" 

// Setter navn på devicen. Må samsvare med devicenavn i Azure IoT-hub 
#define DEVICE_ID                       "wastescan-node1" 

 
// Brukes i kommunikasjon med Azure IOT-hub 
const int Messagemaxlen = 256; 
const int interval = 2000; 
static IOTHUB_CLIENT_LL_HANDLE iotHubClientHandle; 

// brukes i forbindelse med Invoke Method on device
const char *onSuccess = "\"Successfully invoke device method\""; 
const char *notFound = "\"No method found\""; 

 
//Her settes 
void setup() { 
    
    Serial.begin(9600); 
	
    // Starter lcd skjerm og setter display til 16 x 2 rader 
    lcd.begin(16, 2); 
    
	// Skriver velkomstmelding i første rad 
    lcd.setCursor(0,0); 
    lcd.print("   WasteScan"); 
    
	// Skriver velkomstmelding i andre rad 
    lcd.setCursor(0,1); 
    lcd.print("  versjon 0.1"); 
    
	//Kjører funksjonene  initWiFi og initTime . Forklares videre i funksjonene selv
    initWiFi(); 
    initTime(); 
    
	//Lager/definerer en AureIOtHubConnectionString
    iotHubClientHandle = IoTHubClient_LL_CreateFromConnectionString(IOT_CONFIG_CONNECTION_STRING, MQTT_Protocol); 
    
	//Sjekker om vi har kommunikasjons med Azure tejenster     
    if (iotHubClientHandle == NULL) 
    { 
    Serial.println("Failed on IoTHubClient_CreateFromConnectionString."); 
    while (1); 
     } 
   
   else 
   { 
    Serial.println(" IoTHubClient_CreateFromConnectionString Ceated");  
    } 
	//APIen til å administrere IoT-hubene i Azure-abonnementet.
    IoTHubClient_LL_SetDeviceMethodCallback(iotHubClientHandle, deviceMethodCallback, NULL); 
    } 

void loop() { 

  // Keep-alive
   IoTHubClient_LL_DoWork(iotHubClientHandle); 
   delay(10);    
   
} 

// Kontakt og login mot trådløst nettverk 
void initWiFi() 
{ 
   Serial.print("Connecting to "); 
   Serial.println(IOT_CONFIG_WIFI_SSID); 
   WiFi.begin(IOT_CONFIG_WIFI_SSID,IOT_CONFIG_WIFI_PASSWORD); 

   while (WiFi.status() != WL_CONNECTED) 
   { 
    delay(500); 
    Serial.print("."); 
   }  
   
    if(WiFi.status() == WL_CONNECTED) 
    { 
    Serial.println(""); 
    Serial.println("WiFi connected"); 
    Serial.println("IP address: "); 
    Serial.println(WiFi.localIP()); 
    } 
} 

//setter opp tidshåndtering
void initTime() 
{ 
    time_t epochTime; 
    configTime(0, 0, "pool.ntp.org", "time.nist.gov"); 
 
    while (true) 
    { 
        epochTime = time(NULL); 

        if (epochTime == 0) 
        { 
            Serial.println("Fetching NTP epoch time failed! Waiting 2 seconds to retry."); 
            delay(2000); 
        } 

        else 
        { 
            Serial.printf("Fetched NTP epoch time is: %lu.\r\n", epochTime); 
            break; 
        } 
    } 
} 

// Funksjon som håndterer kallbare metoder på device.
int deviceMethodCallback(const char *methodName, const unsigned char *payload, size_t size, unsigned char **response, size_t *response_size, void *userContextCallback) 
{ 
    Serial.printf("Try to invoke method %s.\r\n", methodName); 
    const char *responseMessage = onSuccess; 
    int result = 200; 

    if (strcmp(methodName, "plastikk") == 0) 
    { 
        plastikk(); 
    } 

    else if (strcmp(methodName, "bio") == 0) 
    { 
        bio(); 
    } 

    else if (strcmp(methodName, "glassMetall") == 0) 
    { 
        glassMetall(); 
    } 
    else if (strcmp(methodName, "restavfall") == 0) 
    { 
        restavfall(); 
    } 
	
    else if (strcmp(methodName, "papir") == 0) 
    { 
        papir(); 
    } 

    else 
    { 
        Serial.printf("No method %s found.\r\n", methodName); 
        responseMessage = notFound; 
        result = 404; 
    } 

    *response_size = strlen(responseMessage); 
    *response = (unsigned char *)malloc(*response_size); 
    strncpy((char *)(*response), responseMessage, *response_size); 
    return result; 

} 

//Funksjon som skriver avfallsttype til LCD display når det blir kalt på fra Azure Logic App 
void plastikk() 
{ 
    lcd.setCursor(0,0); 
    lcd.print("  SORTERES SOM  "); 
    lcd.setCursor(0,1); 
    lcd.print("    plastikk    "); 
} 
 
//Funksjon som skriver avfallsttype til LCD display når det blir kalt på fra Azure Logic App 
void bio() 
{ 
    lcd.setCursor(0,0); 
    lcd.print("  SORTERES SOM  "); 
    lcd.setCursor(0,1); 
    lcd.print("    matavfall   "); 
} 

 //Funksjon som skriver avfallsttype til LCD display når det blir kalt på fra Azure Logic App 
void glassMetall() 
{ 
    lcd.setCursor(0,0); 
    lcd.print("  SORTERES SOM  "); 
    lcd.setCursor(0,1); 
    lcd.print("glass og metall "); 
} 

//Funksjon som skriver avfallsttype til LCD display når det blir kalt på fra Azure Logic App 
void restavfall() 
{ 
    lcd.setCursor(0,0); 
    lcd.print("  SORTERES SOM  "); 
    lcd.setCursor(0,1); 
    lcd.print("   restavfall   "); 
} 
 
//Funksjon som skriver avfallsttype til LCD display når det blir kalt på fra Azure Logic App 
void papir() 
{ 
    lcd.setCursor(0,0); 
    lcd.print("  SORTERES SOM  ");
    lcd.setCursor(0,1); 
    lcd.print(" papp og papir  "); 
} 
