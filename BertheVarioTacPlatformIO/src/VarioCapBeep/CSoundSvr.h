////////////////////////////////////////////////////////////////////////////////
/// \file CSoundSvr.h
///
/// \brief
///
/// \date creation     : 03/10/2024
/// \date modification : 04/10/2024
///

#ifndef _CSOUNDSVR_
#define _CSOUNDSVR_

#define SOUND_BUFFER_SIZE 10
#define SOUND_DELAY_ONLY  -1

////////////////////////////////////////////////////////////////////////////////
/// \brief Serveur de son
class CSoundSvr
{
public :
    CSoundSvr() ;

    void LanceTacheSound() ;

    /// \brief requette pour serveur de son
    struct StSoundRequest
        {
        int m_Frequence = 2000 ;    ///< frequence, negative alors juste attente
        int m_DelayMs  = 200 ;      ///< durée de la frequence
        } ;

protected :
    void PostSoundRequest( const StSoundRequest * pReq ) ;

private :
    static void TacheSoundSvr(void *param) ;

    QueueHandle_t   m_queue;
    StSoundRequest  m_SoundRequest ;
} ;

#endif
