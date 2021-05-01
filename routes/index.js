var express = require('express');
var router = express.Router();
var firebase = require('firebase-admin');
var humidityDB;
var heightDB;
var ledDB;
var pumpDB;

//init firebase connection
// const serviceAccount = require('../smart-garden-2020-firebase-adminsdk-7570k-b63df92db5.json');
// firebase.initializeApp({
//   credential: firebase.credential.cert(serviceAccount),
//   databaseURL: 'https://smart-garden-2020-default-rtdb.firebaseio.com/'
// });

// var db = firebase.database();
// var ref = db.ref("Test/DB");

// ref.on("value", function(snapshot) {
//   console.log(snapshot.val());
//   [humidity, height, led, pump] = snapshot.val().Data.split(';');
//   humidityDB = humidity;
//   heightDB = height;
//   ledDB = led;
//   pumpDB = pump;


// }, function (errorObject) {
//   console.log("The read failed: " + errorObject.code);
// });

/* GET home page. */
router.get('/', function(req, res, next) {
  res.render('index', {humidity: humidityDB, height: heightDB});
});

module.exports = router;
