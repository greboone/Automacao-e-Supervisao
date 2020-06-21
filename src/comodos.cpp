#include "comodos.h"

OneWire oneWire(DS18B20);             //CONFIGURA UMA INSTÂNCIA ONEWIRE PARA SE COMUNICAR COM O SENSOR
DallasTemperature sensors(&oneWire);  //BIBLIOTECA DallasTemperature UTILIZA A OneWire
DeviceAddress sensor;

/***************************************Variaveis para o Buzzer*****************************************/

String retorno[28];// = "0000000000000000000000000000";

int initBuzzer[3];
int controlBuzzer[4] = {2,0,0,0};
int doorStatus;
int doorPast;
int doorTimer[4] = {0,0,0,0};
int closeTimeout[2] = {0,0};

float tempC, aux;
int valueSala = 0, valueQuarto = 0;
int salaTemp = 0;
int controleJanelaquarto = 0, controleJanelasala = 0;
int posicaoJanelaquarto = 0, posicaoJanelasala = 0;
int bandaMorta = 2;
float tempAnterior;


/*******************************************************************************************************/

void iniciaRtc(){
  pinMode(BUZZER, OUTPUT);
  pinMode(TRAVAENTRADA, OUTPUT);
  pinMode(PORTAENTRADA, INPUT);
  pinMode(LUZQUARTO, OUTPUT);
  pinMode(LUZSALA, OUTPUT);
  pinMode(AQUECEDOR, OUTPUT);
  pinMode(ENABLESALA, OUTPUT);
  pinMode(MOTORSALAHORARIO, OUTPUT);
  pinMode(MOTORSALAANTIHORARIO, OUTPUT);
  pinMode(ENABLEQUARTO, OUTPUT);
  pinMode(MOTORQUARTOHORARIO, OUTPUT);
  pinMode(MOTORQUARTOANTIHORARIO, OUTPUT);
  
  digitalWrite(TRAVAENTRADA, HIGH);
  digitalWrite(BUZZER, LOW);
  digitalWrite(AQUECEDOR, HIGH);

  Serial.println("Iniciando o RTC e Sensor de temperatura.");

  rtc.begin();
  rtc.isrunning();
   //SE RTC NÃO ESTIVER SENDO EXECUTADO, FAZ
    //Serial.println("DS1307 nao ta rodando!"); //IMPRIME O TEXTO NO MONITOR SERIAL
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); //CAPTURA A DATA E HORA EM QUE O SKETCH É COMPILADO
  //rtc.adjust(DateTime(2020, 6, 13, 14, 00, 00)); //(ANO), (MÊS), (DIA), (HORA), (MINUTOS), (SEGUNDOS)

  delay(500);
  Serial.println("RTC OK!");
  DateTime now = rtc.now();
  Serial.print(now.year()); Serial.print("/"); Serial.print(now.month()); Serial.print("/"); Serial.println(now.day());
  Serial.print(now.hour()); Serial.print(":"); Serial.print(now.minute()); Serial.print(":"); Serial.println(now.second());
  Serial.println(now.unixtime());

  sensors.begin(); //INICIA O SENSOR
  sensors.getAddress(sensor, 0);
  delay(500); //INTERVALO DE 1 SEGUNDO
  Serial.println("SENSOR OK!");
}

