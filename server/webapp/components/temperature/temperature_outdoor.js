var React = require('react');

var TemperatureCard=require("./temperature_ui");
var utils=require("utils/weather");

const OutdoorCard = React.createClass({
  componentDidMount:function(){
    utils.getWeather().then(function(r){
      this.setState(r.data);
    }.bind(this));
  },
  getInitialState:function() {
    return {
      temp:"-",
      descr:"-",
      open:false,
    };
  },
  render:function() {
    var temp=""+this.state.temp;
    return (
      <TemperatureCard
      title="Clima"
      background="assets/outdoor.jpg"
      temperature={temp}
      open={this.state.open}
      description={this.state.descr}
    />
  )},
});

module.exports=OutdoorCard;
