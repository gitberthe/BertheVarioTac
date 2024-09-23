////////////////////////////////////////////////////////////////////////////////
/// \file CTouchButtons.cpp
///
/// \brief Boutons tactiles
///
/// \date creation     : 21/09/2024
/// \date modification : 21/09/2024
///

#include "../BertheVarioTac.h"

////////////////////////////////////////////////////////////////////////////////
/// \brief Lit la position du touch pad et active les boutons en consequence
void CTouchButtons::AfficheButtons()
{
uint16_t ColorFond  = TFT_WHITE ;
uint16_t ColorTexte = TFT_BLACK ;

const int HauteurBoutons = 50 ;
const int Marge = 10 ;

// pour tous les boutons
g_tft.startWrite();
int ib = 0 ;
for ( int x = 0 ; x < g_GlobalVar.m_Screen.m_Largeur ; x += g_GlobalVar.m_Screen.m_Largeur/m_NbButtons , ib++ )
    {
    // bord du boutons
    int ColorDouble = ColorFond ;

    // verification zone clic
    if ( g_GlobalVar.m_Screen.m_XTouch > x && g_GlobalVar.m_Screen.m_XTouch < (x + g_GlobalVar.m_Screen.m_Largeur/m_NbButtons) &&
         g_GlobalVar.m_Screen.m_YTouch > (g_GlobalVar.m_Screen.m_Hauteur - HauteurBoutons) &&
         g_GlobalVar.m_Screen.m_Pressed )
        {
        ColorDouble = ColorTexte ;
        m_PressedArr[ib] = true ;
        m_Pressed = false ;
        }

    // cerclage boutons
    g_tft.fillRect( x + 10 , g_GlobalVar.m_Screen.m_Hauteur - HauteurBoutons + 10 , g_GlobalVar.m_Screen.m_Largeur/m_NbButtons -20 , HauteurBoutons -20 , ColorFond ) ;
    g_tft.fillRect( x , g_GlobalVar.m_Screen.m_Hauteur - HauteurBoutons ,
                    g_GlobalVar.m_Screen.m_Largeur/m_NbButtons , Marge , ColorDouble ) ;
    g_tft.fillRect( x , g_GlobalVar.m_Screen.m_Hauteur - HauteurBoutons ,
                    Marge , HauteurBoutons , ColorDouble ) ;
    g_tft.fillRect( x + g_GlobalVar.m_Screen.m_Largeur/m_NbButtons - Marge , g_GlobalVar.m_Screen.m_Hauteur - HauteurBoutons ,
                    Marge , HauteurBoutons , ColorDouble ) ;
    g_tft.fillRect( x , g_GlobalVar.m_Screen.m_Hauteur - Marge ,
                    g_GlobalVar.m_Screen.m_Largeur/m_NbButtons , Marge , ColorDouble ) ;
    g_tft.drawRect( x , g_GlobalVar.m_Screen.m_Hauteur - HauteurBoutons , g_GlobalVar.m_Screen.m_Largeur/m_NbButtons , HauteurBoutons , ColorTexte ) ;


    // position du bouton
    g_tft.setCursor( g_GlobalVar.m_Screen.m_Largeur / m_NbButtons * ib + 23  , g_GlobalVar.m_Screen.m_Hauteur - HauteurBoutons / 2 - 5 ) ;
    g_tft.setTextColor(ColorTexte) ;

    // texte du boutons
    g_tft.setTextSize(2) ;
    g_tft.print(m_Intitule[ib]);
    }
g_tft.endWrite();

// si reste de l'ecran presse
if ( g_GlobalVar.m_Screen.m_Pressed )
    {
    m_Pressed = false ;
    g_GlobalVar.m_Screen.SetLastEtatAuto() ;
    }
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Remet les bouons à zero
void CTouchButtons::RazButtons()
{
for ( int ib = 0 ; ib < m_NbButtons ; ib++ )
    m_PressedArr[ib] = false ;

//delay( 100 ) ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief positionne le texte des boutons
void CTouchButtons::SetText( const char * Txt3 , int ib )
{
int len = strlen(Txt3 ) ;
for ( int ic = 0 ; ic < 4 ; ic++ )
    {
    if ( ic < len )
        m_Intitule[ib][ic] = Txt3[ic] ;
    else
        m_Intitule[ib][ic] = 0 ;
    }
}
