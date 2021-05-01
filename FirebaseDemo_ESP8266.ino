#include <FirebaseESP8266.h>
#include <ESP8266WiFi.h>

#define WIFI_SSID "viettel291"
#define WIFI_PASSWORD "02101998"

#define FIREBASE_HOST "iot-app-84905-default-rtdb.firebaseio.com"

/** The database secret is obsoleted, please use other authentication methods, 
 * see examples in the Authentications folder. 
*/
#define FIREBASE_AUTH "mBKcqj1XtKEL6KqWLfSD3yuVLLXcjTZ7buVvFI5T"

//Define FirebaseESP8266 data object
FirebaseData fbdo;

const int trig = 13;
const int echo = 15;
const int speaker = 12;

unsigned long duration; //biến đo thời gian
int distance;           // biến đo khoảng cách
unsigned long sendDataPrevMillis = 0;
String path = "/IoT/Stream";

void printResult(FirebaseData &data);
 
void setup()
{

  Serial.begin(115200);
  
  pinMode(trig, OUTPUT); // chân trig sẽ phát tín hiệu
  pinMode(echo, INPUT);  // chân echo sẽ thu tín hiệu
  pinMode(speaker, OUTPUT);
  
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

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);

  //Set the size of WiFi rx/tx buffers in the case where we want to work with large data.
  fbdo.setBSSLBufferSize(1024, 1024);

  //Set the size of HTTP response buffers in the case where we want to work with large data.
  fbdo.setResponseSize(1024);

  if (!Firebase.beginStream(fbdo, path))
  {
    Serial.println("------------------------------------");
    Serial.println("Can't begin stream connection...");
    Serial.println("REASON: " + fbdo.errorReason());
    Serial.println("------------------------------------");
    Serial.println();
  }
}

void loop()
{

  
  
  if (millis() - sendDataPrevMillis > 1000)
  {
    sendDataPrevMillis = millis();
    // Implement for sonic sensor
    // Phát xung từ chân trig
    digitalWrite(trig, 0); // tắt chân trig
    delayMicroseconds(2);
  
    digitalWrite(trig, 1); // phát xung từ chân trig
    delayMicroseconds(10); // phát xung có độ dài 5 miliseconds
    digitalWrite(trig, 0); // tắt chân trig
  
    // Tính toán thời gian
    duration = pulseIn(echo, HIGH);        // đo độ rộng xung HIGH ở chân echo
    distance = int(duration / 29.412 / 2); // tính khoảng cách đến vật
  
    // Get range value (warning distance from firebase)
    String distanceWarn = "";
    if (Firebase.getString(fbdo, path + "/Range/distanceVal"))
    {
      Serial.println("GET Range value -------------------------");
      Serial.println("PASSED");
      Serial.print("VALUE: ");
      distanceWarn = fbdo.stringData();
      Serial.println(distanceWarn);
    }
    else
    {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
      Serial.println("------------------------------------");
      Serial.println();
    }
    
    // Set distance value from sensor to firebase
    if (Firebase.setString(fbdo, path + "/Data", String(distance)))
    {
      Serial.println("SET Distance value -------------------------");
      Serial.println("PASSED");
      Serial.print("VALUE: ");
      printResult(fbdo);
      Serial.println("------------------------------------");
      Serial.println();
    }
    else
    {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
      Serial.println("------------------------------------");
      Serial.println();
    }

    if(distance < distanceWarn.toInt() )
    {
      //digitalWrite(led1Pin, HIGH);
      digitalWrite(speaker, 0);
      //strcpy(ledStatus, "ON");
    }
    else
    {
        //digitalWrite(led1Pin, 0);
        digitalWrite(speaker, 1);
    }

  }

//  if (!Firebase.readStream(fbdo))
//  {
//    Serial.println("------------------------------------");
//    Serial.println("Can't read stream data...");
//    Serial.println("REASON: " + fbdo.errorReason());
//    Serial.println("------------------------------------");
//    Serial.println();
//  }
//
//  if (fbdo.streamTimeout())
//  {
//    Serial.println("Stream timeout, resume streaming...");
//    Serial.println();
//  }
//
//  if (fbdo.streamAvailable())
//  {
//    Serial.println("------------------------------------");
//    Serial.println("Stream Data available...");
//    Serial.println("STREAM PATH: " + fbdo.streamPath());
//    Serial.println("EVENT PATH: " + fbdo.dataPath());
//    Serial.println("DATA TYPE: " + fbdo.dataType());
//    Serial.println("EVENT TYPE: " + fbdo.eventType());
//    Serial.print("VALUE: ");
//    printResult(fbdo);
//    Serial.println("------------------------------------");
//    Serial.println();
//  }
}

