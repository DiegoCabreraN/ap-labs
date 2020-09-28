int infof(const char *format, ...);
int warnf(const char *format, ...);
int errorf(const char *format, ...);
int panicf(const char *format, ...);

int main() {
    
    infof("Hello World %d\n", 12);
    warnf("Hello World %d\n", 2);
    errorf("Hello World %d\n", 3);
    panicf("Hello World %d\n", 4);
    return 0;
}
