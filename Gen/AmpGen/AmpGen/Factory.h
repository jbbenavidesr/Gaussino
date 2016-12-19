#ifndef FACTORY_H
#define FACTORY_H
#include "AmpGen/MsgService.h"
#include <map>

namespace AmpGen { 
  template < class TYPE , class KEY_TYPE=std::string>
    class Factory {
      public : 
        std::map<KEY_TYPE,TYPE*> m_terms;
        static Factory<TYPE,KEY_TYPE>* gImpl; 

        static Factory<TYPE,KEY_TYPE>* getMe(){
          if( ! gImpl ) gImpl = new Factory<TYPE,KEY_TYPE>();
          return gImpl; 
        }
        static TYPE* get( const KEY_TYPE& type){
          auto ptrToStatic = getMe();
          auto raw_base = ptrToStatic->m_terms.find( type );
          if( raw_base == ptrToStatic->m_terms.end() ){
            ERROR( type << " not found in factory" );
            return nullptr;
          }
          auto objectToReturn = raw_base->second->create();
          return objectToReturn; 
        }
        static KEY_TYPE Register( const KEY_TYPE& key, TYPE* object ){
          getMe()->m_terms[key] = object ;
          return key;
        }
    };
}


#define REGISTER(BASE_CLASS, DERIVED_CLASS ) std::string DERIVED_CLASS::_id = AmpGen::Factory<BASE_CLASS>::Register(#DERIVED_CLASS, new DERIVED_CLASS() )

#define REGISTER_WITH_KEY(BASE_CLASS, DERIVED_CLASS, KEY, KEY_TYPE ) KEY_TYPE DERIVED_CLASS::_id = AmpGen::Factory<BASE_CLASS,KEY_TYPE>::Register(KEY, new DERIVED_CLASS() )


#endif 
