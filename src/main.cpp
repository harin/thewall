#include <Adafruit_NeoPixel.h>
#include "ESP8266WiFi.h"
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
// Which pin on the Arduino is connected to the NeoPixels?
#define PIN D1 // On Trinket or Gemma, suggest changing this to 1

// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS 50 // Popular NeoPixel ring size

// When setting up the NeoPixel library, we tell it how many pixels,
// and which pin to use to send signals. Note that for older NeoPixel
// strips you might need to change the third parameter -- see the
// strandtest example for more information on possible values.
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

#define DELAYVAL 500 // Time (in milliseconds) to pause between pixels

const char *ssid = "@anotherbougieday";
const char *password = "hallelujah34";

void getIndex(String str, int *index)
{
  // Take a string like "1 4 7"
  // that describes the pixel to be lit up
  Serial.print("Sizeof = ");
  Serial.println(sizeof(index));
  for (u_int i = 0; i < sizeof(index); i++)
  {
    Serial.print("i = ");
    Serial.println(i);
    index[i] = str.substring(0, str.indexOf(" ")).toInt();
    Serial.print("Extracting ");
    Serial.println(index[i]);
    String removeStr = str.substring(0, str.indexOf(" ") + 1);
    str.remove(0, removeStr.length());
    Serial.print("Remaining str = ");
    Serial.println(str);
  }
  Serial.println("=====");
}

void draw(int *index)
{
  pixels.clear();
  for (u_int i = 0; i < sizeof(index); i++)
  { // For each pixel...
    pixels.setPixelColor(index[i], pixels.Color(0, 150, 0));
    pixels.show(); // Send the updated pixel colors to the hardware.
  }
}

void draw(String s)
{
  Serial.print("Got string <");
  Serial.print(s);
  Serial.println(">");
  char DELIMITER = ' ';
  u_int delimitCount = 1;
  for (u_int i = 0; i < s.length(); i++)
  {
    if (s[i] == DELIMITER)
    {
      delimitCount++;
    }
  }
  int index[delimitCount];
  getIndex(s, index);

  Serial.println("BEGIN PRINT INDEX");
  Serial.print("<");
  for (u_int i = 0; i < delimitCount; i++)
  {
    Serial.print(index[i]);
    if (i != delimitCount - 1)
    {
      Serial.print(" ");
    }
  }
  Serial.print(">");
  Serial.println("END PRINT INDEX");

  draw(index);
}

AsyncWebServer server(80);

const char *PARAM_INPUT_1 = "input1";
const char *PARAM_INPUT_2 = "input2";
const char *PARAM_INPUT_3 = "input3";

// HTML web page to handle 3 input fields (input1, input2, input3)
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html><head>
  <title>ESP Input Form</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  </head><body>
  <form action="/get">
    input1: <input type="text" name="input1">
    <input type="submit" value="Submit">
  </form><br>
  <form action="/get" id="ci">
    input2: <input type="text" name="input2">
    <input type="submit" value="Submit">
  </form><br>
  <form action="/get">
    input3: <input type="text" name="input3">
    <input type="submit" value="Submit">
  </form>
  <script>
    function toIndex(code, numCol=6, numRow=8) {
      let colCode = code.substring(0,1).toLowerCase();
      let rowCode = code.substring(1);
      console.log(numRow * (colCode.charCodeAt(0) - 'a'.charCodeAt(0)));
      let row = Number(rowCode) - 1;
      let col = colCode.charCodeAt(0) - 'a'.charCodeAt(0)
      
      if (col % 2 != 0) {
        // even rows starts from the bottom e.g. row 0,2,4 (A,C,E)
        row = numRow - row-1;
      }
      
      return numRow*col + row;
    }

    window.addEventListener("DOMContentLoaded", function() {
      document.getElementById('ci').addEventListener("submit", function(e) {
        e.preventDefault();
        const target = document.querySelector("input[name=input2]")
        const indexes = target.value.split(" ").map((x) => toIndex(x))
        console.log(indexes.join(" "))
        target.value = indexes.join(" ")
        document.getElementById('ci').submit()
      })
    });

  </script>
</body></html>)rawliteral";

void notFound(AsyncWebServerRequest *request)
{
  request->send(404, "text/plain", "Not found");
}

void setup()
{
  Serial.begin(115200);

  Serial.println("TEST");
  String testInput = "1 24 3 6 10";
  draw(testInput);

  pixels.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi..");
  Serial.println(ssid);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.print(".");
  }

  Serial.println("Connected to WiFi");
  Serial.println(WiFi.localIP()); // Send the IP address of the ESP8266 to the computer

  // Send web page with input fields to client
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send_P(200, "text/html", index_html); });

  // Send a GET request to <ESP_IP>/get?input1=<inputMessage>
  server.on("/get", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    String inputMessage;
    String inputParam;
    // GET input1 value on <ESP_IP>/get?input1=<inputMessage>
    if (request->hasParam(PARAM_INPUT_1))
    {
      inputMessage = request->getParam(PARAM_INPUT_1)->value();
      inputMessage.trim();
      inputParam = PARAM_INPUT_1;
      draw(inputMessage);
    }
    // GET input2 value on <ESP_IP>/get?input2=<inputMessage>
    else if (request->hasParam(PARAM_INPUT_2))
    {
      inputMessage = request->getParam(PARAM_INPUT_2)->value();
      inputMessage.trim();
      inputParam = PARAM_INPUT_2;
      draw(inputMessage);
    }
    // GET input3 value on <ESP_IP>/get?input3=<inputMessage>
    else if (request->hasParam(PARAM_INPUT_3))
    {
      inputMessage = request->getParam(PARAM_INPUT_3)->value();
      inputParam = PARAM_INPUT_3;
    }
    else
    {
      inputMessage = "No message sent";
      inputParam = "none";
    }
    Serial.println(inputMessage);
    request->send(200, "text/html", index_html); });
  server.onNotFound(notFound);
  server.begin();
}

// def to_coords(i_s):
//   return list(map(to_coord, i_s))

// draw(to_coords([2,5,7]), num_row,num_col)

// def is_even(x):
//   return x % 2 ==0

// def to_index(coord):
//   x,y = coord
//   if is_even(x):
//     i = x * num_row + y
//   else:
//     # minus 1 because index start with 0
//     i = x * num_row + (num_row -1 - y)
//   return i

void loop()
{
  // The first NeoPixel in a strand is #0, second is 1, all the way up
  // to the count of pixels minus one.
  // send data only when you receive data:
  if (Serial.available() > 0)
  {
    // read the incoming byte:
    String inputStr = Serial.readString();
    Serial.print("I received: ");
    Serial.println(inputStr);

    if (inputStr[0] == 'c')
    {
      Serial.println("Clear command received.");
      pixels.clear();
      pixels.show();
    }
    else
    {
      draw(inputStr);
    }
    // say what you got:
  }

  // pixels.Color() takes RGB values, from 0,0,0 up to 255,255,255
  // Here we're using a moderately bright green color:
  delay(DELAYVAL); // Pause before next pass through loop
}
