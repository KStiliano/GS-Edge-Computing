#include <LiquidCrystal.h>
#include <DHT.h>
#include <Servo.h>

// LCD
const int rs = 2, en = 3, d4 = 4, d5 = 5, d6 = 6, d7 = 7;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// DHT Sensor (simulando sensor de oxigênio no lugar do de humidade)
#define DHTPIN A0
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

// LDR
float valor_ldr;

// Servo Motor
Servo servo;
const int servoPin = 9;
int pos = 0;
int increment = 1;
int startMotor = 6;
int counterRotation = 0;
unsigned long previousMillis = 0;
const long interval = 20; 
 
// Sensor de pH (Potentiometer 1)
const int pHpin = A2;
float pHValue;
float pHRead;

// Sensor de gás CO2 (Potentiometer 2)
const int gasPinCO2 = A3;
int CO2Value;

// Sensor de gás CH4 (Potentiometer 3)
const int gasPinCH4 = A4;
int CH4Value;

// Sensor de proximidade
const int trigPin = 10;
const int echoPin = 1; 
long duration;
int distance;

// Buzzer
const int buzzerPin = 8;

// LED Pins
const int ledR = 13;
const int ledY = 12;
const int ledG = 11;

void displayTwoLineText(String str1, String str2) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(str1);
  lcd.setCursor(0, 1);
  lcd.print(str2);
}

void activateWhichLED(int led) {
  digitalWrite(ledR, LOW);
  digitalWrite(ledY, LOW);
  digitalWrite(ledG, LOW);
  digitalWrite(led, HIGH);
}

void sonarBeep(int qntdBeeps = 3) {
  int i = 0; 
  while (i < qntdBeeps){
    tone(buzzerPin, 1000);
    delay(1500);
    noTone(buzzerPin);
    delay(1500);
    i++;
  }
}

void rotacaoServoMotor(int velocidade = 5) {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis; 
    pos += increment * velocidade;
    // Se a posição estiver fora do intervalo de 0 a 180 graus, inverte a direção
    if (pos >= 180 || pos <= 0) {
      increment = -increment;
      counterRotation += 1;
    }
    servo.write(pos); // Escreve a nova posição do servo
  }
}

void testaLuminosidade() {
  String luminosidadeTexto = "Luminosidade: ";
  String luminosidadeMetrica = "%";
  valor_ldr = map(analogRead(A1), 8, 1016, 100, 0);
  String exibirLux = valor_ldr + luminosidadeMetrica;
  displayTwoLineText(luminosidadeTexto, exibirLux);
  if (valor_ldr >= 70) {
    activateWhichLED(ledG);
    noTone(buzzerPin);
    delay(2000);
    displayTwoLineText("Claridade", "normal!");
  } 
  else if (valor_ldr < 70 && valor_ldr >= 50) {
    activateWhichLED(ledY);
    noTone(buzzerPin);
    delay(2000);
    displayTwoLineText("Claridade", "em alerta!");
  } 
  else {
    activateWhichLED(ledR);
    tone(buzzerPin, 500);
    delay(2000);
    displayTwoLineText("Claridade", "critica!");
  }
  delay(2000);
}

void testaOxigenio() {
  String oxigenioTexto = "Oxigenio: ";
  String oxigenioMetrica = "mg/L";
  float oxigenio = dht.readHumidity();  // Usando a leitura de humidade como oxigênio
  float oxigenioMgL = map(oxigenio, 0, 100, 0, 15); // Converte % para mg/L
  String exibirOxy = oxigenioMgL + oxigenioMetrica;
  displayTwoLineText(oxigenioTexto, exibirOxy);
  
  if (oxigenioMgL <= 2) {
    activateWhichLED(ledR);
    tone(buzzerPin, 500);
    delay(2000);
    displayTwoLineText("Nivel", "critico!");
  } 
  else if (oxigenioMgL > 2 && oxigenioMgL < 5) {
    activateWhichLED(ledY);
    noTone(buzzerPin);
    delay(2000);
    displayTwoLineText("Nivel", "em alerta!");
  } 
  else {
    activateWhichLED(ledG);
    noTone(buzzerPin);
    delay(2000);
    displayTwoLineText("Nivel", "normal!");
  }
  delay(2000);
}

void testaTemperatura() {
  String temperaturaTexto = "Temperatura: ";
  String temperaturaMetrica = "C";
  float temperatura = dht.readTemperature();
  String exibirTemp = temperatura + temperaturaMetrica;
  displayTwoLineText(temperaturaTexto, exibirTemp);

  if (temperatura >= 30) {
    activateWhichLED(ledR);
    tone(buzzerPin, 500);
    delay(2000);
    displayTwoLineText("Temperatura", "elevada!");
  } 
  else if (temperatura < 30 && temperatura >= 20) {
    activateWhichLED(ledG);
    noTone(buzzerPin);
    delay(2000);
    displayTwoLineText("Temperatura", "normal!");
  } 
  else {
    activateWhichLED(ledR);
    tone(buzzerPin, 500);
    delay(2000);
    displayTwoLineText("Temperatura", "abaixo!");
  }
  delay(2000);
}

