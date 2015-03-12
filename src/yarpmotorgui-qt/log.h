#ifndef LOG_H
#define LOG_H

#ifdef QT_DEBUG
#define LOG(...) qDebug(__VA_ARGS__)
#else
#define LOG(...) fprintf(stdout,__VA_ARGS__)
#endif

#ifdef QT_DEBUG
#define LOG_ERROR(...) qDebug(__VA_ARGS__)
#else
#define LOG_ERROR(...) fprintf(stderr,__VA_ARGS__)
#endif

#endif // LOG_H
