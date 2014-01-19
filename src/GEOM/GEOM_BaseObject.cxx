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

#include "GEOM_BaseObject.hxx"
#include "GEOM_Engine.hxx"

#include <TCollection_ExtendedString.hxx>
#include <TDF_Data.hxx>
#include <TDF_LabelSequence.hxx>
#include <TDF_Reference.hxx>
#include <TDF_Tool.hxx>
#include <TDataStd_ChildNodeIterator.hxx>
#include <TDataStd_Comment.hxx>
#include <TDataStd_Integer.hxx>
#include <TDataStd_Name.hxx>
#include <TDataStd_Real.hxx>
#include <TDataStd_UAttribute.hxx>
#include <TDocStd_Document.hxx>
#include <TDocStd_Owner.hxx>
#include <TFunction_Driver.hxx>
#include <TFunction_DriverTable.hxx>
#include <TDF_ChildIterator.hxx>

#include "utilities.h"

#define FUNCTION_LABEL_ID  1
#define FUNCTION_LABEL(theNb) (_label.FindChild(FUNCTION_LABEL_ID).FindChild((theNb)))
#define TYPE_LABEL  	 2
#define FREE_LABEL  	 3
#define TIC_LABEL   	 4
#define DIRTY_LABEL 	 5
//#define COLOR_LABEL      6
//#define AUTO_COLOR_LABEL 7
#define USER_DATA_LABEL  8
//#define MARKER_LABEL     9


//=======================================================================
//function : GetObjectID
//purpose  :
//=======================================================================

const Standard_GUID& GEOM_BaseObject::GetObjectID()
{
  static Standard_GUID anObjectID("FF1BBB01-5D14-4df2-980B-3A668264EA16");
  return anObjectID;
}

//=======================================================================
//function : GetSubShapeID
//purpose  :
//=======================================================================

const Standard_GUID& GEOM_BaseObject::GetSubShapeID()
{
  static Standard_GUID anObjectID("FF1BBB68-5D14-4df2-980B-3A668264EA16");
  return anObjectID;
}

//=============================================================================
/*!
 *  GetObject
 */
//=============================================================================

Handle(GEOM_BaseObject) GEOM_BaseObject::GetObject(const TDF_Label& theLabel)
{
  if (!theLabel.IsAttribute(GetObjectID())) return NULL;

  TCollection_AsciiString anEntry;
  TDF_Tool::Entry(theLabel, anEntry);

  Handle(TDocStd_Document) aDoc = TDocStd_Owner::GetDocument(theLabel.Data());
  if(aDoc.IsNull()) return NULL;

  Handle(TDataStd_Integer) anID;
  if(!aDoc->Main().FindAttribute(TDataStd_Integer::GetID(), anID)) return NULL;

  GEOM_Engine* anEngine = GEOM_Engine::GetEngine();
  if(anEngine == NULL) return NULL;
  return anEngine->GetObject(anID->Get(), anEntry.ToCString());
}

//=============================================================================
/*!
 *  GetReferencedObject
 */
//=============================================================================
Handle(GEOM_BaseObject) GEOM_BaseObject::GetReferencedObject(const TDF_Label& theLabel)
{
  Handle(TDF_Reference) aRef;
  if (!theLabel.FindAttribute(TDF_Reference::GetID(), aRef)) {
    return NULL;
  }
  
  if(aRef.IsNull() || aRef->Get().IsNull()) {
    return NULL;
  }


  // Get TreeNode of a referenced function
  Handle(TDataStd_TreeNode) aT, aFather;
  if (!TDataStd_TreeNode::Find(aRef->Get(), aT)) {
    return NULL;
  }


  // Get TreeNode of Object of the referenced function
  aFather = aT->Father();
  if (aFather.IsNull()) {
    return NULL;
  }

  // Get label of the referenced object
  TDF_Label aLabel = aFather->Label();


  return GEOM_BaseObject::GetObject(aLabel);
}

//=======================================================================
//function : GetEntryString
//purpose  : Returns an entry of this GEOM_BaseObject
//=======================================================================

