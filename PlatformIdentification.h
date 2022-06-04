#ifndef WPlatformIdentification_h
#define WPlatformIdentification_h

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

#endif
