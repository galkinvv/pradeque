typedef struct
{
	void* element;
	int id;
} PradequeHead;

PradequeHead myarray = { &myarray, 2 };
int main()
{
	return (int)myarray.element;
}
