//
// Page HTML pour gérer les animations
//

const char SITE_animations[] PROGMEM = R"=====(
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
        xmlmsg=xmlDoc.getElementsByTagName('animation')[0].firstChild.nodeValue;
        document.getElementById('animation').innerHTML=xmlmsg; 
      }
    }
    xmlHttp.open('PUT','xml',true);
    xmlHttp.send(null);
  }
}
</SCRIPT>

<BODY onload="process()">
<fieldset> <legend>Choix de l'animation</legend>
  Animation ex&eacutecut&eacutee : <b> <A id=animation> </A> </b> </br></br>
  <div align="Center">
  <form action="/animations" method="POST">
    <input type="submit" name="none" value="Aucune" style="text-align: center; height: 40px; width: 100px">&nbsp&nbsp
    <input type="submit" name="ambiance" value="Ambiance" style="text-align: center; height: 40px; width: 100px">&nbsp&nbsp
    <input type="submit" name="vague" value="Vague" style="text-align: center; height: 40px; width: 100px">
  </form>
  </br>
  <form action="/animations" method="POST">
    <input type="submit" name="arc_en_ciel" value="Arc-en-ciel" style="text-align: center; height: 40px; width: 100px">&nbsp&nbsp
    <input type="submit" name="fondu" value="Fondu" style="text-align: center; height: 40px; width: 100px">&nbsp&nbsp
    <input type="submit" name="theatre" value="Theatre" style="text-align: center; height: 40px; width: 100px">
  </form>
  </br>
  <form action="/animations" method="POST">
    <input type="submit" name="scanner" value="Scanner" style="text-align: center; height: 40px; width: 100px">&nbsp&nbsp
    <input type="submit" name="feu" value="Feu" style="text-align: center; height: 40px; width: 100px">&nbsp&nbsp
    <input type="submit" name="volet" value="Volet" style="text-align: center; height: 40px; width: 100px">
  </form>
  </br>
  <form action="/animations" method="POST">
    <input type="submit" name="heliport" value="Heliport" style="text-align: center; height: 40px; width: 100px">&nbsp&nbsp
    <input type="submit" name="scintillement" value="Scintillement" style="text-align: center; height: 40px; width: 100px">&nbsp&nbsp
    <input type="submit" name="halloween" value="Halloween" style="text-align: center; height: 40px; width: 100px">
  </form>
  </br>
  <form action="/animations" method="POST">
    <input type="submit" name="france" value="France" style="text-align: center; height: 40px; width: 100px">&nbsp&nbsp
    <input type="submit" name="meteor" value="Meteor" style="text-align: center; height: 40px; width: 100px">&nbsp&nbsp
    <input type="submit" name="cylon" value="Cylon" style="text-align: center; height: 40px; width: 100px">
  </form>
  </div>
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
