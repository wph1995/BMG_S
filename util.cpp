//基本的函数 -- 常用的
#include "util.h"
/******************************************************/

/**************************************
** 判断字符串是否为数字
*************************************/
int isnumber(const char *data, int length, int sign)
{
    for (int i = 0; i < length; i++)
    {
        if (data[i] == '\0')
        {
            break;
        }
        if (data[i] < 48 || data[i] > 57)
        {
            if (sign && i == 0)
            {
                if (data[i] == '+' || data[i] == '-')
                {
                    continue;
                }
                else
                {
                    return 0;
                }
            }
            else
            {
                return 0;
            }
        }//不为数字
    }//end for 循环处理

    return 1;
}

/**************************************
** 对字符串执行相加操作
*************************************/
char * add_string(char *data, int length, int value_)
{
    //处理返回标志
    int iret = -1;

    //传入的数据不为0
    if (data == 0 || strlen(data) <= 0 || value_ < 0)
    {
        return 0;
    }

    if (value_ == 0)
    {
        return data;
    }

    //判断是否为数字
    if (!isnumber(data, length))
    {
        return 0;
    }

    //定义存放需要相加的数值
    char temp[10 + 1];
    memset(temp, 0x00, sizeof(temp));
    sprintf(temp, "%d", value_);

    //定义存放操作结果的内存区
    char *buffer = new char[length];
    memset(buffer, 0x00, length);

    //定义传入的字符的长度
    int data_len = (int)strlen(data);

    //定义数字字符串相加时进位标志
    char stepflag = 0;

    //定义循环处理变量
    int i = data_len - 1;           //循环扫描原始需要操作的数据
    int j = (int)strlen(temp) - 1;  //需要增加的值
    int k = 0;                      //写入到临时缓冲的位置变量
    for (; ; i--, j--, k++)
    {
        if (i < 0 && j < 0)
        {
            //已经处理完成
            break;
        }//end if

        if (k >= length - 1)
        {
            //已经越界
            iret = -1;
            goto __EXIT;
        }//end if

        if (i < 0)
        {
            buffer[k] = temp[j] + stepflag;
        }
        else if (j < 0)
        {
            buffer[k] = data[i] + stepflag;
        }
        else
        {
            buffer[k] = data[i] + temp[j] + stepflag - 48;
        }

        //判断是否需要进位
        if (buffer[k] > 57)
        {
            buffer[k] -= 10;
            stepflag = 1;
        }
        else
        {
            stepflag = 0;
        }
    }//end for

    //对于存在进位的处理方式
    if (stepflag)
    {
        if (k < length - 1)
        {
            buffer[k] = 49;
            k++;
        }
        else
        {
            iret = -1;
            goto __EXIT;
        }
    }//end if 针对存在进位标志的处理

    //将字符串进行反转
    for (k -= 1, i = 0; k >= 0; k--, i++)
    {
        data[i] = buffer[k];
    }
    data[i] = '\0';

    iret = 0;

__EXIT:
    if (buffer)
    {
        delete[] buffer;
    }

    if (0 != iret)
    {
        return 0;
    }

    return data;
}

/**************************************
** 获取指定环境变量
*************************************/
char * get_env(const char * envname)
{
    char * env = 0;

    if (envname[0] == '\0')
        goto __Exit;

    env = getenv( envname );

__Exit:
    return env;
}

/**************************************
** 去掉左边的控制字符，包括空格,\n,\t
*************************************/
char* strtrimleft( char** ppStr )
{
    char* c = *ppStr;

    while( c && *c )
    {
        if ( *c != '\t' && *c != '\n' && *c != ' ' )
        {
            break;
        }
        c++;
    }

    *ppStr = c;
    return c;
}

#define StrTrimLeft  strtrimleft

/**************************************
** 去掉字符串两边的空格
**************************************/
char* strtrim( char * str )
{
    size_t len = strlen( str );
    if ( !len )
    {
        *str = 0;
        return str;
    }

    char * p1 = str;
    char * p2 = str+len-1;

    //先去掉末尾的空格
    while( p2 >= p1 && *p2 )
    {
        if ( *p2 == ' ' )
        {
            *p2-- = 0;
        }
        else
        {
            break;
        }
    }

    //再去掉左边的空格
    while( p1 <= p2 && *p1 )
    {
        if ( *p1 == ' ' )
        {
            char * s1 = p1;
            char * s2 = p1+1;
            while( *s2 )
            {
                *s1++ = *s2++;
            }
            *s1 = 0;
        }
        else
        {
            break;
        }
        p1 = str;
    }
    return str;
}
/**************************************
** 去掉左边的控制字符，包括空格,\r,\n,\t,\v,\f,\b
*************************************/
int StringTrimLeft(string &str)
{
    int nFRet = -1;
    size_t i = 0;
    size_t iSize = str.size();
    char c;

    if (str.empty())
    {
        goto __Exit;
    }

    for (i = 0; i < iSize; i++)
    {
        c = str[i];
        if (c != '\t' && c != '\n' && c != '\r' && c != '\b' && c != '\f' && c != '\v' && c != ' ')
        {
            break;
        }
    }

    if (i == iSize)
    {
        str = "";
    }
    else
    {
        str = str.substr(i, iSize - i);
    }

    nFRet = 0;

__Exit:
    return nFRet;
}

