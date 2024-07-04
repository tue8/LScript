function func(a, b, callback)
{
  return callback("Hello " + a + " and " + b);
}

func("people", "stuff", function(msg)
{
  print msg;
});