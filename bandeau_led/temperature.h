//
// Page HTML température
//

const char SITE_temperature[] PROGMEM = R"=====(
<!DOCTYPE HTML>
<meta name="viewport" content="width=device-width, initial-scale=1" />
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<title>Ambiance</title>
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
        xmlmsg=xmlDoc.getElementsByTagName('temp_actuelle')[0].firstChild.nodeValue;
        document.getElementById('temp_act').innerHTML=xmlmsg;
        xmlmsg=xmlDoc.getElementsByTagName('temp_graphique')[0].firstChild.nodeValue;
        x=document.getElementById('graphique');
        x.setAttribute("points", xmlmsg);
      }
    }
    xmlHttp.open('PUT','xml',true);
    xmlHttp.send(null);
  }
}
</SCRIPT>

<BODY onload="process()">
<fieldset> <legend>Mesure de la temp&eacuterature</legend>
  <div align="Center">
    <h1><A id=temp_act> </A> °C</h1>
  </div>
</fieldset>
<fieldset> <legend>Temp&eacuterature des derni&egraveres 24 heures</legend>
  <svg with="300px" height="300px">
    <g class="y-axis">
      <text y="275" x="14">0°C</text>
      <text y="225" x="5">10°C</text>
      <text y="175" x="5">20°C</text>
      <text y="125" x="5">30°C</text>
      <text y="75" x="5">40°C</text>
      <text y="25" x="5">50°C</text>
    </g>
    <g class="x-axis">
      <text y="290" x="40">24h</text>
      <text y="290" x="100">18h</text>
      <text y="290" x="160">12h</text>
      <text y="290" x="220">6h</text>
      <text y="290" x="280">0h</text>
    </g>
    <line x1=50 y1=270 x2=50 y2=20 stroke="black" stroke-width="2" />
    <line x1=60 y1=270 x2=60 y2=20 stroke="black" stroke-width="1" />
    <line x1=70 y1=270 x2=70 y2=20 stroke="black" stroke-width="1" />
    <line x1=80 y1=270 x2=80 y2=20 stroke="black" stroke-width="1" />
    <line x1=90 y1=270 x2=90 y2=20 stroke="black" stroke-width="1" />
    <line x1=100 y1=270 x2=100 y2=20 stroke="black" stroke-width="1" />
    <line x1=110 y1=270 x2=110 y2=20 stroke="black" stroke-width="2" />
    <line x1=120 y1=270 x2=120 y2=20 stroke="black" stroke-width="1" />
    <line x1=130 y1=270 x2=130 y2=20 stroke="black" stroke-width="1" />
    <line x1=140 y1=270 x2=140 y2=20 stroke="black" stroke-width="1" />
    <line x1=150 y1=270 x2=150 y2=20 stroke="black" stroke-width="1" />
    <line x1=160 y1=270 x2=160 y2=20 stroke="black" stroke-width="1" />
    <line x1=170 y1=270 x2=170 y2=20 stroke="black" stroke-width="2" />
    <line x1=180 y1=270 x2=180 y2=20 stroke="black" stroke-width="1" />
    <line x1=190 y1=270 x2=190 y2=20 stroke="black" stroke-width="1" />
    <line x1=200 y1=270 x2=200 y2=20 stroke="black" stroke-width="1" />
    <line x1=210 y1=270 x2=210 y2=20 stroke="black" stroke-width="1" />
    <line x1=220 y1=270 x2=220 y2=20 stroke="black" stroke-width="1" />
    <line x1=230 y1=270 x2=230 y2=20 stroke="black" stroke-width="2" />
    <line x1=240 y1=270 x2=240 y2=20 stroke="black" stroke-width="1" />
    <line x1=250 y1=270 x2=250 y2=20 stroke="black" stroke-width="1" />
    <line x1=260 y1=270 x2=260 y2=20 stroke="black" stroke-width="1" />
    <line x1=270 y1=270 x2=270 y2=20 stroke="black" stroke-width="1" />
    <line x1=280 y1=270 x2=280 y2=20 stroke="black" stroke-width="1" />
    <line x1=290 y1=270 x2=290 y2=20 stroke="black" stroke-width="2" />
    <line x1=50 y1=20 x2=290 y2=20 stroke="black" stroke-width="2" />
    <line x1=50 y1=30 x2=290 y2=30 stroke="black" stroke-width="1" />
    <line x1=50 y1=40 x2=290 y2=40 stroke="black" stroke-width="1" />
    <line x1=50 y1=50 x2=290 y2=50 stroke="black" stroke-width="1" />
    <line x1=50 y1=60 x2=290 y2=60 stroke="black" stroke-width="1" />
    <line x1=50 y1=70 x2=290 y2=70 stroke="black" stroke-width="2" />
    <line x1=50 y1=80 x2=290 y2=80 stroke="black" stroke-width="1" />
    <line x1=50 y1=90 x2=290 y2=90 stroke="black" stroke-width="1" />
    <line x1=50 y1=100 x2=290 y2=100 stroke="black" stroke-width="1" />
    <line x1=50 y1=110 x2=290 y2=110 stroke="black" stroke-width="1" />
    <line x1=50 y1=120 x2=290 y2=120 stroke="black" stroke-width="2" />
    <line x1=50 y1=130 x2=290 y2=130 stroke="black" stroke-width="1" />
    <line x1=50 y1=140 x2=290 y2=140 stroke="black" stroke-width="1" />
    <line x1=50 y1=150 x2=290 y2=150 stroke="black" stroke-width="1" />
    <line x1=50 y1=160 x2=290 y2=160 stroke="black" stroke-width="1" />
    <line x1=50 y1=170 x2=290 y2=170 stroke="black" stroke-width="2" />
    <line x1=50 y1=180 x2=290 y2=180 stroke="black" stroke-width="1" />
    <line x1=50 y1=190 x2=290 y2=190 stroke="black" stroke-width="1" />
    <line x1=50 y1=200 x2=290 y2=200 stroke="black" stroke-width="1" />
    <line x1=50 y1=210 x2=290 y2=210 stroke="black" stroke-width="1" />
    <line x1=50 y1=220 x2=290 y2=220 stroke="black" stroke-width="2" />
    <line x1=50 y1=230 x2=290 y2=230 stroke="black" stroke-width="1" />
    <line x1=50 y1=240 x2=290 y2=240 stroke="black" stroke-width="1" />
    <line x1=50 y1=250 x2=290 y2=250 stroke="black" stroke-width="1" />
    <line x1=50 y1=260 x2=290 y2=260 stroke="black" stroke-width="1" />
    <line x1=50 y1=270 x2=290 y2=270 stroke="black" stroke-width="2" />
    <polyline fill="none" stroke="red" stroke-width="1" id="graphique" points="" />
  </svg>
</fieldset>
</br>
<form action="/" method="POST">
  <div align="Right">
  <button type="submit" style="background-color: lightgreen; text-align: center; text-align: center; height: 30px; width: 80px">Accueil</button>
  </div>
</form>
</BODY>
</HTML>
)=====";
