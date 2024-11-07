/*
 * Puls.cpp
 *
 *  Created on: 17.12.2017
 *      Author: AP02
 */

#include "Puls.h"
#include "Puls_DebugIf.h"



Puls::~Puls()
{

}

Puls::Puls(PulsType_e trigger, uint8_t dio_init_level, uint32_t timeout_time, uint32_t debounce_time) : Edge(dio_init_level)
{
   init(trigger,timeout_time,debounce_time, PULS_RESOLUTION_MS);
}

void Puls::clear(void)
{
    m_current_puls_type = PULS_NO_PULS;
    m_current_puls_width = 0;
    m_puls_count = 0;
   m_error_count = 0;

}

void Puls::init(PulsType_e trigger, int32_t timeout_time, uint32_t debounce_time, PulsResolution_e resolution)
{
   m_current_puls_type = PULS_NO_PULS;
   m_current_puls_width = 0;

   m_puls_count = 0U;
   m_configured_puls_type = trigger;
   m_error_count = 0;

   m_current_edge = { EDGE_NO_EDGE, 0u, };
   m_last_edge = { EDGE_NO_EDGE, 0u,};
   
   pulsResolution(resolution);
   enablePulsTimeout(timeout_time);
   enableDebounce(debounce_time);
   return;
}

void Puls::checkPuls(uint8_t pinStatus)
{
   
   static uint32_t timeoutTime = 0;
   boolean pulsDetected = false;
  
   Edge::setStatus(pinStatus);
   Edge::checkEdges();

   if(getCurrentEdge() != EDGE_NO_EDGE)
   {
      m_current_edge = Edge::info();
      /*Validated PULS for HIGH PULS*/
      if(m_configured_puls_type == PULS_HIGH)
      {  
         if(m_last_edge.m_edge == EDGE_RISING && m_current_edge.m_edge == EDGE_FALLING)
         {
            m_current_puls_type = PULS_HIGH;
            pulsDetected = true;

            DBIF_LOG_DEBUG_0("HIGH_PULS: PULS DETECTED");
         }
         else if(m_current_edge.m_edge == EDGE_RISING && pinStatus == HIGH && m_current_puls_type != PULS_TRIGGERED_HIGH)
         {
            m_current_puls_type = PULS_TRIGGERED_HIGH;

            DBIF_LOG_DEBUG_0("HIGH_PULS: PULS START");
            DBIF_LOG_DEBUG_1("WIDTH_START = %i",m_current_puls_width);
         }
         else if(m_current_edge.m_edge == EDGE_RISING && pinStatus == HIGH && m_current_puls_type == PULS_TRIGGERED_HIGH)
         {
            DBIF_LOG_DEBUG_0("STATE SHOULD NEVER HAPPEN");
            
         }
         else
         {
            /*After a timeout, this case will be called... The actual puls settings needs to be reseted.*/
            reset();
            DBIF_LOG_DEBUG_0("HIGH_PULS: Reset");
            DBIF_LOG_DEBUG_1("Error Count = %i",m_error_count);
         }
      }
      else if (m_configured_puls_type == PULS_LOW)
      {
         if(m_last_edge.m_edge == EDGE_FALLING && m_current_edge.m_edge == EDGE_RISING)
         {
            m_current_puls_type = PULS_LOW;
            pulsDetected = true;

            DBIF_LOG_DEBUG_0("LOW_PULS: PULS DETECTED");
         }
         else if(m_current_edge.m_edge == EDGE_FALLING && pinStatus == LOW && m_current_puls_type != PULS_TRIGGERED_LOW)
         {
            m_current_puls_type = PULS_TRIGGERED_LOW;
            
            DBIF_LOG_DEBUG_0("LOW_PULS: PULS START");
            DBIF_LOG_DEBUG_1("WIDTH_START = %i",m_current_puls_width);
         }
         else if(m_current_edge.m_edge == EDGE_FALLING && pinStatus == LOW && m_current_puls_type == PULS_TRIGGERED_LOW)
         {
               DBIF_LOG_DEBUG_0("STATE SHOULD NEVER HAPPEN");
         }
         else
         {
            /*After a timeout, this case will be called... The actual puls settings needs to be reseted.*/
            reset();
            DBIF_LOG_DEBUG_0("LOW_PULS: Reset");
            DBIF_LOG_DEBUG_1("Error Count = %i",m_error_count);
         }
      }
      else
      {
         if(m_last_edge.m_edge == EDGE_RISING && m_current_edge.m_edge == EDGE_FALLING)
         {
            m_current_puls_type = PULS_HIGH;
            pulsDetected = true;
         }
         else if(m_last_edge.m_edge == EDGE_FALLING && m_current_edge.m_edge == EDGE_RISING)
         {
            m_current_puls_type = PULS_LOW;
            pulsDetected = true;
         }
         else
         {
            /*After a timeout, this case will be called... The actual puls settings needs to be reseted.*/
            reset();
         }
      }

      if (pulsDetected == true)
      {
         m_current_puls_width = m_current_edge.m_time - m_last_edge.m_time;
         
         if(m_puls_debouncing_status == PULS_DEBOUNCING_ENABLED  && 
            m_current_puls_width <= m_puls_debounce_time)
         {
            m_current_puls_type = PULS_NO_PULS;
            m_current_puls_width = 0;
            
            DBIF_LOG_WARN("PULS WIDTH DETECTED IN DEBOUNCE TIME");
         }
         else
         {
            m_puls_count ++;
            DBIF_LOG_DEBUG_1("WIDTH =  (%i - %i) = %i ",m_current_edge.m_time,
                                                               m_last_edge.m_time,
                                                               m_current_puls_width);    
         }
         
         m_current_edge = {
                     .m_edge = EDGE_NO_EDGE,
                     .m_time = 0
                  };  
      }
      m_last_edge = m_current_edge;

      /* Puls Timeout handling start. Backup current mills to compare it with
       * timeout
       */
      if(m_puls_timeout_status == PULS_TIMEOUT_ENABLED)
      {
         timeoutTime = time();
      }
   }
   else
   {
      checkTimeout(timeoutTime);
   }
   return;
}