TCollection_AsciiString GEOM_BaseObject::GetEntryString()
{
  TCollection_AsciiString anEntry;
  TDF_Tool::Entry( GetEntry(), anEntry );
  return anEntry;
}

//=======================================================================
//function : GetType
//purpose  : Returns type of an object (GEOM_POINT, GEOM_VECTOR...) on theLabel,
//           -1 if no object is there
//=======================================================================

int GEOM_BaseObject::GetType(const TDF_Label& theLabel)
{
  Handle(TDataStd_Integer) aType;
  if(theLabel.IsNull() ||
     !theLabel.FindChild(TYPE_LABEL).FindAttribute(TDataStd_Integer::GetID(), aType))
    return -1;

  return aType->Get();
}

//=============================================================================
/*!
 *  Constructor: private
 */
//=============================================================================
GEOM_BaseObject::GEOM_BaseObject(const TDF_Label& theEntry)
: _label(theEntry), _ior(""), _docID(-1)
{
  Handle(TDocStd_Document) aDoc = TDocStd_Owner::GetDocument(_label.Data());
  if(!aDoc.IsNull()) {
    Handle(TDataStd_Integer) anID;
    if(aDoc->Main().FindAttribute(TDataStd_Integer::GetID(), anID)) _docID = anID->Get();
  }

  if(!theEntry.FindAttribute(TDataStd_TreeNode::GetDefaultTreeID(), _root))
    _root = TDataStd_TreeNode::Set(theEntry);
}

//=============================================================================
/*!
 *  Constructor: public
 */
//=============================================================================
GEOM_BaseObject::GEOM_BaseObject(const TDF_Label& theEntry, int theType)
: _label(theEntry), _ior(""), _docID(-1)
{
  Handle(TDocStd_Document) aDoc = TDocStd_Owner::GetDocument(_label.Data());
  if(!aDoc.IsNull()) {
    Handle(TDataStd_Integer) anID;
    if(aDoc->Main().FindAttribute(TDataStd_Integer::GetID(), anID)) _docID = anID->Get();
  }

  theEntry.ForgetAllAttributes(Standard_True);

  if(!theEntry.FindAttribute(TDataStd_TreeNode::GetDefaultTreeID(), _root))
    _root = TDataStd_TreeNode::Set(theEntry);

  TDataStd_Integer::Set(theEntry.FindChild(TYPE_LABEL), theType);

  TDataStd_UAttribute::Set(theEntry, GetObjectID());
}

//=============================================================================
/*!
 *  Destructor
 */
//=============================================================================
GEOM_BaseObject::~GEOM_BaseObject()
{
  //MESSAGE("GEOM_Object::~GEOM_Object()");
}

//=============================================================================
/*!
 *  GetType
 */
//=============================================================================
int GEOM_BaseObject::GetType()
{
  int type = -1;
  Handle(TDataStd_Integer) aType;
  if(_label.FindChild(TYPE_LABEL).FindAttribute(TDataStd_Integer::GetID(), aType))
    type = aType->Get();

  return type;
}

//=============================================================================
/*!
 *  SetType
 */
//=============================================================================
void GEOM_BaseObject::SetType(int theType)
{
  TDataStd_Integer::Set(_label.FindChild(TYPE_LABEL), theType);
}


//=============================================================================
/*!
 *  Returns modifications counter of this object.
 *  Comparing this value with modifications counters of argument objects
 *  (on which this object depends) we decide whether this object needs to be updated.
 */
//=============================================================================
int GEOM_BaseObject::GetTic()
{
  Handle(TDataStd_Integer) aTicAttr;
  if (!_label.FindChild(TIC_LABEL).FindAttribute(TDataStd_Integer::GetID(), aTicAttr))
    return 0;

  return aTicAttr->Get();
}

//=============================================================================
/*!
 *  Set another value of modifications counter.
 *
 *  Use this method to update modifications counter of dependent object
 *  to be equal to modifications counter of its argument.
 *  This is commonly done in GEOM_Function::GetValue()
 */
//=============================================================================
void GEOM_BaseObject::SetTic(int theTic)
{
  TDataStd_Integer::Set(_label.FindChild(TIC_LABEL), theTic);
}

