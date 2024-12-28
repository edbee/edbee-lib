#ifndef CONFIG_ONIGURUMA_EDBEE_H
#define CONFIG_ONIGURUMA_EDBEE_H

#ifdef __clang__
  //#pragma clang diagnostic ignored "-Wunused-variable"
  #pragma clang diagnostic ignored "-Wunused-parameter"
  #pragma clang diagnostic ignored "-Wmissing-field-initializers"
  #pragma clang diagnostic ignored "-Wsign-compare"

#else
  #ifdef  __GNUC__
  //#pragma GCC diagnostic ignored "-Wunused-variable"
  #pragma GCC diagnostic ignored "-Wunused-parameter"
  #pragma GCC diagnostic ignored "-Wmissing-field-initializers"
  #pragma CCC diagnostic ignored "-Wsign-compare"
  #endif
#endif

#ifdef _MSC_VER
  // Visual studio directives
  // C4100: unreferenced formal parameter
  #pragma warning( disable : 4100 )
#endif

// TODO: Add more visual studio and other compiler directives

#endif // CONFIG_ONIGURUMA_EDBEE_H
