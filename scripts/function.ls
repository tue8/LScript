function func(a, b, callback)
{
  return callback("Hello " + a + " and " + b);
}

var myPrint = function(msg)
{
  print msg;
};

func("people", "stuff", myPrint);
myPrint("Hello");
