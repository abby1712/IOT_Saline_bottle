// Header Files
#include <ESP8266Webhook.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
String value1,value2,value3;

#define KEY "Your IFTTT Webhook Key "        // Webhooks Key
#define EVENT "Your IFTTT Webhook Event "      // Webhooks Event Name
#include "HX711.h"
Webhook webhook(KEY, EVENT);    // Create an object.

//SSID and Password of your WiFi router
const char* ssid = "Your Wifi Name ";
const char* password = "Your Wifi Password";
#define LED 2       //On board LED
#define LED1 D8
#define LED2 D5
#define LED3 D6
#define LED4 D7
//#define Buzzer D3


ESP8266WebServer server(80);
HX711 scale(D2, D1);
float calibration_factor = -226.70;
float units,initial_unit;
float ounces;
float initialWeight=380.00;
float Weight, Percentage;

const char MAIN_page[] PROGMEM = R"=====(
<!doctype html>
<html>
<head>

  <title>Data Logger</title>
  <h1 style="text-align:center; color:red;">IOT Smart Bottle</h1>
  <h3 style="text-align:center;">Making Every Bottle Smart!!!</h3>
  
  <h3 style="text-align:center;">NodeMCU Data Logger</h3>
 
  <style>
  canvas{
    -moz-user-select: none;
    -webkit-user-select: none;
    -ms-user-select: none;
  }
  /* Data Table Styling*/ 
  #dataTable {
    font-family: "Trebuchet MS", Arial, Helvetica, sans-serif;
    border-collapse: collapse;
    width: 100%;
    text-align: center;
  }
  #dataTable td, #dataTable th {
    border: 1px solid #ddd;
    padding: 8px;
  }
  #dataTable tr:nth-child(even){background-color: #f2f2f2;}
  #dataTable tr:hover {background-color: #ddd;}
  #dataTable th {
    padding-top: 12px;
    padding-bottom: 12px;
    text-align: center;
    background-color: #050505;
    color: white;
  }
  </style>
</head>
<body>
<div>
  <table id="dataTable">
    <tr><th>Time</th><th>Weight (Grams)</th><th>Percentage (%)</th></tr>
  </table>
</div>
<br>
<br>  
<script>
var Wvalues = [];
var Pvalues = [];
var timeStamp = [];
setInterval(function() {
  // Call a function repetatively with 2 Second interval
  getData();
}, 2000); //5000mSeconds update rate
 function getData() {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
     //Push the data in array
  var time = new Date().toLocaleTimeString();
  var txt = this.responseText;
  var obj = JSON.parse(txt); 
      Wvalues.push(obj.Weight);
      Pvalues.push(obj.Percentage);
      timeStamp.push(time);
  //Update Data Table
    var table = document.getElementById("dataTable");
    var row = table.insertRow(1); //Add after headings
    var cell1 = row.insertCell(0);
    var cell2 = row.insertCell(1);
    var cell3 = row.insertCell(2);
    cell1.innerHTML = time;
    cell2.innerHTML = obj.Weight;
    cell3.innerHTML = obj.Percentage;
    }
  };
  xhttp.open("GET", "readData", true); //Handle readData server on ESP8266
  xhttp.send();
}
</script>
</body>
</html>

)=====";


 

void handleRoot() {

 String s = MAIN_page; //Read HTML contents
 server.send(200, "text/html", s); //Send web page
}

