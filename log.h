#ifndef LOG_H
#define LOG_H

#define log(line, ...) fprintf(stderr, line, ## __VA_ARGS__ )


#endif // LOG_H
