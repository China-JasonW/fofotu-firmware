/*
** EPITECH PROJECT, 2022
** MyLog
** File description:
** MyLog
*/

#include "MyLog.h"

// 日志初始化
/*
* 0 - LOG_LEVEL_SILENT     no output
* 1 - LOG_LEVEL_FATAL      fatal errors
* 2 - LOG_LEVEL_ERROR      all errors
* 3 - LOG_LEVEL_WARNING    errors, and warnings
* 4 - LOG_LEVEL_NOTICE     errors, warnings and notices
* 5 - LOG_LEVEL_TRACE      errors, warnings, notices & traces
* 6 - LOG_LEVEL_VERBOSE    all

begin(int level, Print* logOutput, bool showLevel)
Log.begin(LOG_LEVEL_ERROR, &Serial, true);

* %s	display as string (char*)
* %S    display as string from flash memory (__FlashStringHelper* or char[] PROGMEM)
* %c	display as single character
*  %C    display as single character or as hexadecimal value (prefixed by `0x`) if not a printable character
* %d	display as integer value
* %l	display as long value
* %u	display as unsigned long value
* %x	display as hexadecimal value
* %X	display as hexadecimal value prefixed by `0x` and leading zeros
* %b	display as binary number
* %B	display as binary number, prefixed by `0b`
* %t	display as boolean value "t" or "f"
* %T	display as boolean value "true" or "false"
* %D,%F display as double value
* %p    display a  printable object

Log.verboseln ("ip address   : %p", ipAddress);
https://forum.arduino.cc/t/printable-classes/438816

*/
void log_setup()
{

    // Log.setPrefix(printPrefix); // set prefix similar to NLog
    // Log.setSuffix(printSuffix); // set suffix
    Log.begin(LOG_LEVEL_VERBOSE, &Serial);
    // Log.setShowLevel(false);    // Do not show loglevel, we will do this in the prefix
}

// void printPrefix(Print* _logOutput, int logLevel) {
//     printTimestamp(_logOutput);
//     printLogLevel (_logOutput, logLevel);
// }

// void printTimestamp(Print* _logOutput) {

//   // Division constants
//   const unsigned long MSECS_PER_SEC       = 1000;
//   const unsigned long SECS_PER_MIN        = 60;
//   const unsigned long SECS_PER_HOUR       = 3600;
//   const unsigned long SECS_PER_DAY        = 86400;

//   // Total time
//   const unsigned long msecs               =  millis();
//   const unsigned long secs                =  msecs / MSECS_PER_SEC;

//   // Time in components
//   const unsigned long MilliSeconds        =  msecs % MSECS_PER_SEC;
//   const unsigned long Seconds             =  secs  % SECS_PER_MIN ;
//   const unsigned long Minutes             = (secs  / SECS_PER_MIN) % SECS_PER_MIN;
//   const unsigned long Hours               = (secs  % SECS_PER_DAY) / SECS_PER_HOUR;

//   // Time as string
//   char timestamp[20];
//   sprintf(timestamp, "%02d:%02d:%02d.%03d ", Hours, Minutes, Seconds, MilliSeconds);
//   _logOutput->print(timestamp);
// }

// void printLogLevel(Print* _logOutput, int logLevel) {
//     /// Show log description based on log level
//     switch (logLevel)
//     {
//         default:
//         case 0:_logOutput->print("SILENT " ); break;
//         case 1:_logOutput->print("FATAL "  ); break;
//         case 2:_logOutput->print("ERROR "  ); break;
//         case 3:_logOutput->print("WARNING "); break;
//         case 4:_logOutput->print("INFO "   ); break;
//         case 5:_logOutput->print("TRACE "  ); break;
//         case 6:_logOutput->print("VERBOSE "); break;
//     }
// }

// void printSuffix(Print* _logOutput, int logLevel) {
//   _logOutput->print("");
// }