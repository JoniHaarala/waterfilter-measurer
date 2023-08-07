/*
  Water flow/min and total volume control for a UV wather filter.

  This program drives a unipolar water flow sensor model YF-B1.
  At the same time, store the volume data to a SD card using a Datalogger drive.
  This is to prevent loose volume data if light go off.


  Created 17 May 2023
  (Modified...)
  create by HAARALA, Jonatan
*/
/*
  ESPECIFICACIONES del caudalimetro:
  Modelo: YF-SB1
  Tipo de sensor: efecto Hall
  Material: LATÓN Plateado
  Rosca externa: G 1/2"
  Posición de montaje: Horizontal Vertical
  Tensión de trabajo: 5V - 18V. (recomendado 12)
  Corriente de funcionamiento Máx.: 15mA (5V)
  Tipo de salida: 5V TTL
  Resistencia de aislamiento> 100MΩ
  Caudal de trabajo: 1 L/m a 30 L/m.
  Presión máxima del agua: 1.75MPa
  Precisión: ± 2% -5%
  Temperatura de funcionamiento: -40ºC a +80ºC.
  Temperatura del líquido: No más de 120°C
  Rango de humedad de trabajo: 25% -95% HR
  Ciclo de trabajo de salida: 50% + -10%
  Pulsos de salida: 477 pulsos por litro de agua (16 a 357Hz)
  Pulso de velocidad de flujo: Frecuencia (Hz) = 11 * Q ± 5% *Caudal (L / min)
  Dimensiones:44mm x 28mm x 28 mm
  Cable Rojo: Conector alimentación positivo.
  Cable Negro: Conector alimentación negativo.
  Cable Amarillo: Salida de señal.
  Longitud del cable: 30cm
  Grado de protección de entrada: IP65
  Peso: 76g
*/

/* ++++++++++ LIBRARIES ++++++++++ */
#include <SPI.h>
#include <SD.h>     // librerias de la lectora SD  
#include <LiquidCrystal_I2C.h>  // libreria de la pantalla
#include <Wire.h>

LiquidCrystal_I2C lcd(0x3f, 16, 2);

char filename[] = "DATALOG.txt";
File sdFile;

// GLOBAL VARIABLES

// water flow sensor variables
const int SENSOR_PIN = 2; // n° de pin para el sensor de flujo
const int RELE_PIN = 3;
const int LED_PIN = 8;
const int SD_PIN = 10;
const int measureInterval = 2500;
volatile int pulseConter;
unsigned long frequency;
float volume = 0;
long t0 = 0;
float WATER = 0;
unsigned long datoRestaurado;
float TOTAL = 0;

// Velocidad de flujo K para los distintos modelos de caudalimetros
// YF-S201
//const float factorK = 7.5;

// YF-B1
const float factorK = 11;

// FS400A
//const float factorK = 3.5;

// FS300A
//const float factorK = 5.5;

/************** FUNCTIONS *******************/
unsigned long readSD() {
  unsigned long v;
  File archivo = SD.open(filename);

  if (archivo) { // Sé que no es tipo bool, pero es compatible en contexto booleano. true: el archivo se pudo abrir; false: el archivo no se pudo abrir

    //archivo.readBytes((byte*)&v, sizeof(unsigned long)); // Lectura binaria
    v = archivo.parseInt(); // Lectura textualizada
    archivo.close(); // Siempre es buena práctica cerrar un archivo cuando no se necesita
  }
  else Serial.println(F("El archivo no se pudo abrir o no existe; el contador iniciara desde cero"));

  return v;
}

void CountPulse()
{
  pulseConter++;
}


/*++++++++++++++++++ MAIN PROGRAM ++++++++++++++++++*/
void setup()
{
  Serial.begin(9600);

  pinMode(SENSOR_PIN, INPUT);
  pinMode(RELE_PIN, OUTPUT);
  pinMode(SD_PIN, OUTPUT);

  Wire.begin();
  lcd.init();
  lcd.backlight();

  interrupts();

  t0 = millis();
  attachInterrupt(digitalPinToInterrupt(SENSOR_PIN), CountPulse, RISING);

  lcd.setCursor(3, 0);
  lcd.println("INICIANDO");
  lcd.setCursor(4, 1);
  lcd.print("SISTEMA");
  delay(3000);

  /*
     Esto es un chequeo por si el sistema se apaga.
     Leemos el archivo de forma inicial de forma que guarde en una variable
     el ultimo valor medido de litros totales
  */
  // Verifica que la SD card funciona correctamente
  if (!SD.begin(10)) {
    lcd.clear();
    lcd.print("dont work!");
  }
  delay(1000);
  // Si el archivo no existe lo crea, sino avanza
  if (!SD.exists(filename)) {
    sdFile = SD.open(filename, FILE_WRITE);
    if (sdFile) {
      lcd.print("FILE CREATED!");
      sdFile.close();
    }
    else {
      lcd.print("COULDN'T CREATE!");
    }
  }
  else {
    sdFile = SD.open(filename);
    if (sdFile.size() > 0) {
      
      lcd.clear();
      lcd.setCursor(2, 0);
      lcd.println("RESTAURANDO");
      lcd.setCursor(0, 1);
      lcd.print("DATOS MEDIDOS...");
      
      // Restaura el dato de litros totales consumidos en caso de un interrupcion
      datoRestaurado = readSD();
      TOTAL += (int)datoRestaurado;
      delay(1000);
      lcd.print("DATA RESTORED");
    }
    sdFile.close();
    lcd.clear();
  }
}

void loop()
{
  if (millis() - t0 > 1000) {
    t0 = millis();
    frequency += pulseConter;
    Serial.println(frequency);

    float flow_Lmin = (pulseConter / factorK);
    WATER = frequency * 1.0 / 4770;
    pulseConter = 0;

    if (frequency >= 0)
    {
      if (flow_Lmin == 0)
      {
        digitalWrite(RELE_PIN, HIGH);
        digitalWrite(LED_PIN, LOW);
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Vol: 0.00");
        lcd.print(" L/min");
        lcd.setCursor(0, 1);
        lcd.print("Total: ");
        lcd.print(TOTAL);
        lcd.print(" L");
      }
      else
      {
        digitalWrite(RELE_PIN, LOW);
        digitalWrite(LED_PIN, HIGH);
        SD.remove(filename);
        sdFile = SD.open(filename, FILE_WRITE);
        TOTAL += WATER;
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Vol: ");
        lcd.print(flow_Lmin, 1);
        lcd.print("L/min");
        lcd.setCursor(0, 1);
        lcd.print("Total: ");
        lcd.print(TOTAL, 1);
        lcd.print(" L");

        if (sdFile)
        {
          sdFile.println(TOTAL, 0);
          Serial.println("GUARDADO");
          sdFile.close();
        }
        else
        {
          Serial.println("error al guardar");
        }
      }
    }
    Serial.println("FRECUENCIA NULA AHORA");
  }
}
