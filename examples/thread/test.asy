import "libthread" as m;
size(4cm);
pair[] pts=m.createRandomPoints(200);
pen[] pens=m.createRandomPens(50);
if(pts.length != 200 || pens.length != 50)
  abort("thread example: wrong array length");
for(int i=0; i < 25; ++i)
  dot(pts[i], blue);