/**************************************
** 去掉右边的控制字符
**************************************/
int StringTrimRight(string &str)
{
    int nFRet = -1;
    size_t i = 0;
    bool bFind = false;
    char c;

    if (str.empty())
    {
        goto __Exit;
    }

    for (i = str.size() - 1; i >= 0; i--)
    {
        c = str[i];
        if (c != '\t' && c != '\n' && c != '\r' && c != '\b' && c != '\f' && c != '\v' && c != ' ')
        {
            bFind = true;
            break;
        }

        if (i == 0)
        {
            bFind = false;
            break;
        }
    }

    if (!bFind)
    {
        str = "";
    }
    else
    {
        str = str.substr(0, i + 1);
    }

    nFRet = 0;

__Exit:
    return nFRet;
}

/**************************************
** 去掉两边的控制字符
**************************************/
int StringTrim(string &str)
{
    int nFRet = -1;

    nFRet = StringTrimLeft(str);

    if (nFRet)
        goto __Exit;

    nFRet = StringTrimRight(str);

__Exit:
    return nFRet;
}

/**************************************
** 转换字符串为大、小写
**************************************/
void StringLower( string& str )
{
    for (std::string::iterator i = str.begin(); i != str.end(); i++)
        if (*i >= 'A' && *i <= 'Z')
            *i = (*i) + 32;

    return ;
}

void StringUpper( string& str )
{
 for ( std::string::iterator i = str.begin(); i != str.end(); i++)
    if (*i >= 'a' && *i <= 'z')
       *i = (*i) -32;
 return ;
}

/************************************************************************

   函数功能 : 将指定的字符串替换
   参数说明 :
              str : 原始字符串
              sstr  : 需要查找的字符串
              dstr  : 替代的字符串
   返回值   :
              -1 : 表示没有找到
              非负数: 表示找到相应位置
************************************************************************/
void StringReplace( string &str, const char* sstr,  const char* dstr )
{
    string::size_type pos = 0;
    string::size_type srclen = strlen( sstr );
    string::size_type dstlen = strlen( dstr );
    while( (pos = str.find( sstr, pos)) != string::npos )
    {
        str.replace(pos, srclen, dstr);
        pos += dstlen;
    }
}
void StringReplace(string & strBig, const string & strsrc, const string &strdst)
{
    string::size_type pos = 0;
    string::size_type srclen = strsrc.size();
    string::size_type dstlen = strdst.size();
    while( (pos = strBig.find(strsrc, pos)) != string::npos )
    {
        strBig.replace(pos, srclen, strdst);
        pos += dstlen;
    }
}

/************************************************************************

   函数功能 : 返回字符串中第一个ch字符的位置
   参数说明 :
              str : 原始字符串
              ch  : 需要查找的字符
   返回值   :
              -1 : 表示没有找到
              整数: 表示找到相应位置(这里下标是从0开始)
************************************************************************/
int StringPos( string &str, char ch )
{
    size_t pos = str.find( ch );
    return (int)pos;
}

int StringPos( string &str, const char* chs )
{
    size_t pos = str.find( chs );
    return (int)pos;
}
/************************************************************************

   函数功能 : 删除一段字符串
   参数说明 :
              str : 原始字符串
              pos: 起始位置,从0--(长度-1)
              len: 删除的长度
   返回值   : 空
************************************************************************/
void StringDelete( string& str, int pos, int len )
{
    str.erase( pos, len );
}
/**************************************
** 转换字节序 【大】字节学 <-->【小】字节序
**************************************/
unsigned int convU4( unsigned int ui )
{
    return ( ( ( ui & 0xFF ) << 24) | ( ( ui & 0xFF00 )<< 8 ) | ( ( ui & 0xFF0000 ) >> 8 ) | ( ( ui & 0xFF000000 ) >> 24 ) );
}

unsigned short convU2( unsigned short us )
{
    return ( ( us & 0xFF ) << 8) | ( ( us & 0xFF00 ) >> 8 );
}


