////////////////////////////////////////////////////////////////////////////////
/// \file CSoundSvr.h
///
/// \brief
///
/// \date creation     : 03/10/2024
/// \date modification : 29/01/2025
///

#ifndef _CSOUNDSVR_
#define _CSOUNDSVR_

#define SOUND_BUFFER_SIZE 10
#define SOUND_DELAY_ONLY  -1
#define SOUND_VOLUME_DEF  127

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
        int             m_Frequence = 2000 ;    ///< frequence, negative alors juste attente
        int             m_DelayMs  = 200 ;      ///< durée de la frequence
        //dac_cw_scale_t  m_Volume = DAC_CW_SCALE_1 ; ///< tres fort par defaut
        int             m_Cycle = SOUND_VOLUME_DEF ; ///< tres fort par defaut
        } ;

protected :
    void PostSoundRequest( const StSoundRequest * pReq ) ;

private :
    static void TacheSoundSvr(void *param) ;

    QueueHandle_t   m_queue;
    StSoundRequest  m_SoundRequest ;
} ;

#endif
