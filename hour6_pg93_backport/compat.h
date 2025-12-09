/* compat.h - Compatibility macros for MSVC 2005 / Windows XP */

#ifndef COMPAT_H
#define COMPAT_H

/* MSVC 2005 compatibility: use _snprintf instead of snprintf */
#if defined(_MSC_VER) && _MSC_VER < 1900
#define snprintf _snprintf
#endif

/* ICC_STANDARD_CLASSES may not be defined in older Windows SDK */
#ifndef ICC_STANDARD_CLASSES
#define ICC_STANDARD_CLASSES 0x00004000
#endif

/* LVS_EX_DOUBLEBUFFER may not be defined in older Windows SDK */
#ifndef LVS_EX_DOUBLEBUFFER
#define LVS_EX_DOUBLEBUFFER 0x00010000
#endif

/* CLEARTYPE_QUALITY may not be defined in older Windows SDK */
#ifndef CLEARTYPE_QUALITY
#define CLEARTYPE_QUALITY 5
#endif

#endif /* COMPAT_H */
