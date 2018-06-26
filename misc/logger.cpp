#include "logger.h"

#include <cstring>
#include <stdarg.h>
#include <time.h>

using namespace std;

FILE* Logger::mOutput = NULL;

bool Logger::initialise(int verbose, const char* path)
{
    mVerbose = verbose;
    if (path == NULL)
        mOutput = stderr;
    else
        mOutput = fopen(path, "w");
    return (mOutput != NULL) ? true : false;
}

Logger::~Logger()
{
    /*TODO: 要报错，暂时关闭
      QThread: Destroyed while thread is still running
    if (mOutput != stderr)
        fclose(mOutput);
        */
}

void Logger::log(int verbose, const char* fmt, ...)
{
    if (mVerbose < verbose)
        return;
    va_list ap;
    va_start(ap, fmt);
    vfprintf(mOutput, fmt, ap);
    va_end(ap);
    fflush(mOutput);
}

void Logger::log(int verbose, string msg)
{
    if (mVerbose < verbose)
        return;
    fprintf(mOutput, "%s", msg.c_str());
}

void Logger::log(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vfprintf(mOutput, fmt, ap);
    va_end(ap);
    fflush(mOutput);
}

std::string Logger::getLevelName(int level)
{
    switch (level) {
    case None:
        return "None";
    case Error:
        return "Error";
    case Warn:
        return "Warn";
    case Notice:
        return "Notice";
    case Debug:
        return "Debug";
    case DDebug:
        return "DDebug";
    }
    return "OOPS";
}
