var React = require('react');
var Redirect = require('react-router').Redirect;
var css=require("./auth.scss");
import store from 'store';
var auth_utils= require("utils/auth");
var storage=require("utils/storage");
var Input= require("components/UI/input/input");
var Button= require("components/UI/button/button");
import {notify} from 'react-notify-toast';
var Container=require("components/UI/page_container/index");

const AuthPage = React.createClass({
  contextTypes:{
    router:React.PropTypes.object.isRequired
  },
  getInitialState:function() {
    return {
      login:true
    };
  },
  handleForm:function(){
    this.setState({
        login:!this.state.login
    })
  },
  handleClick:function(){
    if(this.state.login){
      //handle login
      auth_utils
      .login({email:this.input_email.value(), password:this.input_password.value()})
      .then((response)=>{
        console.log("response", response)
        storage.set("access_token", response.user.token)
        store.dispatch({
          type: 'LOGIN',
          payload:response.user
        })
        setTimeout(()=>{
          //USE redux?
          this.context.router.history.push("/")
        },500)
      })
      .catch((error, data)=>{
        notify.show('Senha incorreta.', "error");
      })
      //
    }else{
      //handle register
      auth_utils
      .register({email:this.input_email.value(), password:this.input_password.value(), name:this.input_name.value()})
      .then((response)=>{
        storage.set("access_token", response.user.token)
        setTimeout(()=>{
          //USE redux?
          this.context.router.history.push("/")
        },500)
      })
      .catch((error)=>{

      })

    }
  },
  render:function() {
    var title= this.state.login ? "Login" : "Registrar"
    var alter_button= !this.state.login ? "Login" : "Registrar"
    console.log("RENDER?")
    return (
      <Container>
        <div className={css.container}>
          <h3>{title}</h3>
          <Input label="Email" ref={(ref) => this.input_email = ref} />
          {!this.state.login &&
            <Input label="Nome" ref={(ref) => this.input_name = ref} />
          }
          <Input label="Senha" type="password"  ref={(ref) => this.input_password = ref} />
          <Button type="button" onClick={this.handleClick}>{title}</Button>
          <Button type="link" onClick={this.handleForm}>{alter_button}</Button>
        </div>
      </Container>
    );
  },
});

module.exports=AuthPage;
