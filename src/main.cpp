#include<Arduino.h>
#include <WiFi.h>

#include <WebSocketsClient.h>
#include "TZ.h"
#include "TimeUtils.h"

#include "Sensors.h"

const char* wlan_ssid             = "MightyAP";
const char* wlan_password         = "28675449";

const char* ws_host               = "nemo-smartpets.azurewebsites.net";
const int   ws_port               = 443;
const char* ws_baseurl            = "/wss/";

#define timeZone TZ_Europe_Belgrade

String deviceSerialNumber = "HUB-DEV-1";
String deviceSecret = "12345678";

WebSocketsClient webSocket;
TimeUtils *timeUtils = new TimeUtils();


void webSocketEvent(WStype_t type, uint8_t * payload, size_t length);
void subscribeToChannel(String _channelName);
void sendMessage(String _channelName, String message);
void processJsonData(String _received);
String extractString(String _received);
void connectToWifi();
void connectToWebSocket();

void setup() {
  Serial.begin(115200);
  connectToWifi();

  Serial.println("About to get time: ");
  timeUtils->setClock(timeZone);
  Serial.print(timeUtils->getTime());
  
  connectToWebSocket();
}

void loop() {
  webSocket.loop();
}

void hexdump(const void *mem, uint32_t len, uint8_t cols = 16) {
	const uint8_t* src = (const uint8_t*) mem;
	Serial.printf("\n[HEXDUMP] Address: 0x%08X len: 0x%X (%d)", (ptrdiff_t)src, len, len);
	for(uint32_t i = 0; i < len; i++) {
		if(i % cols == 0) {
			Serial.printf("\n[0x%08X] 0x%08X: ", (ptrdiff_t)src, i);
		}
		Serial.printf("%02X ", *src);
		src++;
	}
	Serial.printf("\n");
}



void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {

  switch (type) {
    case WStype_DISCONNECTED:
      Serial.printf("[WSc] Disconnected!\n");
      break;
    case WStype_CONNECTED:
      {
        Serial.printf("[WSc] Connected to url: %s\n",  payload);
      }
      break;
    case WStype_TEXT:
      {

        String text = (char*) payload;
        if (payload[0] == 'h') {

          Serial.println("Heartbeat!");

        } else if (payload[0] == 'o') {

          // on open connection
          char *msg = "[\"CONNECT\\naccept-version:1.1,1.0\\nheart-beat:10000,10000\\n\\n\\u0000\"]";
          webSocket.sendTXT(msg);
          delay(1000);


        } else if (text.startsWith("a[\"CONNECTED")) {

          // subscribe to some channels
          subscribeToChannel(deviceSerialNumber);
          delay(1000);

          Sensors *sData = new Sensors("DEVICE", deviceSerialNumber, timeUtils);

          Serial.println("\n\n\n\n\n" + sData->returnAsJsonMessage() + "\n\n\n\n");

          sendMessage(deviceSerialNumber, sData->returnAsJsonMessage());

        }
        else if (text.startsWith("a[\"MESSAGE")) {
          processJsonData(text);
        }
        break;
      }
      case WStype_BIN:
            hexdump(payload, length);

            // send data to server
            webSocket.sendBIN(payload, length);
            break;
  }
}

void subscribeToChannel(String _channelName) {
  String msg = "[\"SUBSCRIBE\\nid:sub-0\\ndestination:/topic/messages/" + _channelName + "\\n\\n\\u0000\"]";
  webSocket.sendTXT(msg);
}

void sendMessage(String channelName, String message) {
  String messageData =  "[\"SEND\\ndestination:/app/" +
                        channelName + "\\n\\n"+ message +"\\u0000\"]";
                         
  Serial.println(messageData);
  webSocket.sendTXT(messageData);
}

void processJsonData(String _received) {
  String json = extractString(_received);
  json.replace("\\", "");
  Serial.println(json);
  DynamicJsonDocument doc(1024);
  deserializeJson(doc, json);
  JsonObject obj = doc.as<JsonObject>();
  String receivedMessage = obj["message"];
  String username = obj["username"];
  Serial.println(receivedMessage);
}

String extractString(String _received) {
  char startingChar = '{';
  char finishingChar = '}';

  String tmpData = "";
  bool _flag = false;
  for (int i = 0; i < _received.length(); i++) {
    char tmpChar = _received[i];
    if (tmpChar == startingChar) {
      tmpData += startingChar;
      _flag = true;
    }
    else if (tmpChar == finishingChar) {
      tmpData += finishingChar;
      break;
    }
    else if (_flag == true) {
      tmpData += tmpChar;
    }
  }

  return tmpData;

}

void connectToWifi() {
  delay(500);
  Serial.print("Logging into WLAN: "); Serial.print(wlan_ssid); Serial.print(" ...");
  WiFi.mode(WIFI_STA);
  WiFi.begin(wlan_ssid, wlan_password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" success.");
  Serial.print("IP: "); Serial.println(WiFi.localIP());
}

void connectToWebSocket() {
  String socketUrl = ws_baseurl;
  socketUrl += random(0, 999);
  socketUrl += "/";
  socketUrl += random(0, 999999); // should be a random string, but this works
  socketUrl += "/websocket";

  // connect to websocket

  //webSocket.begin(ws_host, ws_port, socketUrl.c_str());
  webSocket.beginSSL(ws_host, ws_port, socketUrl.c_str());
  webSocket.setExtraHeaders((String("auth_username:") + deviceSerialNumber + String("\r\nauth_password:") + deviceSecret).c_str()); 
  webSocket.onEvent(webSocketEvent);
}