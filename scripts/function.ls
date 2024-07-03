function func(a, b)
{
  function closureFunc()
  {
    return "Hello " + a + " and " + b;
  }
  return closureFunc();
}

print func("people", "stuff");