////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
void SplitString(const char *s, char Delimiter,  vector<string> &Text)
{
    int i, j;

    Text.clear();

    if( Delimiter == '\n' && s[0] == '\0' )
        return;

    for( j = 0, i = 0; ; )
    {
        if( s[i] == '\0' )
        {
            Text.push_back(string(s + j, i - j));
            break;
        }
        if( s[i] == Delimiter )
        {
            Text.push_back(string(s + j, i - j));
            i++;
            j = i;
            continue;
        }
        i++;
    }
}
////////////////////////////////////////////////////////////
int ReadLine(FILE *f, char *s, int MaxLen)
{
    int i, c;

    for( i = 0; i < MaxLen; )
    {
        c = fgetc(f);
        if( c == '\t' )
            c = ' ';
        if( c == EOF || c == '\n' )
            break;

#if !defined(_WIN32)
        if( c != '\r' )  // 过滤Linux下的\r
            s[i++] = c;
#else
        s[i++] = c;
#endif

    }

    s[i] = '\0';

    if( i == MaxLen )
    {
        for(;;)
        {
            c = fgetc(f);
            if( c == EOF || c == '\n' )
                break;
        }
    }

    if( c == EOF && i == 0 )
        return -1;
    else
        return i;
}
////////////////////////////////////////////////////////////
int ReadWord(const char *s, char *w, const char *separators,
             int MaxLen, bool &IsSeparator)
{
    int i, j;

    i = 0;

    if( strchr(separators, ' ') )
    {
        for(; s[i] == ' '; i++)
            ;
    }

    j = 0;

    if( s[i] == '\0' )
    {
        w[0] = '\0';
        IsSeparator = false;
        return 0;
    }

    if( strchr(separators, s[i]) )
    {
        w[j++] = s[i++];
        IsSeparator = true;
    }
    else
    {
        for(; j < MaxLen && s[i] != '\0' && !strchr(separators, s[i]);)
        {
            if( s[i] == '\\' )
            {
                i++;
                if( s[i] != '\0' )
                    w[j++] = s[i++];
            }
            else
                w[j++] = s[i++];
        }
        IsSeparator = false;
    }

    w[j] = '\0';
    return i;
}
////////////////////////////////////////////////////////////
void SplitPath(const string &Path, string &DirName, string &BaseName, string &Postfix)
{
    size_t Pos;

    Pos = Path.rfind("/");

    if( Pos == string::npos )
    {
        DirName = ".";
        BaseName = Path;
    }
    else
    {
        DirName = Path.substr(0, Pos);
        BaseName = Path.substr(Pos + 1);
    }

    Pos = BaseName.rfind(".");

    if( Pos == string::npos )
        Postfix = "";
    else
    {
        Postfix = BaseName.substr(Pos + 1);
        BaseName = BaseName.substr(0, Pos);
    }
}
////////////////////////////////////////////////////////////
void RightTrim(string &Str)
{
    size_t Pos;

    Pos = Str.find_last_not_of(" ");

    if( Pos == string::npos )
        Str.resize(0);
    else
        Str.resize(Pos + 1);
}
////////////////////////////////////////////////////////////
bool IsValidVarName(const char *Name, bool CheckCppKeywords)
{
    static const char *CppKeywords[] =
    {
        "auto", "bool", "false", "true", "break",
        "case", "catch", "char", "class", "const",
        "continue", "default", "do", "double", "enum",
        "explicit", "extern", "float", "for", "friend",
        "goto", "if", "else", "inline", "int",
        "long", "mutable", "operator", "private", "protected",
        "public", "register", "return", "short", "signed",
        "sizeof", "static", "struct", "switch", "this",
        "throw", "try", "typedef", "typename", "typeid",
        "union", "unsigned", "virtual", "void", "volatile",
        "while", "wchar_t", "template", "new", "delete",
        "finally", "using", "namespace", "static_cast", "dynamic_cast",
        "reinterpret_cast", "const_cast",
        NULL
    };

    if( Name[0] != '_' && !isalpha(Name[0]) )
        return false;

    for( const char *s = Name; *s != '\0'; s++ )
    {
        if( *s != '_' && !isalnum(*s) )
            return false;
    }

    if( CheckCppKeywords )
    {
        for( int i = 0; CppKeywords[i] != NULL; i++ )
        {
            if( strcmp(CppKeywords[i], Name) == 0 )
                return false;
        }
    }

    return true;
}
////////////////////////////////////////////////////////////
unsigned int Hash(const void *Buf, int Len)
{
    int i;
    const unsigned char *s = static_cast<const unsigned char *>(Buf);
    unsigned int Result;

    for( Result = 0, i = 0; i < Len; i++, s++ )
        Result = Result * 5 + *s + 1;

    return Result;
}
/*
////////////////////////////////////////////////////////////
void HexToAsc(char *AscBuf, const void *HexBuf, int HexLen)
{
    int c;
    const unsigned char *p;
    int i;

    for( p = static_cast<const unsigned char*>(HexBuf), i = 0;
        i < HexLen; i++, p++ )
    {
        c = *p / 16;
        if( c >= 0 && c <= 9 )
            *AscBuf++ = c + '0';
        else
            *AscBuf++ = c - 10 + 'A';
        c = *p % 16;
        if( c >= 0 && c <= 9 )
            *AscBuf++ = c + '0';
        else
            *AscBuf++ = c - 10 + 'A';
    }
}
////////////////////////////////////////////////////////////
void AscToHex(void *HexBuf, const char *AscBuf, int HexLen)
{
    int c;
    char *p;
    int i;

    for( p = static_cast<char*>(HexBuf), i = 0; i < HexLen; i++, p++ )
    {
        c = *AscBuf++;
        if( c == ' ' )
            c = '0';
        if( c >= '0' && c <= '9' )
            *p = (c - '0') * 16;
        else if( c >= 'a' && c <= 'z' )
            *p = (c - 'a' + 10) * 16;
        else
            *p = (c - 'A' + 10) * 16;
        c = *AscBuf++;
        if( c == ' ' )
            c = '0';
        if( c >= '0' && c <= '9' )
            *p += c - '0';
        else if( c >= 'a' && c <= 'z' )
            *p += c - 'a' + 10;
        else
            *p += c - 'A' + 10;
    }
}*/
////////////////////////////////////////////////////////////
void FillChar(char *s, char c, int len)
{
    int i;

    for( i = 0; *s != '\0' && i < len; s++, i++ )
        ;

    for( ; i < len; i++ )
        *s++ = c;

    *s = '\0';
}
/**************************************
** 从末尾找最前一个空格，返回所在的位置
*************************************/
int GetFirstSpacebar(const char* buffer, int num)
{
    const char * tp = buffer+num-1;
    int tnum = num;
    while( tnum >0 )
    {
        if ( *tp != 32 ) //不等于空格
        {
            break;
        }
        tp--;
        tnum--;
    }
    return tnum;
}
/**************************************
** 获取文件的大小
*************************************/
long GetFileSize(const char* filename)
{
    FILE * fp = fopen( filename, "r" );

    if ( !fp )
    {
        return -1;
    }

    fseek( fp, 0, SEEK_END );
    long pos = ftell( fp );
    fclose( fp );

    return pos;
}

