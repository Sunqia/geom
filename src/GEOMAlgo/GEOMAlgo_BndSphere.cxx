// Copyright (C) 2007-2012  CEA/DEN, EDF R&D, OPEN CASCADE
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

// File:	GEOMAlgo_BndSphere.cxx
// Created:	
// Author:	Peter KURNEV
//		<pkv@irinox>
//
#include <GEOMAlgo_BndSphere.hxx>

//=======================================================================
//function : 
//purpose  : 
//=======================================================================
  GEOMAlgo_BndSphere::GEOMAlgo_BndSphere()
{
  myCenter.SetCoord(0., 0., 0.);
  myRadius=0.;
  myGap=0.;
}
//=======================================================================
//function : ~
//purpose  : 
//=======================================================================
  GEOMAlgo_BndSphere::~GEOMAlgo_BndSphere()
{
}
//=======================================================================
//function : IsOut
//purpose  : 
//=======================================================================
  Standard_Boolean GEOMAlgo_BndSphere::IsOut(const GEOMAlgo_BndSphere& theOther)const
{
  Standard_Real aD2, aT2;
  //
  aD2=myCenter.SquareDistance(theOther.myCenter);
  aT2=myRadius+myGap+theOther.myRadius+theOther.myGap;
  aT2=aT2*aT2;
  //
  return aD2>aT2;
}
