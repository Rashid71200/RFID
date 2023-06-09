#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>


#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN D8
#define RST_PIN D0
#define LED_1 D4

MFRC522 rfid(SS_PIN, RST_PIN); // Instance of the class

MFRC522::MIFARE_Key key;

// Init array that will store new NUID



const char* ssid = "Ahrniloy";
const char* password = "08444554";


bool ledState = 0;
//const int ledPin = D4;

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
<html>
<head>
  <title>ESP Web Server</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style>
    html {
      font-family: Arial, Helvetica, sans-serif;
      text-align: center;
    }
    h1 {
      font-size: 1.8rem;
      color: white;
    }
    h2 {
      font-size: 1.5rem;
      font-weight: bold;
      color: #143642;
    }
    .topnav {
      overflow: hidden;
      background-color: #143642;
    }
    body {
      margin: 0;
    }
    .content {
      padding: 30px;
      max-width: 600px;
      margin: 0 auto;
    }
    .card {
      background-color: #F8F7F9;
      box-shadow: 2px 2px 12px 1px rgba(140, 140, 140, 0.5);
      padding-top: 10px;
      padding-bottom: 20px;
    }
    .button {
      padding: 15px 50px;
      font-size: 24px;
      text-align: center;
      outline: none;
      color: #fff;
      background-color: #0f8b8d;
      border: none;
      border-radius: 5px;
      -webkit-touch-callout: none;
      -webkit-user-select: none;
      -khtml-user-select: none;
      -moz-user-select: none;
      -ms-user-select: none;
      user-select: none;
      -webkit-tap-highlight-color: rgba(0, 0, 0, 0);
    }
    /*.button:hover {background-color: #0f8b8d}*/
    .button:active {
      background-color: #0f8b8d;
      box-shadow: 2 2px #CDCDCD;
      transform: translateY(2px);
    }
    .state {
      font-size: 1.5rem;
      color: #8c8c8c;
      font-weight: bold;
    }

    body {
      font-family: "Roboto", sans-serif;
    }

    h1 {
      text-align: center;
    }

    table,
    form {
      width: 500px;
      margin: 20px auto;
    }

    table {
      border-collapse: collapse;
      text-align: center;
    }

    table td,
    table th {
      border: solid 1px black;
    }

    label,
    input {
      display: block;
      margin: 10px 0;
      font-size: 20px;
    }

    /* Added CSS properties */
    form {
      display: flex;
      flex-direction: column;
      align-items: center;
    }

    button {
      margin-top: 10px;
    }

    table {
      margin-top: 40px;
    }
  </style>
</head>
<body>
  <div class="topnav">
    <h1>ESP WebSocket Server</h1>
  </div>
  <div class="content">
    <div class="card">
      <h2>ESP8266</h2>
      <p class="state">state: <span id="ID">%STATE%</span></p>
    </div>
    <form>
      <div class="input-row">
        <label for="url">Url</label>
        <input type="url" name="url" id="url" />
      </div>
      <div class="input-row">
        <label for="name">Name</label>
        <input type="text" name="name" id="name" />
      </div>
    </form>
    <button onclick="register()">Register</button>
  </div>
  <table>
    <thead>
      <tr>
        <th>User</th>
        <th>NUID tag</th>
        <th></th>
      </tr>
    </thead>
    <tbody></tbody>
  </table>
  <h2>© Azaharul Rashid</h2>
  <script>
    var gateway = `ws://${window.location.hostname}/ws`;
    var websocket;

    const person = {}; // Fix: Changed from array to object

    person["19519436183"] = "Azaharul Rashid";

    const tbodyEl = document.querySelector("tbody");
    const tableEl = document.querySelector("table");
    const formEl = document.querySelector("form");
    const urlInputEl = document.getElementById("url");
    const urlInputEl1 = document.getElementById("name");

    window.addEventListener('load', onLoad);

    function initWebSocket() {
      console.log('Trying to open a WebSocket connection...');
      websocket = new WebSocket(gateway);
      websocket.onopen = onOpen;
      websocket.onclose = onClose;
      websocket.onmessage = onMessage;
    }

    function onOpen(event) {
      console.log('Connection opened');
    }

    function onClose(event) {
      console.log('Connection closed');
      setTimeout(initWebSocket, 2000);
    }

    function onMessage(event) {
      var state;
      if (event.data) {
        state = event.data;

        if (person[state] === undefined) {
          onAddWebsite("Unauthorized Access");
          urlInputEl.value = state; // Set the value of the URL input to the received state
        } else {
          onAddWebsite(person[state]);
          urlInputEl.value = ""; // Set the value of the URL input to the received state
          urlInputEl1.value = ""; // Set the value of the URL input to the received state
        }
      } else {
        state = "OFF";
      }
      document.getElementById('ID').innerHTML = state;
    }

    function onLoad(event) {
      initWebSocket();
      addDeleteButtonListener(); // Add event listener for Delete buttons
    }

    function onAddWebsite(website1) {
      const website = website1;

      const row = document.createElement('tr'); // Create a new table row element
      row.innerHTML = `
        <td>${website}</td>
        <td><button class="deleteBtn">Delete</button></td>
      `;

      tbodyEl.appendChild(row); // Append the new row to the table body

      const deleteButton = row.querySelector('.deleteBtn');
      deleteButton.addEventListener('click', onDeleteButtonClick); // Add event listener to the Delete button
    }

    function onDeleteButtonClick(event) {
      const button = event.target;
      const row = button.closest('tr'); // Find the closest parent table row
      row.remove(); // Remove the row from the table
    }

    function register() {
      const website = document.getElementById("name").value;
      const url = document.getElementById("url").value;

      person[url] = website;
      alert ("register successful");
    }

  </script>