void printResult(FirebaseData &data)
{

  if (data.dataType() == "int")
    Serial.println(data.intData());
  else if (data.dataType() == "float")
    Serial.println(data.floatData(), 5);
  else if (data.dataType() == "double")
    printf("%.9lf\n", data.doubleData());
  else if (data.dataType() == "boolean")
    Serial.println(data.boolData() == 1 ? "true" : "false");
  else if (data.dataType() == "string")
    Serial.println(data.stringData());
  else if (data.dataType() == "json")
  {
    Serial.println();
    FirebaseJson &json = data.jsonObject();
    //Print all object data
    Serial.println("Pretty printed JSON data:");
    String jsonStr;
    json.toString(jsonStr, true);
    Serial.println(jsonStr);
    Serial.println();
    Serial.println("Iterate JSON data:");
    Serial.println();
    size_t len = json.iteratorBegin();
    String key, value = "";
    int type = 0;
    for (size_t i = 0; i < len; i++)
    {
      json.iteratorGet(i, type, key, value);
      Serial.print(i);
      Serial.print(", ");
      Serial.print("Type: ");
      Serial.print(type == FirebaseJson::JSON_OBJECT ? "object" : "array");
      if (type == FirebaseJson::JSON_OBJECT)
      {
        Serial.print(", Key: ");
        Serial.print(key);
      }
      Serial.print(", Value: ");
      Serial.println(value);
    }
    json.iteratorEnd();
  }
  else if (data.dataType() == "array")
  {
    Serial.println();
    //get array data from FirebaseData using FirebaseJsonArray object
    FirebaseJsonArray &arr = data.jsonArray();
    //Print all array values
    Serial.println("Pretty printed Array:");
    String arrStr;
    arr.toString(arrStr, true);
    Serial.println(arrStr);
    Serial.println();
    Serial.println("Iterate array values:");
    Serial.println();
    for (size_t i = 0; i < arr.size(); i++)
    {
      Serial.print(i);
      Serial.print(", Value: ");

      FirebaseJsonData &jsonData = data.jsonData();
      //Get the result data from FirebaseJsonArray object
      arr.get(jsonData, i);
      if (jsonData.typeNum == FirebaseJson::JSON_BOOL)
        Serial.println(jsonData.boolValue ? "true" : "false");
      else if (jsonData.typeNum == FirebaseJson::JSON_INT)
        Serial.println(jsonData.intValue);
      else if (jsonData.typeNum == FirebaseJson::JSON_FLOAT)
        Serial.println(jsonData.floatValue);
      else if (jsonData.typeNum == FirebaseJson::JSON_DOUBLE)
        printf("%.9lf\n", jsonData.doubleValue);
      else if (jsonData.typeNum == FirebaseJson::JSON_STRING ||
               jsonData.typeNum == FirebaseJson::JSON_NULL ||
               jsonData.typeNum == FirebaseJson::JSON_OBJECT ||
               jsonData.typeNum == FirebaseJson::JSON_ARRAY)
        Serial.println(jsonData.stringValue);
    }
  }
  else if (data.dataType() == "blob")
  {

    Serial.println();

    for (int i = 0; i < data.blobData().size(); i++)
    {
      if (i > 0 && i % 16 == 0)
        Serial.println();

      if (i < 16)
        Serial.print("0");

      Serial.print(data.blobData()[i], HEX);
      Serial.print(" ");
    }
    Serial.println();
  }
  else if (data.dataType() == "file")
  {

    Serial.println();

    File file = data.fileStream();
    int i = 0;

    while (file.available())
    {
      if (i > 0 && i % 16 == 0)
        Serial.println();

      int v = file.read();

      if (v < 16)
        Serial.print("0");

      Serial.print(v, HEX);
      Serial.print(" ");
      i++;
    }
    Serial.println();
    file.close();
  }
  else
  {
    Serial.println(data.payload());
  }
}
