////////////////////////////////////////////////////////////////////////////////
/// \file CNumTaskArr.ch
///
/// \brief
///
/// \date creation     : 05/05/2024
/// \date modification : 05/10/2024
///

#ifndef _NUMTASKARR_
#define _NUMTASKARR_

////////////////////////////////////////////////////////////////////////////////
/// \brief Classe pour la gestion des marche/arret des taches.
/// A noter qu'une tache ne doit jamais retouner mais faire un :
/// while( true ) vTaskDelete(NULL) ;
class CNumTaskArr
{
public :
    struct st_run_stop
        {
        bool m_Run = false ;
        bool m_Stopped = true ;
        } ;

    struct st_run_stop m_TaskArr[SIZE_TASK] ;  ///< tableau des marche/arret taches

    void StopAll() ;
} ;

#endif
