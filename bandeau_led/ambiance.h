//
// Page HTML ambiance
//

const char SITE_ambiance[] PROGMEM = R"=====(
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
        for(i=0;i<4;i++)
        {
          xmlmsg=xmlDoc.getElementsByTagName('slidervalue'+i)[0].firstChild.nodeValue;
          document.getElementById('slider'+i).value=xmlmsg;
          document.getElementById('Slidervalue'+i).innerHTML=xmlmsg; 
        }
        xmlmsg=xmlDoc.getElementsByTagName('bouton_status')[0].firstChild.nodeValue;
        if (xmlmsg==1) 
        {
          document.getElementById('bouton_allume').disabled=false;
          document.getElementById('bouton_eteint').disabled=true;
        }
        else 
        {
          document.getElementById('bouton_allume').disabled=true;
          document.getElementById('bouton_eteint').disabled=false;
        }
      }
    }
    xmlHttp.open('PUT','xml',true);
    xmlHttp.send(null);
  }
}

function slider(cnt)
{
  sliderValue=document.getElementById('slider'+cnt).value;
  document.getElementById('Slidervalue'+cnt).innerHTML=sliderValue;
  if(xmlHttp.readyState==0||xmlHttp.readyState==4)
  {
    xmlHttp.open('PUT','set_neopixel?cnt='+cnt+'&val='+sliderValue,true);
    xmlHttp.send(null);
  }
}
</SCRIPT>

<BODY onload="process()">
<fieldset> <legend>Active le mode Ambiance</legend>
  <form action="/ambiance" method="POST">
    <div align="Center">
      <input type="submit" style="text-align: center; height: 30px; width: 80px" name="allume" value="On" id="bouton_allume">
      &nbsp&nbsp&nbsp&nbsp
      <input type="submit" style="text-align: center; height: 30px; width: 80px" name="eteint" value="Off" id="bouton_eteint">
    </div>
  </form>
</fieldset>
<fieldset> <legend>D&eacutefinition d'une couleur</legend>
  <div align="Center">
    Rouge : <A id=Slidervalue0> </A> </br>
    <input style="width:250px; height:50px" type="range" id="slider0" ONINPUT=slider(0) min="0" max="255" step="5"/> </br>
    Vert : <A id=Slidervalue1> </A> </br>
    <input style="width:250px; height:50px" type="range" id="slider1" ONINPUT=slider(1) min="0" max="255" step="5"/> </br> 
    Bleu : <A id=Slidervalue2> </A> </br>
    <input style="width:250px; height:50px" type="range" id="slider2" ONINPUT=slider(2) min="0" max="255" step="5"/> </br>
    <form action="/ambiance" method="POST">
      <input type="submit" style="text-align: center; height: 30px; width: 90px" name="sauve" value="Sauvegarder">
      <input type="submit" style="text-align: center; height: 30px; width: 90px" name="restaure" value="Restaurer">
    </form>  
  </div> 
</fieldset>
<fieldset> <legend>S&eacutelection d'une couleur</legend>
  <div align="Center">
    <form action="/ambiance" method="POST">
      <input type="submit" style="background-color: white; text-align: center; height: 30px; width: 80px" name="blanc" value="">&nbsp
      <input type="submit" style="background-color: red; text-align: center; height: 30px; width: 80px" name="rouge" value="">&nbsp
      <input type="submit" style="background-color: orange; text-align: center; height: 30px; width: 80px" name="orange" value="">&nbsp
      <input type="submit" style="background-color: lightsalmon; text-align: center; height: 30px; width: 80px" name="saumon" value="">
    </form>
    <form action="/ambiance" method="POST">  
      <input type="submit" style="background-color: yellow; text-align: center; height: 30px; width: 80px" name="jaune" value="">&nbsp
      <input type="submit" style="background-color: green; text-align: center; height: 30px; width: 80px" name="vert" value="">&nbsp
      <input type="submit" style="background-color: lightgreen; text-align: center; height: 30px; width: 80px" name="vertclair" value="">&nbsp
      <input type="submit" style="background-color: lightseagreen; text-align: center; height: 30px; width: 80px" name="vertmer" value="">
    </form>
    <form action="/ambiance" method="POST">
      <input type="submit" style="background-color: blue; text-align: center; height: 30px; width: 80px" name="bleu" value="">&nbsp
      <input type="submit" style="background-color: lightblue; text-align: center; height: 30px; width: 80px" name="bleuclair" value="">&nbsp
      <input type="submit" style="background-color: purple; text-align: center; height: 30px; width: 80px" name="violet" value="">&nbsp
      <input type="submit" style="background-color: pink; text-align: center; height: 30px; width: 80px" name="rose" value="">
    </form>
  </div>
</fieldset> 
<fieldset> <legend>Intensit&eacute</legend>
  <div align="Center">
    <A id=Slidervalue3> </A> % </br>
    <input style="width:250px; height:50px" type="range" id="slider3" ONINPUT=slider(3) min="0" max="100" step="10"/> </br>
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
