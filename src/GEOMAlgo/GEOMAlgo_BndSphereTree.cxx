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

// File:	GEOMAlgo_BndSphereTree.cxx
// Created:	
// Author:	Peter KURNEV
//		<pkv@irinox>
//
#include <GEOMAlgo_BndSphereTree.hxx>
//=======================================================================
//function : 
//purpose  : 
//=======================================================================
  GEOMAlgo_BndSphereTreeSelector::GEOMAlgo_BndSphereTreeSelector()
{
}
//=======================================================================
//function : ~
//purpose  : 
//=======================================================================
  GEOMAlgo_BndSphereTreeSelector::~GEOMAlgo_BndSphereTreeSelector()
{
}
//=======================================================================
//function : Reject
//purpose  : 
//=======================================================================
  Standard_Boolean GEOMAlgo_BndSphereTreeSelector::Reject (const GEOMAlgo_BndSphere& aBox) const
{
  Standard_Boolean bRet;
  //
  bRet=myBox.IsOut(aBox);
  return bRet;
}
//=======================================================================
//function : Accept
//purpose  : 
//=======================================================================
  Standard_Boolean GEOMAlgo_BndSphereTreeSelector::Accept (const Standard_Integer& aIndex)
{
  Standard_Boolean bRet=Standard_False;
  //
  if (myFence.Add(aIndex)) {
    myIndices.Append(aIndex);
    bRet=!bRet;
  }
  return bRet;
}
//=======================================================================
//function : SetBox
//purpose  : 
//=======================================================================
  void GEOMAlgo_BndSphereTreeSelector::SetBox(const GEOMAlgo_BndSphere& aBox)
{
  myBox=aBox;
}
//=======================================================================
//function : Clear
//purpose  : 
//=======================================================================
  void GEOMAlgo_BndSphereTreeSelector::Clear()
{
  myFence.Clear();
  myIndices.Clear();
}
//=======================================================================
//function : Indices
//purpose  : 
//=======================================================================
  const TColStd_ListOfInteger& GEOMAlgo_BndSphereTreeSelector::Indices() const
{
  return myIndices;
}
