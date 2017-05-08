program main( input, output );
  var n: integer;
  function foo(n:integer):integer;
  begin
	foo := n
  end;
begin
    read(n);
    write(foo(n))
end.
