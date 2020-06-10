#include <Arduino.h>
#include <stdint.h>
#include <Wire.h>
#include "RTClib.h"
#include "comodos.h"

void portaentrada(String msg){
  switch (msg[2])
  {
  case '1': // Porta de entrada: Entrada Sensor digital 24V (monitora estado)
  {
    if(msg[3] == LEITURA){
        if(digitalRead(PORTAENTRADA) == HIGH){
            Serial.print("Porta Fechada.");
        }else{
            // roda timer quando porta aberta 
            Serial.print("Porta Aberta.");
        }
    }else if(msg[3] == ESCRITA){
        Serial.print("Nada para escrever no estado da porta.");
    }
    break;
  }
  case '2': // Solenoide da Porta: Saída Digital 24V (abre/fecha c/ senha)
            // Senha padrão: 0000 (4 digitos) (destrava porta e desativa alarme)
  {
    if(msg[3] == LEITURA){

    }else if(msg[3] == ESCRITA){
        
    }
    break;
  }
  case '3': // Alarme sonoro: Saída digital 5v (2m ON 5m OFF)
  {
    if(msg[3] == LEITURA){
      if(digitalRead(BUZZER) == HIGH){
        Serial.print("Buzzer Desligado.");
      }

      if(digitalRead(BUZZER) == LOW){
        Serial.print("Buzzer Ligado.");
      }
    }else if(msg[3] == ESCRITA){
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

void ligaBuzzer(){
  DateTime now = rtc.now();
  digitalWrite(BUZZER, LOW);

  initBuzzer[0] = now.hour();
  initBuzzer[1] = now.minute();
  initBuzzer[2] = now.second();
}

void checkBuzzer(){
  DateTime now = rtc.now();
  if((initBuzzer[0] == now.hour()) && (initBuzzer[1] == (now.minute()+2)) && (initBuzzer[2] == now.second()))
    desligaBuzzer();
}

void desligaBuzzer(){
  digitalWrite(BUZZER, HIGH);

  initBuzzer[0] = 0;
  initBuzzer[1] = 0;
  initBuzzer[2] = 0;
}

void saladeestar(String msg){
  switch (msg[2])
  {
  case '1': // Sensor Temperatura Sala de Estar: Entrada Digital 5V
            // a. Controla AC (autoTemp)
            // b. Temperatura padrão: 25ºC
  {
    
    if(msg[3] == LEITURA){

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
/*
void read_input(String msg){
  // numero da entrada
  int contact = ((msg[5]-'0')*10 + (msg[6]-'0')) + INPUT_OFFSET;

  // debug
  Serial.print("Leitura na Entrada ");
  Serial.println(contact-INPUT_OFFSET);

  // Ligada ou desligada?
  if(digitalRead(contact))
  {
      msg[7]='F';
      msg[8]='F';
      msg[9]='0';
      msg[10]='0';
  }
  else
  {
    msg[7]='0';
    msg[8]='0';
    msg[9]='0';
    msg[10]='0';
  }
  
  // Resposta com o valor atual da entrada..
  Serial.print("Resposta do Escravo: ");
  Serial.println(msg);
}
*/