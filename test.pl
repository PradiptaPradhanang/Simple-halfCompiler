
func compare(a,b)
{
	if(a>b)
		print(a);
	else if(a==3)
		print(b);
}
func sub(a, b)
	return a - b;

func fib(n)
{
	if (n < 2)
		return n;
	f = fib(n - 1) + fib(n - 2);
	return f;
}

func add(d,e)
{   
	if (e == 12)
		return d;

	if(d)
	{
		print("========== ", d);
	}

	c=d+e;
	return c;
}
func main()
{	
	print(fib(1));
	print(fib(2));
	print(fib(3));
	print(fib(4));
	print(fib(5));
	print(fib(6));

}