//=============================================================================
/*!
 *  Increment modifications counter to mark this object as modified.
 *
 *  Commonly called from GEOM_Function::SetValue()
 */
//=============================================================================
void GEOM_BaseObject::IncrementTic()
{
  TDF_Label aTicLabel = _label.FindChild(TIC_LABEL);

  Standard_Integer aTic = 0;
  Handle(TDataStd_Integer) aTicAttr;
  if (aTicLabel.FindAttribute(TDataStd_Integer::GetID(), aTicAttr))
    aTic = aTicAttr->Get();

  TDataStd_Integer::Set(aTicLabel, aTic + 1);
}


//=============================================================================
/*!
 *  GetDocID
 */
//=============================================================================
int GEOM_BaseObject::GetDocID()
{
  return _docID;
}

//=============================================================================
/*!
 *  SetName
 */
//=============================================================================
void GEOM_BaseObject::SetName(const char* theName)
{
  TDataStd_Name::Set(_label, (char*)theName);
}

//=============================================================================
/*!
 *  GetName
 */
//=============================================================================
TCollection_AsciiString GEOM_BaseObject::GetName()
{
  TCollection_AsciiString aName;
  Handle(TDataStd_Name) aNameAttr;
  if(_label.FindAttribute(TDataStd_Name::GetID(), aNameAttr))
    aName = aNameAttr->Get();
  // do not return pointer of local variable
  // return aName.ToCString();
  // the following code could lead to memory leak, so take care about recieved pointer
  return aName;
}

//=============================================================================
/*!
 *  SetAuxData
 */
//=============================================================================
void GEOM_BaseObject::SetAuxData(const char* theData)
{
  TDataStd_Comment::Set(_label, (char*)theData);
}

//=============================================================================
/*!
 *  GetAuxData
 */
//=============================================================================
TCollection_AsciiString GEOM_BaseObject::GetAuxData()
{
  TCollection_AsciiString aData;

  Handle(TDataStd_Comment) aCommentAttr;
  if (_label.FindAttribute(TDataStd_Comment::GetID(), aCommentAttr))
    aData = aCommentAttr->Get();

  return aData;
}

//=============================================================================
/*!
 *  SetParameters
 */
//=============================================================================
void GEOM_BaseObject::SetParameters(const TCollection_AsciiString& theParameters)
{
  if( _parameters.IsEmpty() )
    _parameters = theParameters;
  else {
    _parameters += "|";
    _parameters += theParameters;
  }
}

//=============================================================================
/*!
 *  GetParameters
 */
//=============================================================================
TCollection_AsciiString GEOM_BaseObject::GetParameters() const
{
  return _parameters;
}

//=============================================================================
/*!
 *  AddFunction
 */
//=============================================================================
Handle(GEOM_Function) GEOM_BaseObject::AddFunction(const Standard_GUID& theGUID,
                                                   int                  theFunctionType,
                                                   bool                 allowSubShape)
{
  Standard_Integer nb = GetNbFunctions();
  //if(!allowSubShape && nb == 1 && theGUID == GetSubShapeID()) return NULL; //It's impossible to add a function to sub-shape
  TDF_Label aChild = FUNCTION_LABEL(++nb);

  Handle(TDataStd_TreeNode) aNode = TDataStd_TreeNode::Set(aChild);
  _root->Append(aNode);

  Handle(GEOM_Function) aFunction = new GEOM_Function(aChild, theGUID, theFunctionType);

  //If the new function is additional then itterate the tree and
  //update the dependencies.
  if (nb > 1) {

	//Find all dependencies of this object
	TDF_LabelSequence aSeq;
	for (TDF_ChildIterator it(_label.Father()); it.More(); it.Next()) {
	  TDF_Label currObjLabel = it.Value();
	  const Handle_GEOM_Object currObj = GEOM_Object::GetObject(currObjLabel);
	  if (!currObj.IsNull()) {
		if (currObj == this) continue;
		Standard_Integer nb = currObj->GetNbFunctions();
		if(nb == 0) continue;
		for(Standard_Integer i=1; i<=nb; i++) {
		  Handle(GEOM_Function) anObFunction = currObj->GetFunction(i);
		  if(anObFunction.IsNull()) continue;
		  anObFunction->GetDependency(aSeq);
		}
	  }
	}

	//Replace the dependencies with the current last function of this object
	Standard_Integer aLength = aSeq.Length();
	if(aLength > 0) {
	  Handle(GEOM_Function) aLastFunction = GetFunction(nb-1);
	  for(Standard_Integer j =1; j<=aLength; j++) {
		TDF_Label currRefLabel = aSeq(j);
		if(!currRefLabel.IsNull()) {
		  Handle(TDF_Reference) aRef;
		  if(currRefLabel.FindAttribute(TDF_Reference::GetID(), aRef)) {
			if (aRef->Get().IsEqual(aLastFunction->GetEntry())) {
			  aRef->Set(aFunction->GetEntry());
			}
		  }
		}
	  }
	}
	
  }

  return aFunction;
}

