#include <Arduino.h>
#include <WiFi.h>

// Reemplace con su nombre de red y contraseña  
const char* ssid = "Fibertel WiFi851 2.4Ghz"; // Nombre de la red
const char* password = "Breakingsequence8704"; // Contraseña de la red 

// Establece el número de puerto para el servidor web
WiFiServer server(80);

// Variables para almacenar el estado actual de las salidas
String output2State = "off";
String output3State = "off";
String output4State = "off";
String output5State = "off";

// Asigna los números de pin para las entradas y salidas
const int output2 = 2;
const int output3 = 3;
const int output4 = 4;
const int output5 = 5;

const int input32 = 32;
const int input33 = 33;
const int input34 = 34;
const int input35 = 35;
const int input36 = 36;
const int input37 = 37;
const int input38 = 38;
const int input39 = 39;

void setup() {
  Serial.begin(115200);
  // Ajusta los pines de salida como salidas
  pinMode(output2, OUTPUT);
  pinMode(output3, OUTPUT);
  pinMode(output4, OUTPUT);
  pinMode(output5, OUTPUT);
  // Inicializa los pines de salida en estado bajo
  digitalWrite(output2, LOW);
  digitalWrite(output3, LOW);
  digitalWrite(output4, LOW);
  digitalWrite(output5, LOW);

  // Conecta a la red Wi-Fi
  Serial.print("Conectando a ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Imprime la dirección IP asignada por el router
  // Esta es la IP que debe usar para acceder al servidor web
  Serial.println("");
  Serial.println("Conexión establecida.");
  Serial.println("Dirección IP: ");
  Serial.println(WiFi.localIP());
  server.begin();
}

void loop(){
  WiFiClient client = server.available();   // Escucha a los clientes que se conecten

  if (client) {                             // Si un nuevo cliente se conecta,
    currentTime = millis();
    previousTime = currentTime;
    Serial.println("Nuevo cliente.");          // imprime un mensaje en el puerto serie
    String currentLine = "";                // crea una variable para almacenar los datos entrantes del cliente
    while (client.connected() && currentTime - previousTime <= timeoutTime) {  // mientras el cliente esté conectado
      currentTime = millis();
      if (client.available())
      {                         // si hay bytes para leer del cliente,
        char c = client.read(); // lee un byte, y luego
        Serial.write(c);        // imprímelo en el monitor serie
        header += c;
        if (c == '\n')
        { // si el byte es un carácter de nueva línea
          // si la línea actual está vacía, tienes dos caracteres de nueva línea seguidos.
          // ese es el final de la solicitud HTTP del cliente, así que envía una respuesta:
          if (currentLine.length() == 0)
          {
            // Las cabeceras HTTP siempre comienzan con un código de respuesta (por ejemplo, HTTP/1.1 200 OK)
            // y un tipo de contenido para que el cliente sepa lo que viene, luego una línea en blanco:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();

            // enciende o apaga el LED
            if (header.indexOf("GET /2/on") >= 0)
            {
              Serial.println("Encendiendo LED");
              output2State = "on";
              digitalWrite(output2, HIGH);
            }
            else if (header.indexOf("GET /2/off") >= 0)
            {
              Serial.println("Apagando LED");
              output2State = "off";
              digitalWrite(output2, LOW);
            }

            // Ahora agregamos la parte de la página web
            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the on/off buttons
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #555555;}</style></head>");
            // Web Page Heading
            client.println("<body><h1>Control LED desde página web</h1>");

            // Display current state, and ON/OFF buttons for GPIO 2
            client.println("<p>Estado LED - " + output2State + "</p>");
            if (output2State == "off")
            {
              client.println("<p><a href=\"/2/on\"><button class=\"button\">ON</button></a></p>");
            }
            else
            {
              client.println("<p><a href=\"/2/off\"><button class=\"button button2\">OFF</button></a></p>");
            }
            client.println("</body></html>");

            // The HTTP response ends with another blank line
            client.println();
            break;
          }
          else
          {
            currentLine = "";
          }
        }
        else if (c != '\r')
        {
          currentLine += c;
        }
      }
    }
    header = "";
    client.stop();
    Serial.println("Cliente desconectado.");
    Serial.println("");
  }
}