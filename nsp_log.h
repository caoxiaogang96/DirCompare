#ifndef NSP_LOG_H
#define NSP_LOG_H

#ifdef __cplusplus
extern "C" {
#endif

//四种日志模式
//#define NSP_LOG_DEBUG
//#define NSP_LOG_FILE    //文件形式
//#define NSP_LOG_TERM    //终端形式
//#define NSP_NO_LOG      //无日志
#define NSP_LOG_ERR		//只有错误信息

#define NSP_LOG_FILE_PATH   "HexCompare_Log.log"

__attribute((visibility("hidden"))) int nsp_debug_init();

#ifdef NSP_LOG_DEBUG
__attribute((visibility("hidden"))) int nsp_log(const char *func_name, int line, const char *fmt, ...);
#else
#define nsp_log(...)
#endif

#ifdef NSP_LOG_DEBUG
__attribute((visibility("hidden"))) int nsp_print(char *info, unsigned char *data, int data_len);
#else
#define nsp_print(...)
#endif

#ifdef NSP_LOG_ERR
__attribute((visibility("hidden"))) int nsp_err(const char *func_name, int line, const char *err_info, int err_code);
#else
#define nsp_err(...)
#endif

#ifdef __cplusplus
}
#endif

#endif  //NSP_LOG_H
