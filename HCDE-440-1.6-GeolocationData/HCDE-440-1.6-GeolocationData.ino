
/*ICE #1A
   Using HCDE_IoT_4_GeoLocation.ino as a starting point, augment the sketch to include
   a new data type (a struct) called MetData designed to contain name:value pairs from
   http://openweathermap.org/. You will need to get an API key to use this service, the
   key is free for low volume development use. Get your key at http://openweathermap.org/api
   by subscribing to the Current Weather Data api. Examples of the API call are provided in
   the API documnentation.
   Wrap your HTTP request in a function called getMet(), which will be similar to getIP()
   and getGeo(), and print out the data as a message to the Serial Monitor using Imperial
   units. You should report temperature, humidity, windspeed, wind direction, and cloud
   conditions. Call the API by city name.
   
   One possible solution is provided below, though you should try to find your own solution
   first based upon previous examples.
*/

/*ICE #1B
   To complete this ICE, demonstrate your understanding of this sketch by running
   the code and providing complete inline commentary, describing how each line of
   code functions in detail. The completed project will comment every line. Some of
   this has already been done for you in earlier progressions of this project.
   This sketch . . . (briefly relate what this sketch does here)
   
   If your code differs from this solution, fine. Just comment that code thoroughly.
*/

// HCDE 440
// ICE (In-Class Exercise) #1: APIs and Embedding Knowledge
// Sunny Cui

#include <ESP8266WiFi.h>        // include the WiFi library
#include <ESP8266HTTPClient.h>  // include methods to send HTTP requests
#include <ArduinoJson.h>        // include the ArduinoJson.h library which provides the ability to parse and construct JSON objects

const char* ssid = "University of Washington";         // the SSID (name) of the Wi-Fi network I am connected to 
const char* pass = "";                                 // the password of the Wi-Fi network I am connected to
// const char* key = "3b1ec911debf46c7c73150931653a593";  // the API key  

String weatherKey = "b5d7c1770139c2a57c6eb15418c5765b"; // the API key for open weather map

typedef struct { // creates a new data type definition, a box to hold other data types
  String ip;     // creates a string that holds the data
  String cc;
  String cn;
  String rc;
  String rn;
  String cy;
  String tz;
  String ln;
  String lt;
} GeoData; // name the new data structure as "GeoData"

typedef struct { //here we create a new data type definition, a box to hold other data types
  String tp;
  String pr;
  String hd;
  String ws;
  String wd;
  String cd;
} MetData; // name the new data structure as "MetData"

GeoData location;   // initialize a variable "location" of type GeoData
MetData conditions; // initialize a variable "conditions" of type MetData

void setup() {
  Serial.begin(115200); // start the Serial communication to send messages to the computer
  delay(10); // wait for 10 milliseconds

  Serial.print("Connecting to "); Serial.println(ssid); // print this message and the WiFi password to the serial monitor

  WiFi.mode(WIFI_STA);    // switches Wi-Fi mode
  WiFi.begin(ssid, pass); // connects to the network

  while (WiFi.status() != WL_CONNECTED) { // wait for the Wi-Fi to connect
    delay(500); // wait for 500 milliseconds
    Serial.print(".");
  }

  Serial.println(); Serial.println("WiFi connected"); Serial.println(); // if WiFi is connected, print this message
  Serial.print("Your ESP has been assigned the internal IP address ");  // print this message to the serial monitor
  Serial.println(WiFi.localIP());                                       // print IP address to the serial monitor

  String ipAddress = getIP();
  getGeo(ipAddress); // call the getGeo() function

  Serial.println(); // prints an empty line
  
  // prints the GeoData obtained to the serial monitor
  Serial.println("Your external IP address is " + location.ip);                        // print the IP address to the serial monitor
  Serial.print("Your ESP is currently in " + location.cn + " (" + location.cc + "),"); // print the location data to the serial monitor
  Serial.println(" in or near " + location.cy + ", " + location.rc + ".");             // print this message to the serial monitor
  Serial.print("You are in the " + location.tz + " timezone ");                        // print this message to the serial monitor
  Serial.println("and located at (roughly) ");                                         // print this message to the serial monitor
  Serial.println(location.lt + " latitude by " + location.ln + " longitude.");         // print the latitude and longitude to the serial monitor

  getMet(location.cy); // call the getMet() function

  Serial.println(); // prints an empty line
  Serial.println("With " + conditions.cd + ", the temperature in " + location.cy + ", " + location.rc);
  Serial.println("is " + conditions.tp + "F, with a humidity of " + conditions.hd + "%. The winds are blowing");
  Serial.println(conditions.wd + " at " + conditions.ws + " miles per hour, and the ");
  Serial.println("barometric pressure is at " + conditions.pr + " millibars.");
}

