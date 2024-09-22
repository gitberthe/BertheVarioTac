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
/// \file Arrete toutes les taches.
void CNumTaskArr::StopAll()
{
for ( int it = 0 ; it < SIZE_TASK ; it++ )
    m_TaskArr[it].m_Run = false ;
}
