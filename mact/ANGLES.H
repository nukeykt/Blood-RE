#define FINEANGLES 2048

#define DEG(x)    ((((FINEANGLES*FIXED1/360)*(x))+(FIXED1/2))/FIXED1)

#define PI 3.14159

#define COS(x)  (costable[(x) & (FINEANGLES-1)])
#define SIN(x)  (sintable[(x) & (FINEANGLES-1)])
