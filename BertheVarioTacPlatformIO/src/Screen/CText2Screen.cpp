////////////////////////////////////////////////////////////////////////////////
/// \file CText2Screen.cpp
///
/// \brief Texte a afficher
///
/// \date creation     : 21/09/2024
/// \date modification : 21/09/2024
///

#include "../BertheVarioTac.h"

////////////////////////////////////////////////////////////////////////////////
/// \brief
CText2Screen::CText2Screen()
{
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Parametrage des variables
void CText2Screen::SetPos( uint16_t x , uint16_t y , uint16_t taille , char Unite , bool Const )
{
m_x = x ;
m_y = y ;
m_Taille = taille ;
m_Unite = Unite ;
m_Const = Const ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Affichage sans flick.
void CText2Screen::Affiche(const char* pChar)
{
g_tft.setCursor( m_x , m_y ) ;


// texte
if ( m_Taille == TXT_MOYEN )
    g_tft.setTextSize(3) ;
else
    g_tft.setTextSize(6) ;

// effacement ancien texte si pas constant
if ( ! m_Const && m_TextePrincipal != pChar )
    {
    g_tft.setCursor( m_x , m_y ) ;
    g_tft.setTextColor(TFT_WHITE) ;
    g_tft.print(m_TextePrincipal.c_str());
    }

// nouveau text
g_tft.setCursor( m_x , m_y ) ;
g_tft.setTextColor(TFT_BLACK) ;
g_tft.print(pChar);

m_TextePrincipal = pChar ;

// unite
if ( m_Taille == TXT_MOYEN )
    g_tft.setTextSize(3) ;
else
    g_tft.setTextSize(6) ;
if ( m_Unite != ' ' )
    g_tft.print(m_Unite);
}
