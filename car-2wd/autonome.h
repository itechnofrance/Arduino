//
// Page HTML pour gérer le mode autonome
//

const char car2wd_autonome[] PROGMEM = R"=====(
<!DOCTYPE HTML>
<meta name="viewport" content="width=device-width, initial-scale=1" />
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<title>CAR-2WD</title>
<SCRIPT>
xmlHttp=createXmlHttpObject();
function createXmlHttpObject()
{
  if(window.XMLHttpRequest) { xmlHttp=new XMLHttpRequest(); }
  else { xmlHttp=new ActiveXObject('Microsoft.XMLHTTP'); }
  return xmlHttp;
}

function process()
{
  if(xmlHttp.readyState==0||xmlHttp.readyState==4)
  {
    xmlHttp.onreadystatechange=function()
    {
      if(xmlHttp.readyState==4&&xmlHttp.status==200)
      {
        xmlDoc=xmlHttp.responseXML;
        xmlmsg=xmlDoc.getElementsByTagName('bouton_status')[0].firstChild.nodeValue;
        if (xmlmsg==1) 
        {
          document.getElementById('bouton_start').disabled=false;
          document.getElementById('bouton_stop').disabled=true;
        }
        else 
        {
          document.getElementById('bouton_start').disabled=true;
          document.getElementById('bouton_stop').disabled=false;
        }
        xmlmsg=xmlDoc.getElementsByTagName('slidervitesse')[0].firstChild.nodeValue;
        document.getElementById('slider_vitesse').value=xmlmsg;
        document.getElementById('Slidervitesse').innerHTML=xmlmsg;  
      }
    }
    xmlHttp.open('PUT','xml',true);
    xmlHttp.send(null);
  }
}

function slider()
{
  Slidervitesse=document.getElementById('slider_vitesse').value;
  document.getElementById('Slidervitesse').innerHTML=Slidervitesse;
  if(xmlHttp.readyState==0||xmlHttp.readyState==4)
  {
    xmlHttp.open('PUT','set_vitesse?val='+Slidervitesse,true);
    xmlHttp.send(null);
  }
}
</SCRIPT>

<BODY onload="process()">
<div align="Center">
  <h3>CAR-2WD mode autonome</h3>
  <form action="/autonome" method="POST">
    <input type="submit" style="text-align: center; height: 30px; width: 80px" name="bouton_start" value="Start" id="bouton_start">
    &nbsp&nbsp&nbsp&nbsp
    <input type="submit" style="text-align: center; height: 30px; width: 80px" name="bouton_stop" value="Stop" id="bouton_stop">
  </form>
</div>
</br>
<div align="Center">
  Vitesse : <A id=Slidervitesse> </A> </br>
  <input style="width:250px; height:50px" type="range" id="slider_vitesse" ONINPUT=slider() min="0" max="100" step="1"/> </br>  
</div> 
<form action="/" method="POST">
  <div align="Right">
  <button type="submit" style="background-color: lightgreen; text-align: center; text-align: center; height: 30px; width: 80px">Accueil</button>
  </div>
</form>
</BODY>
</HTML>
)=====";

