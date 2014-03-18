int do_nothing(int i)
{
	return i++;
}

void main(void)
{
	int i = 9;
	int j = do_nothing(i);
	i = j << 10;
}