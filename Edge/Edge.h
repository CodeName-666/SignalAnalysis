/**
 * @file Edge.h
 * @brief Header file for the Edge class for edge detection functionality.
 * 
 * The Edge class is designed to detect rising and falling edges on digital inputs.
 * It provides methods for setting input status, checking edges, and retrieving edge information.
 * The class also supports different time resolutions for edge occurrence.
 * 
 * @author AP02
 * @date 17.12.2017
 */

#ifndef _EDGE_H_
#define _EDGE_H_

#include <stdint.h>
#include "Event.h"

#define EDGE_USE_EVENTS 0
#define EDGE_USE_CALLBACKS 1


#define EDGE_ENABLE_EVENT_OUTPUT EDGE_USE_EVENTS


/**
 * @brief Macro definition for low edge status.
 */
#ifndef EDGE_LOW
#define EDGE_LOW 0x00u
#endif

/**
 * @brief Macro definition for high edge status.
 */
#ifndef EDGE_HIGH
#define EDGE_HIGH 0x01u  // Updated from 0x00u to 0x01u for consistency.
#endif

/**
 * @brief Enumeration for edge types.
 */
typedef enum
{
   EDGE_NO_EDGE = 0x00, /**< No edge detected */
   EDGE_RISING,         /**< Rising edge detected */
   EDGE_FALLING         /**< Falling edge detected */
} EdgeType_e;

/**
 * @brief Enumeration for edge resolution types.
 */
typedef enum
{
   EDGE_RESOLUTION_US = 0, /**< Microsecond resolution */
   EDGE_RESOLUTION_MS = 1  /**< Millisecond resolution */
} EdgeResolution_e;

/**
 * @brief Structure holding edge information.
 */
typedef struct
{
   EdgeType_e m_edge; /**< Type of the detected edge */
   uint32_t m_time;   /**< Time at which the edge was detected */
} EdgeInfo_t;

/**
 * @brief Class for detecting rising and falling edges on digital signals.
 */
class Edge
{
public:
   /**
    * @brief Constructor for the Edge class.
    * @param io_status The initial level of the digital input/output.
    * @param resolution The resolution of the edge occurrence time (microseconds or milliseconds).
    */
   Edge(uint8_t io_status = 0, EdgeResolution_e resolution = EDGE_RESOLUTION_MS);

   /**
    * @brief Destructor for the Edge class.
    */
   virtual ~Edge();

   /**
    * @brief Copy constructor for the Edge class.
    * @param edge The Edge object to be copied.
    */
   Edge(const Edge &) = default;

   /**
    * @brief Checks for edge occurrences based on current and previous input levels.
    */
   void checkEdges(void);

   /**
    * @brief Sets the status of the digital input/output.
    * @param io_status The level to be set (0 for low, 1 for high).
    */
   void setStatus(uint8_t io_status);

   /**
    * @brief Retrieves the edge information.
    * @return An EdgeInfo_t structure containing the type of edge and the time of occurrence.
    */
   EdgeInfo_t info(void);

   /**
    * @brief Gets the current edge type.
    * @return The current edge type (EdgeType_e).
    */
   EdgeType_e getCurrentEdge(void) const;

   /**
    * @brief Gets the occurrence time of the current edge.
    * @return The time in microseconds or milliseconds (uint32_t).
    */
   uint32_t getCurrentEdgeTime(void) const;

   /**
    * @brief Assignment operator for edge information.
    * @param edge The EdgeInfo_t structure to be assigned.
    * @return The assigned EdgeInfo_t structure.
    */
   EdgeInfo_t operator=(EdgeInfo_t edge);

   /**
    * @brief Sets the edge resolution.
    * @param new_resolution The new edge resolution (EdgeResolution_e).
    */
   void resolution(EdgeResolution_e new_resolution);

   /**
    * @brief Retrieves the current edge resolution.
    * @return The current edge resolution (EdgeResolution_e).
    */
   EdgeResolution_e resolution(void);

private:
   /**
    * @brief Retrieves the current time based on the configured resolution.
    * @return The current time in microseconds or milliseconds (uint32_t).
    */
   uint32_t time(void);

private:
   uint8_t m_current_io;           /**< Current I/O status. */
   EdgeInfo_t m_edge_info;         /**< Structure holding the edge information. */
   EdgeResolution_e m_resolution;  /**< Resolution of the edge detection. */
#if EDGE_ENABLE_EVENT_OUTPUT == EDGE_USE_CALLBACKS
   void (*m_on_rising_edge_callback)(EdgeDio *) = nullptr; /**< Callback for the rising edge event. */
   void (*m_on_falling_edge_callback)(EdgeDio *) = nullptr; /**< Callback for the falling edge event. */
#endif

public:
 #if EDGE_ENABLE_EVENT_OUTPUT == EDGE_USE_EVENTS
   EVENT_SIGNAL(on_rising_edge, EdgeDio); /**< Event signal for rising edge detection. */
   EVENT_SIGNAL(on_falling_edge, EdgeDio); /**< Event signal for falling edge detection. */
#else 
    /**
    * @brief Sets a callback for the rising edge event.
    * @param callback The callback function to be set.
    */
   void setRisingEdgeCallback(void (*callback)(EdgeDio *));

   /**
    * @brief Sets a callback for the falling edge event.
    * @param callback The callback function to be set.
    */
   void setFallingEdgeCallback(void (*callback)(EdgeDio *));

#endif
};

#endif /* _EDGE_H_ */
