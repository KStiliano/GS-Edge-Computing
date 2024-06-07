# GS - Edge Computing

## Integrantes 👋
<ul>
    <li>João Marcelo Furtado Romero (RM555199)</li>
    <li>Kayky Silva Stiliano (RM555148)</li>
</ul>
 
<img src="https://github.com/KStiliano/GS-Edge-Computing/blob/main/arduino-sketch.png" alt="print do circuito Arduino/printscreen Arduino circuit"/>

Link da simulação no <a href="https://wokwi.com/projects/399229434431228929">Wokwi</a>

## Instruções
O Wokwi está com um bug onde se você da uma certa quantidade de zoom algumas funcionalidades não irão aparecer no display.

## Explicação do Projeto 📖
Um programa que mede:

 
 O nível de luminosidade através de um LDR 🔆

<ul>
<li>
    Alta luminosidade: LED verde + mensagem no LCD
</li>
<li>
    Média luminosidade: LED amarelo + mensagem no LCD
</li>
<li>
    Baixa luminosidade: LED vermelho  + mensagem no LCD + beep
</li>
</ul>
<br>

 O nível de O₂ através do DHT

<ul>
<li>
  Nível abaixo: LED vermelho + mensagem no LCD + beep
</li>
<li>
  Nível em alerta: LED amarelo + mensagem no LCD
</li>
<li>
  Nível ideal: LED verde + mensagem no LCD
</li>
</ul>
<br>

Temperatura (em °C), também através do DHT 🌡
 
<ul>
<li>
  Temperatura elevada: LED vermelho + mensagem no LCD + beep
</li>
<li>
  Temperatura ideal: LED verde + mensagem no LCD
</li>
<li>
  Temperatura abaixo: LED vermelho + mensagem no LCD + beep
</li>
</ul>
<br>

pH, através do primeiro potenciômetro

<ul>
<li>
  pH abaixo do nível ideal: LED vermelho + mensagem no LCD + beep
</li>
<li>
  pH acima do nível ideal: LED vermelho + mensagem no LCD
</li>
<li>
  pH ideal: LED verde + mensagem no LCD
</li>
</ul>
<br>

CO₂, através do segundo potenciômetro

<ul>
<li>
  Concentração ideal: LED verde + mensagem no LCD
</li>
<li>
  Concetração anormal: LED vermelho + mensagem no LCD + beep
</li>
</ul>
<br>

CH4, através do terceiro potenciômetro

<ul>
<li>
  Concentração ideal: LED verde + mensagem no LCD
</li>
<li>
  Concetração excessiva: LED vermelho + mensagem no LCD + beep
</li>
</ul>
<br>

 A proximidade de objetos (lixos marinhos) através de um HC-S404

<ul>
<li>
    Distância curta: LED vermelho + mensagem no LCD + sonar beep
</li>
<li>
    Distância média: LED amarelo + mensagem no LCD
</li>
<li>
    Distância longa: LED verde  + mensagem no LCD
</li>
</ul>
<br>
 
## Componentes 🛠️
<ul>
    <li>1x Placa Arduino Uno</li>
    <li>1x Breadboard</li>
    <li>3x Potenciômetro</li>
    <li>1x DHT11/22</li>
    <li>1x Buzzer</li>
    <li>1x Display LCD</li>
    <li>1x LDR</li>
    <li>3x LEDs</li>
    <li>1x Servo motor</li>
    <li>1x Sensor de proximidade (HC-SR04)</li>
    <li>6x Resistores</li>
</ul>
 
<br>
 
## Dependências 📦
<ul>
    <li>LiquidCrystal</li>
    <li>DHT Sensor library</li>
    <li>Servo library</li>
</ul>
 
<br>

## Explicando o <a href="https://github.com/KStiliano/GS-Edge-Computing/blob/main/arduino-code.cc">Código</a> 🧑‍💻
 
Precisamos incluir as bibliotecas para podermos ter acesso a métodos dos quais vamos precisar.
 
Assim como também é preciso declarar, iniciando tais valores para podermos usá-los depois na aplicação.
 
```c
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
```
<hr>

 ## Funções 🛠️

```c
void displayTwoLineText(String str1, String str2) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(str1);
  lcd.setCursor(0, 1);
  lcd.print(str2);
}
```
<hr>
 
Objetivo: Exibe duas linhas de texto no LCD.
<br>
Parâmetros:
`str1`: Texto para a primeira linha.
`str2`: Texto para a segunda linha.
<br>
Ação: Limpa o LCD e exibe `str1` na linha 1 e `str2` na linha 2.
 
```c
void activateWhichLED(int led) {
  digitalWrite(ledR, LOW);
  digitalWrite(ledY, LOW);
  digitalWrite(ledG, LOW);
  digitalWrite(led, HIGH);
}
```
<hr>
 
