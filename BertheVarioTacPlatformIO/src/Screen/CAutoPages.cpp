////////////////////////////////////////////////////////////////////////////////
/// \file CAutoPage.cpp
///
/// \brief Automate de sequencement des pages ecran
///
/// \date creation     : 21/09/2024
/// \date modification : 05/10/2024
///

#include "../BertheVarioTac.h"

////////////////////////////////////////////////////////////////////////////////
/// \brief Initialisation de l'automate.
CAutoPages::CAutoPages()
{
// fonction erreur par defaut
for ( int ie = 0 ; ie < FIN ; ie++ )
    m_Automate[ie].m_pFunction = & CAutoPages::Erreur ;

m_Automate[ECRAN_0_Vz].m_pFunction       = & CAutoPages::EcranVz ;
m_Automate[ECRAN_1_Histo].m_pFunction    = & CAutoPages::EcranHisto ;
m_Automate[ECRAN_2a_ListeIgc].m_pFunction= & CAutoPages::EcranListeIgcFch ;
m_Automate[ECRAN_2b_ConfirmArchIgc].m_pFunction= & CAutoPages::EcranConfimeArchIgcFch ;
m_Automate[ECRAN_3a_TmaAll].m_pFunction  = & CAutoPages::EcranTmaAll ;
m_Automate[ECRAN_3b_TmaMod].m_pFunction  = & CAutoPages::EcranTmaMod ;
m_Automate[ECRAN_4_CfgFch].m_pFunction   = & CAutoPages::EcranCfgFch ;
m_Automate[ECRAN_5_TmaDessous].m_pFunction= & CAutoPages::EcranTmaDessous ;
m_Automate[ECRAN_6_Sys].m_pFunction      = & CAutoPages::EcranSys ;
m_Automate[ECRAN_7_Wifi].m_pFunction     = & CAutoPages::EcranWifi ;
m_Automate[ECRAN_8_Menu].m_pFunction     = & CAutoPages::EcranMenu ;
m_Automate[ECRAN_6b_CalMag].m_pFunction  = & CAutoPages::EcranCalibreMagnetique ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief pour le retour en arriere a l'ecran Vz
void CAutoPages::SetVzEtatAuto()
{
m_PageChanged = true ;

// ecran de configuration on sauvegarde
if ( m_EtatAuto == ECRAN_4_CfgFch )
    g_GlobalVar.m_Config.EcritureFichier() ;

// ecran de modification Tma on sauvegarde
if ( m_EtatAuto == ECRAN_3b_TmaMod )
    g_GlobalVar.m_ZonesAerAll.EcritureFichierZonesActive() ;

m_EtatAuto = ECRAN_0_Vz ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Affichage des differants ecrans etat de l'automate.
void CAutoPages::SequencementPages()
{
// appel de la fonction de l'automate vers l'etat suivant
CAutoPages::EtatsAuto (CAutoPages::*pFunction)() = m_Automate[m_EtatAuto].m_pFunction ;
EtatsAuto NextStep = (this->*pFunction)() ;

//ScreenOff() ;
// si centre de l'ecran presse
if ( g_GlobalVar.m_Screen.IsCenterPressed() )
    {
    SetVzEtatAuto() ;
    return ;
    }

// si changement de page
if ( m_EtatAuto != NextStep )
    {
    // pour le retour en arriere
    //m_LastEtatAuto = m_EtatAuto ;

    // pour retour automatique vers Vz_0
    m_MillisEcran0 = millis() ;

    // raz screen si changement ecran
    //ScreenRaz() ;
    m_PageChanged = true ;

    unsigned long time = millis() ;
    while( (millis()-time) < 200 )
        {
        ScreenRazButtons() ;
        delay(1) ;
        }
    }
// si meme page alors retour automatique ecran Vz si time out
else
    {
    // pas de changement de page
    m_PageChanged = false ;

    // pas de retour Vz si modification tma ou wifi ou calibration
    if ( (m_CfgFileiChamps != -1 && m_EtatAuto == ECRAN_4_CfgFch ) ||
          m_EtatAuto == ECRAN_3b_TmaMod ||
          m_EtatAuto == ECRAN_7_Wifi ||
          (m_EtatAuto == ECRAN_6b_CalMag && m_CalibrationEnCours) )
        m_MillisEcran0 = millis() ;

    // si page pas Vz
    if ( m_EtatAuto != ECRAN_0_Vz )
        {
        // si time out
        if( (millis()-m_MillisEcran0)/1000 > m_SecondesRetourEcran0 )
            {
            ScreenRaz() ;
            //ScreenRazButtons() ;
            m_PageChanged = true ;
            m_EtatAuto = ECRAN_0_Vz ;
            return ;
            }
        }
    }

// prochain etat
m_EtatAuto = NextStep ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Fonction erreur de l'automate.
CAutoPages::EtatsAuto CAutoPages::Erreur()
{
Serial.println("Erreur automate") ;
delay(200) ;
CGlobalVar::BeepError() ;

return ERREUR ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Fonction static qui lit les boutons
void CAutoPages::LancerTacheTouch()
{
g_GlobalVar.m_TaskArr[TOUCH_NUM_TASK].m_Run = true ;
g_GlobalVar.m_TaskArr[TOUCH_NUM_TASK].m_Stopped = false ;
xTaskCreatePinnedToCore(TacheTouch, "Touch", TOUCH_STACK_SIZE , this, TOUCH_PRIORITY, NULL, TOUCH_CORE);
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Fonction static qui affiche les ecrans
void CAutoPages::TacheTouch(void *param)
{
while (g_GlobalVar.m_TaskArr[TOUCH_NUM_TASK].m_Run)
    {
    // traitement de touch pad
    g_GlobalVar.m_Screen.HandleTouchScreen() ;

    // scan les boutons tactiles
    g_GlobalVar.m_Screen.HandleButtons() ;

    // a 10hz, 50hz ça plante l'affichage avec le Gps
    delay( 100 );
    }
g_GlobalVar.m_TaskArr[TOUCH_NUM_TASK].m_Stopped = true ;
while( true )
    vTaskDelete(NULL) ;
}

