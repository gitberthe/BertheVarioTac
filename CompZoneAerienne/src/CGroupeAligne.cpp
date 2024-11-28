////////////////////////////////////////////////////////////////////////////////
/// \file CGroupeAligne.cpp
///
/// \brief
///
/// \date creation     : 28/11/2024
/// \date modification : 28/11/2024
///

#include "CompZoneAerienne.h"

////////////////////////////////////////////////////////////////////////////////
/// \brief Fonction de tri des points par longitude croissante/decroissante
void CSortPts::Sort( std::vector<CVecZoneReduce::st_coord_poly*> & VecToSort )
{
long size  = VecToSort.size() ;
std::vector< CSortPts > VectCSortPts ;

for ( long i = 0 ; i < size ; i++ )
    {
    CSortPts SortPts ;
    SortPts.m_Lon = VecToSort[i]->m_Lon ;
    SortPts.m_ipts = i ;
    VectCSortPts.push_back( SortPts ) ;
    }

std::sort( VectCSortPts.begin() , VectCSortPts.end() ) ;

std::vector<CVecZoneReduce::st_coord_poly*> VecSorted ;

if ( VecToSort[0]->m_Lon < VecToSort[VecToSort.size()-1]->m_Lon )
    {
    for ( long i = 0 ; i < size ; i++ )
        VecSorted.push_back( VecToSort[ VectCSortPts[i].m_ipts ] ) ;
    }
else
    {
    for ( long i = size - 1 ; i >= 0 ; i-- )
        VecSorted.push_back( VecToSort[ VectCSortPts[i].m_ipts ] ) ;
    }

VecToSort = VecSorted ;
}
