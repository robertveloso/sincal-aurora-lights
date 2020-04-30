var React = require('react');
var SwitchCard=require("components/switch_card/switch_card");
import SceneCard from "components/scene_card/scene_card";
var Section=require("components/UI/section/section");
var HeaderCard=require("components/header/header_card");
var WeatherCard=require("components/temperature/temperature_outdoor");
var TemperatureCard=require("components/temperature/temperature_card");
var utils=require("utils/auth");
var storage=require("utils/storage");
var Anime = require("react-anime").default;
var Button=require("components/UI/button/button")
import { Link } from 'react-router-dom'
import { connect } from 'react-redux';
import store from 'store';

const HomePage = React.createClass({
  contextTypes:{
    router:React.PropTypes.object.isRequired
  },
  componentDidMount:function(){
    this.setState({rendered:true})
    if(!this.props.user.authenticated){
      this.context.router.history.push("/auth")
    }else{
      utils.getUserState(this.props.token).then(function(state){
        store.dispatch({
          type: 'GET_USER_STATE',
          payload:state
        })
      }.bind(this))
    }
  },
  getInitialState:function(){
    return {
      rendered:false,
      edit:false
    }
  },
  getDefaultProps:function(){
    return {
      token: storage.get("access_token"),
      info:{
        name:""
      },
      nodes:[]
    };
  },
  toggleEdit:function(){
    this.setState({
      edit:!this.state.edit
    })
  },
  render:function() {
    return (
      <div>
        <HeaderCard user={this.props.user}/>
        <Anime autoplay={!this.state.rendered} opacity={[0, 1]} duration={1000} translateY={['-1em','0em']} delay={(e, i) => i * 300}>
        <div>
          <WeatherCard/>
          { this.props.nodes.filter((n)=>{return n.type=="temperature"}).map((node)=>{
            return (
                <TemperatureCard editMode={this.state.edit} name={node.name} node={node.uuid} key={node.uuid} image={node.image} state={node.state}/>
              )
            })
          }
        </div>
        <div>
          {this.props.scenes.length>0 &&
            <Section title="Blocos" direction="horizontal">
              { this.props.scenes.map((scene)=>{
                  return (
                    <SceneCard name={scene.name} scene={scene.uuid} key={scene.uuid} triggers={scene.triggers} image={scene.image}/>
                  )
                })
              }
            </Section>
          }
        </div>
        <div>
          <Section title="Módulos" direction="horizontal">
            { this.props.nodes.filter((n)=>{return ["switch", "hue"].includes(n.type)}).map((node)=>{
                return (
                  <SwitchCard editMode={this.state.edit} name={node.name} node={node.uuid} key={node.uuid} image={node.image} state={node.state} verb="está"/>
                )
              })
            }
          </Section>
        </div>


        <div>
          <Button type="block" to="/new">Novo</Button>
        </div>
      </Anime>
      </div>
    );

  },
});

const mapStateToProps = function(state) {
  return state;
}

module.exports=connect(mapStateToProps)(HomePage);
