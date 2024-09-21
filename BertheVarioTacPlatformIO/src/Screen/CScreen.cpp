////////////////////////////////////////////////////////////////////////////////
/// \file CScreen.cpp
///
/// \brief Definition des pages ecran
///
/// \date creation     : 21/09/2024
/// \date modification : 21/09/2024
///

#include "../BertheVarioTac.h"

////////////////////////////////////////////////////////////////////////////////
/// \brief Constructeur
CScreen::CScreen()
{
m_T2SPageVzArr.resize(1) ;
m_T2SPageVzArr[PAGE_VZ_VZ].SetPos( 0 , 0 , TXT_GROS ) ;

m_T2SPageSysArr.resize(1) ;
m_T2SPageSysArr[PAGE_SYS_SYS].SetPos( 0 , 0 , TXT_MOYEN , '%' ) ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief A appeler souvent pour le touch screen
void CScreen::HandleTouchScreen()
{
lv_timer_handler(); /* let the GUI do its work */
}

////////////////////////////////////////////////////////////////////////////////
/// \brief
void CScreen::ScreenRaz()
{
g_tft.fillRect( 0 , 0 , m_Largeur , m_Hauteur , TFT_WHITE ) ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief
CAutoPages::EtatsAuto CScreen::EcranVz()
{
static int count = 0 ;
count++ ;

if ( count%2 )
    m_T2SPageVzArr[PAGE_VZ_VZ].Affiche("-0.5") ;
else
    m_T2SPageVzArr[PAGE_VZ_VZ].Affiche(" 0.3") ;

g_tft.fillCircle(100, 100,50,TFT_BLUE) ;

// defilement autre ecran
if ( g_GlobalVar.m_Screen.IsButtonPressed( 0 ) )
    return ECRAN_6_Sys ;
else if ( g_GlobalVar.m_Screen.IsButtonPressed( 1 ) )
    {}
else if ( g_GlobalVar.m_Screen.IsButtonPressed( 2 ) )
    return ECRAN_6_Sys ;

return ECRAN_0_Vz ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief
CAutoPages::EtatsAuto CScreen::EcranSys()
{
static int count = 0 ;
count++ ;

if ( count%2 )
    m_T2SPageSysArr[PAGE_SYS_SYS].Affiche("10") ;
else
    m_T2SPageSysArr[PAGE_SYS_SYS].Affiche("15") ;

g_tft.fillCircle(100, 100,50,TFT_RED) ;

// defilement autre ecran
if ( g_GlobalVar.m_Screen.IsButtonPressed( 0 ) )
    return ECRAN_0_Vz ;
else if ( g_GlobalVar.m_Screen.IsButtonPressed( 1 ) )
    {}
else if ( g_GlobalVar.m_Screen.IsButtonPressed( 2 ) )
    return ECRAN_0_Vz ;

return ECRAN_6_Sys ;
}
