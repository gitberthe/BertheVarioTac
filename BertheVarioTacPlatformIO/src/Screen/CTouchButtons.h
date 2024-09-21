////////////////////////////////////////////////////////////////////////////////
/// \file CTouchButtons.h
///
/// \brief Boutons tactiles
///
/// \date creation     : 21/09/2024
/// \date modification : 21/09/2024
///

#ifndef _TOUCHBUTTONS_
#define _TOUCHBUTTONS_

////////////////////////////////////////////////////////////////////////////////
/// \brief Pour les boutons tactiles en modifiables
class CTouchButtons
{
public :
    void AfficheButtons() ;
    bool IsButtonPressed( int ib ) const
        { return m_PressedArr[ib] ; } ;
    void RazButtons() ;


private :
    int  m_NbButtons = 3 ;  ///< bombre de boutons a afficher
    bool m_PressedArr[3] ;  ///< si boutons pressés
} ;

#endif
