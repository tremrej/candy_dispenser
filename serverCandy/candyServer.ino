// This is UDP server which output the letter 'B' on the serial port when receiving a message from UDP port 8888 
// This is used to activate a cycle of the candy dispenser.

// To compile in Arduino IDE: board ESP8266 Modules/Adafruit HUZZAH ESP8266

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

#ifndef STASSID
#define STASSID "tremrej"
#define STAPSK  "Victor10"
#endif

unsigned int localPort = 8888;      // local port to listen on

// buffers for receiving and sending data
char packetBuffer[UDP_TX_PACKET_MAX_SIZE + 1]; //buffer to hold incoming packet,
char  ReplyBuffer[] = "acknowledged\r\n";       // a string to send back

WiFiUDP Udp;

IPAddress staticIp_g(192, 168, 2, 5);
IPAddress gateway_g(192, 168, 2, 1);
IPAddress subnet_g(255,255,255,0);


void setup()
{  
    Serial.begin(115200);         
    Serial.println("Candy server v1!");

    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);    // turn the LED off by making the voltage LOW

    WiFi.mode(WIFI_STA);
    WiFi.config(staticIp_g, gateway_g, subnet_g);
    WiFi.begin(STASSID, STAPSK);
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print('.');
        digitalWrite(LED_BUILTIN, LOW);
        delay(250);
        digitalWrite(LED_BUILTIN, HIGH);
        delay(250);
    }
    Serial.print("Connected! IP address: ");
    Serial.println(WiFi.localIP());
    Serial.printf("Candy server on port %d\n", localPort);
    Udp.begin(localPort);

    digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
}

void loop() {
    // if there's data available, read a packet
    int packetSize = Udp.parsePacket();
    if (packetSize) {
        // read the packet into packetBufffer
        int n = Udp.read(packetBuffer, UDP_TX_PACKET_MAX_SIZE);
        packetBuffer[n] = 0;
        if (packetBuffer[0] == 'B')
        {
            Serial.write('B');
            digitalWrite(LED_BUILTIN, HIGH);
            delay(500);
            digitalWrite(LED_BUILTIN, LOW);
        }
    }
}


/*
  test (shell/netcat):
  --------------------
	  nc -u 192.168.esp.address 8888
	  nc -u 192.168.2.5 8888
*/