/**************************************
** 拷贝文件
*************************************/
int CopyFile( const char* srcfile, const char* destfile )
{
    int nFRet = -1;
    FILE  *  pfsrc = fopen( srcfile, "rb" );
    FILE  * pfdes = fopen( destfile, "wb" );
    long srclen = 0;
    const int NUM = 2048;

    if ( !pfsrc )
    {
        goto __Exit;
    }
    if ( !pfdes )
    {
        goto __Exit;
    }

    fseek( pfsrc, 0, SEEK_END );
    srclen = ftell( pfsrc );
    rewind( pfsrc );
    {
        int cycle = srclen / NUM;
        int last = srclen % NUM;
        char buf[2050] = "";
        for( int i = 0; i < cycle; i++ )
        {
            fread( buf, NUM, 1, pfsrc );
            fwrite( buf, NUM, 1, pfdes );
        }
        fread( buf, last, 1, pfsrc );
        fwrite( buf, last, 1, pfdes );
    }
    fflush( pfdes );

    nFRet = 0;
__Exit:
    if ( pfsrc ) fclose( pfsrc );
    if ( pfdes ) fclose( pfdes );

    return nFRet;
}

/**************************************
** 从已经打开的文件做复制指定长度的数据
*************************************/
int CopyFileByLen( FILE * fdin, FILE * fdout, int len)
{
    int nFRet = -1;
    //long srclen = 0;
    const int NUM = 2048;

    if ( !fdin || !fdout )
    {

        goto __Exit;
    }
    {
        int cycle = len / NUM;
        int last = len % NUM;
        char buf[2048] = "";
        for( int i = 0; i < cycle; i++ )
        {
            memset(buf, 0, sizeof(buf));
            fread( buf, NUM, 1, fdin );
            fwrite( buf, NUM, 1, fdout );
        }
        memset(buf, 0, sizeof(buf));
        fread( buf, last, 1, fdin );
        fwrite( buf, last, 1, fdout );
    }

    nFRet = 0;
__Exit:
    return nFRet;
}
/**************************************
** 获取当前时间
*************************************/
const char* GetCurrentDate( DATE_FMT fmt )
{
    static const char* FMT1 = "%4d%02d%02d";
    static const char* FMT2 = "%02d%02d%02d";
    static const char* FMT3 = "%4d%02d%02d_%02d%02d%02d";
    static const char* FMT4 = "%4d-%02d-%02d %02d:%02d:%02d";

    time_t  now_t = time( NULL );
    struct  tm  *pTm = localtime( &now_t );
    static char _str[ 19 + 2 ] = "";

    //输出日期类型为YYYY-MM-DD HH:MI:SS
    const char * pfmt = NULL;
    switch( fmt )
    {
        case E_FMT_DAY:
            pfmt = FMT1;
            break;
        case E_FMT_TIME:
            pfmt = FMT2;
            break;
        case E_FMT_DATE1:
            pfmt = FMT3;
            break;

        case E_FMT_DATE2:
        default:
            pfmt = FMT4;
            break;
    }

    if ( E_FMT_TIME != fmt )
    {
        snprintf( _str, sizeof(_str)-1, pfmt,
            pTm->tm_year + 1900, pTm->tm_mon + 1, pTm->tm_mday,
            pTm->tm_hour, pTm->tm_min, pTm->tm_sec);
    }
    else
    {
        snprintf( _str, sizeof(_str)-1, pfmt,
            pTm->tm_hour, pTm->tm_min, pTm->tm_sec);
    }

    return( _str );
}


