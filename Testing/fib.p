program main( input, output );
  var n: integer;
  function fib(n:integer):integer;
  begin
	if (n <= 1) then
    begin
        fib := 1
    end
    else
    begin
        fib := fib(n-1) + fib(n-2)
    end
  end;
begin
    read(n);
    while (n > 0) do 
    begin
        write(fib(n));
        n := n-1
    end
end.