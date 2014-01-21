/*
 * W - simple cross-platform OpenGL windows
 *
 * =================================
 *  WInt_PlatformIdentification.h
 * =================================
 *
 * Copyright (C) 2012 - Ben Hallstein - http://ben.am
 * Published under the MIT license: http://opensource.org/licenses/MIT
 *
 */

#ifndef OGLMultiWindowTest_WInt_PlatformIdentification_h
#define OGLMultiWindowTest_WInt_PlatformIdentification_h

#ifdef __APPLE__

	#include "TargetConditionals.h"

	#if TARGET_OS_IPHONE
		#define WTARGET_IOS

	#elif TARGET_OS_MAC
		#define WTARGET_MAC

#endif


#elif defined _WIN32 || _WIN64

	#define WTARGET_WIN

#endif

#endif // Header guard
