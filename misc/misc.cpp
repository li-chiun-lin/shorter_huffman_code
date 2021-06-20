#include <iostream>

void gentable()
{
	printf("unsigned char table[256] = {\n\t");
	for (int i = 0; i < 256; ++i)
	{
		int t = 0;
		unsigned char mask = 0x80;

		while (mask && (mask & i) == 0)
		{
			++t;
			mask >>= 1;
		}

		printf("%d, ", t);
		if (i % 16 == 15)
			printf("\n\t");
	}
	printf("};\n");
}

const static unsigned char mask[8] = { 0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01 };

void genbits()
{
	printf("unsigned char byte2bits[256][8] = {\n");
	for (int byte = 0; byte < 256; ++byte)
	{
		printf("{");
		for (int b = 0; b < 8; ++b)
		{
			printf("%d%s ", (byte & mask[b]) ? 1 : 0, (b != 7) ? "," : "");
		}
		printf("}, ");
		if (byte % 4 == 3)
			printf("\n");
	}
	printf("}\n");
}

int main()
{
	gentable();

	genbits();
}

