#include <iostream>
#include <cstdio>

int main() {
	FILE* source  = fopen("const_IRs.txt", "r");
	char buff[50];
	fgets(buff, 4, source);
	std::cout << buff << std::endl;
	fgets(buff, 4, source);
	std::cout << buff << std::endl;
	return 0;
}
