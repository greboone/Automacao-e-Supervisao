// file: modularServer.js

// modulos
var http = require('http')
var   fs = require('fs');

// porta
const port = 3000

//tamanho de mensagem de monitoramento (mais detalhes na descrição de UltraMsg)
const monitsize = 147

// pagina principal
const mainPage = 'index.html'
const myStyle1  = 'style.css'
const myStyle2  = 'styleConfig.css'
const myStyle3  = 'styleMonit.css'

/*---- Converte String para ArrayBUffer -----*/
var convertStringToArrayBuffer = function (str) {
  var buf = new ArrayBuffer(str.length);
  var bufView = new Uint8Array(buf);
  for (var i = 0; i < str.length; i++) {
      bufView[i] = str.charCodeAt(i);
  }
  return buf;
}

/*---- soma dois valores somente com operadores binários -----*/
function somabin(a, b){
	var uncommonBits = a ^ b;
	var commonBits = a & b;

	if(commonBits == 0)
	  return uncommonBits

	return somabin(uncommonBits, (commonBits << 1));
}

/*---- Converte string hexadecimal para ascii -----*/
function hex_to_ascii(str1)
{
	var hex  = str1.toString();
	var str = '';
	for (var n = 0; n < hex.length; n += 2) {
	  str += String.fromCharCode(parseInt(hex.substr(n, 2), 16));
	}
	return str;
}

/*---- Calcula LRC -----*/
function calculateLRC(str) {
	var buffer = convertStringToArrayBuffer(str);
	var testeray = new Uint8Array(Buffer.from(buffer));

	var lrc = 0;
	var exant = 0;
	for (var i = 0; i < str.length; i++) {
	var ex = hex_to_ascii(testeray[i].toString(16));
	if(i%2!=0){
	var duex = exant.concat('',ex);
	var decim = parseInt(duex, 16);
	lrc += decim & 0xFF;
	}
	exant = ex;
	}
	lrc = (somabin((lrc ^ 0xFF), 1) & 0xFF);

	return lrc;
}

//função para delays, deve ser asyn e com uso de await
function sleep(ms) {
  return new Promise((resolve) => {
    setTimeout(resolve, ms);
  });
}
/* Exemplo de chamada:
chama em outro lugar a later()
async function later()
	{
		await sleep(3000);
		socket.emit('conrai2', [slaveOut, global2]);
		global2 = '';
	}
*/

/*função que define a quantidade de caracteres recebidos e transforma em 4*/
function handleSize(text){
  var tam = (text.toString()).length;
  switch (tam){
    case 1:
      return ('000' + text);
      
    case 2:
      return ('00' + text);

    case 3:
      return ('0' + text);

    case 4:
      return (text);

    default:
      return ('0000');
  }
}

/*função que reorganiza mensagens de horário, recebendo no formato 'HH:MM', e retornando como 'HHMM' */
function handleTime(text){
  var str = (text.toString()).split(':');
  var hour = str[0];
  var min = str[1];
  return (msg=hour+min);
}

// servidor ouvindo em 'port'
var app = http.createServer(function(req, res) {
    
    var content = '';   // conteúdo da resposta
    var type = '';      // tipo da resposta: text/html, text/css
    
    if(req.url === '/') {   // requisição na raiz, carrega página principal
        content = fs.readFileSync(mainPage);      
        type = 'text/html';
    }
    else if(req.url === '/style.css') {
        content = fs.readFileSync(myStyle1);	// load css
        type = 'text/css';
    }else if(req.url === '/styleConfig.css') {
        content = fs.readFileSync(myStyle2);	// load css
        type = 'text/css';
    }else if(req.url === '/styleMonit.css') {
        content = fs.readFileSync(myStyle3);	// load css
        type = 'text/css';
    }
    
    console.log(req.url) // tipo de requisição realizada
    res.writeHead(200, {'Content-Type': type}); // responde com tipo
    res.end(content + '\n'); // envia conteúdo 
}).listen(port)

var socket = require('socket.io').listen(app);
var flag = 0;
var incremento = 1;

