
var app = require('http').createServer(handler);
var io = require('socket.io').listen(app);
var fs = require('fs');

app.listen(8095);


io.sockets.on('connection', function (socket) {
   socket.on('ligar', function (data) {
 
      if (data == 1){
          socket.emit('ligado', 'green');
          socket.broadcast.emit('ligado', 'green');
     }else{
         socket.emit('ligado', 'blue');
         socket.broadcast.emit('ligado', 'blue');
         }
    
  })});


//console.log('Rodando em: http://' + '10.0.10.2' + ':8095');

function handler (req, res) {
  fs.readFile('index.htm',
  function (err, data) {
    if (err) {
      res.writeHead(500);
      return res.end('Erro ao abrir o arquivo: index.html');
    }
    res.writeHead(200);
    res.end(data);
  });
}
 