// file: modularServer.js

// modulos
var http = require('http')
var   fs = require('fs');

// porta
const port = 3000

// pagina principal
const mainPage = 'index.html'
const myStyle  = 'style.css'

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

// servidor ouvindo em 'port'
var app = http.createServer(function(req, res) {
    
    var content = '';   // conteúdo da resposta
    var type = '';      // tipo da resposta: text/html, text/css
    
    if(req.url === '/') {   // requisição na raiz, carrega página principal
        content = fs.readFileSync(mainPage);      
        type = 'text/html';
    }
    else if(req.url === '/style.css') {
        content = fs.readFileSync(myStyle);	// load css
        type = 'text/css';
    }
    
    console.log(req.url) // tipo de requisição realizada
    res.writeHead(200, {'Content-Type': type}); // responde com tipo
    res.end(content + '\n'); // envia conteúdo 
}).listen(port)

var socket = require('socket.io').listen(app);
var flag = 0;
var incremento = 1;

socket.on('connection', function(client) {
    /*client.on('state', function(data){//Do EBONE
        console.log('Valor de tempo recebido do HTML:' + data);
        slaveCmd = data[0]
        slaveOut = data[1]
        slaveState = data[2]
        mensagem = ':'+slaveAdr+slaveCmd+slaveOut+slaveState//":030501FF00";
        lrc = LRC(mensagem)
        mensagem = ':'+slaveAdr+slaveCmd+slaveOut+slaveState+lrc//":030501FF00lrc";
		sPort.write(mensagem)
		console.log(mensagem)
    })*/
	client.on('state', function(Data){//envia na porta serial a msg c/ lrc, com base nos dados recebidos (Data) na conexão state
        console.log('NODEJS: Setado ' + Data);
		slaveAdr = Data[0] //end. escravo
		slaveRW = Data[1] //op. leitura/escrita
		slaveAD = Data[2] //port analógica/digital
		slaveIO = Data[3] //Input / Output
        slaveData = Data[4] //dados: 0000 / 0001 / 0255 / 1024...
		var mensagem = ':' + slaveAdr + slaveRW + slaveAD + slaveIO + slaveData;//mensagem s/ lrc
		msglrc = ((calculateLRC(((Buffer.from(mensagem)).toString()).slice(1))).toString(16)).toUpperCase();//calcula lrc e armazena aqui
		var mensagemlrc = ':'+slaveAdr+slaveRW+slaveAD+slaveIO+slaveData+msglrc;//mensagem c/ lrc
		sPort.write(mensagemlrc)
		//nada a emitir...
    })
	
})
/*
function LRC(str) //do EBONE
{
  var bytes;
  var aux = [];
  var lrc = 0;

*/
  /* ----- Transformando em hexadecimal ----- */
  /*
  for (var i = 1; i < str.length; i+=2) 
  { 
    if (str[i] > '9' && str[i+1] > '9')
      bytes = (Number(str.charCodeAt(i)) - '55')*16 + (Number(str.charCodeAt(i+1)) - '55');
    
    else if (str[i] > '9')
      bytes = (Number(str.charCodeAt(i)) - '55')*16 + (Number(str.charCodeAt(i+1)) - '48');

    else if (str[i+1] > '9')
      bytes = (Number(str.charCodeAt(i)) - '48')*16 + (Number(str.charCodeAt(i+1)) - '55');
    
    else
      bytes = (Number(str.charCodeAt(i)) - '48')*16 + (Number(str.charCodeAt(i+1)) - '48');
    
    aux.push(bytes);
  }

  for (var i = 0; i < (aux.length); i++)
  {

    if((lrc + aux[i]) & 0xFF)
      lrc += aux[i];

  }
  lrc = ((lrc^0xFF) + 1) & 0xFF;

  lrc = lrc.toString(16);
  lrc+="";
  lrc = lrc.toUpperCase();

  return lrc;
}
*/
/*****   Porta Serial *****/
const SerialPort = require('serialport');
const Readline = SerialPort.parsers.Readline;
const sPort = new SerialPort('com3', {
  baudRate: 9600
})
const parser = new Readline();
sPort.pipe(parser);

//Variáveis da mensagem Firmware/backend
var msglrc = '00'; //contém o LRC
var slaveAdr = '00'; //endereço do escravo: Comodo+Objeto
var slaveRW = '0'; //0 ou 1, leitura ou escrita
var slaveAD = 'D'; //A ou D, analógico ou digital
var slaveIO = 'I'; //I(nput) para entrada, O(utput) para saída
var slaveData = '0000'; //dados do escravo, como pwm 0255, on/off 0001/0000, ou outro valor a ser convertido 0000-1023
//var mensagem = ':' + slaveAdr + slaveRW + slaveAD + slaveIO + slaveData; //+(LRC) formato padrão de mensagem

//Senha de acesso (front end)
var pass = '0000'; //padrão é '0000', de 4 digitos. À definir tratamento posteriormente

//mensagem de monitoração: conterá um array de strings com informações de sensores e atuadores monitorados
var UltraMsg; //à definir tamanho, tipo e tratamento posteriormente

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
  console.log(data);
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
})


/*
//do Ebone
parser.on('data', (data) => {
  var new_data = data.split('');
  console.log(data);
  if(new_data[21] == ':'){
    if((new_data[24] == '0') && (new_data[25] == '1')){
      socket.emit('retorno', define_estado(new_data));
    }
    if((new_data[24] == '0') && (new_data[25] == '2')){
      socket.emit('retorno', define_estado(new_data));
    }
    if((new_data[24] == '0') && (new_data[25] == '6')){
      socket.emit('retorno', [new_data[27], new_data[25], [new_data[28],new_data[29],new_data[30],new_data[31]] ]);
    }
  }
})

function define_estado(data) {
  if((data[28] && data[29]) == 'F'){
    return [data[27], data[25], 'Ligado'];
  }
  if((data[28] && data[29]) == '0'){
    return [data[27], data[25], 'Desligado'];
  }
}*/
