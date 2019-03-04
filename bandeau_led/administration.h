//
// Page HTML administration
//

const char SITE_admin[] PROGMEM = R"=====(
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
        xmlmsg=xmlDoc.getElementsByTagName('macaddress')[0].firstChild.nodeValue;
        document.getElementById('macaddress').innerHTML=xmlmsg; 
        xmlmsg=xmlDoc.getElementsByTagName('ssid')[0].firstChild.nodeValue;
        document.getElementById('ssid').innerHTML=xmlmsg; 
      }
    }
    xmlHttp.open('PUT','xml',true);
    xmlHttp.send(null);
  }
}
</SCRIPT>

<BODY onload="process()">
<fieldset> <legend>Red&eacutemarrer le module</legend>
  <form action="/administration" method="POST">
    <div align="Center">
      <input type="submit" style="text-align: center; height: 30px; width: 90px" name="reboot" value="Reboot">
    </div>
  </form>
</fieldset>
<fieldset> <legend>R&eacuteinitialiser le WiFi</legend>
  Le module sera initialis&eacute en point d'acc&egraves WiFi 
  avec pour SSID : <b>Ambiance_<A id=macaddress> </A> </b>, pour adresse IP : <b>192.168.4.1 </b> et pour mot de passe : <b>ambiance</b>
  <form action="/administration" method="POST">
    </br>
    <div align="Center">
      <input type="submit" style="text-align: center; height: 30px; width: 90px" name="reinit" value="Initialiser">
    </div>
  </form>
</fieldset>
<fieldset> <legend>Connexion au point d'acc&egraves WiFi</legend>
  SSID WiFi : <b><A id=ssid></b>
  <form action="/administration" method="POST">
    </br>
    <TABLE>
      <TR><TD><label>SSID </TD><TD>:</TD></label><TD><input name='ssid' length=32></TD></TR>
      <TR><TD><label>Password </TD><TD>:</TD></label><TD><input name='passwd' length=64></TD></TR>
    </TABLE>
    <div align="Center">
      <input type="submit" style="text-align: center; height: 30px; width: 90px" name="conf_wifi" value="Connexion">
    </div>
  </form>
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