//=============================================================================
/*!
 *  RemoveFunction
 */
//=============================================================================
Standard_Boolean GEOM_BaseObject::RemoveFunction(Handle(GEOM_Function) aFunction)
{
  if (aFunction.IsNull()) return Standard_False;
  Standard_Integer nb = GetNbFunctions();

  //Must have at least two functions in order to remove one
  //otherwise an orphan GEOM_Object will be created
  if(nb <= 1) return Standard_False;

  for(Standard_Integer i=1; i<=nb; i++) {

	//Check if the aFunction is attached to this Object
	Handle(GEOM_Function) anItFunction = GetFunction(i);
	if(anItFunction.IsNull()) continue;
	if (anItFunction->GetEntry().IsEqual(aFunction->GetEntry())) {

	  //The first function cannot be removed
	  if (i==1) return Standard_False;

	  //Find all dependencies of this document
	  TDF_LabelSequence aSeq;
	  for (TDF_ChildIterator it(_label.Father()); it.More(); it.Next()) {
		TDF_Label currObjLabel = it.Value();
		Handle_GEOM_Object currObj = GEOM_Object::GetObject(currObjLabel);
		if (!currObj.IsNull()) {
		  Standard_Integer nb = currObj->GetNbFunctions();
		  if(nb == 0) continue;
		  for(Standard_Integer i=1; i<=nb; i++) {
			Handle(GEOM_Function) anObFunction = currObj->GetFunction(i);
			if(anObFunction.IsNull()) continue;
			if(anObFunction->GetEntry().IsEqual(aFunction->GetEntry())) continue;
			anObFunction->GetDependency(aSeq);
		  }
		}
	  }

	  //Replace the dependencies with the previous function of this object
	  Standard_Integer aLength = aSeq.Length();
	  if(aLength > 0) {
		for(Standard_Integer j =1; j<=aLength; j++) {
		  TDF_Label currRefLabel = aSeq(j);
		  if(!currRefLabel.IsNull()) {
			Handle(TDF_Reference) aRef;
			if(currRefLabel.FindAttribute(TDF_Reference::GetID(), aRef)) {
			  if (aRef->Get().IsEqual(aFunction->GetEntry())) {
				aRef->Set( GetFunction(i-1)->GetEntry() );
			  }
			}
		  }
		}
	  }

	  //Function found.Remove its attributes
	  aFunction->GetEntry().ForgetAllAttributes(Standard_True);

	  return Standard_True;
	}
  }
  return Standard_False;
}

//=============================================================================
/*!
 *  GetNbFunctions
 */
//=============================================================================
int GEOM_BaseObject::GetNbFunctions()
{
  Standard_Integer nb = 0;
  for(TDataStd_ChildNodeIterator CI(_root); CI.More(); CI.Next()) nb++;
  return nb;
}

//=============================================================================
/*!
 *  GetFunction
 */
//=============================================================================
Handle(GEOM_Function) GEOM_BaseObject::GetFunction(int theFunctionNumber)
{
  TDF_Label aChild = FUNCTION_LABEL(theFunctionNumber);
  return GEOM_Function::GetFunction(aChild);
}

//=============================================================================
/*!
 *  GetlastFunction
 */
