////////////////////////////////////////////////////////////////////////////////
/// \file WifiOta.cpp
///
/// \brief
///
/// \date creation     : 14/10/2024
/// \date modification : 14/10/2024
///

#include "BertheVarioTac.h"
#include <ElegantOTA.h>

WebServer  g_server_ota(80);

CGlobalVar g_GlobalVar ;


//unsigned long ota_progress_millis = 0;

////////////////////////////////////////////////////////////////////////////////
/// \brief
void onOTAStart() {
  // Log when OTA has started
  //Serial.println("OTA update started!");
  g_tft.println("OTA update started!");
  // <Add your own code here>
}

////////////////////////////////////////////////////////////////////////////////
/// \brief
void onOTAProgress(size_t current, size_t final) {
  // Log every 1 second
  /*if (millis() - ota_progress_millis > 1000) {
    ota_progress_millis = millis();
    Serial.printf("OTA Progress Current: %u bytes, Final: %u bytes\n", current, final);
  }*/

 g_tft.print(".");
}

////////////////////////////////////////////////////////////////////////////////
/// \brief
void onOTAEnd(bool success) {
  // Log when OTA has finished
  if (success) {
    g_tft.println("OTA update finished successfully!");
  } else {
    g_tft.println("There was an error during OTA update!");
  }
  // <Add your own code here>
}

////////////////////////////////////////////////////////////////////////////////
/// \brief
void WifiInitOta()
{
// arret des autres taches
g_GlobalVar.StopAll() ;

delay( 500 ) ;

g_GlobalVar.m_Screen.m_MutexTft.PrendreMutex() ;

// raz des boutons
g_GlobalVar.m_Screen.RazButtons() ;

// creation fichier de validation zones
g_GlobalVar.m_ZonesAerAll.Valid() ;

// liberation memoire
g_GlobalVar.m_ZonesAerAll.DeleteAll() ;

// affichage
g_GlobalVar.m_Screen.ScreenRaz() ;
g_tft.setCursor( 10 , 50 ) ;
g_tft.println( "Touch/reboot" ) ;
g_tft.println( "Connect to wifi\n" ) ;

WiFi.mode(WIFI_STA);
WiFi.begin( g_GlobalVar.m_Config.m_Ssid.c_str() , g_GlobalVar.m_Config.m_Passwd.c_str() );

// Wait for connection
while (WiFi.status() != WL_CONNECTED)
    {
    delay(500);
    Serial.print(".");
    }

g_tft.print( " " ) ;
g_tft.println( WiFi.localIP() ) ;

g_server_ota.on("/", []() {
    g_server_ota.send(200, "text/plain", "ElegantOTA BertVarioTac.");});

ElegantOTA.begin(&g_server_ota);    // Start ElegantOTA

// ElegantOTA callbacks
ElegantOTA.onStart(onOTAStart);
ElegantOTA.onProgress(onOTAProgress);
ElegantOTA.onEnd(onOTAEnd);

g_GlobalVar.m_Screen.m_MutexTft.RelacherMutex() ;

g_server_ota.begin();
}

////////////////////////////////////////////////////////////////////////////////
/// \brief
/// Dans platformio.ini mettre :
/// extra_scripts = platformio_upload.py
/// upload_protocol = custom
/// upload_url = http://192.168.148.237
void WifiOtaHandle()
{
g_server_ota.handleClient();
ElegantOTA.loop();
}