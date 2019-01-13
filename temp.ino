

#include <SPI.h>
#include <Ethernet.h>
#include "DHT.h"
#define DHTPIN 2 
#define DHTTYPE DHT22

byte mac[] = {
  0x00, 0x22, 0x19, 0x00, 0x00, 0x20
};
IPAddress ip(192, 168, 192, 69);

EthernetServer server(80);
DHT dht(DHTPIN, DHTTYPE);


void setup() {
  dht.begin();


  // Open serial communications and wait for port to open:
  Serial.begin(9600);
 
  

  // start the Ethernet connection and the server:
  Ethernet.begin(mac, ip);

  // Check for Ethernet hardware present
  if (Ethernet.hardwareStatus() == EthernetNoHardware) {
    Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
    while (true) {
      delay(1); // do nothing, no point running without Ethernet hardware
    }
  }
  if (Ethernet.linkStatus() == LinkOFF) {
    Serial.println("Ethernet cable is not connected.");
  }

  // start the server
  server.begin();
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());
}


void loop() {
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
        float t = dht.readTemperature();
        float h = dht.readHumidity();
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) {
          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close");  // the connection will be closed after completion of the response
          client.println("Refresh: 5");  // refresh the page automatically every 5 sec
          client.println();
          client.println("<!DOCTYPE HTML>");
          client.println("<head><title>Temperatura w serwerowni by RavenIT</title><meta charset=utf-8></head>");
          client.println("<html>");
          client.println("<br><br><br><br>");
          client.println("<center><h1>");
          client.print("Temperatura ");
          client.print(t);
          client.println("&#x2103;");
          client.println("<br>");
          client.print("Wilgotność ");
          client.print(h);
          client.println("%");
          client.println("</h1></center>");
          client.println("</html>");
          break;
        }
        
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        } else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    // give the web browser time to receive the data
    delay(1);
    // close the connection:
    client.stop();
    Serial.println("client disconnected");
  }
}
