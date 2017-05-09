program main(input, output);
 var a,b: integer;
 function foo(x:integer):integer;
  function boo(y:integer):integer;
  begin
   boo := a*a + 2*a + 1
  end;
 begin
   foo := a*a
 end;
begin
 read(a);
 b := foo(11);
 write(b)
end.
