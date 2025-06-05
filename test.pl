
func main()
{	
	a=3*4+5;
	b=3;

	g = sum(1, 2, sum(3, 4));

	print(g);

	a= -3 + add(2, add(1, 3 * 4));
	print(a, b, a);
}

func add(d,e)
{
	c=d+e;
	return c;
}