Objetivo: Liga um LED específico e desliga os outros.
<br>
Parâmetros:
`led`: O pino do LED a ser ligado.
<br>
Ação: Desliga todos os LEDs e liga o LED especificado.
 
```c
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
```
<hr>
 
Objetivo: Emite um sinal sonoro pelo buzzer.
<br>
Parâmetros: `qntdBeeps`: Número de bipes (padrão é 3).
<br>
Ação: Emite um tom de 1000 Hz por 2 segundos, espera 1 segundo, e repete.
 
```c
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
```
<hr>
 
Objetivo: Controla a rotação do servo motor simulando a partida e do servo motor do drone.
<br>
Parâmetros: `velocidade`: Velocidade da rotação (padrão é 5).
<br>
Ação: Incrementa ou decrementa a posição do servo motor a cada interval (20 ms), invertendo a direção ao atingir 0 ou 180 graus. Conta rotações.
 
```c
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
```
<hr>
 
Objetivo: Lê o valor do sensor LDR e determina o estado da luminosidade.
<br>
Ação: Lê o valor do LDR (convertido para um percentual de 0 a 100%).
Exibe o valor no LCD.
Ativa um LED baseado no valor:
Verde se `valor_ldr >= 70`.
Amarelo se `50 <= valor_ldr < 70`.
Vermelho e buzzer se `valor_ldr < 50`.
 
```c
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
```
<hr>

Objetivo: Lê o valor de umidade do sensor DHT, simulado como oxigênio.
<br>
Ação: Lê a umidade e converte para mg/L (0 a 15).
Exibe o valor no LCD.
Ativa um LED baseado no valor:
Vermelho e buzzer se `oxigenioMgL <= 2`.
Amarelo se `2 < oxigenioMgL < 5`.
Verde se `oxigenioMgL >= 5`.

```c
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
```
<hr>

Objetivo: Lê a temperatura do sensor DHT.
<br>
Ação: Lê a temperatura em graus Celsius.
Exibe o valor no LCD.
Ativa um LED baseado no valor:
Vermelho e buzzer se `temperatura >= 30 ou temperatura < 20`.
Verde se `20 <= temperatura < 30`.

```c
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
```
<hr>

Objetivo: Lê o valor do sensor de pH.
<br>
Ação: Lê o valor do pino analógico (0 a 1023) e converte para pH (0 a 14).
Exibe o valor no LCD.
Ativa um LED baseado no valor:
Vermelho e buzzer se `pH < 7.5 ou pH > 8.4`.
Verde se `7.5 <= pH <= 8.4`.

```c
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
```
<hr>

Objetivo: Lê o valor do sensor de gás CO2.
<br>
Ação: Lê o valor do pino analógico (convertido para um percentual de 0 a 100%).
Exibe o valor no LCD.
Ativa um LED baseado no valor:
Verde se `gasPercentage >= 45 && gasPercentage <= 55`.
Vermelho e buzzer caso contrário.

```c
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
```
<hr>

Objetivo: Lê o valor do sensor de gás CH4.
<br>
Ação: Lê o valor do pino analógico (convertido para um percentual de 0 a 100%).
Exibe o valor no LCD.
Ativa um LED baseado no valor:
Verde se `gasPercentage >= 0 && gasPercentage <= 5`.
Vermelho e buzzer caso contrário.

```c
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
```
<hr>

Objetivo: Lê a distância medida pelo sensor de proximidade ultrassônico.
<br>
Ação: Dispara um pulso ultrassônico e mede o tempo de retorno.
Calcula a distância em metros.
Exibe o valor no LCD.
Ativa um LED baseado no valor:
Vermelho e buzzer se `distance` <= 50.
Amarelo se `distance < 200`.
Verde caso contrário.

```c
void telaVazia() {
  lcd.clear();
  delay(4000);
  noTone(buzzerPin);
}
```
<hr>

A função servirá como uma "helper" function para que possamos causar delays entre as diferentes funções, limpando o display e silenciando o beep.

```c
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
```
<hr>

A função serve para fazermos o setup da nosso projeto, dizendo para o programa quais pinos utilizaremos e em qual modo, iniciar os componentes, o código extra com diferentes passos para já escrever no display serve para melhorar a experiência de usuário indicando-o que o programa está sendo inicializado.

```c
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
```
<hr>

A função `loop()` que retorna void, servirá como o ciclo de vida inteiro do projeto, é uma função que por padrão roda infinitamente até algo dentro ou fora do código pará-lo.
Aqui, vamos chamar nossas funções principais já declaradas para termos uma melhor leitura e manutenção do código.

<hr>
 
<center>Este projeto encontra sob a <a href="https://github.com/KStiliano/GS-Edge-Computing/blob/main/LICENSE">MIT License.</a></center>
