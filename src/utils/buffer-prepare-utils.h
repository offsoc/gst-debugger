/*
 * buffer-prepare-utils.h
 *
 *  Created on: Jul 20, 2015
 *      Author: loganek
 */

#ifndef SRC_UTILS_BUFFER_PREPARE_UTILS_H_
#define SRC_UTILS_BUFFER_PREPARE_UTILS_H_

#define SAFE_PREPARE_BUFFER_INIT(BUFFER_SIZE) \
  gchar buff[BUFFER_SIZE]; \
  gchar *m_buff = buff; \
  gint max_m_buff_size = BUFFER_SIZE;

#define SAFE_PREPARE_BUFFER(FUNCTION_CALL, SIZE_VAR) \
  do { \
    SIZE_VAR = (FUNCTION_CALL); \
    if (SIZE_VAR > 1024) { \
      m_buff = (gchar *) g_malloc (SIZE_VAR); \
      max_m_buff_size = SIZE_VAR; \
      SIZE_VAR = (FUNCTION_CALL); \
    } \
  } while (0)

#define SAFE_PREPARE_BUFFER_CLEAN \
  do { \
    if (m_buff != buff) { \
      g_free (m_buff); \
    } \
  } while (0)


#endif /* SRC_UTILS_BUFFER_PREPARE_UTILS_H_ */
