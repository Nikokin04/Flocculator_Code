//Código Control Velocidad Motor con Temporizador - NSE Soluciones Industriales S.A.S

//Librerías
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "FireTimer.h"

//Instancias

//Reinicio Proceso
void (*resetFunc)(void) = 0;

//Potenciómetro
unsigned int AnalogValue = 0;
unsigned int RPM_Display = 0;
unsigned int RPM = 0;

//LCD
LiquidCrystal_I2C lcd(0x27, 16, 2);

//Botones
const int AscendingButtonPin = 2;
const int DescendingButtonPin = 3;
const int BeginButtonPin = 4;
unsigned long ultimoTiempo = 0;

//Temporizadores
unsigned long variableDeTiempo = 0;
int contador = 0;
int contador2 = 0;
FireTimer Temporizador;

//Motor
const int MotorA = 9;
const int MotorB = 10;

void setup() {
  //Comunicación Serial
  Serial.begin(9600);

  //Inicialización Pantalla LCD
  lcd.init();
  lcd.backlight();

  lcd.setCursor(3, 0);
  lcd.print("EQUIPO  DE");
  lcd.setCursor(5, 1);
  lcd.print("JARRAS");

  delay(2000);
  lcd.clear();

  lcd.setCursor(1, 0);
  lcd.print("NSE SOLUCIONES");
  lcd.setCursor(0, 1);
  lcd.print("INDUSTRIALES SAS");

  delay(2000);
  lcd.clear();

  lcd.setCursor(7, 0);
  lcd.print("PML");
  lcd.setCursor(0, 1);
  lcd.print("INGENIERIA LTDA");

  delay(2000);
  lcd.clear();

  lcd.setCursor(1, 0);
  lcd.print("Motor:");
  lcd.setCursor(1, 1);
  lcd.print("Tiempo:");

  lcd.setCursor(8, 1);
  lcd.print(int(variableDeTiempo));
  lcd.setCursor(12, 1);
  lcd.print("Min");

  //Inicialización Botones
  pinMode(AscendingButtonPin, INPUT_PULLUP);
  pinMode(DescendingButtonPin, INPUT_PULLUP);
  pinMode(BeginButtonPin, INPUT_PULLUP);

  //Inicialización Motor
  pinMode(MotorA, OUTPUT);
  pinMode(MotorB, OUTPUT);
}

void loop() {
  //Potenciómetro Lectura - Visualización
  AnalogValue = analogRead(A0);
  if (contador != 1) {
    RPM_Display = map(AnalogValue, 0, 1023, 30, 170);
    RPM = map(AnalogValue, 0, 1023, 152, 255);
    lcd.setCursor(7, 0);
    lcd.print(RPM_Display < 10 ? "00" : RPM_Display < 100 ? "0" : "");
    lcd.print(RPM_Display);
    lcd.setCursor(11, 0);
    lcd.print("RPM");
  }
  //Botones
  if (digitalRead(AscendingButtonPin) == HIGH && millis() - ultimoTiempo >= 200) {
    variableDeTiempo += 1;
    lcd.setCursor(8, 1);
    lcd.print(int(variableDeTiempo));
    lcd.setCursor(12, 1);
    lcd.print("Min");
    ultimoTiempo = millis();
  } else if (digitalRead(DescendingButtonPin) == HIGH && millis() - ultimoTiempo >= 200) {
    if (variableDeTiempo > 0) {
      variableDeTiempo -= 1;
      lcd.setCursor(8, 1);
      lcd.print(int(variableDeTiempo));
      lcd.setCursor(12, 1);
      lcd.print("Min");
      ultimoTiempo = millis();
    } else {
      variableDeTiempo = 0;
    }
  }
  //Temporizadores y Motor
  if (digitalRead(BeginButtonPin) == HIGH && millis() - ultimoTiempo >= 200) {
    contador = 1;
    lcd.clear();
    ultimoTiempo = millis();
  }
  if (contador == 1) {
    CuentaRegresiva(variableDeTiempo);
    contador2 = contador2 + 1;
  }
  if (contador2 == 1) {
    lcd.clear();
    analogWrite(MotorA, 0);
    analogWrite(MotorB, 0);
    lcd.setCursor(5, 0);
    lcd.print("Tiempo");
    lcd.setCursor(3, 1);
    lcd.print("Finalizado");
    delay(2000);
    resetFunc();
  }
}

//Funciones

void CuentaRegresiva(unsigned long Minutos) {
  const unsigned long Regresiva = Minutos * 60000U;
  unsigned int SegundosRestantes = Minutos * 60;

  unsigned long tAnteriorSeg = 0;
  const unsigned long Inicio = millis();
  Temporizador.begin(Regresiva);

  while (millis() - Inicio < Regresiva) {
    unsigned long tActualSeg = millis();

    AnalogValue = analogRead(A0);
    RPM_Display = map(AnalogValue, 0, 1023, 30, 170);
    RPM = map(AnalogValue, 0, 1023, 152, 255);
    lcd.setCursor(9, 1);
    lcd.print(RPM_Display < 10 ? "00" : RPM_Display < 100 ? "0" : "");
    lcd.print(RPM_Display);
    lcd.setCursor(13, 1);
    lcd.print("RPM");

    analogWrite(MotorA, RPM);
    analogWrite(MotorB, 0);

    if (tActualSeg - tAnteriorSeg >= 1000) {
      // Código para variar la velocidad del motor mientras se encuentra en cuenta regresiva
      tAnteriorSeg = tActualSeg;

      unsigned int minutos = SegundosRestantes / 60;
      unsigned int segundos = SegundosRestantes % 60;

      // Mostrar en la LCD
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Tiempo");
      lcd.setCursor(8, 0);
      lcd.print("Restante ");
      lcd.setCursor(0, 1);
      lcd.print(minutos);
      lcd.print(":");
      if (segundos < 10) lcd.print("0");  // Agregar un 0 para segundos < 10
      lcd.print(segundos);
      lcd.setCursor(5, 1);
      lcd.print("Min");

      SegundosRestantes--;
    }
  }
}
