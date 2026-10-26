#ifndef __PCEM_LOGGING_INTERNAL_H__
#define __PCEM_LOGGING_INTERNAL_H__

#ifdef __cplusplus
extern "C" {
#endif

//#define printf pclog
extern void pclog(const char *format, ...);
extern void error(const char *format, ...);
extern void fatal(const char *format, ...);
extern void warning(const char *format, ...);

// Used to flush the log contents to disk.
void pclog_flush();
// Used to close the file handle and flush the last contents to disk. Only use it when closing the application
void pclog_end();

#ifdef __cplusplus
}
#endif

#endif /* __PCEM_LOGGING_INTERNAL_H__ */
