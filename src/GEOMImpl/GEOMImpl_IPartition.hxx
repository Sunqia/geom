// Copyright (C) 2005  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either 
// version 2.1 of the License.
// 
// This library is distributed in the hope that it will be useful 
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
//NOTE: This is an intreface to a function for the Partition creation.


#include "GEOM_Function.hxx"

#include "TColStd_HSequenceOfTransient.hxx"
#include "TColStd_HArray1OfInteger.hxx"

#define PART_ARG_LIMIT   1
#define PART_ARG_SHAPES  2
#define PART_ARG_TOOLS   3
#define PART_ARG_KEEP_IN 4
#define PART_ARG_REM_IN  5

#define PART_ARG_MATERIALS 6

#define PART_ARG_SHAPE 7
#define PART_ARG_PLANE 8

#define PART_ARG_KEEP_NONLIMIT_SHAPES 9

class GEOMImpl_IPartition
{
 public:

  GEOMImpl_IPartition(Handle(GEOM_Function) theFunction): _func(theFunction) {}

  void SetLimit(int theLimit) { _func->SetInteger(PART_ARG_LIMIT, theLimit); }

  void SetShapes(const Handle(TColStd_HSequenceOfTransient)& theShapes)
  { _func->SetReferenceList(PART_ARG_SHAPES, theShapes); }

  void SetTools(const Handle(TColStd_HSequenceOfTransient)& theShapes)
  { _func->SetReferenceList(PART_ARG_TOOLS, theShapes); }

  void SetKeepIns(const Handle(TColStd_HSequenceOfTransient)& theShapes)
  { _func->SetReferenceList(PART_ARG_KEEP_IN, theShapes); }

  void SetRemoveIns(const Handle(TColStd_HSequenceOfTransient)& theShapes)
  { _func->SetReferenceList(PART_ARG_REM_IN, theShapes); }

  void SetMaterials(const Handle(TColStd_HArray1OfInteger)& theMaterials)
  { _func->SetIntegerArray(PART_ARG_MATERIALS, theMaterials); }


  int GetLimit() { return _func->GetInteger(PART_ARG_LIMIT); }

  Handle(TColStd_HSequenceOfTransient) GetShapes()    { return _func->GetReferenceList(PART_ARG_SHAPES); }
  Handle(TColStd_HSequenceOfTransient) GetTools()     { return _func->GetReferenceList(PART_ARG_TOOLS); }
  Handle(TColStd_HSequenceOfTransient) GetKeepIns()   { return _func->GetReferenceList(PART_ARG_KEEP_IN); }
  Handle(TColStd_HSequenceOfTransient) GetRemoveIns() { return _func->GetReferenceList(PART_ARG_REM_IN); }

  Handle(TColStd_HArray1OfInteger) GetMaterials() { return _func->GetIntegerArray(PART_ARG_MATERIALS); }


  void SetShape(Handle(GEOM_Function) theShape) { _func->SetReference(PART_ARG_SHAPE, theShape); }
  void SetPlane(Handle(GEOM_Function) thePlane) { _func->SetReference(PART_ARG_PLANE, thePlane); }

  Handle(GEOM_Function) GetShape() { return _func->GetReference(PART_ARG_SHAPE); }
  Handle(GEOM_Function) GetPlane() { return _func->GetReference(PART_ARG_PLANE); }

  void SetKeepNonlimitShapes(int theKeepNonlimitShapes) { _func->SetInteger(PART_ARG_KEEP_NONLIMIT_SHAPES,theKeepNonlimitShapes ); }
  int GetKeepNonlimitShapes() { return _func->GetInteger(PART_ARG_KEEP_NONLIMIT_SHAPES); }


 private:

  Handle(GEOM_Function) _func;
};
