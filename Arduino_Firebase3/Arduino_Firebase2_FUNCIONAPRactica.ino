
/**
 * Created by K. Suwatchai (Mobizt)
 * 
 * Email: k_suwatchai@hotmail.com
 * 
 * Github: https://github.com/mobizt/Firebase-ESP8266
 * 
 * Copyright (c) 2022 mobizt
 *
*/
#include <DHT.h>
#define DHTPIN 0    // Pin number to which DHT11 data pin is connected
#define DHTTYPE DHT11 // DHT type (DHT11 or DHT22)
//PARA QUE EL SENSOR HAGA BIEN LAS MEDIDAS TENEMOS QUE DESCONECTAR Y CONECTAR EL PIN DE ALIMENTACION DEL SENSOR, SI LO TENEMOS INICIALMENTE EN 3.3V PONERLO EN VIN Y LUEGO REGRESARLO 
// INICIALMENTE APARECERA Nan PERO AL HACER ESTO YA DARA MEDICIONES

DHT dht(DHTPIN, DHTTYPE); // Initialize DHT sensor

#if defined(ESP32)
#include <WiFi.h>
#include <FirebaseESP32.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>
#endif

//Provide the token generation process info.
#include <addons/TokenHelper.h>

//Provide the RTDB payload printing info and other helper functions.
#include <addons/RTDBHelper.h>

/* 1. Define the WiFi credentials */
#define WIFI_SSID "Diego Phone"
#define WIFI_PASSWORD "lingruyto"

//For the following credentials, see examples/Authentications/SignInAsUser/EmailPassword/EmailPassword.ino

/* 2. Define the API Key */
#define API_KEY "AIzaSyCW-hwWZUK6xrrTm7BH420t9gX4eVjG6Xc"

/* 3. Define the RTDB URL */
#define DATABASE_URL "https://proyecto-prueb-944a0-default-rtdb.firebaseio.com/" //<databaseName>.firebaseio.com or <databaseName>.<region>.firebasedatabase.app

/* 4. Define the user Email and password that alreadey registerd or added in your project */
#define USER_EMAIL "espuser@mail.com"
#define USER_PASSWORD "esppassword"

/* Librerias y declaracion de sensor DHT11*/



//Define Firebase Data object
FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;
unsigned long sendDataPrevMillis1 = 0;
unsigned long count = 0;
unsigned long count2 = 0;
unsigned long temperatura=0;
String sensorValue;
String sensorValue2;
String sensorValue3;
String baseData = "/TempsRandom/lectura";
String baseData2 = "/TempsSensor/lectura";
String baseData3 = "/HumSensor/lectura";

void setup()
{
  Serial.begin(115200);
  dht.begin();              // Start DHT sensor


  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  Serial.printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);

  /* Assign the api key (required) */
  config.api_key = API_KEY;

  /* Assign the user sign in credentials */
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;

  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;

  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h

  //Or use legacy authenticate method
  //config.database_url = DATABASE_URL;
  //config.signer.tokens.legacy_token = "<database secret>";

  //To connect without auth in Test Mode, see Authentications/TestMode/TestMode.ino

  //////////////////////////////////////////////////////////////////////////////////////////////
  //Please make sure the device free Heap is not lower than 80 k for ESP32 and 10 k for ESP8266,
  //otherwise the SSL connection will fail.
  //////////////////////////////////////////////////////////////////////////////////////////////

  Firebase.begin(&config, &auth);

  //Comment or pass false value when WiFi reconnection will control by your code or third party library
  Firebase.reconnectWiFi(true);

  Firebase.setDoubleDigits(5);

  /** Timeout options.

  //WiFi reconnect timeout (interval) in ms (10 sec - 5 min) when WiFi disconnected.
  config.timeout.wifiReconnect = 10 * 1000;

  //Socket connection and SSL handshake timeout in ms (1 sec - 1 min).
  config.timeout.socketConnection = 10 * 1000;

  //Server response read timeout in ms (1 sec - 1 min).
  config.timeout.serverResponse = 10 * 1000;

  //RTDB Stream keep-alive timeout in ms (20 sec - 2 min) when no server's keep-alive event data received.
  config.timeout.rtdbKeepAlive = 45 * 1000;

  //RTDB Stream reconnect timeout (interval) in ms (1 sec - 1 min) when RTDB Stream closed and want to resume.
  config.timeout.rtdbStreamReconnect = 1 * 1000;

  //RTDB Stream error notification timeout (interval) in ms (3 sec - 30 sec). It determines how often the readStream
  //will return false (error) when it called repeatedly in loop.
  config.timeout.rtdbStreamError = 3 * 1000;

  Note:
  The function that starting the new TCP session i.e. first time server connection or previous session was closed, the function won't exit until the 
  time of config.timeout.socketConnection.

  You can also set the TCP data sending retry with
  config.tcp_data_sending_retry = 1;

  */
}

