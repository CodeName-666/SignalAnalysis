/*
 * Puls.h
 *
 *  Created on: 17.12.2017
 *      Author: AP02
 */

#ifndef _PULS_H_
#define _PULS_H_

#include <Arduino.h>
#include "Edge.h"

#define PULS_WIDTH_TIME_INVALID 0
#define PULS_DEBOUNCE_DISABLED -1

typedef enum
{
   PULS_DEBOUNCING_DISABLED = 0,
   PULS_DEBOUNCING_ENABLED = 1,

} PulsDebouncing_e;

typedef enum
{
   PULS_TIMEOUT_DISABLED = 1,
   PULS_TIMEOUT_ENABLED

} PulsTimeout_e;

/**
 * @brief The PulsType_t enum
 */
typedef enum
{
   PULS_NO_PULS = 0x00,
   PULS_TRIGGERED_HIGH = 0x01,
   PULS_HIGH = 0x02,
   PULS_TRIGGERED_LOW = 0x03,
   PULS_LOW = 0x04,
   PULS_BOTH = 0x05,
   PULS_TIMEOUT = 0x06
} PulsType_e;

typedef enum
{
   PULS_RESOLUTION_US = 0,
   PULS_RESOLUTION_MS = 1
} PulsResolution_e;

class Puls : private Edge
{

public:
   /**
    * @brief Puls
    * @param trigger
    */
   Puls(PulsType_e trigger, uint8_t ioStatus = 0, uint32_t timeout_time = 0, uint32_t debounce_time = 0);

   Puls(const Puls &) = default;

   /**
    * @brief ~Puls
    */
   virtual ~Puls();
   /**
    * @brief checkPuls
    * @param pinStatus
    */
   void checkPuls(uint8_t pinStatus);
   /**
    * @brief setTrigger
    * @param trigger
    */
   void setTrigger(PulsType_e trigger);

   PulsType_e getPulsType(void);

   int32_t getPulsWidth(void);

   uint32_t getPulsCount(void);

   boolean isErrorOccurred(void);

   uint32_t getErrorCount(void);

   void enablePulsTimeout(uint32_t timeout_time);

   void enableDebounce(uint32_t msDebounce = 500);

   void disablePulsTimeout(void);

   void disableDebounce(void);

   void clear(void);

   PulsType_e getTrigger(void);

   void pulsResolution(PulsResolution_e new_resolution);

   PulsResolution_e pulsResolution(void);

private:
   void init(PulsType_e trigger, int32_t timeout_time = 0, uint32_t debounce_time = 0, PulsResolution_e resolution = PULS_RESOLUTION_MS);
   uint32_t time(void);
   void checkTimeout(uint32_t timeout_time);
   void reset(void);

private:
   PulsType_e m_configured_puls_type;

   PulsType_e m_current_puls_type;
   uint32_t m_current_puls_width;
   uint32_t m_puls_count;
   uint32_t m_error_count;

   uint32_t m_puls_timeout;
   PulsTimeout_e m_puls_timeout_status;

   uint32_t m_puls_debounce_time;
   PulsDebouncing_e m_puls_debouncing_status;
   PulsResolution_e m_puls_resolution;

   EdgeInfo_t m_current_edge;
   EdgeInfo_t m_last_edge;
};

#endif /*_PULS_H_ */
