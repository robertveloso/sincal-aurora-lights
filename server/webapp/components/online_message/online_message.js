const React=require("react");
const cn=require("classnames");
const css=require("./online_message.scss");

const OnlineMessage=function(props){
  let online=navigator.onLine ? css.online : css.offline
  return (
    <div className={cn(css.online_message, online)}>
      No momento você está desconectado.
    </div>
  )
}

module.exports=OnlineMessage;
