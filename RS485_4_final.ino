#include <SPIFFS.h>

#define DE_RE_PIN 4
#define MODE_SEND HIGH
#define MODE_RECV LOW

float soil1 = 0; // Variable para almacenar los datos del primer puerto RS-485
float soil2 = 0; // Variable para almacenar los datos del segundo puerto RS-485

byte bufferPH[7];
byte i = 0;

unsigned long previousMillis = 0; // Almacena el tiempo del último guardado
const unsigned long interval = 600000; // Intervalo de 5 minutos (en milisegundos)300000

float readRS485(HardwareSerial& port, int rxPin, int txPin) {
  uint8_t buff[] = {
    0x01, // Devices Address
    0x03, // Function code
    0x00, // Start Address HIGH
    0x00, // Start Address LOW
    0x00, // Quantity HIGH
    0x01, // Quantity LOW
    0x84, // CRC LOW
    0x0A  // CRC HIGH
  };

  digitalWrite(DE_RE_PIN, MODE_SEND);
  port.write(buff, sizeof(buff));
  port.flush(); // Espera a que se complete MODE_SEND
  digitalWrite(DE_RE_PIN, MODE_RECV);
  delay(500);

  while (port.available()) {
    uint8_t data = port.read();
    bufferPH[i] = data;
    i = i + 1;
    if (bufferPH[0] == 0) {
      //Serial.print("es correcto");
      i = 0;
    }
    // Haz algo con el dato recibido, como imprimirlo o procesarlo
    //Serial.print("Dato recibido: ");
    //Serial.println(data, HEX);
  }

  i = 0;

  // Devuelve el valor de los datos leídos
  return (bufferPH[4]) / 10.0;
}

void setup() {
  pinMode(DE_RE_PIN, OUTPUT);
  digitalWrite(DE_RE_PIN, MODE_RECV);
  Serial.begin(115200);
  Serial1.begin(4800, SERIAL_8N1, 14, 13); // Rx, Tx para Serial1
  Serial2.begin(4800, SERIAL_8N1, 17, 16); // Rx, Tx para Serial2
  Serial1.setTimeout(1000);
  Serial2.setTimeout(1000);

  // Inicializar SPIFFS
  if (!SPIFFS.begin()) {
    Serial.println("Error al montar el sistema de archivos SPIFFS.");
    return;
  }
}

void loop() {
    unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    // Leer datos desde Serial1 (RS-485) y almacenar en soil1
  soil1 = readRS485(Serial1, 14, 13);

  // Leer datos desde Serial2 (RS-485) y almacenar en soil2
  soil2 = readRS485(Serial2, 17, 16);

  Serial.print("Soil 1: ");
  Serial.print(soil1);
  Serial.println(" %");

  Serial.print("Soil 2: ");
  Serial.print(soil2);
  Serial.println(" %");

    // Crear o abrir el archivo para el valor de soil1 en modo "append"
    File file1 = SPIFFS.open("/soil1.txt", "a");
    if (!file1) {
      Serial.println("Error al abrir el archivo soil1.txt");
    } else {
      // Escribir el valor de soil1 en el archivo y agregar una nueva línea
      file1.print(soil1);
      file1.println();
      file1.close();
    }

    // Crear o abrir el archivo para el valor de soil2 en modo "append"
    File file2 = SPIFFS.open("/soil2.txt", "a");
    if (!file2) {
      Serial.println("Error al abrir el archivo soil2.txt");
    } else {
      // Escribir el valor de soil2 en el archivo y agregar una nueva línea
      file2.print(soil2);
      file2.println();
      file2.close();
    }
  }

  // Llamar a la función para procesar comandos desde el puerto serial
  processSerialCommands();
}

void processSerialCommands() {
  while (Serial.available()) {
    String command = Serial.readStringUntil('\n');
    command.trim();

    if (command == "soil1") {
      // Leer y mostrar los datos del archivo soil1.txt
      Serial.println("Contenido de soil1.txt:");
      File file1 = SPIFFS.open("/soil1.txt", "r");
      if (!file1) {
        Serial.println("Error al abrir el archivo soil1.txt");
      } else {
        while (file1.available()) {
          String line = file1.readStringUntil('\n');
          Serial.println(line);
        }
        Serial.println("1122");
        file1.close();
      }
    } else if (command == "soil2") {
      // Leer y mostrar los datos del archivo soil2.txt
      Serial.println("Contenido de soil2.txt:");
      File file2 = SPIFFS.open("/soil2.txt", "r");
      if (!file2) {
        Serial.println("Error al abrir el archivo soil2.txt");
      } else {
        while (file2.available()) {
          String line = file2.readStringUntil('\n');
          Serial.println(line);
        }
        Serial.println("1122");
        file2.close();
      }
    } else if (command == "clear") {
      // Borrar contenido de los archivos soil1.txt y soil2.txt
      File file1 = SPIFFS.open("/soil1.txt", "w");
      File file2 = SPIFFS.open("/soil2.txt", "w");
      file1.close();
      file2.close();
      Serial.println("Archivos soil1.txt y soil2.txt borrados.");
    }
  }
}
