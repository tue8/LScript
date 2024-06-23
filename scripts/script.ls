var a = 100;
var msg;

while (a > 0)
{
  if (a > 50)
  {
    msg = "Greater";
  }
  else
  {
    msg = "Not greater";
  }
  print msg + " than 50: " + a;
  a = a - 1; 
}
