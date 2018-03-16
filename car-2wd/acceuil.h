//
// Page HTML acceuil
//

const char car2wd_acceuil[] PROGMEM = R"=====(
<!DOCTYPE HTML>
<meta name="viewport" content="width=device-width, initial-scale=1" />
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<title>CAR-2WD</title>
<div align="Center">
  <h3>CAR-2WD</h3>
  <form action="/telecommande" method="POST">
    <button type="submit" style="text-align: center; height: 50px; width: 200px">Mode T&eacutel&eacutecommande</button>
  </form>
  </br>
  <form action="/autonome" method="POST">
    <button type="submit" style="text-align: center; height: 50px; width: 200px">Mode Autonome</button>
  </form>
  </br>
  <form action="/suiveur_ligne" method="POST">
    <button type="submit" style="text-align: center; height: 50px; width: 200px">Mode Suiveur de ligne</button>
  </form>
  </br>
  <form action="/suiveur_lumiere" method="POST">
    <button type="submit" style="text-align: center; height: 50px; width: 200px">Mode Suiveur de lumi&egravere</button>
  </form>
</div>
</HTML>
)=====";
