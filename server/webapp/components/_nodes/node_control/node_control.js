var React = require('react');
var css=require("./node_control.scss");
var Switch= require('components/switch/switch');
var Slider=require("components/UI/slider/slider");

const NodeControls = React.createClass({
  onChange:function(what){
    let change={open: !this.props.node.state.open}
    this.props.onChange(change)
  },
  onChangeSlider:function(what){
    let change={brightness: what}
    this.props.onChange(change)
  },
  render:function(){
    let label=(this.props.node.state.open ? "ligado" : "desligado");
    return(
      <div>
        <div className={css.subtitle}>Atualmente {this.props.node.name} está {label}</div>
        <div className={css.cont}>
          <div className={css.switch_container}>
            <Switch open={this.props.node.state.open} onChange={this.onChange} labelOff={"Switch On"} labelOn={"Switch Off"} />
          </div>
        </div>
        { this.props.node.type=="hue" &&
        <div className={css.slider}>
          <div className={css.subtitle}>Brilho: {parseInt(this.props.node.state.brightness/254*100)}%</div>
          <Slider onChange={this.onChangeSlider} value={this.props.node.state.brightness || 254} />
        </div>
        }
      </div>
    )
  }
});

module.exports=NodeControls;
