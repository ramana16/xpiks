#ifndef LIBTHMBNLR_GLOBAL_H
#define LIBTHMBNLR_GLOBAL_H

#ifdef _WIN32
#  if defined(LIBTHMBNLR_LIBRARY)
#    define LIBTHMBNLR_EXPORT __declspec(dllexport)
#  else
#    define LIBTHMBNLR_EXPORT __declspec(dllimport)
#  endif
#else
#  define LIBTHMBNLR_EXPORT
#endif

#define UNUSED(x) (void)x;

#ifdef _WIN32
#define STD_STRING_TYPE std::wstring
#else
#define STD_STRING_TYPE std::string
#endif

#endif // LIBTHMBNLR_GLOBAL_H
