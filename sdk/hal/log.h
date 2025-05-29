#ifndef LOG_H
#define LOG_H

#include "os.h"

#define LOG_LEVEL_OFF     0
#define LOG_LEVEL_ERROR   1
#define LOG_LEVEL_WARNING 2
#define LOG_LEVEL_INFO    3
#define LOG_LEVEL_DEBUG   4


#if LOG_ENABLE

  #define LOG_DEF(prefix) static const char *TAG = prefix

  #define LOG_DEBUG( ... )   log_msg(LOG_LEVEL_DEBUG, TAG, __VA_ARGS__)
  #define LOG_INFO( ... )    log_msg(LOG_LEVEL_INFO, TAG, __VA_ARGS__)
  #define LOG_WARNING( ... ) log_msg(LOG_LEVEL_WARNING, TAG, __VA_ARGS__)
  #define LOG_ERROR( ... )   log_msg(LOG_LEVEL_ERROR, TAG, __VA_ARGS__)

  void log_msg(int level, const ascii *id, const ascii *fmt, ... );
  void log_dump(const ascii *id, u8 *data, int len);
  void log_level_set(int level);
  int log_level_get(void);


#else // LOG_ENABLE

  #define LOG_DEF(x)

  #if (MAIN_DEBUG == 1)
    #define LOG_DEBUG( ... )   {OS_PRINTF(__VA_ARGS__); OS_PRINTF(NL);}
  #else
    #define LOG_DEBUG( ... )
  #endif // MAIN_DEBUG != 1

  #define LOG_INFO( ... )    {OS_PRINTF(__VA_ARGS__); OS_PRINTF(NL);}
  #define LOG_WARNING( ... ) {OS_PRINTF( __VA_ARGS__); OS_PRINTF(NL);}
  #define LOG_ERROR( ... )   {OS_PRINTF("ERROR:" __VA_ARGS__); OS_PRINTF(NL);}

  #define log_dump( ... )
  #define log_level_set(n)
  #define log_level_get() 0

#endif // not LOG_ENABLE

#endif //  LOG_H

