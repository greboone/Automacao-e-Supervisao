// file: modularServer.js

// modulos
var http = require('http')
var   fs = require('fs');
// porta
const port = 3000
// pagina principal
const mainPage = 'index.html'
const myStyle  = 'style.css'

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


  /* ----- Transformando em hexadecimal ----- */
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

/*****   Porta Serial *****/
const SerialPort = require('serialport');
const Readline = SerialPort.parsers.Readline;
const sPort = new SerialPort('com11', {
  baudRate: 9600
})
const parser = new Readline();
sPort.pipe(parser);

var slaveAdr = '03';
var slaveCmd = '05';
var slaveOut = '01';
var slaveState = '0000';
var mensagem = ':' + slaveAdr + slaveCmd + slaveOut + slaveState;

sPort.open(function (err) {
  if(err) {
      console.log(err.message)
  }
  console.log('Porta Serial Aberta')
})

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
}

