int foo()
{
  return 5;
}
int main()
{

  int x = foo();
  x();
  return foo(1);
}
