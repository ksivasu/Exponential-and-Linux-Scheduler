#define EXPDISTSCHED 1
#define LINUXSCHED 2

extern int scheduler_class;

void setschedclass(int class_value);
int getschedclass();
void preemption_required(int a, int b, int c);



















