//  Copyright (C) 2007-2008  CEA/DEN, EDF R&D, OPEN CASCADE
//
//  Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
//  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
//
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com

#ifndef _GEOMImpl_SmoothingSurfaceDriver_HXX
#define _GEOMImpl_SmoothingSurfaceDriver_HXX

#ifndef _TFunction_Driver_HeaderFile
#include <TFunction_Driver.hxx>
#endif
#ifndef _TFunction_Logbook_HeaderFile
#include <TFunction_Logbook.hxx>
#endif
#ifndef _Standard_CString_HeaderFile
#include <Standard_CString.hxx>
#endif
#include <TColgp_HArray1OfPnt.hxx>
#include <TopoDS_Shape.hxx>

class Handle_Standard_Type;
class GEOMImpl_SmoothingSurfaceDriver;



#include "GEOM_BaseDriver.hxx"

DEFINE_STANDARD_HANDLE( GEOMImpl_SmoothingSurfaceDriver, GEOM_BaseDriver );

class GEOMImpl_SmoothingSurfaceDriver : public GEOM_BaseDriver {
public:
 // Methods PUBLIC
  // 
  Standard_EXPORT GEOMImpl_SmoothingSurfaceDriver();
  Standard_EXPORT virtual  Standard_Integer Execute(TFunction_Logbook& log) const; 
  Standard_EXPORT virtual void Validate(TFunction_Logbook&) const {}
  Standard_EXPORT Standard_Boolean MustExecute(const TFunction_Logbook&) const
  {
    return Standard_True;
  }
  Standard_EXPORT static const Standard_GUID& GetID();
  Standard_EXPORT ~GEOMImpl_SmoothingSurfaceDriver() {};
  
  Standard_EXPORT virtual
  bool GetCreationInformation(std::string&             theOperationName,
                              std::vector<GEOM_Param>& params);
  // Type management
  //
DEFINE_STANDARD_RTTI( GEOMImpl_SmoothingSurfaceDriver )

private:
  TopoDS_Shape MakeSmoothingSurfaceUnClosed(Handle_TColgp_HArray1OfPnt myListOfPoints) const;
};

#endif // _GEOMImpl_SmoothingSurfaceDriver_HXX
