var React = require('react');
var Redirect = require('react-router').Redirect;
var css=require("./nodes.scss");
import store from 'store';
import {ImageSelect, TypeSelect} from "components/UI/image_select/image_select"
import nodes_utils from "utils/nodes";
var Input= require("components/UI/input/input");
var Button= require("components/UI/button/button");
import {notify} from 'react-notify-toast';
var Container=require("components/UI/page_container/index");

const NodesPage = React.createClass({
  contextTypes:{
    router:React.PropTypes.object.isRequired
  },
  getInitialState:function() {
    return {
      login:true
    };
  },
  handleClick:function(){
    nodes_utils
    .createNode({name:this.input_name.value(), image:this.image_select.value(), type:this.type_select.value()})
    .then((response)=>{
      notify.show('Módulo criado.');
      store.dispatch({
        type: 'NEW_NODE',
        payload:response
      })
      setTimeout(()=>{
        this.context.router.history.push("/")
      },100)
    })
    .catch((error, data)=>{
      notify.show('Senha incorreta.', "error");
    })
  },
  render:function() {
    return (
      <Container icon="back">
        <div className={css.container}>
          <h3>Adicionar novo módulo</h3>
          <Input label="Nome do módulo" ref={(ref) => this.input_name = ref} />
          <span className={css.select}>
              <TypeSelect ref={(ref)=>this.type_select=ref} />
          </span>
          <span className={css.select}>
            <ImageSelect ref={(ref)=>this.image_select=ref} />
          </span>
        <Button type="button" onClick={this.handleClick}>Salvar</Button>
        </div>
      </Container>

    );
  },
});

module.exports=NodesPage;
