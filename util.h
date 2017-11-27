/********************************************************************************
 模块名       : 公共模块
 实现功能     : 公共函数的声明
 作者         : voosin.com
 版本         :
 --------------------------------------------------------------------------------
 修改记录 :
 日 期        版本     修改人              修改内容
 YYYY/MM/DD   X.Y      <作者或修改者名>    <修改内容>

*******************************************************************************/
#ifndef __COMMON_H__
#define __COMMON_H__

#ifdef _WIN32
    #include <windows.h>
    #include <io.h>
    #include <process.h>
    #include <sys/types.h>
    #include <sys/stat.h>
    #include <sys/locking.h>
    #include <share.h>
    #include <fcntl.h>
    #include <stdlib.h>

#else
    #include <pthread.h>
    #include <unistd.h>
    //#include <asm/errno.h>
    #include <sys/types.h>
    #include <sys/stat.h>
    #include <unistd.h>    
    #include <sys/time.h>
    #include <errno.h>
    #include <glob.h>
    #include <libgen.h>
	#include <stdarg.h>
	#include <vector>
	#include <string>
	#include <map>
    #include <fcntl.h>
    #include <stdlib.h>
	#include <stdio.h>
	#include <string.h>

#endif

using namespace std;


/**************************************
** LOG相关宏定义
*************************************/
//函数名宏
#if defined(_WIN32)
#define _FUNC_NAME_   __FUNCTION__
#elif defined(LINUX)
#define _FUNC_NAME_   __FUNCTION__
#else
#define _FUNC_NAME_   __FUNC__
#endif

/**********************************************************
** 宏定义区
*********************************************************/
#if !defined(strcasecmp)
#if defined(_WIN32)
#define strcasecmp StrCmpI
#else
#define strcasecmp strcasecmp
#endif
#endif

#if !defined(SLEEP)
#define SLEEP(minitime) Sleep(minitime)
#endif

#ifndef MAXPATHLEN
#define MAXPATHLEN  1024
#endif



#define THREAD_T pthread_t   //线程标识
#define THFUNC_T void *      //线程函数返回值类型
#define TPARAM_T void *      //线程函数参数

#define INT64 "ll"
typedef  long long __int64;

#define FPD '/'
#define FPDs "/"


#ifdef _WIN32
typedef unsigned (WINAPI *PBEGINTHREADEX_THREADFUNC)(
    LPVOID lpThreadParameter);
typedef unsigned *PBEGINTHREADEX_THREADID;
#endif
/******************************************************/

//获取程序的绝对全路径
extern const char * get_exe_name( void );

//扫描可匹配的文件
extern int scan_files( const char* matchname, vector<string>& filelist );

/**************************************
 ** 判断字符串是否为数字
 ** 输入: data  : 需要检查的字符串
 **       length: 传入的字符串长度
 **       sign  : 区别符号标志
 **          0  : 不区别正负号
 **          1  : 区别正负号
 ** 输出: NA
 ** 返回: 1  : 该字符串为数字
 **       0  : 该字符串不为数字
 *************************************/
extern int isnumber(const char *data, int length, int sign = 0);

/**************************************
 ** 对字符串执行相加操作
 ** 输入: data  : 需要操作的字符串
 **       length: 传入的字符串长度
 **       value_: 字符串需要相加的数值
 ** 输出: data  : 相加后的结果
 ** 返回: 相加后的结果指针
 *************************************/
extern char * add_string(char *data, int length, int value_);

/**************************************
** 获取指定环境变量
** 输入: envname  : 环境变量名
** 输出: 环境变量值
** 返回:  环境变量值 或者 NULL
*************************************/
extern char * get_env(const char * envname);

/**************************************
** 去掉左边的控制字符，包括空格,\r,\n,\t,\v,\f,\b
** 输入: str : 要操作的字符串
** 输出: str : 操作后的字符串
** 返回: 0   : 成功
**       -1  : 失败
*************************************/
extern int StringTrimLeft(string &str);

/**************************************
** 去掉右边的控制字符
** 输入: str : 要操作的字符串
** 输出: str : 操作后的字符串
** 返回: 0   : 成功
**       -1  : 失败
*************************************/
extern int StringTrimRight(string &str);

/**************************************
** 去掉两边的控制字符
** 输入: str : 要操作的字符串
** 输出: str : 操作后的字符串
** 返回: 0   : 成功
**       -1  : 失败
*************************************/
extern int StringTrim(string &str);
extern char* strtrim( char * str );
extern char* strtrimleft( char** ppStr );

// 转换字符串为大写
extern void StringUpper( string &str );
extern void StringLower( string& str );
//寻找字符串中第一个ch字符的位置
extern int StringPos( string &str, char ch );
extern int StringPos( string &str, const char* chs );

//删除一段字符串
extern void StringDelete( string& str, int pos, int len );
extern void StringReplace( string &str, const char* sstr,  const char* dstr );
extern void StringReplace(string & strBig, const string & strsrc, const string &strdst);

extern unsigned int   convU4( unsigned int ui );
extern unsigned short convU2( unsigned short us );

#ifndef _CONV_BYTE_ORDER
    #define CONVERT_U4(ui) (ui)
    #define CONVERT_U2(us) (us)
#else
    #define CONVERT_U4(ui) convU4(ui)
    #define CONVERT_U2(us) convU2(us)