void readData() {

 String data = "{\"Weight\":\""+ String(Weight) +"\", \"Percentage\":\""+ String(Percentage) +"\"}";
 digitalWrite(LED,!digitalRead(LED)); //Toggle LED on data request ajax
 server.send(200, "text/plane", data); //Send ADC value, Weight and Percentage JSON to client ajax request
 delay(2000);
  scale.set_scale(calibration_factor);
 
  Serial.print("Reading: ");
  units = scale.get_units(), 10;
  if (units < 0)
  {
    units = 0.00;
  }
  ounces = units * 0.035274;
  Serial.print(units);
  Serial.print(" grams");
  Serial.print(" \n");
   
  Weight = units; // Gets the values of the Weight
  Percentage = (Weight/initialWeight)*100; // Gets the values of the Percentage 
  IFTTT_Trigger(Percentage,Weight,value1,value2,value3);
  LED_Display(Percentage);
  //Buzzer_alert(Percentage);

}
void handle_NotFound(){
  server.send(404, "text/plain", "Not found");
}
void setup ()
{
  Serial.begin(115200);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  pinMode(LED4, OUTPUT);
 //pinMode(Buzzer, OUTPUT);
  Serial.println();

  WiFi.begin(ssid, password);     //Connect to your WiFi router
  Serial.println("");
  //Onboard LED port Direction output
  pinMode(LED,OUTPUT); 
  digitalWrite(LED1, HIGH);
  digitalWrite(LED2, HIGH);
  digitalWrite(LED3, HIGH);
  digitalWrite(LED4, HIGH);
  
  
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
 
  //If connection successful show IP address in serial monitor
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());  //IP address assigned to your ESP
 
  server.on("/", handleRoot);      //Which routine to handle at root location. This is display page
  server.on("/readData", readData); //This page is called by java Script AJAX
 
  server.begin();                  //Start server
  Serial.println("HTTP server started");
  scale.set_scale(500);
  Serial.println("Enter Patient's Name:  ");
  value1= Serial.readStringUntil('\n');
  delay(10000);
  Serial.println("Enter Patient's Room Number:  ");
  value2= Serial.readStringUntil('\n');//Room Number
  delay(8000);
  
  Serial.println("Enter Patient's age:  ");
  value3=Serial.readStringUntil('\n');//age
  delay(8000);

  

  Serial.println("Remove Weight \n ");
  delay(200);
  Serial.println("1....\n ");
   digitalWrite(LED1, LOW);
  digitalWrite(LED2, HIGH);
  digitalWrite(LED3, HIGH);
  digitalWrite(LED4, HIGH);
  delay(500);
  Serial.println("2....\n ");
  digitalWrite(LED1, LOW);
  digitalWrite(LED2, LOW);
  digitalWrite(LED3, HIGH);
  digitalWrite(LED4, HIGH);
    delay(500);
  Serial.println("3....\n ");
   digitalWrite(LED1, LOW);
  digitalWrite(LED2, LOW);
  digitalWrite(LED3, LOW);
  digitalWrite(LED4, HIGH);
    delay(500);
  Serial.println("4....\n ");
   digitalWrite(LED1, LOW);
  digitalWrite(LED2, LOW);
  digitalWrite(LED3, LOW);
  digitalWrite(LED4, LOW);
  delay(500);
  Serial.println(" Load Cell in Tare Mode....\n ");
   digitalWrite(LED1, LOW);
  digitalWrite(LED2, HIGH);
  digitalWrite(LED3, HIGH);
  digitalWrite(LED4, LOW);
  
    delay(1000);
  scale.tare();  //Reset the scale to 0
  Serial.println("  Tare Mode Completed ....Place Saline \n ");
   digitalWrite(LED1, LOW);
  digitalWrite(LED2, LOW);
  digitalWrite(LED3, LOW);
  digitalWrite(LED4, LOW);
  delay(500);
   digitalWrite(LED1, HIGH);
  digitalWrite(LED2, HIGH);
  digitalWrite(LED3, HIGH);
  digitalWrite(LED4, HIGH);
  
   
 
}
void IFTTT_Trigger(int percentage,int weight,String value1,String value2,String value3)
{  if (weight>50){
  if(percentage<25){
  webhook.trigger(value1,value2,value3);
}

}
}

void loop(void){
  
  server.handleClient();          //Handle client requests
}

// LED Alert System
void LED_Display(int percentage)
{
if(percentage>75){
    digitalWrite(LED1, HIGH);
  digitalWrite(LED2, HIGH);
  digitalWrite(LED3, HIGH);
  digitalWrite(LED4, HIGH);
  
  
}
if(percentage<75){
    digitalWrite(LED1, LOW);
  digitalWrite(LED2, HIGH);
  digitalWrite(LED3, HIGH);
  digitalWrite(LED4, HIGH);
  
  
}
if(percentage<50){
    digitalWrite(LED1, LOW);
  digitalWrite(LED2, LOW);
  digitalWrite(LED3, HIGH);
  digitalWrite(LED4, HIGH);
  
  
}
if(percentage<25){
    digitalWrite(LED1, LOW);
  digitalWrite(LED2, LOW);
  digitalWrite(LED3, LOW);
  digitalWrite(LED4, HIGH);
}
}
