#include "comodos.h"

OneWire oneWire(DS18B20);             //CONFIGURA UMA INSTÂNCIA ONEWIRE PARA SE COMUNICAR COM O SENSOR
DallasTemperature sensors(&oneWire);  //BIBLIOTECA DallasTemperature UTILIZA A OneWire
DeviceAddress sensor;

/***************************************Variaveis para o Buzzer*****************************************/

int initBuzzer[3];
int controlBuzzer[4] = {2,0,0,0};
int doorStatus;
int doorPast;
int doorTimer[4] = {0,0,0,0};
int closeTimeout[3] = {0,1,30};

/*******************************************************************************************************/

void iniciaRtc(){
  pinMode(BUZZER, OUTPUT);
  pinMode(TRAVAENTRADA, OUTPUT);
  pinMode(PORTAENTRADA, INPUT);
  pinMode(LUZQUARTO, OUTPUT);
  pinMode(LUZSALA, OUTPUT);
  pinMode(LUZSALALEITURA, INPUT);
  
  digitalWrite(TRAVAENTRADA, HIGH);
  digitalWrite(BUZZER, LOW);

  Serial.println("Iniciando o RTC e Sensor de temperatura.");
  if (! rtc.isrunning()) { //SE RTC NÃO ESTIVER SENDO EXECUTADO, FAZ
    Serial.println("DS1307 rodando!"); //IMPRIME O TEXTO NO MONITOR SERIAL
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); //CAPTURA A DATA E HORA EM QUE O SKETCH É COMPILADO
    rtc.adjust(DateTime(2020, 6, 13, 14, 00, 00)); //(ANO), (MÊS), (DIA), (HORA), (MINUTOS), (SEGUNDOS)
  }

  delay(1000);
  Serial.println("RTC OK!");
  sensors.begin(); //INICIA O SENSOR
  sensors.getAddress(sensor, 0);
  delay(1000); //INTERVALO DE 1 SEGUNDO
  Serial.println("SENSOR OK!");
}


void checkDoor(int call){
  if(call == 0){
    DateTime now = rtc.now();
    if(digitalRead(PORTAENTRADA) == HIGH){
      doorStatus = FECHADA;
      doorTimer[0] = 0;
      desligaBuzzer();
      controlBuzzer[0] = 2;
      if(doorStatus != doorPast){
        Serial.println("Porta Fechada.");
        doorPast = doorStatus;
      }
    }else{
      doorStatus = ABERTA;
      if(doorStatus != doorPast){
        Serial.println("Porta Aberta.");
        doorPast = doorStatus;
      }
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
          ){
          ligaBuzzer();
          }
      }
    }
    
  }else{
      if(digitalRead(PORTAENTRADA) == HIGH){
        Serial.println("Porta Fechada.");
      }else{
        Serial.println("Porta Aberta.");
      }
    }


}

void portaentrada(String msg){
  Serial.println("Mensagem enviada para Porta de Entrada!");
  switch (msg[2])
  {
  case '1': // Porta de entrada: Entrada Sensor digital 24V (monitora estado)
  {
    if(msg[3] == LEITURA){
      Serial.println("Leitura do estado da porta!");
      checkDoor(1);
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
        Serial.print("Trava Fechada.");
      }else{
        Serial.print("Trava Aberta.");
      }
    }else if(msg[3] == ESCRITA){
      if((msg[6] == '0') && (msg[7] == '0') && (msg[8] == '0') && (msg[9] == '1')){
        Serial.println("Abrindo trava.");
        digitalWrite(TRAVAENTRADA, LOW);  // Abre trava
      }
      if((msg[6] == '0') && (msg[7] == '0') && (msg[8] == '0') && (msg[9] == '0')){
        Serial.println("Fechando trava.");
        digitalWrite(TRAVAENTRADA, HIGH); // Fecha trava
      }
    }
    break;
  }
  case '3': // Alarme sonoro: Saída digital 5v (2m ON 5m OFF)
  {
    if(msg[3] == LEITURA){
      if(digitalRead(BUZZER) == HIGH){
        Serial.print("Alarme Ligado.");
      }

      if(digitalRead(BUZZER) == LOW){
        Serial.print("Alarme Desligado.");
      }

    }else 
    if(msg[3] == ESCRITA){
      if((msg[6] == '0') && (msg[7] == '0') && (msg[8] == '0') && (msg[9] == '1')){
        Serial.println("Ligando buzzer");
        ligaBuzzer();
        doorTimer[0] = 0;
        controlBuzzer[0] = 2;
        Serial.println(digitalRead(BUZZER));
      }
      if((msg[6] == '0') && (msg[7] == '0') && (msg[8] == '0') && (msg[9] == '0')){
        Serial.println("Desligando buzzer");
        desligaBuzzer();
        controlBuzzer[0] = 2;
        Serial.println(digitalRead(BUZZER));
      }
    }
    break;
  }
  default:
    break;
  }
}


/***********************************Funcoes para o Buzzer e Porta***************************************/


void ligaBuzzer(){
  DateTime now = rtc.now();
  digitalWrite(BUZZER, HIGH);

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
  }else{
    //Serial.print();
    Serial.println(digitalRead(BUZZER));
  }
}

void desligaBuzzer(){
  DateTime now = rtc.now();
  digitalWrite(BUZZER, LOW);
  
  controlBuzzer[0] = 1;
  controlBuzzer[1] = now.hour();
  controlBuzzer[2] = now.minute();
  controlBuzzer[3] = now.second();
}

/*******************************************************************************************************/


float tempC, aux;
int valueSala = 0, valueQuarto = 0;

void temperaturaSala(){
  sensors.requestTemperatures();
  aux = sensors.getTempC(sensor);
  if (aux != -127.00 && aux != 85){
    tempC = aux;
  }
  sensors.requestTemperatures();
  //Serial.print("Aux: ");
  //Serial.println(aux);
}

void saladeestar(String msg){
  Serial.println("Mensagem enviada para Sala de Estar!");
  switch (msg[2])
  {
  case '1': // Sensor Temperatura Sala de Estar: Entrada Digital 5V
            // a. Controla AC (autoTemp)
            // b. Temperatura padrão: 25ºC
  {
    Serial.println("Mensagem para o sensor de temperatura DS18B20.");
    
    if(msg[3] == LEITURA){
      temperaturaSala();
      Serial.print("Temperatura: ");
      Serial.println(tempC);
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
      
      Serial.print("Luz da sala em ");
      Serial.println(valueSala);
    }else if(msg[3] == ESCRITA){
      int c,d,u;
      // Converte os valores recebidos em string para inteiros
      c = (msg[7] - '0') * 100;
      d = (msg[8] - '0') * 10;
      u = (msg[9] - '0');
      valueSala = c + d + u;
      if(u <= 255 || u >= 0){
        analogWrite(LUZSALA, valueSala);
      }
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



/****************************************Funcoes para o Buzzer******************************************/

/*******************************************************************************************************/


void quartoebanheiro(String msg){
  Serial.println("Mensagem enviada para Quarto/Banheiro!");
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
      Serial.print("Luz da sala em ");
      Serial.println(valueQuarto);
    }else if(msg[3] == ESCRITA){
      int c,d,u;
      // Converte os valores recebidos em string para inteiros
      c = (msg[7] - '0') * 100;
      d = (msg[8] - '0') * 10;
      u = (msg[9] - '0');
      valueQuarto = c + d + u;
      if(u <= 255 || u >= 0){
        analogWrite(LUZQUARTO, valueQuarto);
      }
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

