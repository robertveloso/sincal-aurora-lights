var React = require('react');
var TemperatureUI=require("./temperature_ui");
const TemperatureCard = React.createClass({
  getDefaultProps:function() {
    return {
      state:{
        temp:"-",
        humidity:"-",
      }
    };
  },
  getInitialState:function(){
    return {
      open:false
    }
  },
  handleClick:function(){
    this.setState({
      open:!this.state.open,
    })
  },
  
  render:function() {
    var descr="Umidade: "+(this.props.state.humidity !== undefined ? Math.round(100-((this.props.state.humidity/4095)*100)):'0')+"%"
    return (
      <TemperatureUI
      title={this.props.name}
      background={this.props.image}
      //temperature={this.props.state.temp}
      data={this.props.state.last_day}
      open={this.state.open}
      onClick={this.handleClick}
      description={descr}
    />
  )},
});

module.exports=TemperatureCard;
