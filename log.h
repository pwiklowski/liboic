#ifndef LOG_H
#define LOG_H

#define log(line) fprintf(stderr, "%s:%d [%s]: %s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__, line)


#endif // LOG_H