//nota: a tratar endereços de envio, ou formas de envio específicas para a serial quanto às msg de configuração
socket.on('connection', function(client) {
  /////////////     MENSAGENS DE AÇÃO     /////////////
  //valores recebidos na ordem da varial mensagem, contendo sliders e inputs da pagina de ação
	client.on('state', function(Data){//envia na porta serial a msg c/ lrc, com base nos dados recebidos (Data) na conexão state
        console.log('Setado ' + Data);
		slaveAdr = Data[0] //end. escravo
		slaveRW = Data[1] //op. leitura/escrita
		slaveAD = Data[2] //port analógica/digital
		slaveIO = Data[3] //Input / Output
        slaveData = Data[4] //dados: 0000 / 0001 / 0255 / 1024...
		var mensagem = ':' + slaveAdr + slaveRW + slaveAD + slaveIO + slaveData;//mensagem s/ lrc
		msglrc = ((calculateLRC(((Buffer.from(mensagem)).toString()).slice(1))).toString(16)).toUpperCase();//calcula lrc e armazena aqui
		var mensagemlrc = ':'+slaveAdr+slaveRW+slaveAD+slaveIO+slaveData+msglrc;//mensagem c/ lrc
		sPort.write(mensagemlrc)
		//nada a retornar...
  })

  /////////////     MENSAGENS DE CONFIGURAÇÃO     /////////////

  //ENTRADA PRINCIPAL
  client.on('Entry', function(Data){//armazena em variaveis a senha e time de fechamento da porta com base nos dados recebidos (Data) na conexão Entry
    console.log('Recebido da web: ' + Data);
    pass = Data[0];
    var dct = Data[1];  
    doorCloseTime = handleSize(dct);
    console.log('Senha '+pass+' e tempo de destravamento '+doorCloseTime+' armazenados');
    //msglrc = ((calculateLRC(((Buffer.from(mensagem)).toString()).slice(1))).toString(16)).toUpperCase();
    //sPort.write(mensagemlrc)
    //nada a retornar...
  })
  
  //SALA DE ESTAR
  client.on('LivRoom', function(Data){//armazena em variavel a temperatura inicial do AC da sala de estar, com base nos dados recebidos (Data) na conexão LivRoom
    console.log('Recebido da web:' + Data);
    ACdeg = handleSize(Data);
    console.log('temperatura de AC '+ACdeg+' armazenada');
    //nada a retornar...
  })

  //JANELAS DA SALA DE ESTAR E DE JANTAR (NOTA: ORGANIZAR RECEBIMENTO DO WIND ALERT)
  client.on('Windows', function(Data){//armazena em variaveis os horários de controle das janelas, com base nos dados recebidos (Data) na conexão Windows
    console.log('Recebido da web:' + Data);
    var aux = Data[0] //meio aberto
    windhalf = handleTime(aux);
    aux = Data[1] //abre td
    windop = handleTime(aux);
    aux = Data[2]; //fecha td
    windclose = handleTime(aux);
    console.log('Hora de abertura '+windop+', de intermedio '+windhalf+', e fechamento '+windclose+ ' armazenadas');
    //nada a retornar...
  })

  //QUARTO E BANHEIRO (nota: tratar corretamente o tipo de dado da banda morta)
  client.on('Heater', function(Data){//armazena em variavel o valor da banda morta com base nos dados recebidos (Data) na conexão Heater
    console.log('Recebido da web: ' + Data);
    var aux = Data;
    deadBand = handleSize(aux);
    console.log('Senha '+pass+' e tempo de destravamento '+doorCloseTime+' armazenados');
    //nada a retornar...
  })
  
  //comunicação de monitoramento lá embaixo...
})

/***** Porta Serial *****/
const SerialPort = require('serialport');
const Readline = SerialPort.parsers.Readline;
const sPort = new SerialPort('com7', {
  baudRate: 9600
})
const parser = new Readline();
sPort.pipe(parser);

/* DEFINIÇÔES DE VARIAVEIS GLOBAIS */

//Variáveis da mensagem Firmware/backend
var msglrc = '00'; //contém o LRC
var slaveAdr = '00'; //endereço do escravo: Comodo+Objeto
var slaveRW = '0'; //0 ou 1, leitura ou escrita
var slaveAD = 'D'; //A ou D, analógico ou digital
var slaveIO = 'I'; //I(nput) para entrada, O(utput) para saída
var slaveData = '0000'; //dados do escravo, como pwm 0255, on/off 0001/0000, ou outro valor a ser convertido 0000-1023
//var mensagem = ':' + slaveAdr + slaveRW + slaveAD + slaveIO + slaveData; //+(LRC) formato padrão de mensagem

//Senha de acesso (front end)
var pass = '0000'; //padrão é '0000', deve conter 4 digitos
//tempo de fechamento da porta, em minutos
var doorCloseTime = '0001'; //padrão 1 minuto

//temperatura padrão do ar condicionado (25ºC)
var ACdeg = '0025';

//horarios de controle das janelas (formato: HHMM)
var windop = '0830'; //abre totalmente as 8:30
var windhalf = '1600'; //meio abertas as 16:30
var windclose = '1830'; //fecha totalmente as 18:30

//valor da banda morta, que controla o aquecedor
var deadBand = '0';

//mensagem de monitoração: conterá um array de strings com informações de sensores e atuadores monitorados
var UltraMsg; //tamanho e formato: char de entrada ':' + 9 caracteres * 16 itens + LRC, contendo 147 caracteres no total

sPort.open(function (err) {
  if(err) {
      console.log(err.message)
  }
  console.log('Porta Serial Aberta')
})
//var mensagemantiga = ':'+slaveAdr+slaveCmd+slaveOut+slaveState;=01/23/45/6789...
//:+10+2(LRC)=01/2/3/4/5678...
//tratamento de mensagens recebidas do arduino (respostas)
parser.on('data', (data) => {
  console.log('Node recebe do controlador: '+data);

  if(data.length >= monitsize){
    /////////////     MENSAGENS DE MONITORAMENTO     /////////////
    //16 mensagens de tamanho 9, formando: ":+(9chars)*16+LRC"
    //Formato segue conforme a descrição no arquivo disponível no docs, de item a item
    //link do docs: https://docs.google.com/document/d/10i8FvYkEybzUDhKXuwmf3X4So2C-hNTiveFeKyaUtrA/edit
    socket.emit('Monit', [UltraMsg]);//comunicação 'Monit', dado: UltraMsg (descrito nas definições de variaveis)

    //nota: ainda não definido, pois pode ser q mudemos essa parte, sendo o tratamento feito pelo servidor ou pela web
  }else{
    //nota: não usado pra nada por enquanto...
    var msgarray = data.split(':');//string to char array, a partir do ':'
    //console.log(msgarray[3]);
    var wordarray = '';
    if(msgarray[3]){//se tem 3 strings separas por ':'
      var aux = msgarray[3];//pega a última parte, a partir de ':'
      var wordarray = aux.split('');//e separa em um array de chars
    }
    /*AQUI: Definir comunicações com frontend
    if(wordarray[6] == 'F' && wordarray[7] == 'F'){
      global = 'Ligada';
    }else if(wordarray[6] == '0' && wordarray[7] == '0'){
      global = 'Desligada';
    }
    if(wordarray){
      global2 = wordarray[6]+wordarray[7]+wordarray[8]+wordarray[9];
    }
    */
    //console.log(global2);
  }
})
