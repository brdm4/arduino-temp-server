#include <SPI.h>
#include <Ethernet.h>
#include <SD.h>
#include "DHT.h"

#include <LiquidCrystal.h>

LiquidCrystal lcd(9, 8, 7, 6, 5, 3);

#define DHTPIN 2     // what pin we're connected to

#define DHTTYPE DHT11   // DHT 11 

DHT dht(DHTPIN, DHTTYPE); // DHT lib var

byte mac[] = { 
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
  
EthernetServer server(80); //Port 80 for web server

//vars for building a string from our sd card data
String content = "";
char character;


void setup()
{
  lcd.begin(16, 2);
  
  
  
 // Open serial communications and wait for port to open:
  Serial.begin(9600);
   while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }

  //Serial.print("Initializing SD card...");
  lcd.print("Booting...");
  // make sure that the default chip select pin is set to
  // output, even if you don't use it:
  pinMode(10, OUTPUT);
  
  // see if the card is present and can be initialized:
  if (!SD.begin(4)) {
    //Serial.println("Card failed, or not present");
    lcd.clear();
    lcd.print("Check SD card");
    // don't do anything more:
    return;
  }
  //Serial.println("card initialized.");
    lcd.clear();
    lcd.print("SD card found");
    delay(1000);
  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  File dataFile = SD.open("ip.txt");

  // if the file is available, read it:
  if (dataFile) {
    while (dataFile.available()) {
      
      //inside the read loop we colect each charicter off the serial stream 
      character = dataFile.read();
      //glue each icoming charicter together into a sting
      content.concat(character);
    }
    
    //close the file now we have looped over it
    dataFile.close();
    
    //Debug - check whats coming off the sd
    //Serial.println(content);
    
    //take the sting form sd card and find the first block of our ip and turn string to int
    int a = content.substring(0,3).toInt();
    //Serial.println(a);
    
    int b = content.substring(4,7).toInt();
    //Serial.println(b);
 
    int c = content.substring(8,11).toInt();
    //Serial.println(c);
 
    int d = content.substring(12,16).toInt();
    //Serial.println(d);
    
    // take the 4 intagers we have made and use as ip
    IPAddress ip(a,b,c,d);
   
    // start the Ethernet connection and the server:
    Ethernet.begin(mac, ip);
    server.begin();
    Serial.print("server is at ");
    //Serial.println(Ethernet.localIP());
    lcd.clear();
    lcd.print("Server at:");
    lcd.setCursor(0, 1);
    lcd.print(Ethernet.localIP());
 
  }
  
  // if the file isn't open, pop up an error:
else {

    lcd.clear();
    lcd.print("ip.txt not found");
    lcd.setCursor(0, 1);
    lcd.print("Makeing help.txt");
    
    
    
    
    
        
    dataFile = SD.open("help.txt", FILE_WRITE);
    if (dataFile) {

dataFile.println("I look for a file called ip.txt, IP adress on the first line with no other text.");
dataFile.println("IP adress must have padded 0's eg 192.168.1.5 must be writen as 192.168.001.005");

     
     // close the file:
     dataFile.close();
     lcd.clear();
     lcd.print("ip.txt not found");
     lcd.setCursor(0, 1);
     lcd.print("help.txt on SD"); 
    
  } else {
    // if the file didn't open, print an error:
    //Serial.println("error opening test.txt");
  } 
    
  } 
}



void loop() {
 
  delay(2000);

  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius
  float t = dht.readTemperature();

  // listen for incoming clients
  EthernetClient client = server.available();
  if (client) {
    Serial.println("new client");
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
 
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) {
          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close");  // the connection will be closed after completion of the response
	  //client.println("Refresh: 5");  // refresh the page automatically every 5 sec
          client.println();
          client.println("<!DOCTYPE HTML>");
          client.println("<html>");
          
          // output the value of each analog input pin
          
          if (isnan(h) || isnan(t)) {
            client.println("Failed to read from DHT sensor!");
            return;
          }
  
       

         client.print("<p id=\"h\">"); 
         client.print(h);
         client.print("</p>");
         client.print("<p id=\"t\">"); 
         client.print(t);
         client.print("</p>");
         client.println("</html>");
         break;
        }
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        } 
        else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    // give the web browser time to receive the data
    delay(1);
    // close the connection:
    client.stop();
   // Serial.println("client disonnected");
  }
}

