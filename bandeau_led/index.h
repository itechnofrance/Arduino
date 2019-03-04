//
// Page HTML par défaut
//

const char SITE_index[] PROGMEM = R"=====(
<!DOCTYPE HTML>
<meta name="viewport" content="width=device-width, initial-scale=1" />
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<title>Ambiance</title>
<div align="Center">
  <form action="/ambiance" method="POST">
    <button type="submit" style="background-color: lightgreen; text-align: center; height: 50px; width: 200px">Ambiance</button>
  </form>
  </br>
  <form action="/animations" method="POST">
    <button type="submit" style="background-color: lightgreen; text-align: center; height: 50px; width: 200px">Animations</button>
  </form>
  </br>
  <form action="/temperature" method="POST">
    <button type="submit" style="background-color: lightgreen; text-align: center; height: 50px; width: 200px">Temp&eacuterature</button>
  </form>
  </br>
  <form action="/administration" method="POST">
    <button type="submit" style="background-color: lightgreen; text-align: center; height: 50px; width: 200px">Administration</button>
  </form>
</div>
</HTML>
)=====";
