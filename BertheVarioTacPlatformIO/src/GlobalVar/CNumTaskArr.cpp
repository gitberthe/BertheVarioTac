////////////////////////////////////////////////////////////////////////////////
/// \file CNumTaskArr.cpp
///
/// \brief
///
/// \date creation     : 05/05/2024
/// \date modification : 22/09/2024
///

#include "../BertheVarioTac.h"

////////////////////////////////////////////////////////////////////////////////
/// \file Arrete toutes les taches sauf touch
void CNumTaskArr::StopAll()
{
for ( int it = 0 ; it < SIZE_TASK ; it++ )
    {
    if ( it == TOUCH_NUM_TASK )
        continue ;
    m_TaskArr[it].m_Run = false ;
    }
}
