#include <WiFiS3.h>
#include "secrets.h"

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include <Servo.h>
#define LEFT_SERVO_PIN 9
#define RIGHT_SERVO_PIN 10

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)

#define SCREEN_ADDRESS 0x3D ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
#define IMU_ADDRESS 0x4a
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


int status = WL_IDLE_STATUS;
 
///////please enter your sensitive data in the Secret tab/secrets.h
char ssid[] = SECRET_SSID;        // your network SSID (name)
char pass[] = SECRET_PASS;    // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;            // your network key index number (needed only for WEP)

unsigned int local_port = 2390;      // local port to listen on

char packetBuffer[256]; //buffer to hold incoming packet
String response;       // a string to send back
String received;

WiFiUDP Udp;

Servo left_servo;
Servo right_servo;

int left_servo_pin {LEFT_SERVO_PIN};
int right_servo_pin {RIGHT_SERVO_PIN};

void setup() {
  // put your setup code here, to run once:
  
  left_servo.attach(left_servo_pin);
  right_servo.attach(right_servo_pin);

  stop();
  delay(2000);

    //Initialize serial
  Serial.begin(115200);


  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  display.display();
  delay(1000); // Pause for 1 second
  // Clear the buffer
  display.clearDisplay();

  delay(100);


  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true);
  }

  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Please upgrade the firmware");
  }

  // attempt to connect to WiFi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);
  }
  Serial.println("Connected to WiFi");
  printWifiStatus();

  Serial.println("\nStarting connection to server...");
  // if you get a connection, report back via serial:
  Udp.begin(local_port);
}

void loop() {
  // put your main code here, to run repeatedly:
    // if there's data available, read a packet

  int packetSize = Udp.parsePacket();
  if (packetSize) {

    IPAddress remoteIp = Udp.remoteIP();


    // read the packet into packetBuffer
    int len = Udp.read(packetBuffer, 255);
    if (len > 0) {
      packetBuffer[len] = 0;
    }
    Serial.println("Contents:");
    Serial.println(packetBuffer);
    received = String(packetBuffer);

    if (received == "forward")
      forward();
    
    else if (received == "reverse")
      reverse();

    else if (received == "turn_ccw")
      turn_ccw();

    else if (received == "turn_cw")
      turn_cw();

    else if (received == "stop")
      stop();

    else
      stop();

    // send a reply, to the IP address and port that sent us the packet we received
    Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
    response = "Received " + received;
    Udp.print(response);
    Udp.endPacket();
  }
  
}


void printWifiStatus() {

  display.setTextSize(1);             // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE); 
  display.setCursor(0, 0);
  // print the SSID of the network you're attached to:
  // Serial.print("SSID: ");
  // Serial.println(WiFi.SSID());
  display.print(F("SSID: "));
  display.println(WiFi.SSID());
  display.println(); // Adding space between previous line and next line

  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  // Serial.print("IP Address: ");
  // Serial.println(ip);

  display.print(F("IP: "));
  display.println(ip);
  display.println(); // Adding space between previous line and next line

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  // Serial.print("signal strength (RSSI):");
  // Serial.print(rssi);
  // Serial.println(" dBm");

  display.print(F("Port: "));
  display.println(local_port);

  display.display();
}

void stop()
{
  left_servo.write(90);
  right_servo.write(90);
}

void forward()
{
  left_servo.write(180);
  right_servo.write(0);
}

void reverse()
{
  left_servo.write(0);
  right_servo.write(180);
}

void turn_ccw()
{
  left_servo.write(0);
  right_servo.write(0);
}

void turn_cw()
{
  left_servo.write(180);
  right_servo.write(180);
}
