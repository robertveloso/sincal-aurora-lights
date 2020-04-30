var mqtt = require('mqtt');
var db = require('../utils/db');
var env=require("../_env.js");
var client  = mqtt.connect(env.mqtt_server);

client.on("connect", ()=>{
  client.subscribe("+/+/updated")
  client.subscribe("register")
});

client.on("message", (topic,message)=>{
  if(topic.includes("updated")){
    update(topic, message.toString());
  }
  if(topic.includes("register")){
    register(topic, message.toString());
  }
});

function register(topic, message){
  var data=JSON.parse(message);
  db.nodes.register(data.code)
  .then((node)=>{
    client.publish(data.code+"/registered", JSON.stringify({topic:node.user+"/"+node.uuid}))
  })
}

function update(topic, message){
  console.log("TOPIC:", topic)
  console.log("MESSAGE:",message)
  var user=topic.split("/")[0]
  var node=topic.split("/")[1]
  var data=JSON.parse(message)
  //TODO: refactor this part.
  db.nodes.get(user, node).then(function(n){
    /*if(n.type=="temperature"){
      var last_day_arr=n.state.last_day || []
      console.log("DATA:", data)
      last_day_arr.push(parseInt(data.temp))
      data.last_day=last_day_arr.slice(Math.max(last_day_arr.length-24, 0))
      console.log("DATA:", data)
    }*/        
    db.nodes.updateState({token:user}, node, data);
  })
}

module.exports=client;