/**********************************************************
 * Function: 获取当前系统的时间，精确到秒
 * Input:     ioSec :  保存当前时间的数字格式
                 logfmt_time : 日志文件中的日期时间字符串,
                                     格 式"YYYYMMDD_HHmmSS"
 * Output:
 * Return: 当前时间的字符串
**********************************************************/
const char* getdatetime( long * ioSec, char* logfmt_time )
{
    static const int DATETIME_LEN = 20;
    time_t m_time;
    struct tm *pNow = NULL;
    static char tmpDate[DATETIME_LEN] = "";

    m_time = time( NULL );
    if ( NULL != ioSec )
    {
        *ioSec = (long)m_time;
    }
    pNow = localtime( &m_time );
    memset( tmpDate, 0x00, DATETIME_LEN );
    sprintf( tmpDate, "%4d-%02d-%02d %02d:%02d:%02d",
            pNow->tm_year + 1900, pNow->tm_mon + 1, pNow->tm_mday,
            pNow->tm_hour, pNow->tm_min, pNow->tm_sec );
    if ( NULL != logfmt_time )
    {
        sprintf( logfmt_time, "%4d%02d%02d_%02d%02d%02d",
            pNow->tm_year + 1900, pNow->tm_mon + 1, pNow->tm_mday,
            pNow->tm_hour, pNow->tm_min, pNow->tm_sec );
    }
    return tmpDate;
}
/**********************************************************
 * Function: 获取当前系统的时间，精确到秒
 * Input:     ioSec 保存当前时间的数字格式
 * Output:
 * Return: 当前时间的字符串
**********************************************************/
const char* gettime( void )
{
    static const int TIME_LEN = 10;
    time_t m_time;
    struct tm *pNow = NULL;
    static char tmpDate[TIME_LEN] = "";

    m_time = time( NULL );
    pNow = localtime( &m_time );
    memset( tmpDate, 0x00, TIME_LEN );
    sprintf( tmpDate, "%02d:%02d:%02d",  pNow->tm_hour, pNow->tm_min, pNow->tm_sec);
    return tmpDate;
}

/**********************************************************/
//打印测试执行的代码行数
void _Test_(  int n_line, char* c_file, char* c_date, char* c_time )
{
    printf( " <<<<<<<<<< TEST >>>>>>>>>>[ %s %s ] Line: %d, File: %s  \n", c_date, c_time, n_line, c_file );
    return;
}
/***********************************************************
 *   函数功能：获取程序文件上面的第几层路径
 *   参数：
          dir   : 返回的路径
          layer : 程序文件的上几层
 *   返回值:
          true - success, false - failed
***********************************************************/
bool GetProgramDir( string& dir, int layer )
{
    static string s_EXE_PATH;


    //当前系统是windows系列
#ifdef WIN32
    char szFileName[512] = "";
    char *pTemp = NULL;
    if ( s_EXE_PATH.empty() )
    {
	    GetModuleFileName( NULL , szFileName , 511 );

	    size_t len = strlen( szFileName );
        pTemp = szFileName;

        //把" \  "转换成 "/"
        for( size_t i = 0; i < len; i++, pTemp++ )
        {
            if ( '\\' == *pTemp )
            {
                *pTemp = '/';
            }
        }
        s_EXE_PATH = szFileName;
    }
    else
    {
        strncpy( szFileName, s_EXE_PATH.c_str(), s_EXE_PATH.length() );
    }

    //按照层次来进行处理
    for( int j = 0; j < layer; j++ )
    {
	    pTemp = strrchr( szFileName, '/' );
	    if ( NULL != pTemp )
	    {
		    *pTemp = 0;
	    }
    }

    dir = szFileName;
    return true;

    //当前系统是其它系统
#else

    return false;

#endif

}


