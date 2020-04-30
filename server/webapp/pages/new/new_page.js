import React from 'react';
import css from "./style.scss";
import Container from "components/UI/page_container/index";
import { Link } from 'react-router-dom'

function ChooseBox(props){
  return (
    <div className={css.choosebox}>
      <Link to={props.to}>
        <h3>{props.label}</h3>
      </Link>
    </div>

  )
}

 export default class NewPage extends React.Component{
  render(){
    return (
      <Container icon="back">
        <div className={css.container}>
          <h3>O que você deseja adicionar?</h3>
          <div className={css.box_container}>
            <ChooseBox to="/nodes" label="Módulo"/>
            <ChooseBox to="/scenes" label="Bloco"/>
          </div>
      </div>
      </Container>

    );
  }
}
