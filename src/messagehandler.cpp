#include <Arduino.h>
#include <stdint.h>
#include "lrc.h"
#include "messagehandler.h"
#include "comodos.h"


void msgInit()
{
    Serial.begin(9600);
    Serial.println("Sistema Inciado.");
}

void msgHandler()
{
if (Serial.available() > 0) 
   {
    while(Serial.available() > 0 )
    {
      // ler toda a mensagem até encontrar LF
      String msg = Serial.readStringUntil('\n');
      String usar;
      usar = msg;
      int tam;

      //teste
      Serial.print("Msg recebida: ");
      Serial.println(msg);
      
      while(msg[tam] != '\0')
          ++tam;
      
      // if(tam == 13) tam-=1;
      // else if(tam == 14) tam-=2;

      // início é valido?
      if(msg[0] == ':')
      {
        Serial.println("Caracter de inicio (:) encontrado");

        //Cálculo LRC
        int j = 0;
        uint8_t dado[tam/2], t, valor = 0;
        
        //conversao para decimal
        for (int i = 1; i <= tam; i+=2)     
        {                                  
            if (msg[i] > '9' && msg[i+1] > '9')
                dado[j] = ((usar[i]-'7'))*16 + ((usar[i+1]-'7'));
            
            else if (msg[i] > '9')
                dado[j] = ((usar[i]-'7'))*16 + (usar[i+1]-'0');
            
            else if (msg[i+1] > '9')
                dado[j] = (usar[i]-'0')*16 + ((usar[i+1]-'7'));
            
            else
                dado[j] = (usar[i]-'0')*16 + (usar[i+1]-'0');
                
            j++;
        }
        
        t = (uint8_t)(sizeof(dado)) - 1;
        valor = lrc(dado, t); //cálculo do lrc
        Serial.print("Valor LRC ");
        Serial.println(valor);

        Serial.print("Valor dado ");
        Serial.println(dado[t]);

        //compara se o LRC da msg é igual ao calculado
        if (valor == dado[t])
            Serial.println("LRC correto");
        else
        {
            Serial.println("LRC incorreto");
            break;
        }
        // mensagem é para mim?
        switch (msg[1])
        {
          case '0':
          {
            portaentrada(msg);
            break;
          }
          
          case '1':
          {
            saladeestar(msg);
            break;
          }

          case '2':
          {
            quartoebanheiro(msg);
            break;
          }

          default:
            break;

        }        
        else
        {
          Serial.println("Mensagem para outro escravo."); 
        }
      } 
      else
      {
        Serial.println("Formato de mensagem desconhecido.");    
      }
    }
  }
}



// Funcoes para o Switch case
void read_coil(String msg){
  int coil = ((msg[5]-'0')*10 + (msg[6]-'0')) + OUTPUT_OFFSET;
  // debug
  Serial.print("Leitura na Saida ");
  Serial.println(coil-OUTPUT_OFFSET);

  // Ligada ou desligada?
  if(!digitalRead(coil))
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

void write_coil(String msg){
  // string para inteiro aplicando offset, saida 1 no Arduino está mapeada para o pino 8, saída 2 para o pino 9 ....
  int coil = ((msg[5]-'0')*10 + (msg[6]-'0')) + OUTPUT_OFFSET;

  // padrao é desligar
  int value = OFF;

  // debug
  Serial.print("Escrita na Saida ");
  Serial.println(coil-OUTPUT_OFFSET);

  // ligar ou desligar?
  if(msg[7]=='F' && msg[8]=='F' && msg[9]=='0' && msg[10]=='0')
  {
    value = ON;
    Serial.println(" -> Mudar para Ligado");
  }
  else if(msg[7]=='0' && msg[8]=='0' && msg[9]=='0' && msg[10]=='0')
  {
    value = OFF;
    Serial.println(" Mudar para Desligado");
  }
  // executao comando
  digitalWrite(coil, value);
  
  // Para esse caso, a resposta é um simples echo da mensagem original
  Serial.print("Resposta do Escravo: ");
  Serial.println(msg);
}

void read_analog(String msg){
  // decodifica entrada a ser lida
  int ain = ((msg[5]-'0')*10 + (msg[6]-'0')) + ANALOG_INPUT_OFFSET;

  // debug
  Serial.print("Leitura na Entrada Analogica ");
  Serial.println(ain+1);

  // executao comando
  uint16_t value = analogRead(ain);
  
  // int para string
  char buf[5];
  sprintf(buf,"%04d", value);

  // monta valor de retonro
  msg[7]=buf[0];
  msg[8]=buf[1];
  msg[9]=buf[2];
  msg[10]=buf[3];
  
  // Responde para o mestre
  Serial.print("Resposta do Escravo: ");
  Serial.println(msg);
}

void write_analog(String msg){
  // decodifica entrada a ser lida
  int aon = ((msg[5]-'0')*10 + (msg[6]-'0')) + ANALOG_OUTPUT_OFFSET;
  // de string para inteito
  int value = (msg[7]-'0')*1000 + (msg[8]-'0')*100 + (msg[9]-'0')*10 +(msg[10]-'0');

  // debug
  Serial.print("Escrita na Saida Analogica ");
  Serial.println(aon-ANALOG_OUTPUT_OFFSET);
  Serial.print("Valor: ");
  Serial.println(value);

  // escreve na saída
  analogWrite(aon, value);

  // Para esse caso, a resposta é um simples echo da mensagem original
  Serial.print("Resposta do Escravo: ");
  Serial.println(msg);
}