/**********************************************************
* Function: 初始化文件锁
* Input:  filename 文件名
* Output:
* Return: true, false
**********************************************************/
bool InitLockFile( const char* filename )
{
    bool bFRet = false;
    int  fh = 0;

    // 先判断文件是否存在，不存在则创建

    // 创建新文件
#ifdef _WIN32
    fh = _sopen(  filename, _O_RDWR|_O_CREAT, _SH_DENYNO, _S_IREAD | _S_IWRITE );
#else
    fh = open(  filename, O_WRONLY | O_CREAT | O_APPEND,
                S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH );
#endif
    if( fh == -1 )
    {
        return false;
    }
    //写入初始时间
    char buf[128] = "";
    sprintf( buf, "%s initialized", GetCurrentDate( E_FMT_DATE2 ) );
    write( fh, buf, (int)strlen(buf) );
    close( fh );

    bFRet = true;
    return bFRet;
}
/**********************************************************
* Function: 获取文件锁
* Input:  filename 文件名
* Output:
* Return: true, false
**********************************************************/
bool LockFile( const char* filename, int& filehandler )
{
    filehandler = 0;
    bool bFRet = false;
    int  fh = 0;

#ifdef _WIN32
    fh = _sopen(  filename, _O_RDWR, _SH_DENYNO, _S_IREAD | _S_IWRITE );
#else
    fh = open(  filename, O_WRONLY,
                S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH );
#endif
    if( fh == -1 )
    {
        
        return false;
    }
    //上锁
    bFRet = GetWriteLock( fh );

    filehandler = fh;
    return bFRet;
}

/**********************************************************
* Function: 释放文件锁
* Input:  filename 文件名
* Output:
* Return: true, false
**********************************************************/
bool UnlockFile(  int filehandler )
{
    bool bFRet = false;
    int  fh = filehandler;
    bFRet = ReleaseLock( fh );
    close( fh );
    fh = 0;
    return bFRet;
}

/**********************************************************

跨平台函数

**********************************************************/

#ifdef _WIN32

