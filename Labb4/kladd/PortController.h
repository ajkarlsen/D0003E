#ifndef PORTCONTROLL_H_
#define PORTCONTROLL_H_

typedef struct{
	Object super;	
	
}PortController;


void togglePin4();
void togglePin6();
void disablePin4();
void disablePin6();

#define initPortController(){initObject()}

#endif /* PORTCONTROLL_H_ */