var React = require('react');
var css=require("./user.scss");
var User=require("./user.js");
var utils=require("utils/switch");
import store from 'store';
var Button=require("components/UI/button/button");
var Input=require("components/UI/input/input");

const Users=React.createClass({
  onClick:function(){
    let email=this.input_email.value()
    utils.addUser(this.props.node, email)
  },
  render:function(){
    return(
        <div className={css.users_container}>
          <div className={css.subtitle}>
            Compartilhar com:
          </div>
          { this.props.users.map(function(u){
              return <User node={this.props.node} key={u} user={u}></User>
            }.bind(this))
          }
          <Input type="email" theme="transparent" ref={(ref)=>this.input_email=ref} placeholder="Adicionar email..."></Input>
          <Button type="round" onClick={this.onClick}>Adicionar usuário</Button>
        </div>
    )
  }
})


module.exports=Users;