void read_pH() {
  pHRead = analogRead(pHpin);
  float pHValue = map(pHRead, 0, 1023, 0, 14); // Simulação de pH de 0 a 14
  String phTexto = "pH: ";
  String exibirPH = phTexto + pHValue;
  displayTwoLineText(exibirPH, "");

  if (pHValue < 7.5) {
    activateWhichLED(ledR);
    tone(buzzerPin, 500);
    delay(2000);
    displayTwoLineText("Acidificacao", "");
  } 
  else if (pHValue > 8.4) {
    activateWhichLED(ledR);
    tone(buzzerPin, 500);
    delay(2000);
    displayTwoLineText("Alcalinidade", "excessiva!");
  } 
  else {
    activateWhichLED(ledG);
    noTone(buzzerPin);
    delay(2000);
    displayTwoLineText("Concentracao", "normal!");
  }
  delay(2000);
}

void read_Gas_CO2() {
  CO2Value = analogRead(gasPinCO2);
  float gasPercentage = map(CO2Value, 0, 1023, 0, 100); // Simulação de 0 a 100%
  String gasMetrica = "%";
  String exibirCO2 = gasPercentage + gasMetrica;
  displayTwoLineText("CO2: ", exibirCO2);

  if (gasPercentage >= 45 && gasPercentage <= 55) {
    activateWhichLED(ledG);
    noTone(buzzerPin);
    delay(2000);
    displayTwoLineText("Concentracao", "normal!");
  } 
  else {
    activateWhichLED(ledR);
    tone(buzzerPin, 500);
    delay(2000);
    displayTwoLineText("Concentracao", "anormal!");
  }
  delay(2000);
}

void read_Gas_CH4() {
  CH4Value = analogRead(gasPinCH4);
  float gasPercentage = map(CH4Value, 0, 1023, 0, 100); // Simulação de 0 a 100%
  String gasMetrica = "%";
  String exibirCH4 = gasPercentage + gasMetrica;
  displayTwoLineText("CH4: ", exibirCH4);

  if (gasPercentage >= 0 && gasPercentage <= 5) {
    activateWhichLED(ledG);
    noTone(buzzerPin);
    delay(2000);
    displayTwoLineText("Concentracao", "normal!");
  } 
  else {
    activateWhichLED(ledR);
    tone(buzzerPin, 500);
    delay(2000);
    displayTwoLineText("Concentracao", "excessiva!");
  }
  delay(2000);
}

void read_Proximidade() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.034 / 2;
  String proximidadeMetrica = "m";
  String exibirDist = distance + proximidadeMetrica;
  displayTwoLineText("Distancia: ", exibirDist);

  if (distance <= 50) {
    activateWhichLED(ledR);
    sonarBeep();
    displayTwoLineText("Lixo", "detectado!");
  }
  else if (distance < 200) {
    activateWhichLED(ledY);
    noTone(buzzerPin);
    delay(2000);
    displayTwoLineText("Objeto", "detectado!");
  }
  else {
    activateWhichLED(ledG);
    noTone(buzzerPin);
    delay(2000);
    displayTwoLineText("Lixo nao", "detectado!");
  }
  delay(2000);
}

void telaVazia() {
  lcd.clear();
  delay(4000);
  noTone(buzzerPin);
}

void setup() {
  lcd.begin(16, 2);
  dht.begin();
  Serial.begin(9600);
  servo.attach(servoPin);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(ledR, OUTPUT);
  pinMode(ledY, OUTPUT);
  pinMode(ledG, OUTPUT);
  pinMode(A1, INPUT);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Initializing");
  delay (1000);
  lcd.setCursor(8, 1);
  lcd.print("SETUP...");
  delay (2000);
  displayTwoLineText("GS - Edge","Computing" );
  delay (2000);
  lcd.clear();

  for (int i = 0; i <= 15; i++) {
    lcd.setCursor(0, 0);
    lcd.print("Carregando...");
    lcd.setCursor(i, 1);
    lcd.print("*");
    delay(200);
    lcd.clear();
  }
}

void loop() {
  if (counterRotation < startMotor) { // Simulando drone ligando
    rotacaoServoMotor(); 
  }
  else {
    testaLuminosidade();
    telaVazia();
  
    testaOxigenio();
    telaVazia();
  
    testaTemperatura();
    telaVazia();
  
    read_pH();
    telaVazia();
  
    read_Gas_CO2();
    telaVazia();
  
    read_Gas_CH4();
    telaVazia();
  
    read_Proximidade();
    telaVazia();
  }
}
