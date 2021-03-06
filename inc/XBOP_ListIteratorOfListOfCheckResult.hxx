// This file is generated by WOK (CPPExt).
// Please do not edit this file; modify original file instead.
// The copyright and license terms as defined for the original file apply to 
// this header file considered to be the "object code" form of the original source.

#ifndef _XBOP_ListIteratorOfListOfCheckResult_HeaderFile
#define _XBOP_ListIteratorOfListOfCheckResult_HeaderFile

#ifndef _Standard_HeaderFile
#include <Standard.hxx>
#endif
#ifndef _Standard_Macro_HeaderFile
#include <Standard_Macro.hxx>
#endif

#ifndef _Standard_Address_HeaderFile
#include <Standard_Address.hxx>
#endif
#ifndef _Handle_XBOP_ListNodeOfListOfCheckResult_HeaderFile
#include <Handle_XBOP_ListNodeOfListOfCheckResult.hxx>
#endif
#ifndef _Standard_Boolean_HeaderFile
#include <Standard_Boolean.hxx>
#endif
class Standard_NoMoreObject;
class Standard_NoSuchObject;
class XBOP_ListOfCheckResult;
class XBOP_CheckResult;
class XBOP_ListNodeOfListOfCheckResult;



class XBOP_ListIteratorOfListOfCheckResult  {
public:

  void* operator new(size_t,void* anAddress) 
  {
    return anAddress;
  }
  void* operator new(size_t size) 
  {
    return Standard::Allocate(size); 
  }
  void  operator delete(void *anAddress) 
  {
    if (anAddress) Standard::Free((Standard_Address&)anAddress); 
  }

  
  Standard_EXPORT   XBOP_ListIteratorOfListOfCheckResult();
  
  Standard_EXPORT   XBOP_ListIteratorOfListOfCheckResult(const XBOP_ListOfCheckResult& L);
  
  Standard_EXPORT     void Initialize(const XBOP_ListOfCheckResult& L) ;
  
        Standard_Boolean More() const;
  
  Standard_EXPORT     void Next() ;
  
  Standard_EXPORT     XBOP_CheckResult& Value() const;


friend class XBOP_ListOfCheckResult;



protected:





private:



Standard_Address current;
Standard_Address previous;


};

#define Item XBOP_CheckResult
#define Item_hxx <XBOP_CheckResult.hxx>
#define TCollection_ListNode XBOP_ListNodeOfListOfCheckResult
#define TCollection_ListNode_hxx <XBOP_ListNodeOfListOfCheckResult.hxx>
#define TCollection_ListIterator XBOP_ListIteratorOfListOfCheckResult
#define TCollection_ListIterator_hxx <XBOP_ListIteratorOfListOfCheckResult.hxx>
#define Handle_TCollection_ListNode Handle_XBOP_ListNodeOfListOfCheckResult
#define TCollection_ListNode_Type_() XBOP_ListNodeOfListOfCheckResult_Type_()
#define TCollection_List XBOP_ListOfCheckResult
#define TCollection_List_hxx <XBOP_ListOfCheckResult.hxx>

#include <TCollection_ListIterator.lxx>

#undef Item
#undef Item_hxx
#undef TCollection_ListNode
#undef TCollection_ListNode_hxx
#undef TCollection_ListIterator
#undef TCollection_ListIterator_hxx
#undef Handle_TCollection_ListNode
#undef TCollection_ListNode_Type_
#undef TCollection_List
#undef TCollection_List_hxx


// other Inline functions and methods (like "C++: function call" methods)


#endif