void loop()
{

  if (Firebase.ready() && (millis() - sendDataPrevMillis > 2000 || sendDataPrevMillis == 0))
  {
    //cuantos milisegundos representa una hora
    sendDataPrevMillis = millis();


    //Se guarda variable base data y se muestra junto con el contador
    sensorValue=baseData+count;


    temperatura=random(20,30);
    Serial.printf("Set int... %s",Firebase.setInt(fbdo,sensorValue,temperatura)?"ok":fbdo.errorReason().c_str());
    count++;
    
  }
    if (Firebase.ready() && (millis() - sendDataPrevMillis1 >  1000 || sendDataPrevMillis1 == 0))
  {
  float temperature = dht.readTemperature(); // Read temperature value
  float humidity = dht.readHumidity();       // Read humidity value
  //cuantos milisegundos representa una hora
    sendDataPrevMillis1 = millis();


    //Se guarda variable base data y se muestra junto con el contador
    sensorValue2=baseData2+count2;
    sensorValue3=baseData3+count2;
  Serial.print("Temperatura: ");
  Serial.print(temperature);
  Serial.print(" °C, Humedad: ");
  Serial.print(humidity);


    Serial.printf("Set int... %s",Firebase.setInt(fbdo,sensorValue2,temperature)?"ok":fbdo.errorReason().c_str());
      Serial.println(".");
      Serial.printf("Set int... %s",Firebase.setInt(fbdo,sensorValue3,humidity)?"ok":fbdo.errorReason().c_str());
    count2++;
    
  }
}

/// PLEASE AVOID THIS ////

//Please avoid the following inappropriate and inefficient use cases
/**
 * 
 * 1. Call get repeatedly inside the loop without the appropriate timing for execution provided e.g. millis() or conditional checking,
 * where delay should be avoided.
 * 
 * Everytime get was called, the request header need to be sent to server which its size depends on the authentication method used, 
 * and costs your data usage.
 * 
 * Please use stream function instead for this use case.
 * 
 * 2. Using the single FirebaseData object to call different type functions as above example without the appropriate 
 * timing for execution provided in the loop i.e., repeatedly switching call between get and set functions.
 * 
 * In addition to costs the data usage, the delay will be involved as the session needs to be closed and opened too often
 * due to the HTTP method (GET, PUT, POST, PATCH and DELETE) was changed in the incoming request. 
 * 
 * 
 * Please reduce the use of swithing calls by store the multiple values to the JSON object and store it once on the database.
 * 
 * Or calling continuously "set" or "setAsync" functions without "get" called in between, and calling get continuously without set 
 * called in between.
 * 
 * If you needed to call arbitrary "get" and "set" based on condition or event, use another FirebaseData object to avoid the session 
 * closing and reopening.
 * 
 * 3. Use of delay or hidden delay or blocking operation to wait for hardware ready in the third party sensor libraries, together with stream functions e.g. Firebase.RTDB.readStream and fbdo.streamAvailable in the loop.
 * 
 * Please use non-blocking mode of sensor libraries (if available) or use millis instead of delay in your code.
 * 
 * 4. Blocking the token generation process.
 * 
 * Let the authentication token generation to run without blocking, the following code MUST BE AVOIDED.
 * 
 * while (!Firebase.ready()) <---- Don't do this in while loop
 * {
 *     delay(1000);
 * }
 * 
 */
