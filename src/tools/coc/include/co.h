#ifndef _CO_H_
#define _CO_H_

#include <co-runtime.h>

@+class Object : object {
  Class_t* _class;
};

@+class Class : Object {
  char* name;
  Class_t* parent;
  size_t size;
  list_head_t methods;
  list_head_t classes; // links all classes together
};
     
@+class UnknownObject {
  
};

#endif