//=============================================================================
Handle(GEOM_Function) GEOM_BaseObject::GetLastFunction()
{
  Standard_Integer nb = GetNbFunctions();
  if(nb) return GetFunction(nb);

  return NULL;
}

//=============================================================================
/*!
 *  GetAllDependency
 */
//=============================================================================
Handle(TColStd_HSequenceOfTransient) GEOM_BaseObject::GetAllDependency()
{
  Handle(TColStd_HSequenceOfTransient) anArray;
  TDF_LabelSequence aSeq;
  Standard_Integer nb = GetNbFunctions();
  if(nb == 0) return anArray;
  for(Standard_Integer i=1; i<=nb; i++) {
	Handle(GEOM_Function) aFunction = GetFunction(i);
	if(aFunction.IsNull()) continue;
	aFunction->GetDependency(aSeq);
  }

  Standard_Integer aLength = aSeq.Length();
  if(aLength > 0) {
	anArray = new TColStd_HSequenceOfTransient;
	for(Standard_Integer j =1; j<=aLength; j++) {
      Handle(GEOM_BaseObject) aRefObj = GetReferencedObject(aSeq(j));
	  if(!aRefObj.IsNull()) anArray->Append(aRefObj);
	}
  }

  return anArray;
}

//=============================================================================
/*!
 *  GetLastDependency
 */
//=============================================================================
Handle(TColStd_HSequenceOfTransient) GEOM_BaseObject::GetLastDependency()
{
  Handle(TColStd_HSequenceOfTransient) anArray;
  Handle(GEOM_Function) aFunction = GetLastFunction();
  if (aFunction.IsNull()) return anArray;

  TDF_LabelSequence aSeq;
  aFunction->GetDependency(aSeq);
  Standard_Integer aLength = aSeq.Length();
  if (aLength > 0) {
    anArray = new TColStd_HSequenceOfTransient;
    for (Standard_Integer i = 1; i <= aLength; i++)
      anArray->Append(GetReferencedObject(aSeq(i)));
  }

  return anArray;
}

//================================================================================
/*!
 * \brief Returns a driver creator of this object
 */
//================================================================================

Handle(TFunction_Driver) GEOM_BaseObject::GetCreationDriver()
{
  Handle(TFunction_Driver) aDriver;

  Handle(GEOM_Function) function = GetFunction(1);
  if ( !function.IsNull() )
  {
    Standard_GUID aGUID = function->GetDriverGUID();
    if ( TFunction_DriverTable::Get()->FindDriver(aGUID, aDriver))
      aDriver->Init( function->GetEntry() );
  }
  return aDriver;
}

//=============================================================================
/*!
 *  GetFreeLabel
 */
//=============================================================================
TDF_Label GEOM_BaseObject::GetFreeLabel()
{
  return _label.FindChild(FREE_LABEL);
}

//=============================================================================
/*!
 *  GetUserDataLabel
 */
//=============================================================================
TDF_Label GEOM_BaseObject::GetUserDataLabel()
{
  return _label.FindChild(USER_DATA_LABEL);
}

//=============================================================================
/*!
 *
 */
//=============================================================================
Standard_Boolean GEOM_BaseObject::IsDirty()
{
  Handle(TDataStd_Integer) aDirtyAttr;
  if (!_label.FindChild(DIRTY_LABEL).FindAttribute(TDataStd_Integer::GetID(), aDirtyAttr))
	return Standard_False;

  Standard_Integer theFlagVal = aDirtyAttr->Get();

  if (theFlagVal == 0)
	return Standard_False;
  else
	return Standard_True;
}

//=============================================================================
/*!
 *
 */
//=============================================================================
void GEOM_BaseObject::SetDirty(Standard_Boolean theFlag)
{
  Standard_Integer theFlagVal;

  if (theFlag)
	theFlagVal = 1;
  else
	theFlagVal = 0;

  TDataStd_Integer::Set(_label.FindChild(DIRTY_LABEL), theFlagVal);
}

IMPLEMENT_STANDARD_HANDLE (GEOM_BaseObject, Standard_Transient );
IMPLEMENT_STANDARD_RTTIEXT(GEOM_BaseObject, Standard_Transient );