/**********************************************************
* Function:     获取当前程序的绝对全路径
* description:
* Input:
* Output:
* Return:  成功则返回路径，否则为空
**********************************************************/
const char * get_exe_name( void )
{
    const int PATH_MAX = 1024;
    static char   buf[PATH_MAX] = "";
    if ( 0 == GetModuleFileName( NULL, buf, PATH_MAX ) )
    {
        return NULL;
    }
    else
    {
        return buf;
    }
}
/**********************************************************
* Function:     扫描可匹配的文件
* description:  matchname，匹配的关键字
*                   filelist  文件列表
*                   maxnum: 最多扫描文件个数，默认为0表示不限制
* Input:
* Output:
* Return:  -1 扫描失败, 0 没有文件, >0 文件数量
**********************************************************/
int scan_files( const char* matchname, vector<string>& filelist )
{
    const int PATH_MAX = 1024;
    char szFind[PATH_MAX] = "";
    char szFile[PATH_MAX] = "";
    WIN32_FIND_DATA FindFileData;

    strncpy(szFind, matchname, PATH_MAX );
    filelist.clear();

    HANDLE hFind=::FindFirstFile( szFind,&FindFileData );

    if( INVALID_HANDLE_VALUE == hFind )
    {
        return 0;
    }

    while( true )
    {
        if( FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
        {
            //directory
        }
        else
        {
            filelist.push_back( FindFileData.cFileName );
        }

        if( !FindNextFile( hFind, &FindFileData) )
        {
            break;
        }
    }

    FindClose(hFind);
    return (int)filelist.size();

}

int SeeLock(int fd, int start, int len)
{
    int nlen = len;
    if ( nlen == -1 )
    {
       nlen = filelength( fd );
    }
    long ret = lseek( fd, start, SEEK_SET );
    if( _locking( fd, _LK_NBLCK, nlen ) != -1 )
    {
        return 0;
    }
    else
    {
        return -1;
    }
}
bool GetReadLock( int fd, int start, int len )
{
    int nlen = len;
    if ( nlen == -1 )
    {
       nlen = filelength( fd );
    }
    long ret = lseek( fd, start, SEEK_SET );
    if( _locking( fd, _LK_LOCK, nlen ) != -1 )
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool GetWriteLock( int fd, int start, int len )
{
    int nlen = len;
    if ( nlen == -1 )
    {
       nlen = filelength( fd );
    }
    long ret = lseek( fd, start, SEEK_SET );
    if( _locking( fd, _LK_RLCK, nlen ) != -1 )
    {
        return true;
    }
    else
    {
        return false;
    }
}
bool ReleaseLock( int fd, int start, int len )
{
    int nlen = len;
    if ( nlen == -1 )
    {
       nlen = filelength( fd );
    }
    long ret = lseek( fd, start, SEEK_SET );
    if( _locking( fd, LK_UNLCK, nlen ) != -1 )
    {
        return true;
    }
    else
    {
        return false;
    }
}

//目录是否存在的检查：
bool IsFolderExist(const char* strPath)
{
    WIN32_FIND_DATA   wfd;
    bool rValue = false;
    HANDLE hFind = FindFirstFile(strPath, &wfd);
    if ((hFind != INVALID_HANDLE_VALUE) && (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
    {
        rValue = true;   
    }
    FindClose(hFind);
    return rValue;
}
// 文件是否存在
bool IsFileExist(const char *pFile)
{
    bool bRet = false;
    FILE* fp = fopen(pFile, "r");
    if (fp)
    {
        bRet = true;
        fclose(fp);
    }
    return bRet;
}

//创建目录：
bool CreateFolder(const char* strPath)
{
    SECURITY_ATTRIBUTES attrib;
    attrib.bInheritHandle = FALSE;
    attrib.lpSecurityDescriptor = NULL;
    attrib.nLength =sizeof(SECURITY_ATTRIBUTES);
    //上面定义的属性可以省略。 直接return ::CreateDirectory( path, NULL); 即可
    return ( ::CreateDirectory( strPath, &attrib) == TRUE);
} 


////////////////////////////////////////////////////////////
#else   /*******    [ Unix/Linux ]  (Not Windows)   ******/
////////////////////////////////////////////////////////////
/**********************************************************
* Function:  线程暂停 MSec 毫秒
* Input:      MSec  需要暂停的毫秒数
* Output:
* Return:
**********************************************************/
void Sleep( unsigned int MSec )
{
	/*
    pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;
    pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

    struct timeval now;
    struct timespec timeout;
    int retcode = 0;

    pthread_mutex_init( &mut, NULL );
    pthread_mutex_lock( &mut );
    gettimeofday( &now, NULL );
    timeout.tv_sec = now.tv_sec;
    //timeout.tv_nsec = now.tv_usec * 1000 + ( MSec * 1000000 );
    timeout.tv_nsec = now.tv_usec * 1000 * MSec ;

    retcode = pthread_cond_timedwait(&cond, &mut, &timeout);
    pthread_mutex_unlock(&mut);
	*/
	struct timeval tv;
	tv.tv_sec=0;
	tv.tv_usec = MSec*1000;
	select(0, NULL, NULL, NULL, &tv);
}
/**********************************************************
* Function:     获取当前程序的绝对全路径
* description:
* Input:
* Output:
* Return:  成功则返回路径，否则为空
**********************************************************/
const char * get_exe_name( void )
{
    static char   buf[1024] = "";
    int   rslt   =   readlink("/proc/self/exe",   buf,   1024 );
    if   (   rslt   <   0   ||   rslt   >=  1024  )
    {
        return   NULL;
    }
    else
    {
        buf[rslt]   =   '\0';
        return   buf;
    }
}

/**********************************************************
* Function:     扫描可匹配的文件
* description:  matchname，匹配的关键字
*                   filelist  文件列表
*                   maxnum: 最多扫描文件个数，默认为0表示不限制
* Input:
* Output:
* Return:  -1 扫描失败, 0 没有文件, >0 文件数量
**********************************************************/
int scan_files( const char* matchname, vector<string>& filelist )
{
    int nFRet = -1;
    const int NAME_LEN = 256;
    char filename[NAME_LEN];
    glob_t globbuf;

    nFRet = glob( matchname, 0, NULL, &globbuf );

    if ( GLOB_NOMATCH == nFRet )
    {
        nFRet = 0;
        goto __Exit;
    }
    else if ( GLOB_ABORTED == nFRet )
    {
        nFRet = -1;
        goto __Exit;
    }
    else if ( GLOB_NOSPACE == nFRet )
    {
        nFRet = -1;
        goto __Exit;
    }

    filelist.clear();
    for ( size_t i = 0; i < globbuf.gl_pathc; i++)
    {
        memset( filename, 0x00, NAME_LEN );
        memcpy( filename, basename(globbuf.gl_pathv[i]), strlen( globbuf.gl_pathv[i] ) );

        filelist.push_back( filename );
    }
    nFRet = (int)filelist.size();

    globfree( &globbuf );

__Exit:
    return nFRet;
}
//查看文件的锁的状态
int SeeLock(int fd, int start, int len )
{
    int nlen = len;
    if ( nlen == -1 )
    {
        nlen = 0;
        struct stat statbuf;
        if ( -1 != fstat( fd, &statbuf ) )
        {
            nlen = statbuf.st_size;
        }
    }

    struct flock arg;
    arg.l_type = F_WRLCK;
    arg.l_whence = SEEK_SET;
    arg.l_start = start;
    arg.l_len = nlen;

    return arg.l_type;
}

bool GetReadLock( int fd, int start, int len )
{
    int nlen = len;
    if ( nlen == -1 )
    {
        nlen = 0;
        struct stat statbuf;
        if ( -1 != fstat( fd, &statbuf ) )
        {
            nlen = statbuf.st_size;
        }
    }
   struct flock arg;
   arg.l_type = F_RDLCK;
   arg.l_whence = SEEK_SET;
   arg.l_start = start;
   arg.l_len = nlen;

   if ( fcntl( fd, F_SETLKW, &arg) == -1 )
   {
         return false;
    }
   else
   {
         return true;
    }
}

bool GetWriteLock( int fd, int start, int len )
{
    int nlen = len;
    if ( nlen == -1 )
    {
        nlen = 0;
        struct stat statbuf;
        if ( -1 != fstat( fd, &statbuf ) )
        {
            nlen = statbuf.st_size;
        }
    }
   struct flock arg;
   arg.l_type = F_WRLCK;
   arg.l_whence = SEEK_SET;
   arg.l_start = start;
   arg.l_len = nlen;

   if ( fcntl( fd, F_SETLKW, &arg ) == -1 )
   {
         return false;
    }
   else
   {
         return true;
    }
}

bool ReleaseLock( int fd, int start, int len )
{
    int nlen = len;
    if ( nlen == -1 )
    {
        nlen = 0;
        struct stat statbuf;
        if ( -1 != fstat( fd, &statbuf ) )
        {
            nlen = statbuf.st_size;
        }
    }
   struct flock arg;
   arg.l_type = F_UNLCK;
   arg.l_whence = SEEK_SET;
   arg.l_start = start;
   arg.l_len = nlen;

   if ( fcntl( fd, F_SETLKW, &arg ) == -1 )
   {
         return false;
    }
   else
    {
         return true;
    }
}

// 文件是否存在
bool IsFileExist(const char *pFile)
{
    bool bRet = false;
    FILE* fp = fopen(pFile, "r");
    if (fp)
    {
        bRet = true;
        fclose(fp);
    }
    return bRet;
}

// 文件夹是否存在
bool IsFolderExist(const char *pFolder)
{
    bool bRet = false;
    int ret = access(pFolder, R_OK | W_OK | X_OK);
    if (ret == 0)
        bRet = true;
    return bRet;
}

//创建目录：
bool CreateFolder(const char* strPath)
{
    mode_t mode = S_IRWXU | S_IRWXG;
    int ret = mkdir( strPath, mode );
    return ( ret == 0 ) ? true : false;
} 

//创建一个文件
bool CreateFile(const char  * strFile, off_t nSize)
{
	
	FILE * fp = fopen(strFile, "wb+");
	if (fp) {
		if (nSize > 0) ftruncate(fileno(fp), nSize);
		fclose(fp);
		return true;
	 }
	return false;
}
////////////////////////////////////////////////////////////
#endif  // not _WIN32
////////////////////////////////////////////////////////////


///下面的函数是JQD 另外加的 
int CopyFileNoOpen(FILE * pfsrc, FILE * pfdes)
{
    if (!pfsrc || !pfdes) return -1;
    const int NUM = 8*1024;
    long srclen = 0;
    char buf[NUM] = {0};
    
    srclen = fseek( pfsrc, 0, SEEK_END );
    srclen = ftell( pfsrc );
    rewind( pfsrc );

    int cycle = srclen / NUM;
    int last = srclen % NUM;
    int ret = 0;
    for( int i = 0; i < cycle; i++ )
    {
        ret = (int)fread( buf, NUM, 1, pfsrc );
        ret = (int)fwrite( buf, NUM, 1, pfdes );
    }
    ret = (int)fread( buf, last, 1, pfsrc );
    ret = (int)fwrite( buf, last, 1, pfdes );
    return 0;
}
//字符串大写
char* strupr( char* s )
{
	char* p = s;
	while ((*p = toupper(*p))) p++;
	return s;
}
//字符串小写
char* strlwr( char* s )
{
	char* p = s;
	while ((*p = tolower( *p ))) p++;
	return s;
}
/*
//转换为ASCII的16进制格式
//长度放大一倍
//upCase 表示输出的16进制数据是否大写
void Bin2Hex(unsigned char * pBin, int nBinLen, unsigned char * pHex, int nUpCase)
{
    int i=0;
    unsigned char * pHexCur = pHex;
    for (; i < nBinLen; i++)
    {
        unsigned char c = pBin[i];
        //printf("CurChar:%02x, ", c);
        *pHexCur = Val2Char((c & 0xF0)>>4, nUpCase);
        *(pHexCur + 1) = Val2Char(c & 0x0F, nUpCase);        
        //printf("Left:%c, Right:%c\n", *pHexCur, *(pHexCur + 1));
        pHexCur += 2;
    }
    *pHexCur = '\0';                    //字符串结尾
    return;
}
//ASCII 16进制字符格式转换为内存中的BIN格式
//长度缩小一倍
void Hex2Bin(unsigned char * pHex, int nHexLen, unsigned char * pBin)
{
    int nBinLen = nHexLen /2;
    unsigned char * pHexCur = pHex;
    int i = 0;
    //printf("BinOut:");
    for (; i < nBinLen; i++)
    {
        unsigned char cLeft = Char2Val(*pHexCur);
        unsigned char cRight = Char2Val(*(pHexCur+1));
        //printf("Line:%d, cL:%c, vL:%d, cR:%c, vR:%d, ", i, *pHexCur, cLeft, *(pHexCur+1), cRight);
        pBin[i] = cLeft*16 + cRight;
        //printf("%02x", pBin[i]);
        pHexCur += 2;        
    }
    //pBin[nBinLen] = '\0';
    return;
}
*/
/*
 * 获取一个十六进制字符所对应的数值
 */