PulsType_e Puls::getPulsType(void)
{
   return m_current_puls_type;
}


int32_t Puls::getPulsWidth(void)
{
   int32_t width = 0;

   if((m_current_puls_type == PULS_TRIGGERED_HIGH) || 
      (m_current_puls_type == PULS_TRIGGERED_LOW))
   {
       width = time() - m_current_puls_width;
       return width;
   }
   else
   {
       return m_current_puls_width;
   }
}

void Puls::setTrigger(PulsType_e trigger)
{
   m_configured_puls_type = trigger;
   DBIF_LOG_INFO("Trigger Set = %i", trigger);
}


boolean Puls::isErrorOccurred(void)
{
   return m_error_count > 0 ? true : false;
}

uint32_t Puls::getErrorCount(void)
{
   return m_error_count;
}


void Puls::enablePulsTimeout(uint32_t timeout_time)
{
   if(timeout_time > 0)
   {
      m_puls_timeout = timeout_time;
      m_puls_timeout_status = PULS_TIMEOUT_ENABLED;
      DBIF_LOG_INFO("Timeout enbaled = %ims", timeout_time);
   }
   else
   {
      DBIF_LOG_WARN("Timeout invalid = %ims", timeout_time);
      disablePulsTimeout();
      
   }
  return;
}

void Puls::disablePulsTimeout(void)
{
   m_puls_timeout = 0;
   m_puls_timeout_status = PULS_TIMEOUT_DISABLED;
   DBIF_LOG_INFO("Timeout disabled");
   return;
}

PulsType_e Puls::getTrigger(void)
{
   return m_configured_puls_type;
}

uint32_t Puls::getPulsCount(void)
{
    return m_puls_count;
}

void Puls::enableDebounce(uint32_t debounce_time)
{
   if(debounce_time > 0)
   {
      m_puls_debounce_time = debounce_time;
      m_puls_debouncing_status = PULS_DEBOUNCING_ENABLED;
      DBIF_LOG_INFO("Debouncing enbaled = %ims", debounce_time);
   }
   else
      disableDebounce();
      DBIF_LOG_WARN("Debouncing invalid = %ims", debounce_time);

   return;
}

void Puls::disableDebounce(void)
{
   m_puls_debounce_time = 0;
   m_puls_debouncing_status = PULS_DEBOUNCING_DISABLED;
}

void Puls::pulsResolution(PulsResolution_e new_resolution)
{  
   Edge::resolution(static_cast<EdgeResolution_e>(new_resolution));
   m_puls_resolution = new_resolution;
}

PulsResolution_e Puls::pulsResolution(void)
{
   return m_puls_resolution;
}

uint32_t  Puls::time(void)
 {
    if(m_puls_resolution == PULS_RESOLUTION_US)
      return micros();
    else 
      return millis();
 }

 void Puls::checkTimeout(uint32_t timeout_time)
 {
   /*Do timeout check only when the first edge was detected*/
   if (m_current_puls_type == PULS_TRIGGERED_HIGH ||
       m_current_puls_type == PULS_TRIGGERED_LOW)
   {
      if(m_puls_timeout_status == PULS_TIMEOUT_ENABLED)
      {
         if((time() - timeout_time) > (uint32_t)m_puls_timeout)
         {
            m_current_puls_type = PULS_TIMEOUT;
            m_current_puls_width = PULS_WIDTH_TIME_INVALID;
            m_error_count++;

            m_last_edge = {
                        .m_edge = EDGE_NO_EDGE,
                        .m_time = 0
                     };

            DBIF_LOG_WARN("Puls Timeout detected - Error %i", m_error_count);
         }
      }
   }
}


void Puls::reset(void)
{
   m_current_puls_type = PULS_NO_PULS;
   m_current_puls_width = 0;
}