#endif

// 将字符串s按照分隔符Delimiter拆成字符串数组
extern void SplitString(const char *s, char Delimiter,  vector<string> &Text);

// 从文件f中读入一行到s，到文件尾返回-1，否则返回s的长度
extern int ReadLine(FILE *f, char *s, int MaxLen);

// 从字符串s中获得子串，如果separators含有空格则忽略开头空格，返回
// 子串的下一个字符的位置
extern int ReadWord(const char *s, char *w, const char *separators,  int MaxLen, bool &IsSeparator);

// 将路径分解为目录、文件和后缀

extern void SplitPath(const string &Path, string &DirName, string &BaseName, string &Postfix);

// 去除字符串尾部空格
extern void RightTrim(string &Str);

// 判断是否是合法的变量名
extern bool IsValidVarName(const char *Name, bool CheckCppKeywords = true);

// 计算hash值
extern unsigned int Hash(const void *Buf, int Len);

// 将长度为 HexLen 的内存区域 HexBuf 的内容用 16 进制表示，
// 结果放在 AscBuf 中，每个字节变成两个字节，如 0xfe 变成
// 'f' 和 'e' 两个字符，因此 AscBuf 的长度至少要 HexLen * 2
//extern void HexToAsc(char *AscBuf, const void *HexBuf, int HexLen);

// 将长度为 HexLen * 2 的 16 进制字符串 AscBuf 放在内存区域 HexBuf
// 中，每两个字节变成一个字节，如 'f' 和 'e' 两个字符 变成 0xfe，
// AscBuf 中可以用大写字母也可以用小写字母，HexBuf 的长度至少要 HexLen
//extern void AscToHex(void *HexBuf, const char *AscBuf, int HexLen);

// 如果字符串 s 长度小于 len，将其用字符 c 补够长度 len
// 如果 s 长度大于 len，将其截断
// 总之，函数执行完毕，s 的长度一定变为 len
extern void FillChar(char *s, char c, int len);

extern int GetFirstSpacebar(const char* buffer, int num);

//获取文件大小
extern long GetFileSize(const char* filename);
//拷贝文件
extern int CopyFile( const char* srcfile, const char* destfile );
extern int CopyFileByLen( FILE * fdin, FILE * fdout, int len);

//文件锁操作
extern int SeeLock(int fd, int start=0, int len=-1);
extern bool GetReadLock( int fd, int start=0, int len=-1);
extern bool GetWriteLock( int fd, int start=0, int len=-1 );
extern bool ReleaseLock( int fd, int start=0, int len=-1 );

//删除DDL-SQL 语句中多余的空格和末尾的分号
extern void trim_ddl_sql( const char* pSSql, string & strDSql );

//去掉DDL-SQL语句中的首部注释
extern int trim_firstnote_sql( const char* pSSql, string & strDSql );

//获取当前时间
enum DATE_FMT
{
    E_FMT_DAY,        // "%4d%02d%02d";
    E_FMT_TIME,       // "%02d%02d%02d";
    E_FMT_DATE1,   // "%4d%02d%02d_%02d%02d%02d";
    E_FMT_DATE2    // "%4d-%02d-%02d %02d:%02d:%02d";
};
extern const char* GetCurrentDate( DATE_FMT fmt  = E_FMT_DATE1  );

/* 获取当前系统的日期时间，精确到秒
    ioSec :  保存当前时间的数字格式
    logfmt_time : 日志文件中的日期时间字符串,
                        格 式"YYYYMMDD_HHmmSS"
*/
extern const char* getdatetime( long * ioSec = NULL,char* logfmt_time = NULL );
//获取当前系统的时间，单位: 秒
extern const char* gettime( void );

//打印测试执行的代码行数
extern void _Test_( int n_line = __LINE__, char* c_file = __FILE__,
                char* c_date = __DATE__, char* c_time = __TIME__ );

#define TEST()  _Test_( __LINE__, __FILE__ );

/***********************************************************
 *   函数功能：获取程序文件上面的第几层路径
 *   参数：
          dir   : 返回的路径
          layer : 程序文件的上几层
 *   返回值:
          true - success, false - failed
***********************************************************/
extern bool GetProgramDir( string& dir, int layer = 0 );


extern bool InitLockFile( const char* filename );
extern bool LockFile( const char* filename, int& filehandler );
extern bool UnlockFile( int filehandler );

extern bool IsFolderExist(const char* strPath);
extern bool IsFileExist(const char* strFileName);
extern bool CreateFolder(const char* strPath);
//
extern bool CreateFile(const char  * strFile, off_t nSize = 0);
//下面的函数是JQD 加的
//extern long filesize(FILE *stream);
extern int CopyFileNoOpen(FILE * pfsrc, FILE * pfdes);

extern void Sleep( unsigned int MSec );

extern char* strupr( char* s );

extern char* strlwr( char* s );

//12-10  jqd added to process license info
extern void Hex2Bin(unsigned char * pHex, int nHexLen, unsigned char * pBin);
extern void Bin2Hex(unsigned char * pBin, int nBinLen, unsigned char * pHex, int nUpCase);
extern unsigned char Val2Char(unsigned char cIn, int nUpCase);
extern unsigned char Char2Val(unsigned char cIn);

#endif

