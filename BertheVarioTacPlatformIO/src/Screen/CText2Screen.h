////////////////////////////////////////////////////////////////////////////////
/// \file CText2Screen.h
///
/// \brief Texte a afficher
///
/// \date creation     : 21/09/2024
/// \date modification : 21/09/2024
///

#ifndef _TXT2SCREEN_
#define _TXT2SCREEN_

#define TXT_MOYEN 0
#define TXT_GROS  1

////////////////////////////////////////////////////////////////////////////////
/// \brief Du tete a afficher qui ne flick pas.
class CText2Screen
{
public :
    CText2Screen() ;

    void SetPos( uint16_t x , uint16_t y , uint16_t taille , char Unite = ' ' ) ;
    void Affiche( const char * pChar ) ;

private :
    std::string m_TextePrincipal ;  ///< texte courant principal
    char        m_Unite = ' ' ;
    uint16_t    m_x = 0 ;
    uint16_t    m_y = 0 ;
    uint16_t    m_Taille = TXT_MOYEN ;
} ;

#endif
