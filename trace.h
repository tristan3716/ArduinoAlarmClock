/* ===========================================
 *    Trace
 *      for Debug message
 *    2019.12.14 v1 - by Hu
 */

#if !defined (_TRACE_H)
#define _TRACE_H

#if defined (_DEBUG)
#define traceln(fmt, ...) _traceln(fmt, ##__VA_ARGS__)
#define trace(fmt, ...) _trace(fmt, ##__VA_ARGS__)
#define traceBegin() Serial.begin(9600)
#else
#define traceln(fmt, ...)
#define trace(fmt, ...)
#define traceBegin()
#endif

#if defined (_DEBUG)
template <typename T, typename ...Ts>
void _traceln(T x){
    Serial.println(x);
}
template <typename T, typename ...Ts>
void _traceln(T x, Ts ...xs){
    Serial.print(x);
    traceln(xs...);
}
template <typename T, typename ...Ts>
void _trace(T x){
    Serial.print(x);
}
template <typename T, typename ...Ts>
void _trace(T x, Ts ...xs){
    Serial.print(x);
    trace(xs...);
}
#endif

void cerror(const char *msg){
    traceln("!! critical error : ", msg);
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
