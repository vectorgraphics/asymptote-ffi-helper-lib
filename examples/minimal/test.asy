import libminimal;
size(5cm);
int r=doubleIt(21);
if(r != 42) abort("minimal plugin: expected 42, got "+string(r));
filldraw(unitsquare,pink,black+2pt);
label((0.5,0.5),"doubleIt(21)="+string(r));
