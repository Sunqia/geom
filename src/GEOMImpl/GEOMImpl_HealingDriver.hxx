// Copyright (C) 2007-2013  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
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
//  File   : GEOMImpl_HealingDriver.hxx
//  Module : GEOMImpl

#ifndef _GEOMImpl_HealingDriver_HeaderFile
#define _GEOMImpl_HealingDriver_HeaderFile

#ifndef _TColStd_SequenceOfExtendedString_HeaderFile
#include <TColStd_SequenceOfExtendedString.hxx>
#endif
#ifndef _Standard_TypeMismatch_HeaderFile
#include <Standard_TypeMismatch.hxx>
#endif

#ifndef _Standard_HeaderFile
#include <Standard.hxx>
#endif

#ifndef _Standard_Macro_HeaderFile
#include <Standard_Macro.hxx>
#endif
#ifndef _Standard_HeaderFile
#include <Standard.hxx>
#endif
#ifndef _Standard_GUID_HeaderFile
#include <Standard_GUID.hxx>
#endif

#include <TopoDS_Shape.hxx>
#include <TopoDS_Edge.hxx>
#include <TColStd_HSequenceOfTransient.hxx>


#ifndef _TFunction_Driver_HeaderFile
#include <TFunction_Driver.hxx>
#endif
#ifndef _TFunction_Logbook_HeaderFile
#include <TFunction_Logbook.hxx>
#endif
#ifndef _Standard_CString_HeaderFile
#include <Standard_CString.hxx>
#endif

class GEOMImpl_IHealing;


#include "GEOM_BaseDriver.hxx"

DEFINE_STANDARD_HANDLE( GEOMImpl_HealingDriver, GEOM_BaseDriver );

class GEOMImpl_HealingDriver : public GEOM_BaseDriver {

public:

 // Methods PUBLIC
 //
Standard_EXPORT GEOMImpl_HealingDriver();
  Standard_EXPORT ~GEOMImpl_HealingDriver() {};

  Standard_EXPORT static const Standard_GUID& GetID();

Standard_EXPORT virtual  Standard_Integer Execute(TFunction_Logbook& log) const;
Standard_EXPORT virtual void Validate(TFunction_Logbook&) const {}
Standard_EXPORT Standard_Boolean MustExecute(const TFunction_Logbook&) const { return Standard_True; }

  Standard_EXPORT static Standard_Boolean AreEdgesC1 (const TopoDS_Edge& E1, const TopoDS_Edge& E2);
  Standard_EXPORT static void FuseCollinearEdges (const TopoDS_Shape&,
                                                  const Handle(TColStd_HSequenceOfTransient)&,
                                                  TopoDS_Shape&);
  Standard_EXPORT virtual
  bool GetCreationInformation(std::string&             theOperationName,
                              std::vector<GEOM_Param>& params);

 // Type management
 //
DEFINE_STANDARD_RTTI( GEOMImpl_HealingDriver )


private:
Standard_Boolean ShapeProcess  ( GEOMImpl_IHealing*, const TopoDS_Shape&, TopoDS_Shape& ) const;
Standard_Boolean SuppressFaces ( GEOMImpl_IHealing*, const TopoDS_Shape&, TopoDS_Shape& ) const;
Standard_Boolean CloseContour  ( GEOMImpl_IHealing*, const TopoDS_Shape&, TopoDS_Shape& ) const;
Standard_Boolean RemoveIntWires( GEOMImpl_IHealing*, const TopoDS_Shape&, TopoDS_Shape& ) const;
Standard_Boolean RemoveHoles   ( GEOMImpl_IHealing*, const TopoDS_Shape&, TopoDS_Shape& ) const;
  Standard_Boolean Sew           ( GEOMImpl_IHealing*, const TopoDS_Shape&, TopoDS_Shape&, Standard_Boolean ) const;
  Standard_Boolean RemoveInternalFaces ( const TopoDS_Shape&, TopoDS_Shape& ) const;
Standard_Boolean AddPointOnEdge( GEOMImpl_IHealing*, const TopoDS_Shape&, TopoDS_Shape& ) const;
Standard_Boolean ChangeOrientation( GEOMImpl_IHealing*, const TopoDS_Shape&, TopoDS_Shape& ) const;
void             LimitTolerance( GEOMImpl_IHealing*, const TopoDS_Shape&, TopoDS_Shape& ) const;

};

#endif
