/*
 * Programme qui permet de gérer un châssis 2WD via un navigateur WEB
 * 
 * Matériel :
 *            Châssis 2WD
 *            WEMOS D1 Mini
 *            
 *            
 */

// Ajout librairie gestion WIFI
#include <ESP8266WiFi.h>

// Ajout librairie Web Server
#include <ESP8266WebServer.h>
// Création d'une instance serveur
ESP8266WebServer server(80); // port d'écoute 80

// Ajout librairie UDP et OTA pour téléverser un programme via WiFi
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

// Ajoute la librairie de gestion de nom DNS
#include <ESP8266mDNS.h>

// pages HTML
#include "acceuil.h"  // page html d'acceuil
#include "telecommande.h"  // page html mode télécommande
#include "autonome.h"  // page html mode autonome
#include "suiveur_ligne.h"  // page html mode suiveur de ligne
#include "suiveur_lumiere.h"  // page html mode suiveur de lumière

String ssid_apmode = "CAR2WD";  // SSID en mode point d'accès
String password_apmode = "guimacoad";  // mot de passe pour le point d'accès
// nom du module utilisé pour la MAJ OTA et pour le réseau
// supporté par Windows, Linux, Mac OS, IOS mais pas sur Android
char *nom_ota = "car-2wd"; // accessible via http://car-2wd.local
char *mdp_ota = "1964"; // mot de passe pour téléversement via OTA
String XML; // variable qui permet les échanges en XML
byte bouton_status = 1; // gestion des boutons pour les activer ou désactiver
int slidervitesse = 50;  // slider vitesse
int sliderproportionnelle = 0;  // slider correction proportionnelle
int sliderintegrale = 0;  // slider correction intégral
int sliderderivee = 0;  // slider correction dérivée
int slidersensibilite = 200;  // slider sensibilité en Lux

void setup() 
{
  Serial.begin(115200);
  WiFi.mode(WIFI_AP); // configure le WiFi en point d'accès
  WiFi.softAP(ssid_apmode.c_str(), password_apmode.c_str(), 10); // configure le module en point d'accès en utilisant le canal 10
  server.on("/", html_acceuil); // page HTML d'acceuil
  server.on("/telecommande", html_telecommande); // page HTML télécommande
  server.on("/autonome", html_autonome); // page HTML autonome
  server.on("/suiveur_ligne", html_suiveur_ligne); // page HTML suiveur de ligne
  server.on("/suiveur_lumiere", html_suiveur_lumiere); // page HTML suiveur de lumière
  server.on("/xml", traite_XML); // Traite la requête XML
  server.on("/set_vitesse", traite_slider_vitesse); // traite le slider pour la vitesse
  server.on("/set_pid", traite_slider_pid); // traite les sliders pour le PID
  server.on("/set_sensibilite_lux", traite_slider_lux); // traite le slider pour la sensibilité en Lux
  server.onNotFound(html_non_trouve); // page non trouvée, on associe la fonction à traiter
  server.begin(); // démarre le serveur WEB
  ArduinoOTA.setHostname(nom_ota); // définit le nom du module pour l'utilisation réseau et OTA
  ArduinoOTA.setPassword(mdp_ota); // définit un mot de passe pour la mise à jour
  ArduinoOTA.onStart([]() {}); // action à effectuer lors du commencement du téléchargement
  ArduinoOTA.onEnd([]() {}); // action à effectuer à la fin du téléchargement
  ArduinoOTA.begin(); // initialise la possibilité de MAJ via le WiFi
}

void traite_slider_vitesse() 
{
  slidervitesse = server.arg("val").toInt();  // récupère la valeur du Slider vitesse
  construit_XML(); // construit le XML
  server.send(200, "text/xml", XML); // envoie le XML
}

void traite_slider_pid()
{
  int slidercnt = server.arg("cnt").toInt();  // détermine la valeur reçue
  if (slidercnt == 0) { sliderproportionnelle = server.arg("val").toInt(); }  // récupère la valeur du Slider Proportionnelle
  if (slidercnt == 1) { sliderintegrale = server.arg("val").toInt(); }  // récupère la valeur du Slider Intégrale
  if (slidercnt == 2) { sliderderivee = server.arg("val").toInt(); }  // récupère la valeur du Slider Dérivée
  construit_XML(); // construit le XML
  server.send(200, "text/xml", XML); // envoie le XML
}