int checkDoor(int call){
  if(call == 0){
    DateTime now = rtc.now();
    if(digitalRead(PORTAENTRADA) == HIGH){
      doorStatus = FECHADA;
      doorTimer[0] = 0;
      desligaBuzzer();
      controlBuzzer[0] = 2;
      if(doorStatus != doorPast){
        //Serial.println("Porta Fechada.");
        doorPast = doorStatus;
      }
    }else{
      doorStatus = ABERTA;
      if(doorStatus != doorPast){
        //Serial.println("Porta Aberta.");
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
        //Serial.println("Porta Fechada.");
        return FECHADA;
      }else{
        //Serial.println("Porta Aberta.");
        return ABERTA;
      }
    }

  return(0);
}

int travaPorta(String msg, int type){
  if(type == 0){
    if((msg[6] == '0') && (msg[7] == '0') && (msg[8] == '0') && (msg[9] == '1')){
          //Serial.println("Abrindo trava.");
          digitalWrite(TRAVAENTRADA, LOW);  // Abre trava
          return ABERTA;
    }
    if((msg[6] == '0') && (msg[7] == '0') && (msg[8] == '0') && (msg[9] == '0')){
      if(checkDoor(1) == FECHADA){
        //Serial.println("Fechando trava.");
        digitalWrite(TRAVAENTRADA, HIGH); // Fecha trava
        return FECHADA;
      }else{
        //Serial.println("Fechar a porta para trava-la");
        return 2;
      }
    }
  }else 
  if(type == 1){
    if(digitalRead(PORTAENTRADA) == ABERTA){
      return 0;
    }else{
      return 1;
    }
  }
  return 2;
}

void portaentrada(String msg){
  //Serial.println("Mensagem enviada para Porta de Entrada!");
  switch (msg[2])
  {
  case '1': // Porta de entrada: Entrada Sensor digital 24V (monitora estado) 
  {
    if(msg[3] == LEITURA){
      //Serial.println("Leitura do estado da porta!");
      checkDoor(1);
    }else if(msg[3] == ESCRITA){
      // escreve os valores de close timeout
      int H,h,M,m;
      // Converte os valores recebidos em string para inteiros
      H = (msg[6] - '0') * 10;
      h = (msg[7] - '0');
      closeTimeout[0] = H+h;
      M = (msg[8] - '0') * 10;
      m = (msg[9] - '0');
      closeTimeout[1] = M+m;
    }
    break;
  }
  case '2': // Solenoide da Porta: Saída Digital 24V (abre/fecha c/ senha)
            // Senha padrão: 0000 (4 digitos) (destrava porta e desativa alarme)
  {
    if(msg[3] == LEITURA){
      if(digitalRead(TRAVAENTRADA) == HIGH){
        //Serial.print("Trava Fechada.");
      }else{
        //Serial.print("Trava Aberta.");
      }
    }else if(msg[3] == ESCRITA){
      travaPorta(msg,0);
    }
    break;
  }
  case '3': // Alarme sonoro: Saída digital 5v (2m ON 5m OFF)
  {
    if(msg[3] == LEITURA){
      if(digitalRead(BUZZER) == HIGH){
        //Serial.print("Alarme Ligado.");
      }

      if(digitalRead(BUZZER) == LOW){
        //Serial.print("Alarme Desligado.");
      }

    }else 
    if(msg[3] == ESCRITA){
      if((msg[6] == '0') && (msg[7] == '0') && (msg[8] == '0') && (msg[9] == '1')){
        //Serial.println("Ligando buzzer");
        ligaBuzzer();
        doorTimer[0] = 0;
        //Serial.println(digitalRead(BUZZER));
      }
      if((msg[6] == '0') && (msg[7] == '0') && (msg[8] == '0') && (msg[9] == '0')){
        //Serial.println("Desligando buzzer");
        desligaBuzzer();
        //Serial.println(digitalRead(BUZZER));
      }
    }
    break;
  }
  default:
    break;
  }
}


/***********************************Funcoes para o Buzzer***********************************************/


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
    //Serial.println(digitalRead(BUZZER));
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

// Horario abre, Antihorario fecha
// Motor Sala
// Horario 43    Antihorario 44
// Motor Quarto
// Horario 46    Antihorario 47

void desligaMotores(){
  if(analogRead(ESTADOJANELAQUARTO) >= 819 || analogRead(ESTADOJANELAQUARTO) <= 204){
    digitalWrite(MOTORQUARTOHORARIO, LOW);
    digitalWrite(MOTORQUARTOANTIHORARIO, LOW);
  }
  if(analogRead(ESTADOJANELASALA) >= 819 || analogRead(ESTADOJANELASALA) <= 204){
    digitalWrite(MOTORSALAHORARIO, LOW);
    digitalWrite(MOTORSALAANTIHORARIO, LOW);
  }
  if(posicaoJanelaquarto ==  analogRead(ESTADOJANELAQUARTO) ||
     posicaoJanelaquarto == (analogRead(ESTADOJANELAQUARTO) + 1) ||
     posicaoJanelaquarto == (analogRead(ESTADOJANELAQUARTO) - 1) ||
     posicaoJanelaquarto == (analogRead(ESTADOJANELAQUARTO) + 2) ||
     posicaoJanelaquarto == (analogRead(ESTADOJANELAQUARTO) - 2) ||
     posicaoJanelaquarto == (analogRead(ESTADOJANELAQUARTO) + 3) ||
     posicaoJanelaquarto == (analogRead(ESTADOJANELAQUARTO) - 3) ||
     posicaoJanelaquarto == (analogRead(ESTADOJANELAQUARTO) + 4) ||
     posicaoJanelaquarto == (analogRead(ESTADOJANELAQUARTO) - 4) ||
     posicaoJanelaquarto == (analogRead(ESTADOJANELAQUARTO) + 5) ||
     posicaoJanelaquarto == (analogRead(ESTADOJANELAQUARTO) - 5)){
        digitalWrite(ENABLEQUARTO, LOW);
        digitalWrite(MOTORQUARTOHORARIO, LOW);
        digitalWrite(MOTORQUARTOANTIHORARIO, LOW);
        controleJanelaquarto = 0;
  }
  if(posicaoJanelasala ==  analogRead(ESTADOJANELASALA) ||
     posicaoJanelasala == (analogRead(ESTADOJANELASALA) + 1) ||
     posicaoJanelasala == (analogRead(ESTADOJANELASALA) - 1) ||
     posicaoJanelasala == (analogRead(ESTADOJANELASALA) + 2) ||
     posicaoJanelasala == (analogRead(ESTADOJANELASALA) - 2) ||
     posicaoJanelasala == (analogRead(ESTADOJANELASALA) + 3) ||
     posicaoJanelasala == (analogRead(ESTADOJANELASALA) - 3) ||
     posicaoJanelasala == (analogRead(ESTADOJANELASALA) + 4) ||
     posicaoJanelasala == (analogRead(ESTADOJANELASALA) - 4) ||
     posicaoJanelasala == (analogRead(ESTADOJANELASALA) + 5) ||
     posicaoJanelasala == (analogRead(ESTADOJANELASALA) - 5)){
        digitalWrite(ENABLESALA, LOW);
        digitalWrite(MOTORSALAHORARIO, LOW);
        digitalWrite(MOTORSALAANTIHORARIO, LOW);
        controleJanelasala = 0;
  }
}

void controlaMotores(){
  if(controleJanelaquarto == 1){
    //Serial.print("Posicao usuario: "); Serial.print(posicaoJanelaquarto);
    //Serial.print(" Valor lido: ");Serial.println(analogRead(ESTADOJANELAQUARTO));
    if(posicaoJanelaquarto > analogRead(ESTADOJANELAQUARTO)){
      digitalWrite(MOTORQUARTOANTIHORARIO, LOW);
      digitalWrite(MOTORQUARTOHORARIO, HIGH);
    }else
    if(posicaoJanelaquarto < analogRead(ESTADOJANELAQUARTO)){
      digitalWrite(MOTORQUARTOHORARIO, LOW);
      digitalWrite(MOTORQUARTOANTIHORARIO, HIGH);
    }else{
      digitalWrite(ENABLEQUARTO, LOW);
      digitalWrite(MOTORQUARTOHORARIO, LOW);
      digitalWrite(MOTORQUARTOANTIHORARIO, LOW);
      controleJanelaquarto = 0;
    }
  }
  desligaMotores();
  if(controleJanelasala == 1){
    //Serial.print("Posicao usuario: "); Serial.print(posicaoJanelasala);
    //Serial.print(" Valor lido: ");Serial.println(analogRead(ESTADOJANELASALA));
    if(posicaoJanelasala > analogRead(ESTADOJANELASALA)){
      digitalWrite(MOTORSALAANTIHORARIO, LOW);
      digitalWrite(MOTORSALAHORARIO, HIGH);
    }else
    if(posicaoJanelasala < analogRead(ESTADOJANELASALA)){
      digitalWrite(MOTORSALAHORARIO, LOW);
      digitalWrite(MOTORSALAANTIHORARIO, HIGH);
    }else{
      digitalWrite(ENABLESALA, LOW);
      digitalWrite(MOTORSALAHORARIO, LOW);
      digitalWrite(MOTORSALAANTIHORARIO, LOW);
      controleJanelasala = 0;
    }
  }
  desligaMotores();
  
}

void temperaturaSala(){
  sensors.requestTemperatures();
  aux = sensors.getTempC(sensor);
  if (aux != -127.00 && aux != 85){// && aux != 0){
    tempC = aux;
  }
  sensors.requestTemperatures();

  if (!sensors.getAddress(sensor, 0))
  {
    Serial.println("Sensores nao encontrados !");
  }

  Serial.print("Aux: ");
  Serial.print(aux);
  Serial.print(" Temp: ");
  Serial.println(tempC);
}

int temptoBytes(int temp){
  return ((temp * 51)/10);
}

void saladeestar(String msg){
  //Serial.println("Mensagem enviada para Sala de Estar!");
  switch (msg[2])
  {
  case '1': // Sensor Temperatura Sala de Estar: Entrada Digital 5V
            // a. Controla AC (autoTemp)
            // b. Temperatura padrão: 25ºC
  {
    //Serial.println("Mensagem para o sensor de temperatura DS18B20.");
    
    if(msg[3] == LEITURA){
      temperaturaSala();
      //Serial.print("Temperatura: ");
      //Serial.println(tempC);
    }else if(msg[3] == ESCRITA){
      
    }
    
    break;
  }
  case '2': // AC sala de estar remoto: (manual mode): 
            // saída analógica 5V (temp = Tensão*10, temp > 0 && temp < 50)
  {
    if(msg[3] == LEITURA){
      //Serial.print("Valor de temperatura configurado: ");
      //Serial.println(salaTemp);
    }else if(msg[3] == ESCRITA){
      int T,t;
      // Converte os valores recebidos em string para inteiros
      T = (msg[8] - '0') * 10;
      t = (msg[9] - '0');
      salaTemp = T+t;
      //Serial.print("Valor recebido: ");
      //Serial.println(salaTemp);
      if(salaTemp > 50 || salaTemp < 17){
        //Serial.println("Valor de temperatura invalido!");
        break;
      }
      analogWrite(ACSALA,temptoBytes(salaTemp));
    }
    break;
  }
  case '3': // Luz de estar: saída analógica 5V (0 = off, 5V = brilho max)
  {
    if(msg[3] == LEITURA){
      //Serial.print("Luz da sala em ");
      //Serial.println(valueSala);
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
            // Motor Janela Sala de estar
  {
    if(msg[3] == LEITURA){
      //Serial.print("Posição da janela: ");
      //Serial.println(analogRead(ESTADOJANELASALA));
    }else if(msg[3] == ESCRITA){
      int m,c,d,u;
      // Converte os valores recebidos em string para inteiros
      m = (msg[6] - '0') * 1000;
      c = (msg[7] - '0') * 100;
      d = (msg[8] - '0') * 10;
      u = (msg[9] - '0');
      posicaoJanelasala = m+c+d+u;
      //Serial.print("Valor recebido: ");
      //Serial.println(posicaoJanelasala);
      controleJanelasala = 1;
      digitalWrite(ENABLESALA, HIGH);
    }
    break;
  }
  case '7': // Wind Speed: sensor entrada analógico 5V 
            // (Speed(Km/h) = (Tensão - 0,36206896551)/0,0275862069). 
            // Se speed >= 50, fechar janelas + Wind Alert 
  {
    if(msg[3] == LEITURA){
      //Serial.println(controlaVento());
    }else if(msg[3] == ESCRITA){
      
    }
    break;
  }

  default:
    break;
  }
}

int controlaVento(){
  int wind = analogRead(VENTO);
  int tensao = 0;

  tensao = (5 * wind)/1023;
  wind = (tensao - 0.36206896551)/0.0275862069;

  if(wind >= 50){
    if(analogRead(ESTADOJANELAQUARTO) > 204){
      digitalWrite(ENABLEQUARTO, HIGH);
      digitalWrite(MOTORQUARTOANTIHORARIO, HIGH);
    }
    if(analogRead(ESTADOJANELASALA) > 204){
      digitalWrite(ENABLESALA, HIGH);
      digitalWrite(MOTORSALAANTIHORARIO, HIGH);
    }
  }
  return wind;
}

int controlaAquecedor(){
  float temp=0;
  temp = analogRead(LM35);
  temp = (((temp) * 5 / (1023)) / 0.01);
  //Serial.print("Temperatura lida: ");
  //Serial.println(temp);

  if(digitalRead(AQUECEDOR) == 1){
    if((int)temp < 17){
      digitalWrite(AQUECEDOR, LOW); // Liga Aquecedor
      tempAnterior = temp;
    }
  }
  if(digitalRead(AQUECEDOR) == 0){
  if(tempAnterior+bandaMorta <= temp){
    digitalWrite(AQUECEDOR, HIGH); // Desliga Aquecedor
  }
  }
  return temp;
}

void quartoebanheiro(String msg){
  //Serial.println("Mensagem enviada para Quarto/Banheiro!");
  switch (msg[2])
  {
  case '1': // Janela do Quarto: Duas portas digitais de saída 24V controlam (cima/baixo)
  {
    if(msg[3] == LEITURA){
      //Serial.print("Posição da janela: ");
      //Serial.println(analogRead(ESTADOJANELAQUARTO));
    }else if(msg[3] == ESCRITA){
      int m,c,d,u;
      // Converte os valores recebidos em string para inteiros
      m = (msg[6] - '0') * 1000;
      c = (msg[7] - '0') * 100;
      d = (msg[8] - '0') * 10;
      u = (msg[9] - '0');
      posicaoJanelaquarto = m+c+d+u;
      //Serial.print("Valor recebido: ");
      //Serial.println(posicaoJanelaquarto);
      controleJanelaquarto = 1;
      digitalWrite(ENABLEQUARTO, HIGH);
    }
    break;
  }

  case '4': // Luz quarto: saída analógica 5V (0 = off, 5V = brilho max)
  {
    if(msg[3] == LEITURA){
      //Serial.print("Luz da sala em ");
      //Serial.println(valueQuarto);
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
      //Serial.print("Temperatura do LM35: ");
      //Serial.println((float(analogRead(LM35)) * 5 / (1023)) / 0.01);
    }else if(msg[3] == ESCRITA){
      int d,u;
      // Converte os valores recebidos em string para inteiros
      d = (msg[8] - '0') * 10;
      u = (msg[9] - '0');
      bandaMorta = d + u;
    }
    break;
  }


  default:
    break;
  }
}

void retorna(){
  int m, c, d, u, completo;
  

  /*  ENDERECO 01  */
  retorno[0] = String(0);
  retorno[1] = String(0);
  retorno[2] = String(0);
  if(travaPorta(String("oi"),1) == 0){
    retorno[3] = String(0);
  }else
  if(travaPorta(String("oi"),1) == 1){
    retorno[3] = String(1);
  }

  /*  ENDERECO 11  */
  d = tempC / 10;
  u = tempC - (d*10);

  retorno[4] = String(0);
  retorno[5] = String(0);
  retorno[6] = String(d);
  retorno[7] = String(u);

  /*  ENDERECO 14  */
  completo = analogRead(ESTADOJANELASALA);

  m = completo /1000; 
  c = (completo - (m*1000)) / 100;
  d = (completo - (m*1000) - (c*100))/10; 
  u = (completo - (m*1000) - (c*100) - (d*10));

  retorno[8] = String(m);
  retorno[9] = String(c);
  retorno[10] = String(d);
  retorno[11] = String(u);

  /*  ENDERECO 17  */
  completo = controlaVento();

  m = completo /1000; 
  c = (completo - (m*1000)) / 100;
  d = (completo - (m*1000) - (c*100))/10; 
  u = (completo - (m*1000) - (c*100) - (d*10));

  retorno[12] = String(m);
  retorno[13] = String(c);
  retorno[14] = String(d);
  retorno[15] = String(u);

  /*  ENDERECO 21  */
  completo = analogRead(ESTADOJANELAQUARTO);

  m = completo /1000; 
  c = (completo - (m*1000)) / 100;
  d = (completo - (m*1000) - (c*100))/10; 
  u = (completo - (m*1000) - (c*100) - (d*10));

  retorno[16] = String(m);
  retorno[17] = String(c);
  retorno[18] = String(d);
  retorno[19] = String(u);

  /*  ENDERECO 25  */
  completo = controlaAquecedor();

  m = completo /1000; 
  c = (completo - (m*1000)) / 100;
  d = (completo - (m*1000) - (c*100))/10; 
  u = (completo - (m*1000) - (c*100) - (d*10));

  retorno[20] = String(m);
  retorno[21] = String(c);
  retorno[22] = String(d);
  retorno[23] = String(u);

  /*  ENDERECO 26  */
  retorno[24] = String(0);
  retorno[25] = String(0);
  retorno[26] = String(0);
  if(digitalRead(AQUECEDOR) == 1){
    retorno[27] = '0';
  }else{
    retorno[27] = '1';
  }

  for(int i=0;i < 27; i++){
    Serial.print(retorno[i]);
  }
  Serial.println(retorno[27]);
}