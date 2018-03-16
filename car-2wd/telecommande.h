//
// Page HTML pour gérer le mode Télécommande
//

const char car2wd_telecommande[] PROGMEM = R"=====(
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
<h3>CAR-2WD mode t&eacutel&eacutecommande</h3>
</div>
<form action="/telecommande" method="POST">
  <center>
  <div><input type="submit" style="text-align: center; height: 50px; width: 100px" name="avance" value="Avance"></div>
  </br>
  <div><input type="submit" style="text-align: center; height: 50px; width: 100px" name="gauche" value="Gauche">
  &nbsp&nbsp
  <input type="submit" style="text-align: center; height: 50px; width: 100px" name="stop" value="Stop">
  &nbsp&nbsp
  <input type="submit" style="text-align: center; height: 50px; width: 100px" name="droite" value="Droite"></div>
  </br>
  <div><input type="submit" style="text-align: center; height: 50px; width: 100px" name="recule" value="Recule"></div>
  </br>
  </center>
</form>
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
