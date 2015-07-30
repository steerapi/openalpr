var express = require('express');
var bodyParser = require('body-parser');
var alpr = require('./build/Release/nodealpr');
var app = express();

alpr.initialize("us","/etc/openalpr/openalpr.conf","/usr/share/openalpr/runtime_data");
alpr.setTopN(7);

app.use(bodyParser.raw());

app.post('/upload', function(req,res){
  json = alpr.recognizeArray(req.body);
  res.status(200).send(json);
});

app.listen(3000);