#include "comodos.h"

/***************************************Variaveis para o Buzzer*****************************************/

int initBuzzer[3];
int controlBuzzer[4] = {2,0,0,0};
int doorStatus;
int doorTimer[4] = {0,0,0,0};
int closeTimeout[3] = {0,1,30};

/*******************************************************************************************************/

void iniciaRtc(){
  if (! rtc.isrunning()) { //SE RTC NÃO ESTIVER SENDO EXECUTADO, FAZ
    Serial.println("DS1307 rodando!"); //IMPRIME O TEXTO NO MONITOR SERIAL
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); //CAPTURA A DATA E HORA EM QUE O SKETCH É COMPILADO
    rtc.adjust(DateTime(2020, 6, 11, 14, 00, 00)); //(ANO), (MÊS), (DIA), (HORA), (MINUTOS), (SEGUNDOS)
  }
  sensors.begin(); //INICIA O SENSOR
  sensors.getAddress(sensor, 0);
  delay(1000); //INTERVALO DE 1 SEGUNDO
}

void portaentrada(String msg){
  switch (msg[2])
  {
  case '1': // Porta de entrada: Entrada Sensor digital 24V (monitora estado)
  {
    if(msg[3] == LEITURA){
        checkDoor();
    }else if(msg[3] == ESCRITA){
      // escreve os valores de close timeout
      
    }
    break;
  }
  case '2': // Solenoide da Porta: Saída Digital 24V (abre/fecha c/ senha)
            // Senha padrão: 0000 (4 digitos) (destrava porta e desativa alarme)
  {
    if(msg[3] == LEITURA){
      if(digitalRead(TRAVAENTRADA) == HIGH){
        Serial.print("Porta Fechada.");
      }else{
        Serial.print("Porta Aberta.");
      }
    }else if(msg[3] == ESCRITA){
      if((msg[6] == '0') && (msg[7] == '0') && (msg[8] == '0') && (msg[9] == '1')){
        digitalWrite(TRAVAENTRADA, LOW);  // Abre trava
      }
      if((msg[6] == '0') && (msg[7] == '0') && (msg[8] == '0') && (msg[9] == '0')){
        digitalWrite(TRAVAENTRADA, HIGH); // Fecha trava
      }
    }
    break;
  }
  case '3': // Alarme sonoro: Saída digital 5v (2m ON 5m OFF)
  {
    if(msg[3] == LEITURA){
      if(digitalRead(BUZZER) == HIGH){
        Serial.print("Alarme Desligado.");
      }

      if(digitalRead(BUZZER) == LOW){
        Serial.print("Alarme Ligado.");
      }

    }else 
    if(msg[3] == ESCRITA){
      if((msg[6] == '0') && (msg[7] == '0') && (msg[8] == '0') && (msg[9] == '1')){
        ligaBuzzer();
      }
      if((msg[6] == '0') && (msg[7] == '0') && (msg[8] == '0') && (msg[9] == '0')){
        desligaBuzzer();
      }
    }
    break;
  }
  default:
    break;
  }
}


/***********************************Funcoes para o Buzzer e Porta***************************************/

void checkDoor(){
  DateTime now = rtc.now();
  if(digitalRead(PORTAENTRADA) == HIGH){
    Serial.print("Porta Fechada.");
    doorStatus = FECHADA;
    doorTimer[0] = 0;
    desligaBuzzer();
    controlBuzzer[0] = 2;
  }else{
    Serial.print("Porta Aberta.");
    doorStatus = ABERTA;
    if(doorTimer[0] == 0){
      doorTimer[0] = 1;
      doorTimer[1] = now.hour();
      doorTimer[2] = now.minute();
      doorTimer[3] = now.second();
    }else
    if(doorTimer[0] == 1){
      if((doorTimer[1] <= (now.hour()   + closeTimeout[0]))   && 
         (doorTimer[2] <= (now.minute() + closeTimeout[1]))   && 
         (doorTimer[3] <= (now.second() + closeTimeout[2]))
        )
        ligaBuzzer();
    }

  }
}

void ligaBuzzer(){
  DateTime now = rtc.now();
  digitalWrite(BUZZER, LOW);

  controlBuzzer[0] = 0;
  controlBuzzer[1] = now.hour();
  controlBuzzer[2] = now.minute();
  controlBuzzer[3] = now.second();
}

void checkBuzzer(){
  DateTime now = rtc.now();
  if(controlBuzzer[0] == 0){
    if((controlBuzzer[1] <= (now.hour()    )) && 
       (controlBuzzer[2] <= (now.minute()+2)) && 
       (controlBuzzer[3] <= (now.second()  ))
      )
      desligaBuzzer();
  }else 
  if(controlBuzzer[0] == 1){
    if((controlBuzzer[1] <= (now.hour()    )) && 
       (controlBuzzer[2] <= (now.minute()+5)) && 
       (controlBuzzer[3] <= (now.second()  ))
      )
      ligaBuzzer();
  }
}

