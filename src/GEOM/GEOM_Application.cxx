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

#include <Standard_Stream.hxx>
#include <GEOM_Application.hxx>

#include <TColStd_SequenceOfExtendedString.hxx>
#include <Standard_TypeMismatch.hxx>

GEOM_Application::~GEOM_Application() {}
 
Standard_EXPORT Handle_Standard_Type& GEOM_Application_Type_()
{

    static Handle_Standard_Type aType1 = STANDARD_TYPE(TDocStd_Application);
  if ( aType1.IsNull()) aType1 = STANDARD_TYPE(TDocStd_Application);
  static Handle_Standard_Type aType2 = STANDARD_TYPE(CDF_Application);
  if ( aType2.IsNull()) aType2 = STANDARD_TYPE(CDF_Application);
  static Handle_Standard_Type aType3 = STANDARD_TYPE(CDM_Application);
  if ( aType3.IsNull()) aType3 = STANDARD_TYPE(CDM_Application);
  static Handle_Standard_Type aType4 = STANDARD_TYPE(Standard_Transient);
  if ( aType4.IsNull()) aType4 = STANDARD_TYPE(Standard_Transient);
 

  static Handle_Standard_Transient _Ancestors[]= {aType1,aType2,aType3,aType4,NULL};
  static Handle_Standard_Type _aType = new Standard_Type("GEOM_Application",
			                                 sizeof(GEOM_Application),
			                                 1,
			                                 (Standard_Address)_Ancestors,
			                                 (Standard_Address)NULL);

  return _aType;
}

// DownCast method
// allow safe downcasting


const Handle(GEOM_Application) Handle(GEOM_Application)::DownCast(const Handle(Standard_Transient)& AnObject)
{
  Handle(GEOM_Application) _anOtherObject;

  if (!AnObject.IsNull()) {
     if (AnObject->IsKind(STANDARD_TYPE(GEOM_Application))) {
       _anOtherObject = Handle(GEOM_Application)((Handle(GEOM_Application)&)AnObject);
     }
  }

  return _anOtherObject ;
}


const Handle(Standard_Type)& GEOM_Application::DynamicType() const 
{ 
  return STANDARD_TYPE(GEOM_Application) ; 
}
Standard_Boolean GEOM_Application::IsKind(const Handle(Standard_Type)& AType) const 
{ 
  return (STANDARD_TYPE(GEOM_Application) == AType || TDocStd_Application::IsKind(AType)); 
}
Handle_GEOM_Application::~Handle_GEOM_Application() {}





//=======================================================================
//function : GEOM_Application
//purpose  : 
//=======================================================================

GEOM_Application::GEOM_Application() 
{
  myCB = 0;
  myClientData = 0;
}


//=======================================================================
//function : Formats
//purpose  : 
//=======================================================================

void GEOM_Application::Formats(TColStd_SequenceOfExtendedString& Formats) 
{
  Formats.Append(TCollection_ExtendedString ("GEOM_ASCII"));
  Formats.Append(TCollection_ExtendedString ("GEOM_BIN"));
  Formats.Append(TCollection_ExtendedString ("GEOM_XML"));
}


//=======================================================================
//function : ResourcesName
//purpose  :
//=======================================================================

Standard_CString GEOM_Application::ResourcesName()
{
  return Standard_CString ("Resources");
}


//=======================================================================
//function : SetTransactionCallBack
//purpose  :
//=======================================================================

void GEOM_Application::SetTransactionCallBack(const GEOM_TransactionCallbackProc& aCB,const Standard_Address aClientData)
{
  myCB = aCB;
  myClientData = aClientData;
}


//=======================================================================
//function : SetTransactionCallBack
//purpose  :
//=======================================================================

void GEOM_Application::OnOpenTransaction(const Handle(TDocStd_Document)& theDoc)
{
  if (myCB)
    myCB(theDoc, cbOpenTransaction, myClientData);
}


//=======================================================================
//function : SetTransactionCallBack
//purpose  :
//=======================================================================

void GEOM_Application::OnCommitTransaction(const Handle(TDocStd_Document)& theDoc)
{
  if (myCB)
    myCB(theDoc, cbCommitTransaction, myClientData);
}


//=======================================================================
//function : SetTransactionCallBack
//purpose  :
//=======================================================================

void GEOM_Application::OnAbortTransaction(const Handle(TDocStd_Document)& theDoc)
{
  if (myCB)
    myCB(theDoc, cbAbortTransaction, myClientData);
}
