/* ===========================================
 *    Trace
 *      for Debug message
 *    2019.12.14 v1 - by Hu
 */

#if !defined (_TRACE_H)
#define _TRACE_H

#if defined (_DEBUG)
#define traceln(fmt, ...) _loggerln(fmt, ##__VA_ARGS__)
#define trace(fmt, ...) _logger(fmt, ##__VA_ARGS__)
#define traceBegin() Serial.begin(9600)
#else
#define traceln(fmt, ...)
#define trace(fmt, ...)
#define traceBegin()
#endif

#if defined (_DEBUG)
template <typename T, typename ...Ts>
void _loggerln(T x){
    Serial.println(x);
}
template <typename T, typename ...Ts>
void _loggerln(T x, Ts ...xs){
    Serial.print(x);
    traceln(xs...);
}
template <typename T, typename ...Ts>
void _logger(T x){
    Serial.print(x);
}
template <typename T, typename ...Ts>
void _logger(T x, Ts ...xs){
    Serial.print(x);
    trace(xs...);
}
#else
template <typename T, typename ...Ts>
void _loggerln(T x){
    Serial.println(x);
}
template <typename T, typename ...Ts>
void _loggerln(T x, Ts ...xs){
    Serial.print(x);
    traceln(xs...);
}
#endif
class ReleaseLog {
public:
    void init() {
        Serial.begin(9600);
    }
    template <typename T>
    void log(T x){
        _loggerln(x);
    }
};
extern ReleaseLog release;
ReleaseLog release;

void cerror(const char *msg){
    _loggerln("!! critical error : ", msg);
    delay(1000);
    noInterrupts();
    while(1);
}

#if defined(_DEBUG)
#define IsDebug() true
#else
#define IsDebug() false
#endif

#endif //! _TRACE_H