void desligaBuzzer(){
  DateTime now = rtc.now();
  digitalWrite(BUZZER, HIGH);

  controlBuzzer[0] = 1;
  controlBuzzer[1] = now.hour();
  controlBuzzer[2] = now.minute();
  controlBuzzer[3] = now.second();
}

/*******************************************************************************************************/

float tempC, aux;

void saladeestar(String msg){
  switch (msg[2])
  {
  case '1': // Sensor Temperatura Sala de Estar: Entrada Digital 5V
            // a. Controla AC (autoTemp)
            // b. Temperatura padrão: 25ºC
  {
    
    if(msg[3] == LEITURA){

      tempC = sensors.getTempC(sensor);
      if (tempC == -127.00){
        Serial.print("C: ");
        Serial.print(aux);
      }else{
        Serial.print("C: ");
        Serial.print(tempC);
        aux = tempC;
      }
      sensors.requestTemperatures();
      Serial.print("\n");

    }else if(msg[3] == ESCRITA){
      
    }
    
    break;
  }
  case '2': // AC sala de estar remoto: (manual mode): 
            // saída analógica 5V (temp = Tensão*10, temp > 0 && temp < 50)
  {
    if(msg[3] == LEITURA){

    }else if(msg[3] == ESCRITA){
        
    }
    break;
  }
  case '3': // Luz de estar: saída analógica 5V (0 = off, 5V = brilho max)
  {
    if(msg[3] == LEITURA){

    }else if(msg[3] == ESCRITA){
        
    }
    break;
  }
  case '4': // Duas portas digitais de saída 24V controlam (cima/baixo)
  {
    if(msg[3] == LEITURA){

    }else if(msg[3] == ESCRITA){
        
    }
    break;
  }
  case '5': // Duas portas digitais de saída 24V controlam (cima/baixo)
  {
    if(msg[3] == LEITURA){

    }else if(msg[3] == ESCRITA){
        
    }
    break;
  }
  case '6': // sensor entrada analógico 5v de posição (1v full fechada - 4v full aberta)
            // 3 Horários padronizados
            // Time 1: meio aberta (padrão 8:00h)
            // Time 2: full aberta (padrão 12:00h)
            // Time 3: full fechada (padrão 18:00h
  {
    if(msg[3] == LEITURA){

    }else if(msg[3] == ESCRITA){
        
    }
    break;
  }
  case '7': // Wind Speed: sensor entrada analógico 5V 
            // (Speed(Km/h) = (Tensão - 0,36206896551)/0,0275862069). 
            // Se speed >= 50, fechar janelas + Wind Alert 
  {
    if(msg[3] == LEITURA){

    }else if(msg[3] == ESCRITA){
        
    }
    break;
  }

  default:
    break;
  }
}


void temperaturaSala(){
  aux = sensors.getTempC(sensor);
  if (aux != -127.00){
    tempC = aux;
  }
  
  sensors.requestTemperatures();

}

/****************************************Funcoes para o Buzzer******************************************/

/*******************************************************************************************************/


void quartoebanheiro(String msg){
  switch (msg[2])
  {
  case '1': // Janela do Quarto: Duas portas digitais de saída 24V controlam (cima/baixo)
  {
    if(msg[3] == LEITURA){

    }else if(msg[3] == ESCRITA){
        
    }
    break;
  }
  case '2': // Janela do Quarto: Duas portas digitais de saída 24V controlam (cima/baixo)
  {
    if(msg[3] == LEITURA){

    }else if(msg[3] == ESCRITA){
        
    }
    break;
  }
  case '3': // sensor de entrada analógico 5v de posição (1v full fechada - 4v full aberta)
            // 3 Horários padronizados
            // Time 1: meio aberta (padrão 8:00h)
            // Time 2: full aberta (padrão 12:00h)
            // Time 3: full fechada (padrão 18:00h
  {
    if(msg[3] == LEITURA){

    }else if(msg[3] == ESCRITA){
        
    }
    break;
  }
  case '4': // Luz quarto: saída analógica 5V (0 = off, 5V = brilho max)
  {
    if(msg[3] == LEITURA){

    }else if(msg[3] == ESCRITA){
        
    }
    break;
  }
  case '5': // Sensor de Temperatura: Entrada LM35 Analógico 4-20V (Se < 17ºC, liga aquecedor)
  {
    if(msg[3] == LEITURA){

    }else if(msg[3] == ESCRITA){
        
    }
    break;
  }
  case '6': // Aquecedor: Saída Digital controlado pelo sensor (Dead Band Temp)
  {
    if(msg[3] == LEITURA){

    }else if(msg[3] == ESCRITA){
        
    }
    break;
  }

  default:
    break;
  }
}

