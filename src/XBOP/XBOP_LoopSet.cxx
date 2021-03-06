// Created on: 1993-03-23
// Created by: Jean Yves LEBEY
// Copyright (c) 1993-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.


#include <XBOP_LoopSet.ixx>

//=======================================================================
//function : XBOP_LoopSet::XBOP_LoopSet
//purpose  : 
//=======================================================================
XBOP_LoopSet::XBOP_LoopSet() 
: 
  myLoopIndex(1), 
  myNbLoop(0)
{}
//=======================================================================
//function : Delete
//purpose  : 
//=======================================================================
  void XBOP_LoopSet::Delete()
{}
//=======================================================================
//function : InitLoop
//purpose  : 
//=======================================================================
  void XBOP_LoopSet::InitLoop()
{
  myLoopIterator.Initialize(myListOfLoop);
  myLoopIndex = 1;
  myNbLoop = myListOfLoop.Extent();
}
//=======================================================================
//function : MoreLoop
//purpose  : 
//=======================================================================
  Standard_Boolean XBOP_LoopSet::MoreLoop() const
{
  Standard_Boolean b = myLoopIterator.More();
  return b;
}
//=======================================================================
//function : NextLoop
//purpose  : 
//=======================================================================
  void XBOP_LoopSet::NextLoop()
{
  myLoopIndex++;
  myLoopIterator.Next();
}
//=======================================================================
//function : Loop
//purpose  : 
//=======================================================================
  const Handle(XBOP_Loop)& XBOP_LoopSet::Loop() const
{
  const Handle(XBOP_Loop)& L = myLoopIterator.Value();
  return L;
}
//=======================================================================
//function : ChangeListOfLoop
//purpose  : 
//=======================================================================
  XBOP_ListOfLoop& XBOP_LoopSet::ChangeListOfLoop()
{
  return myListOfLoop;
}
