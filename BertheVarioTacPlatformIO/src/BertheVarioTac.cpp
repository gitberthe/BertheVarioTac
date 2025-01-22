////////////////////////////////////////////////////////////////////////////////
/// \file BertheVarioTac.cpp
///
/// \brief Main du projet de vario tactile
///
/// \date creation     : 20/09/2024
/// \date modification : 21/01/2025
///

#include "BertheVarioTac.h"

char g_NumVersion[] = "20250122a" ;

////////////////////////////////////////////////////////////////////////////////
/// \brief Affiche le nom et le firmware
void AfficheEcranDebut()
{
g_GlobalVar.m_Screen.ScreenRaz() ;
// nom
g_tft.setTextColor(TFT_MAGENTA) ;
g_tft.setTextSize(2) ;
g_tft.setCursor( 35 , 50 ) ;
g_tft.print("BertheVarioTac");
// version
g_tft.setTextColor(TFT_WHITE) ;
g_tft.setTextSize(3) ;
g_tft.setCursor( 40 , 100 ) ;
g_tft.print(g_NumVersion);
// batterie
char TmpChar[20] ;
g_tft.setTextColor(TFT_CYAN) ;
g_tft.setTextSize(3) ;
g_tft.setCursor( 80 , 150 ) ;
sprintf(TmpChar,"%4.2fv",g_GlobalVar.GetBatteryVoltage());
g_tft.print(TmpChar) ;
}

