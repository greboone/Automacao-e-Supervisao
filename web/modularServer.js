// file: modularServer.js

// modulos
var http = require('http')
var   fs = require('fs');

// porta
const port = 3000

// pagina principal
const mainPage = 'index.html'
const myStyle  = 'style.css'

function define_estado(data) {//EBONE
  if((data[28] && data[29]) == 'F'){
    return [data[27], data[25], 'Ligado'];
  }
  if((data[28] && data[29]) == '0'){
    return [data[27], data[25], 'Desligado'];
  }
}

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

/*---- Da um delay na função que à chama (usar await, em uma async) ----*/
function sleep(ms) {
  return new Promise((resolve) => {
    setTimeout(resolve, ms);
  });
}
/*
EXEMPLO DE CHAMADA:
async function later()
	{
		await sleep(3000);
		socket.emit('conrai2', [slaveOut, global2]);
		global2 = '';
	}
*/

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
  client.on('state', function(writeData){
      console.log('NODEJS: Setado ' + writeData);
  slaveCmd = writeData[0]
  slaveOut = writeData[1] //porta: 01 / 02 / 03 / 04
      slaveState = writeData[2] //estado: FF00 / 0000 / ...
  var mensagem = ':'+slaveAdr+slaveCmd+slaveOut+slaveState;
  msglrc = ((calculateLRC(((Buffer.from(mensagem)).toString()).slice(1))).toString(16)).toUpperCase();
  var mensagemplrc = ':'+slaveAdr+slaveCmd+slaveOut+slaveState+msglrc;
  sPort.write(mensagemplrc)
  //nada a emitir...
  })

client.on('conrdi', function(RDI){
  console.log('NODEJS: Lido Digital ' + RDI);
  slaveCmd = RDI[0]
  slaveOut = RDI[1] //porta: 01 / 02 / 03 / 04
      slaveState = RDI[2] //estado: FF00 / 0000 / ...
  var mensagem = ':'+slaveAdr+slaveCmd+slaveOut+slaveState;
  msglrc = ((calculateLRC(((Buffer.from(mensagem)).toString()).slice(1))).toString(16)).toUpperCase();
  var mensagemplrc = ':'+slaveAdr+slaveCmd+slaveOut+slaveState+msglrc;
  sPort.write(mensagemplrc)
  socket.emit('conrdi2', [slaveOut, global]);
  global = '';
})

client.on('conrdo', function(RDO){
  console.log('NODEJS: Lido Digital ' + RDO);
  slaveCmd = RDO[0]
  slaveOut = RDO[1] //porta: 01 / 02 / 03 / 04
      slaveState = RDO[2] //estado: FF00 / 0000 / ...
  var mensagem = ':'+slaveAdr+slaveCmd+slaveOut+slaveState;
  msglrc = ((calculateLRC(((Buffer.from(mensagem)).toString()).slice(1))).toString(16)).toUpperCase();
  var mensagemplrc = ':'+slaveAdr+slaveCmd+slaveOut+slaveState+msglrc;
  sPort.write(mensagemplrc)
  socket.emit('conrdo2', [slaveOut, global]);
  global = '';
})

client.on('conrai', function(RAI){
  console.log('NODEJS: Lido Analog ' + RAI);
  slaveCmd = RAI[0]
  slaveOut = RAI[1] //porta: 01 / 02 / 03 / 04
      slaveState = RAI[2] //estado: FF00 / 0000 / ...
  var mensagem = ':'+slaveAdr+slaveCmd+slaveOut+slaveState;
  msglrc = ((calculateLRC(((Buffer.from(mensagem)).toString()).slice(1))).toString(16)).toUpperCase();
  var mensagemplrc = ':'+slaveAdr+slaveCmd+slaveOut+slaveState+msglrc;
  sPort.write(mensagemplrc)
  later();
})

async function later()
{
  await sleep(3000);
  socket.emit('conrai2', [slaveOut, global2]);
  global2 = '';
}
})

/*
socket.on('connection', function(client) {
    client.on('state', function(data){
        console.log('Valor de tempo recebido do HTML:' + data);
        slaveCmd = data[0]
        slaveOut = data[1]
        slaveState = data[2]
        mensagem = ':'+slaveAdr+slaveCmd+slaveOut+slaveState//":030501FF00";
        lrc = LRC(mensagem)
        mensagem = ':'+slaveAdr+slaveCmd+slaveOut+slaveState+lrc//":030501FF00lrc";
		sPort.write(mensagem)
		console.log(mensagem)
    })})

function LRC(str) 
{
  var bytes;
  var aux = [];
  var lrc = 0;

*/
  /* ----- Transformando em hexadecimal ----- *//*
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
const sPort = new SerialPort('com7', {
  baudRate: 9600
})
const parser = new Readline();
sPort.pipe(parser);

var global = '';
var global2 = '0';
var slaveAdr = '00';
var slaveCmd = '1';
var slavePType = 'D';
var slaveState = '0000';
var msglrc = '00';
var flag = 0;
var mensagem = ':' + slaveAdr + slaveCmd + slavePType + slaveState;

sPort.open(function (err) {
  if(err) {
      console.log(err.message)
  }
  console.log('Porta Serial Aberta')
})



parser.on('data', (data) => {
  var new_data = data.split('');
  console.log(data);
  var msgarray = data.split(':');//string to char array
  var wordarray = '';
  if(msgarray[3]){
	  var aux = msgarray[3];
	  var wordarray = aux.split('');
  }
  if(wordarray[6] == 'F' && wordarray[7] == 'F'){
	  global = 'Ligada';
  }else if(wordarray[6] == '0' && wordarray[7] == '0'){
	  global = 'Desligada';
  }
  if(wordarray){
	  global2 = wordarray[6]+wordarray[7]+wordarray[8]+wordarray[9];
  }
  /*
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
 */
})


<<<<<<< HEAD


=======
function define_estado(data) {
  if((data[28] && data[29]) == 'F'){
    return [data[27], data[25], 'Ligado'];
  }
  if((data[28] && data[29]) == '0'){
    return [data[27], data[25], 'Desligado'];
  }
}
>>>>>>> 7a9571da549d0d9e64e0d72f2eb60eafe9a528b9
