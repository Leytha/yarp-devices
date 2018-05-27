// Copyright: 
// Author: 
// CopyPolicy: 

//////////////////////////////////////////////////////////////////////////
// 
// This is a configuration file to explain ROBOT_DEVASTATION_ROBOTS to SWIG
//
// SWIG, for the most part, understands ROBOT_DEVASTATION_ROBOTS auto-magically.
// There are a few things that need to be explained:
//  + use of multiple inheritance
//  + use of names that clash with special names in Java/Python/Perl/...
//  + use of templates

%module asrob_yarp_devices

//%import "yarp.i"

%define SWIG_PREPROCESSOR_SHOULD_SKIP_THIS %enddef

%{
/* Includes the header in the wrapper code */
#include "IRobotManager.hpp"
%}

/* See https://github.com/robotology/yarp/pull/1696 */
#if SWIG_VERSION >= 0x030011 && !defined(SWIGCSHARP)
    %define VOCAB(x1,x2,x3,x4) x4*16777216+x3*65536+x2*256+x1
    %enddef
#elif defined(SWIGCSHARP)
    %define VOCAB(x1,x2,x3,x4) 0
    %enddef
#endif

/* Parse the header file to generate wrappers */
%include "IRobotManager.hpp"

%{
#include <yarp/dev/all.h>
asrob::IRobotManager *viewIRobotManager(yarp::dev::PolyDriver& d)
{
    asrob::IRobotManager *result;
    d.view(result);
    return result;
}
%}
extern asrob::IRobotManager *viewIRobotManager(yarp::dev::PolyDriver& d);

