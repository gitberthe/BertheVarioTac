////////////////////////////////////////////////////////////////////////////////
/// \file CTouchButtons.h
///
/// \brief Boutons tactiles
///
/// \date creation     : 21/09/2024
/// \date modification : 25/01/2025
///

#ifndef _TOUCHBUTTONS_
#define _TOUCHBUTTONS_

#define ANTI_REBONDS_MS 200

////////////////////////////////////////////////////////////////////////////////
/// \brief Pour les boutons tactiles en modifiables
class CTouchButtons : private CBrightness
{
public :
    CTouchButtons() ;

    void AfficheButtons() ;
    bool HandleButtons() ;
    bool IsButtonPressed( int ib ) ;
    void SetText( const char * Txt3 , int ib ) ;
    void RazButtons( int button = -1 ) ;

    void SetX( int x )
        { m_XTouch = x ; } ;
    void SetY( int y )
        { m_YTouch = y ; } ;
    int GetX() const
        { return m_XTouch ; } ;
    int GetY() const
        { return m_YTouch ; } ;
    void SetPressed() ;
    void SetFrozenDelaySec( int SecDelay ) ;
    bool IsCenterPressed() ;

    friend class CLGFX ;

protected :
    int   m_XTouch = -1 ;       ///< coordonnees du touch
    int   m_YTouch = -1 ;       ///< coordonnees du touch
    bool  m_ScreenPressed = false ;   ///< si est appuyé sur l'ecran

private :
    unsigned long   m_TimePressed = 0 ;///< pour l' anti-rebond
    int             m_FrozenSec = 0 ;  ///< temps ou les boutons sont geles
    static const int m_NbButtons = 3 ;  ///< bombre de boutons a afficher
    bool            m_PressedArr[m_NbButtons] ;  ///< si boutons pressés
    bool            m_PressedArrLast[m_NbButtons] ;  ///< si boutons pressés pour affichage rapide
    char            m_Intitule[m_NbButtons][4] ; ///< texte de bouton
    bool            m_CenterPressed = false ; ///< si est appuyé en milieu d'ecran

    const int m_HauteurBoutons = 50 ;
    const int m_Marge = 10 ;

} ;

#endif
