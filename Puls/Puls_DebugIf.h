#ifndef FILE_DEBUG_IF_H
#define FILE_DEBUG_IF_H
#ifdef DBIF_MODULE_NAME
#undef DBIF_MODULE_NAME
#endif


#if defined (DEBUG)
/*--- Required DBIF Flags ------------------------------------------------*/
      
  #define DBIF_ENABLED

  #define DBIF_LOGLEVEL                             LOG_DEBUG_0

  #define DBIF_MODULE_NAME                          "Puls"



/**
 * @brief Init Macro to initialze the debug interface
 * 
 */
//#define DBIF_INIT(args...)                      Serial.begin(args)
    
    
/**
  * @brief Print function interface definition for the whole debug interface
  * This flag is the main debug fuction definition. This means, this flag 
  * combines DBIF_STREAM and DBIF_PRINT to
  *          DBIF_STREAM.DBIF_PRINT
  * If a other print function is required, this flag can be used.
  */
  //#define DBIF_PRINTF_FUN                          Serial.printf
  
  /**
  * @brief Timestamp defintion function
  * This flag can be used to modifie the timestamp funciton, which will be written 
  * each time the DBIF_LOG... function is called.
  */
  //#define DBIF_TIMESTAMP_FUN                        millis()

  /**
  * @brief Debug function output enable flag
  * Flag to ouput the function name where debug fucntion was called.
  */
  //#define DBIF_FUNCTION_OUTPUT_ENABLE   

    /**
   * @brief  Debug Macro ouput on change
   * This flag enables the debug output on change functionality
   */
  //#define DBIF_ON_CHANGE_ENABLE

  #include "Debug_If.h"
#endif
#endif