void traite_slider_lux() 
{
  slidersensibilite = server.arg("val").toInt();  // récupère la valeur du Slider lux
  construit_XML(); // construit le XML
  server.send(200, "text/xml", XML); // envoie le XML
}

void traite_XML() 
{
  construit_XML(); // construit le format XML
  server.send(200, "text/xml", XML); // envoie le XML
}

void construit_XML() 
{
  XML = "<?xml version='1.0'?>";
  XML += "<xml>";
  XML += "<bouton_status>";
  XML += byte(bouton_status);
  XML += "</bouton_status>";
  XML += "<slidervitesse>";
  XML += String(slidervitesse);
  XML += "</slidervitesse>";
  XML += "<sliderpid0>";
  XML += String(sliderproportionnelle);
  XML += "</sliderpid0>";
  XML += "<sliderpid1>";
  XML += String(sliderintegrale);
  XML += "</sliderpid1>";
  XML += "<sliderpid2>";
  XML += String(sliderderivee);
  XML += "</sliderpid2>";
  XML += "<slidersensibilite>";
  XML += String(slidersensibilite);
  XML += "</slidersensibilite>";
  XML += "</xml>";
}

void html_acceuil()
{
  server.send_P(200, "text/html", car2wd_acceuil); // envoie la page principale
  bouton_status = 1; // bouton 'Start' activé, bouton 'Stop' désactivé
}

void html_telecommande()
{
  server.send_P(200, "text/html", car2wd_telecommande); // envoie la page télécommande
  if (server.hasArg("avance")) 
  {
     
  }
  if (server.hasArg("gauche")) 
  {
     
  }
  if (server.hasArg("stop")) 
  {
     
  }
  if (server.hasArg("droite")) 
  {
     
  }
  if (server.hasArg("recule")) 
  {
     
  }
}

void html_autonome()
{
  server.send_P(200, "text/html", car2wd_autonome); // envoie la page autonome
  if (server.hasArg("bouton_start")) 
  { 
    bouton_status = 0; // bouton 'Start' désactivé, bouton 'Stop' activé
    construit_XML(); // construit le XML
    server.send(200, "text/xml", XML); // envoie le XML
  }
  if (server.hasArg("bouton_stop"))
  {
    bouton_status = 1; // bouton 'Start' activé, bouton 'Stop' désactivé
    construit_XML(); // construit le XML
    server.send(200, "text/xml", XML); // envoie le XML
  }
}

void html_suiveur_ligne()
{
  server.send_P(200, "text/html", car2wd_suiveur_ligne); // envoie la page suiveur de ligne 
  if (server.hasArg("bouton_start")) 
  { 
    bouton_status = 0; // bouton 'Start' désactivé, bouton 'Stop' activé
    construit_XML(); // construit le XML
    server.send(200, "text/xml", XML); // envoie le XML
  }
  if (server.hasArg("bouton_stop"))
  {
    bouton_status = 1; // bouton 'Start' activé, bouton 'Stop' désactivé
    construit_XML(); // construit le XML
    server.send(200, "text/xml", XML); // envoie le XML
  }
}

void html_suiveur_lumiere()
{
  server.send_P(200, "text/html", car2wd_suiveur_lumiere); // envoie la page suiveur de lumière
  if (server.hasArg("bouton_start")) 
  { 
    bouton_status = 0; // bouton 'Start' désactivé, bouton 'Stop' activé
    construit_XML(); // construit le XML
    server.send(200, "text/xml", XML); // envoie le XML
  }
  if (server.hasArg("bouton_stop"))
  {
    bouton_status = 1; // bouton 'Start' activé, bouton 'Stop' désactivé
    construit_XML(); // construit le XML
    server.send(200, "text/xml", XML); // envoie le XML
  }
}

void html_non_trouve()
{
  server.send(200, "text/html", "page non existante"); // la page demandée n'existe pas
}

void loop() 
{
  server.handleClient(); // une requête HTTP ?
  ArduinoOTA.handle(); // une requête pour la MAJ via WiFi ?
}

