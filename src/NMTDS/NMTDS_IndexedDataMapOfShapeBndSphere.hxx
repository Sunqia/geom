// Copyright (C) 2007-2013  CEA/DEN, EDF R&D, OPEN CASCADE
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

// File:        NMTDS_IndexedDataMapOfShapeBndSphere.hxx
// Created:     Mon Feb 20 09:17:01 2012
// Author:      
//              <pkv@BDEURI37616>


#ifndef NMTDS_IndexedDataMapOfShapeBndSphere_HeaderFile
#define NMTDS_IndexedDataMapOfShapeBndSphere_HeaderFile

#include <TopoDS_Shape.hxx>  
#include <TopTools_ShapeMapHasher.hxx>
#include <NMTDS_BndSphere.hxx>

#define _NCollection_MapHasher
#include <NCollection_IndexedDataMap.hxx>



typedef NCollection_IndexedDataMap<TopoDS_Shape, NMTDS_BndSphere, TopTools_ShapeMapHasher> NMTDS_IndexedDataMapOfShapeBndSphere; 
 
#undef _NCollection_MapHasher




#endif
