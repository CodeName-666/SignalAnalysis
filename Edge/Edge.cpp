/**
 * @file Edge.cpp
 * @brief Implementation file for the Edge class for edge detection functionality.
 * 
 * This file contains the method implementations for the Edge class, providing
 * functionality to detect rising and falling edges on digital inputs.
 * 
 * @author AP02
 * @date 17.12.2017
 */

#include "Edge.h"
#include "Edge_DebugIf.h"  // For debugging macros

/**
 * @brief Constructor for the Edge class.
 * @param io_status The initial level of the digital input/output.
 * @param resolution The resolution of the edge occurrence time (microseconds or milliseconds).
 */
Edge::Edge(uint8_t io_status, EdgeResolution_e res) : m_current_io(io_status)
{
   m_edge_info.m_edge = EDGE_NO_EDGE;
   m_edge_info.m_time = 0u;
   resolution(res);
}

Edge::~Edge()
{
   // Destructor logic if needed
}


void Edge::checkEdges(void)
{
   static uint8_t old_io = EDGE_LOW;

   if (old_io == EDGE_LOW && m_current_io == EDGE_HIGH)
   {
      m_edge_info.m_edge = EDGE_RISING;
      m_edge_info.m_time = time();
   
#if EDGE_ENABLE_EVENT_OUTPUT == EDGE_USE_EVENTS
      EVENT_EMIT(m_on_rising_edge_signal, *this);
#elif EDGE_ENABLE_EVENT_OUTPUT == EDGE_USE_CALLBACKS
      if (m_on_rising_edge_callback) {
         m_on_rising_edge_callback(this);
      }
#endif

      DBIF_LOG_DEBUG_0("... Rising Edge Detected ...");
      DBIF_LOG_DEBUG_1("... At Time = %i ...", m_edge_info.m_time);
   }
   else if (old_io == EDGE_HIGH && m_current_io == EDGE_LOW)
   {
      m_edge_info.m_edge = EDGE_FALLING;
      m_edge_info.m_time = time();
      
#if EDGE_ENABLE_EVENT_OUTPUT == EDGE_USE_EVENTS
      EVENT_EMIT(m_on_rising_edge_signal, *this);

#elif EDGE_ENABLE_EVENT_OUTPUT == EDGE_USE_CALLBACKS
      if (m_on_falling_edge_callback) {
         m_on_falling_edge_callback(this);
      }
#endif

      DBIF_LOG_DEBUG_0("... Falling Edge Detected ...");
      DBIF_LOG_DEBUG_1("... At Time = %i ...", m_edge_info.m_time);
   }
   else
   {
      m_edge_info.m_edge = EDGE_NO_EDGE;
      m_edge_info.m_time = 0;
   }

   old_io = m_current_io;
   return;
}

void Edge::setStatus(uint8_t io_status)
{
   m_current_io = io_status;
}


EdgeInfo_t Edge::operator=(EdgeInfo_t edge)
{
   m_edge_info.m_edge = edge.m_edge;
   m_edge_info.m_time = edge.m_time;
   return m_edge_info;
}


EdgeInfo_t Edge::info(void)
{
   return m_edge_info;
}


EdgeType_e Edge::getCurrentEdge(void) const
{
   return m_edge_info.m_edge;
}


uint32_t Edge::getCurrentEdgeTime(void) const
{
   return m_edge_info.m_time;
}


void Edge::resolution(EdgeResolution_e new_resolution)
{
   m_resolution = new_resolution;
   DBIF_LOG_DEBUG_0("New Resolution = %i", new_resolution);
}


EdgeResolution_e Edge::resolution(void)
{
   return m_resolution;
}


uint32_t Edge::time(void)
{
   if (m_resolution == EDGE_RESOLUTION_US)
      return micros();
   else
      return millis();
}