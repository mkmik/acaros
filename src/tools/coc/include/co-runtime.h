#ifndef _CO_RUNTIME_H_
#define _CO_RUNTIME_H_

#define BEGIN_CLASS(arg) 

#include <stdarg.h>
#include <list.h>
#include <types.h>

//typedef struct class_t class_t;
typedef struct Class_s Class_t;

typedef struct {
  Class_t* _class;
} object_t;

/*struct class_t {
  object_t;
  char* name;
  class_t* parent;
  size_t size;
  list_head_t methods;
  list_head_t classes; // links all classes together
  };*/

typedef object_t* (*method_t)(object_t *self, va_list args);

typedef struct {
  char* name;
  method_t method;
  list_head_t method_list;
} method_desc_t;

#define nil ((object_t*)0)

//#define newObject(class) (class*) createObject(class ## _class)
void* newObject(char* class);
Class_t* createClass(char* name, Class_t* parent, size_t size);
Class_t* findClass(char* name);

void addMethod(Class_t* class, char* name, method_t method);
object_t* createObject(Class_t* class);
method_t lookup(Class_t* class, char* selector);
object_t* invoke(char* selector, object_t* object,...);
object_t* invokeSuper(Class_t* class, char* selector, object_t* object,...);


typedef struct {
  char* class;
  char* selector;
  method_t method;
} co_method_initializer_t;

typedef struct {
  char* class;
  char* superclass;
  size_t size;
  Class_t **globalSymbol;
} co_class_initializer_t;

#endif
