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

//  File   : GEOMImpl_Fillet1dDriver.ixx
//  Module : GEOMImpl

#ifndef _GEOMImpl_Fillet1dDriver_HeaderFile
#define _GEOMImpl_Fillet1dDriver_HeaderFile

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

#include <TopoDS_Wire.hxx>
#include <TopTools_ListOfShape.hxx>

class Standard_Transient;
class Handle_Standard_Type;
class Handle(TFunction_Driver);
class GEOMImpl_Fillet1dDriver;



#ifndef _TFunction_Driver_HeaderFile
#include <TFunction_Driver.hxx>
#endif
#ifndef _TFunction_Logbook_HeaderFile
#include <TFunction_Logbook.hxx>
#endif
#ifndef _Standard_CString_HeaderFile
#include <Standard_CString.hxx>
#endif

class TColStd_SequenceOfExtendedString;


#include "GEOM_BaseDriver.hxx"

DEFINE_STANDARD_HANDLE( GEOMImpl_Fillet1dDriver, GEOM_BaseDriver );

class GEOMImpl_Fillet1dDriver : public GEOM_BaseDriver {

public:

 // Methods PUBLIC
 // 
Standard_EXPORT GEOMImpl_Fillet1dDriver();
  Standard_EXPORT ~GEOMImpl_Fillet1dDriver() {};

  Standard_EXPORT static const Standard_GUID& GetID();

Standard_EXPORT virtual  Standard_Integer Execute(TFunction_Logbook& log) const; 
Standard_EXPORT virtual void Validate(TFunction_Logbook&) const {}
Standard_EXPORT Standard_Boolean MustExecute(const TFunction_Logbook&) const { return Standard_True; }

  Standard_EXPORT virtual
  bool GetCreationInformation(std::string&             theOperationName,
                              std::vector<GEOM_Param>& params);
private:

  Standard_EXPORT bool MakeFillet(const TopoDS_Wire&, const TopTools_ListOfShape&,
                                  const Standard_Real, bool, TopoDS_Wire&) const; 

DEFINE_STANDARD_RTTI( GEOMImpl_Fillet1dDriver )
};

#endif