</body>
</html>

)rawliteral";

void notifyClients(const char* vars) {             //Send Message to the Browser.
  ws.textAll(String(vars));
}


void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,
             void *arg, uint8_t *data, size_t len) {
    switch (type) {
      case WS_EVT_CONNECT:
        Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
        break;
      case WS_EVT_DISCONNECT:
        Serial.printf("WebSocket client #%u disconnected\n", client->id());
        break;
      case WS_EVT_DATA:
        //handleWebSocketMessage(arg, data, len);
        break;
      case WS_EVT_PONG:
      case WS_EVT_ERROR:
        break;
  }
}

void initWebSocket() {
  ws.onEvent(onEvent);
  server.addHandler(&ws);
}

void setup() {
  Serial.begin(115200);
  
  
    WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  // Print ESP Local IP Address
  Serial.println(WiFi.localIP());

  initWebSocket();

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html);
  });

  // Start server
  server.begin();
  
  
  SPI.begin(); // Init SPI bus
  pinMode(LED_1, OUTPUT);
  rfid.PCD_Init(); // Init MFRC522
  Serial.println();
  Serial.print(F("Programd By AZAHARUL RASHID"));
  rfid.PCD_DumpVersionToSerial();
  digitalWrite(LED_1, LOW);

  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }

  Serial.println();
  Serial.println(F("This code scan the MIFARE Classic NUID."));
  Serial.print(F("Using the following key:"));
  printHex(key.keyByte, MFRC522::MF_KEY_SIZE);
}


void Clients( const char* massege1 ) {
      notifyClients(massege1);
      delay(100);

}

void loop() {

  ws.cleanupClients();
  
  // Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
  if ( ! rfid.PICC_IsNewCardPresent())
    return;

  // Verify if the NUID has been readed
  if ( ! rfid.PICC_ReadCardSerial())
    return;

  Serial.print(F("PICC type: "));
  MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
  Serial.println(rfid.PICC_GetTypeName(piccType));

  // Check is the PICC of Classic MIFARE type
  if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI &&
      piccType != MFRC522::PICC_TYPE_MIFARE_1K &&
      piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
    Serial.println(F("Your tag is not of type MIFARE Classic."));
    return;
  }


  else {
  
    Serial.println(F("The NUID tag is:"));

    Serial.print(F("In Byte: "));
    printByte(rfid.uid.uidByte, rfid.uid.size);
    
    Serial.println();
    
  }

  // Halt PICC
  rfid.PICC_HaltA();

  // Stop encryption on PCD
  rfid.PCD_StopCrypto1();
}


/**
   Helper routine to dump a byte array as hex values to Serial.
*/
void printHex(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }
}

String convertToString(byte *buffer, byte bufferSize) {
  String str;
  for (byte i = 0; i < bufferSize; i++) {
    str += String(buffer[i]);
  }
  return str;
}

void printByte(byte *buffer, byte bufferSize) {
  String str = convertToString(buffer, bufferSize);
  const char* charPtr = str.c_str();
  notifyClients(charPtr);
  Serial.println(str);
}


void card_detected(){
  
  Serial.println(F("LED SHOULD BE HIGH"));
  digitalWrite(LED_1, HIGH);
  delay(1000);
  
  }
