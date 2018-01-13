#ifndef __SIMULATOR_H__
#define __SIMULATOR_H__

#include <Python.h>

class cppSimulatorImp;
class Config;

typedef struct {
    PyObject_HEAD
    cppSimulatorImp* pImp;
    Config* pConfig;
} cppSimulatorObject;

#endif//__SIMULATOR_H__