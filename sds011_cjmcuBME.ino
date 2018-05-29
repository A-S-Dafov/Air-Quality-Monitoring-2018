#include <ESP8266WiFi.h>
#include<SDS011.h>
#include<ESP8266HTTPClient.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>


//sds sensor variables
SDS011 my_sds;
float p10, p25;
int error;
//


//The BME sensor is wired for the I2C protocol.


//bme280 variables
#define SEALEVELPRESSURE_HPA (1012)//Data for Sofia, change the number to the local messurements .
Adafruit_BME280 bme;
float temperature = -1.0;
float pressure = -1.0;
float altitude = -1.0;
float humidity = -1.0;
//

const char *ssid =  "wifi_name";   // replace with your wifi ssid and wpa2 key
const char *pass =  "wifi_password";

//D1 = 5, D2 = 4, D3=2, D4=0

void setup()
{
  
  
  Serial.begin(9600);//Begins serial communication at 9600 bit rate.
  Wire.begin(D3,D4);//Configures the pins for the I2C protocol communication with the BME sensor.
  //sds011
  my_sds.begin(D1, D2); //I2C with the SDS sensor.
  //

  //bme280
  bool status;
    status = bme.begin();  
    if (!status) {
        Serial.println("Could not find a valid BME280 sensor, check wiring!");
        while (1);
    }

  //
  
  

  //wifi
  WiFi.disconnect();
  delay(3000);
   WiFi.begin(ssid,pass);//Connects to the WiFi network.
  while ((!(WiFi.status() == WL_CONNECTED))){
    delay(300);

  }
  Serial.println("Connected. My IP is: ");
  Serial.println((WiFi.localIP().toString()));
  //

}


void loop()
{
  error = my_sds.read(&p25, &p10);//Reading the sensor data from the sds011 and getting the error.

//bme280
  temperature = bme.readTemperature();//Reads the date from the BME sensor.
  pressure = bme.readPressure()/100.0F;
  altitude = bme.readAltitude(SEALEVELPRESSURE_HPA);
  humidity = bme.readHumidity();
  //Prints messages for debugging in the Serial Monitor. (Ctrl+Shift+M  Is the shortcut ATM.)
    Serial.print("Temperature = ");
    Serial.print(String(temperature));
    Serial.println(" *C");

    Serial.print("Pressure = ");

    Serial.print(String(pressure));
    Serial.println(" hPa");

    Serial.print("Approx. Altitude = ");
    Serial.print(String(altitude));
    Serial.println(" m");

    Serial.print("Humidity = ");
    Serial.print(String(humidity));
    Serial.println(" %");

   
  //
  if (!error)
  {
    Serial.println("P2.5: " + String(p25));// Measured in micrograms/m^3
    Serial.println("P10:  " + String(p10));// Same.
     Serial.println();
  }

  if(WiFi.status()==WL_CONNECTED)
  {
    HTTPClient http;

    http.begin("http://fathomless-peak-36706.herokuapp.com");//Begins to connection to the server.
    http.addHeader("Content-Type", "text/plain");//Sets up the header of the request

    int httpCode = http.POST("<password>|"+String(p25) +"|" + String(p10)+ "|" + String(pressure)
    + "|"+ String(temperature) + "|"+String(altitude)+"|"+String(humidity));//This is the format of the message we send to the server, with the data from the sensors.
    //In our project we have replaced the <password> with a control word so that the server only save data sent from the NodeMCU.
    //You can setup the message from NodeMCU to your server in similar fashion.
    
    String payload = http.getString();//Answer from the server to the requset we sent.

    Serial.println(httpCode);
    Serial.println(payload);
    http.end();//Ends connection.
  }  
  else
  {
    Serial.println("WIFI CONNECTION ERROR");
  }
 
    delay(15000);// We get the readings from the sensors every 15 secs, the value can change accordingly to the user's needs.

}
