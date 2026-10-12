#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <SD.h>
#include "Update.h"
#include <Preferences.h>
#include <WiFi.h>
// #include <ESPAsyncWebServer.h>
// #include <AsyncTCP.h>
//#include "HX711.h"

LiquidCrystal_I2C lcd(0x3f, 20, 4);
Preferences preferences;
//HX711 hx;

#define potPin 13
#define buttonPin 4  
#define PINO_CS 10
#define RXD2 16
#define TXD2 17
#define LOADCELL_DOUT_PIN 27
#define LOADCELL_SCK_PIN 14

const char* ssid = "SCD";
const char* password = "123456789";

void setup() {
  lcd.init();
  lcd.backlight();
  Serial.begin(115200);
  
  pinMode(buttonPin, INPUT_PULLDOWN);
  pinMode(potPin, INPUT);
  pinMode(LOADCELL_DOUT_PIN, INPUT_PULLUP);  //entrada para receber os dados
  pinMode(LOADCELL_SCK_PIN, OUTPUT);

  // Cria o Mutex que irá proteger o acesso ao SD Card e LittleFS
  // sdMutex = xSemaphoreCreateMutex();
  
  //hx.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  lcd.clear();
  while (!SD.begin()) {
    Serial.println("Falha ao inicializar o cartão SD.");
    Serial.println("[LCD]: FALHA NO SD");
    lcd.setCursor(0, 2);
    lcd.print("FALHA NO SD");
    delay(1000);
  }

  preferences.begin("storage", false);
  preferences.putInt("cont", preferences.getInt("cont", 0) + 1);
  int pathSave = "/Coleta_" + String(preferences.getInt("cont", 0)) + ".txt";
  Serial.println("SCD inicializado com sucesso.");
}

void loop() {

}

void printMenu() {
  lcd.clear();
  lcd.setCursor(4, 0);
  lcd.print("PRD - SCD");
  lcd.setCursor(2, 3); // (coluna, linha)
  lcd.print("Iniciar coleta");
  lcd.setCursor(2, 4);
  lcd.print("Transmitir dados");
}

bool readPot() {
  int valuePot = analogRead(potPin);
  bool cursorPosition;

  if (valuePot > 2043) {
    cursorPosition = HIGH;
  } else {
    cursorPosition = LOW;
  }

  return cursorPosition;
}

void readBut() {

}

void printCursor() {
  bool cursorPosition = readPot();

  lcd.setCursor(1, 3);
  lcd.print(" ");
  lcd.setCursor(1, 4);
  lcd.print(" ");

  if (cursorPosition == HIGH) {
    lcd.setCursor(1, 3);
    lcd.print(">");
  } else {
    lcd.setCursor(1, 4);
    lcd.print(">");
  }
}

void printCollection() {
  lcd.clear();
  lcd.setCursor(4, 0);
  lcd.print("PRD - SCD");
  lcd.setCursor(1, 1);
  lcd.print("COLETANDO DADOS...");
  lcd.setCursor(1, 2);
  lcd.print(pathSave);
}

void printTransmission() {
  lcd.clear();
  lcd.setCursor(4, 0);
  lcd.print("PRD - SCD");
  lcd.setCursor(1, 2);
  lcd.print("REDE: ");
  lcd.print(ssid);
  lcd.setCursor(1, 3);
  lcd.print("SENHA: ");
  lcd.print(password);
}

void readLoadCell() {
  unsigned long count = 0; //unsigned são apenas valores positivos
  unsigned char i;
  if (!digitalRead(LOADCELL_DOUT_PIN)) { //ele inicia alto pois não há nenhuma informação
    int col_time = millis(); //coleta o tempo em mili segundos
    digitalWrite(LOADCELL_SCK_PIN, LOW); //apenas confirma qu está baixo

    for (i = 0; i < 24; i++) {
      digitalWrite(LOADCELL_SCK_PIN, HIGH); //o sck sobe
      count = count << 1; //está preparando para escrever o próximo bit
      digitalWrite(LOADCELL_SCK_PIN, LOW); //significa que ele está pronto para receber
      if (digitalRead(LOADCELL_DOUT_PIN)) count++; //lê o bit e escreve no count

    }  

    //protocolo para sinalizar que leu o último bit
    digitalWrite(LOADCELL_SCK_PIN, HIGH);
    count = count ^ 0x800000;
    digitalWrite(LOADCELL_SCK_PIN, LOW);
    return (count);
  }
}

void coletaEmp() {
  emp = readLoadCell();
  unsigned long t1;
  if (coleta == true) {
    //Serial.println(emp);
    if (emp != 0) {
      save();
    }
  } else {
    if (activedMenu == 1) {
      if (millis() - fz_lcd > 500) {
        fz_lcd = millis();
        lcd.setCursor(9, 3);
        lcd.print("        ");
        lcd.setCursor(9, 3);
        lcd.print(emp);
      }
    }
  }
}

void printValues() {

}
