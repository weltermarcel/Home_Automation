/*
Rui Santos
Complete Project Details http://randomnerdtutorials.com
*/

#include <WiFi.h>

// Replace with your network credentials
const char* ssid = "Netgear 2.4";
const char* password = "0798336607";

WiFiServer server(80);

const int led1 = 13;      // the number of the LED pin

						 // Client variables
char linebuf[80];
int charcount = 0;

void setup() {
	// initialize the LEDs pins as an output:
	pinMode(led1, OUTPUT);
	digitalWrite(led1, HIGH);

	//Initialize serial and wait for port to open:
	Serial.begin(115200);
	while (!Serial) {
		; // wait for serial port to connect. Needed for native USB port only
	}

	// We start by connecting to a WiFi network
	Serial.println();
	Serial.println();
	Serial.print("Connecting to ");
	Serial.println(ssid);

	WiFi.begin(ssid, password);

	// attempt to connect to Wifi network:
	while (WiFi.status() != WL_CONNECTED) {
		// Connect to WPA/WPA2 network. Change this line if using open or WEP network:
		delay(500);
		Serial.print(".");
	}
	Serial.println("");
	Serial.println("WiFi connected");
	Serial.println("IP address: ");
	Serial.println(WiFi.localIP());

	server.begin();
}

void loop() {
	// listen for incoming clients
	WiFiClient client = server.available();
	if (client) {
		Serial.println("New client");
		memset(linebuf, 0, sizeof(linebuf));
		charcount = 0;
		// an http request ends with a blank line
		boolean currentLineIsBlank = true;
		while (client.connected()) {
			if (client.available()) {
				char c = client.read();
				Serial.write(c);
				//read char by char HTTP request
				linebuf[charcount] = c;
				if (charcount < sizeof(linebuf) - 1) charcount++;
				// if you've gotten to the end of the line (received a newline
				// character) and the line is blank, the http request has ended,
				// so you can send a reply
				if (c == '\n' && currentLineIsBlank) {
					// send a standard http response header
					client.println("HTTP/1.1 200 OK");
					client.println("Content-Type: text/html");
					client.println("Connection: close");  // the connection will be closed after completion of the response
					client.println();
					client.println("<!DOCTYPE HTML><html><head>");
					client.println("<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"></head>");
					client.println("<h1>Living room</h1>");
					client.println("<p>Floor lamp <a href=\"on1\"><button>ON</button></a>&nbsp;<a href=\"off1\"><button>OFF</button></a></p>");
					client.println("</html>");
					break;
				}
				if (c == '\n') {
					// you're starting a new line
					currentLineIsBlank = true;
					if (strstr(linebuf, "GET /on1") > 0) {
						Serial.println("LED 1 ON");
						digitalWrite(led1, HIGH);
					}
					else if (strstr(linebuf, "GET /off1") > 0) {
						Serial.println("LED 1 OFF");
						digitalWrite(led1, LOW);
					}
					// you're starting a new line
					currentLineIsBlank = true;
					memset(linebuf, 0, sizeof(linebuf));
					charcount = 0;
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
		Serial.println("client disconnected");
	}
}