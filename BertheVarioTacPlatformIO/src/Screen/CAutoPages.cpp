////////////////////////////////////////////////////////////////////////////////
/// \file CAutoPage.cpp
///
/// \brief Automate de sequencement des pages ecran
///
/// \date creation     : 21/09/2024
/// \date modification : 21/09/2024
///

#include "../BertheVarioTac.h"

////////////////////////////////////////////////////////////////////////////////
/// \brief Initialisation de l'automate.
CAutoPages::CAutoPages()
{
// fonction erreur par defaut
for ( int ie = 0 ; ie < FIN ; ie++ )
    m_Automate[ie].m_pFunction = & CAutoPages::Erreur ;

m_Automate[ECRAN_0_Vz].m_pFunction      = & CAutoPages::EcranVz ;
//m_Automate[ECRAN_1_Histo].m_pFunction   = & CAutoPages::EcranHisto ;
//m_Automate[ECRAN_2a_ListeIgc].m_pFunction= & CAutoPages::EcranListeIgcFch ;
//m_Automate[ECRAN_2b_ConfirmArchIgc].m_pFunction= & CAutoPages::EcranConfimeArchIgcFch ;
//m_Automate[ECRAN_3a_TmaAll].m_pFunction = & CAutoPages::EcranTmaAll ;
//m_Automate[ECRAN_3b_TmaMod].m_pFunction = & CAutoPages::EcranTmaMod ;
//m_Automate[ECRAN_4_CfgFch].m_pFunction  = & CAutoPages::EcranCfgFch ;
//m_Automate[ECRAN_5_TmaDessous].m_pFunction= & CAutoPages::EcranTmaDessous ;
m_Automate[ECRAN_6_Sys].m_pFunction     = & CAutoPages::EcranSys ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Affichage des differants ecrans etat de l'automate.
void CAutoPages::SequencementPages()
{
// appel de la fonction de l'automate vers l'etat suivant
CAutoPages::EtatsAuto (CAutoPages::*pFunction)() = m_Automate[m_EtatAuto].m_pFunction ;
EtatsAuto NextStep = (this->*pFunction)() ;

//ScreenOff() ;

// si changement de page
if ( m_EtatAuto != NextStep )
    {
    // pour retour automatique vers Vz_0
    m_MillisEcran0 = millis() ;

    // raz screen si changement ecran
    ScreenRaz() ;
    }
// si meme page alors retour automatique ecran Vz si time out
else
    {
    if ( m_EtatAuto != ECRAN_0_Vz )
        {
        if( (millis()-m_MillisEcran0)/1000 > m_SecondesRetourEcran0 )
            {
            ScreenRaz() ;
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
