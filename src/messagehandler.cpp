#include "messagehandler.h"

unsigned int BAUDRATE ;//= 9600;

void msgInit()
{
    Serial.begin(BAUDRATE);
    Serial.println("Serial Iniciado.");
    iniciaRtc();
}

uint8_t calculalrc(uint8_t *msg, uint8_t t)
{
    uint8_t LRCV = 0;
    for(int j = 0; j < t; j++){
        LRCV += msg[j] & 0xFF;
    }
    LRCV = (((LRCV ^ 0xFF) + 1) & 0xFF);
    
    return LRCV;
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

        // //Cálculo LRC
        // int j = 0;
        // uint8_t dado[tam/2], t, valor = 0;
        
        // //conversao para decimal
        // for (int i = 1; i <= tam; i+=2)     
        // {                                  
        //     if (msg[i] > '9' && msg[i+1] > '9')
        //         dado[j] = ((usar[i]-'7'))*16 + ((usar[i+1]-'7'));
            
        //     else if (msg[i] > '9')
        //         dado[j] = ((usar[i]-'7'))*16 + (usar[i+1]-'0');
            
        //     else if (msg[i+1] > '9')
        //         dado[j] = (usar[i]-'0')*16 + ((usar[i+1]-'7'));
            
        //     else
        //         dado[j] = (usar[i]-'0')*16 + (usar[i+1]-'0');
                
        //     j++;
        // }
        
        // t = (uint8_t)(sizeof(dado)) - 1;
        // valor = lrc(dado, t); //cálculo do lrc
        // Serial.print("Valor LRC ");
        // Serial.println(valor);

        // Serial.print("Valor dado ");
        // Serial.println(dado[t]);

        // //compara se o LRC da msg é igual ao calculado
        // if (valor == dado[t])
        //     Serial.println("LRC correto");
        // else
        // {
        //     Serial.println("LRC incorreto");
        //     //break;
        // }

        uint8_t soma=0;
        uint8_t buff[((msg.length()-1)/2)-1];
        for(int x = 1; x<(msg.length()-2); x++){//x=9 msg[x]=5 //0255
          if(msg[x] >= '0' && msg[x] <= '9')//entre 0 e 9
          {
          if(x % 2 != 0){//primeiro valor
            soma += ((msg[x] - '0') * 16);//TAVA 10
          }else{//segundo valor
            soma += (msg[x] - '0');
            buff[(x - 1) / 2] = soma;
            soma = 0;
          }
          }else if(msg[x] >= 'A' && msg[x] <= 'F')//entre A e F
          {
          if(x % 2 != 0){//primeiro valor
            soma += ((msg[x] - 'A' + 10) * 16);
          }else{//segundo valor
            soma += (msg[x] - 'A' + 10);
            buff[(x - 1) / 2] = soma;
            soma = 0;
          }
          }else{
          Serial.println("ARDUINO: Caractere estranho encontrado");
          }
        }

        
        char hexa[2];
        char aux;
        uint8_t quotient = calculalrc(buff, sizeof(buff));
        uint8_t remainder = 0;
        int k = 0;
        
        while(quotient != 0)
        {
          remainder = quotient % 16;
          if(remainder < 10)
          {
          hexa[k++] = 48 + remainder;
          }else{
          hexa[k++] = 55 + remainder;
          }
          quotient /= 16;
        }
        
        aux = hexa[0];
        hexa[0] = hexa[1];
        hexa[1] = aux;

        if(msg[10]==hexa[0] && msg[11]==hexa[1]){
          
          Serial.println("ARDUINO: LRC validado!");
          
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

            case '3':
            {
              setBaud(msg);
              break;
            }
            default:
              break;

          }
          //...
        }
        else
        {
          Serial.println("Erro, LRC invalido!");
        }
        
      } 
      else
      {
        Serial.println("Formato de mensagem desconhecido.");    
      }
    }
  }
}

void schedule(){
  controlaMotores();
  checkDoor(0);
  checkBuzzer();
  temperaturaSala();
  controlaAquecedor();
  controlaVento();
  retorna();
}

void setBaud(String msg){
  int novo = 0;
  novo = ((msg[4] - '0') * 100000) + novo;
  novo = ((msg[5] - '0') * 10000) + novo;
  novo = ((msg[6] - '0') * 1000) + novo;
  novo = ((msg[7] - '0') * 100) + novo;
  novo = ((msg[8] - '0') * 10) + novo;
  novo = ((msg[9] - '0')) + novo;
  if(novo == 300 || novo == 1200 || novo == 2400 || novo == 4800 || novo == 9600 || 
     novo == 19200 || novo == 38400 || novo == 57600 || novo == 74880 || novo == 115200 ||
     novo == 230400 || novo == 250000 || novo == 500000)
  {
    BAUDRATE = novo;
  }else{
    Serial.print("Valor invalido!");
  }

  Serial.println("Alterando BaudRate");
  msgInit();
}