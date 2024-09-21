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

m_T2SPageSysArr.resize(6) ;
// cpu
m_T2SPageSysArr[PAGE_SYS_CPU0_TXT].SetPos( 4   , 35 , TXT_MOYEN , ' ' , true ) ;
m_T2SPageSysArr[PAGE_SYS_CPU1_TXT].SetPos( 4   , 60 , TXT_MOYEN , ' ' , true ) ;
m_T2SPageSysArr[PAGE_SYS_CPU0_VAL].SetPos( 160 , 35 , TXT_MOYEN , '%' ) ;
m_T2SPageSysArr[PAGE_SYS_CPU1_VAL].SetPos( 160 , 60 , TXT_MOYEN , '%' ) ;
// free memory
m_T2SPageSysArr[PAGE_SYS_FMEM_TXT].SetPos( 4   , 85 , TXT_MOYEN , ' ' , true ) ;
m_T2SPageSysArr[PAGE_SYS_FMEM_VAL].SetPos(  90 , 85 , TXT_MOYEN , 'o' ) ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief A appeler souvent pour le touch screen
void CScreen::HandleTouchScreen()
{
lv_timer_handler(); /* let the GUI do its work */
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Raz page blanche ecran
void CScreen::ScreenRaz()
{
g_tft.fillRect( 0 , 0 , m_Largeur , m_Hauteur , TFT_WHITE ) ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Affichage de la Vz et autres parametres de navigation.
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
/// \brief Affichage des parametres systemes.
CAutoPages::EtatsAuto CScreen::EcranSys()
{

// % cpu 0
char TmpChar[10] ;
m_T2SPageSysArr[PAGE_SYS_CPU0_TXT].Affiche( "cpu0:" ) ;
sprintf( TmpChar , "%3d" , g_GlobalVar.m_PercentCore0 ) ;
m_T2SPageSysArr[PAGE_SYS_CPU0_VAL].Affiche( TmpChar ) ;
// % cpu 1
m_T2SPageSysArr[PAGE_SYS_CPU1_TXT].Affiche( "cpu1:" ) ;
sprintf( TmpChar , "%3d" , g_GlobalVar.m_PercentCore1 ) ;
m_T2SPageSysArr[PAGE_SYS_CPU1_VAL].Affiche( TmpChar ) ;
// free mem
m_T2SPageSysArr[PAGE_SYS_FMEM_TXT].Affiche( "fmem:" ) ;
sprintf( TmpChar , "%7d" , (int) esp_get_free_heap_size() ) ;
m_T2SPageSysArr[PAGE_SYS_FMEM_VAL].Affiche( TmpChar ) ;

// defilement autre ecran
if ( g_GlobalVar.m_Screen.IsButtonPressed( 0 ) )
    return ECRAN_0_Vz ;
else if ( g_GlobalVar.m_Screen.IsButtonPressed( 1 ) )
    {}
else if ( g_GlobalVar.m_Screen.IsButtonPressed( 2 ) )
    return ECRAN_0_Vz ;

return ECRAN_6_Sys ;
}