void loop() { // we don't need any repeats so leave the loop() empty
}

String getIP() {
  HTTPClient theClient;
  String ipAddress;

  theClient.begin("http://api.ipify.org/?format=json");
  int httpCode = theClient.GET();

  if (httpCode > 0) {
    if (httpCode == 200) {

      DynamicJsonBuffer jsonBuffer;

      String payload = theClient.getString();
      JsonObject& root = jsonBuffer.parse(payload);
      ipAddress = root["ip"].as<String>();

    }
    else {
      Serial.println("Something went wrong with connecting to the endpoint in getIP()."); // print this message to the serial monitor
      return "error";
    }
  }
  return ipAddress; // returns the IP address
}

void getGeo(String IP) {
  HTTPClient theClient;
  theClient.begin("http://freegeoip.net/json/" + IP);
  int httpCode = theClient.GET();

  if (httpCode > 0) {
    if (httpCode == 200) {
      DynamicJsonBuffer jsonBuffer;
      String payload = theClient.getString();
      JsonObject& root = jsonBuffer.parse(payload);
      if (!root.success()) {
        Serial.println("parseObject() failed in getGeo()");
        return;
      }
      location.ip = root["ip"].as<String>();
      location.cc = root["country_code"].as<String>();
      location.cn = root["country_name"].as<String>();
      location.rc = root["region_code"].as<String>();
      location.rn = root["region_name"].as<String>();
      location.cy = root["city"].as<String>();
      location.tz = root["time_zone"].as<String>();
      location.lt = root["latitude"].as<String>();
      location.ln = root["longitude"].as<String>();

    }
    else {
      Serial.println("Something went wrong with connecting to the endpointin getGeo().");
    }
  }
}

void getMet(String city) {
  HTTPClient theClient;
  String apiCall = "http://api.openweathermap.org/data/2.5/weather?q=Seattle";
  apiCall += "&units=imperial&appid=";
  apiCall += weatherKey;
  theClient.begin(apiCall);
  int httpCode = theClient.GET();
  if (httpCode > 0) {

    if (httpCode == HTTP_CODE_OK) {
      String payload = theClient.getString();
      DynamicJsonBuffer jsonBuffer;
      JsonObject& root = jsonBuffer.parseObject(payload);
      if (!root.success()) {
        Serial.println("parseObject() failed in getMet().");
        return;
      }
      conditions.tp = root["main"]["temp"].as<String>();
      conditions.pr = root["main"]["pressure"].as<String>();
      conditions.hd = root["main"]["humidity"].as<String>();
      conditions.cd = root["weather"][0]["description"].as<String>();
      conditions.ws = root["wind"]["speed"].as<String>();
      int deg = root["wind"]["deg"].as<int>();
      conditions.wd = getNSEW(deg);
    }
  }
  else {
    Serial.printf("Something went wrong with connecting to the endpoint in getMet().");
  }
}

String getNSEW(int d) {
  String direct; // initialize a string variable "direct"

  //Conversion based upon http://climate.umn.edu/snow_fence/Components/winddirectionanddegreeswithouttable3.htm
  if (d > 348.75 && d < 360 || d >= 0  && d < 11.25) {
    direct = "north";
  };
  if (d > 11.25 && d < 33.75) {
    direct = "north northeast";
  };
  if (d > 33.75 && d < 56.25) {
    direct = "northeast";
  };
  if (d > 56.25 && d < 78.75) {
    direct = "east northeast";
  };
  if (d < 78.75 && d < 101.25) {
    direct = "east";
  };
  if (d < 101.25 && d < 123.75) {
    direct = "east southeast";
  };
  if (d < 123.75 && d < 146.25) {
    direct = "southeast";
  };
  if (d < 146.25 && d < 168.75) {
    direct = "south southeast";
  };
  if (d < 168.75 && d < 191.25) {
    direct = "south";
  };
  if (d < 191.25 && d < 213.75) {
    direct = "south southwest";
  };
  if (d < 213.25 && d < 236.25) {
    direct = "southwest";
  };
  if (d < 236.25 && d < 258.75) {
    direct = "west southwest";
  };
  if (d < 258.75 && d < 281.25) {
    direct = "west";
  };
  if (d < 281.25 && d < 303.75) {
    direct = "west northwest";
  };
  if (d < 303.75 && d < 326.25) {
    direct = "south southeast";
  };
  if (d < 326.25 && d < 348.75) {
    direct = "north northwest";
  };
  return direct; // returns string
}
