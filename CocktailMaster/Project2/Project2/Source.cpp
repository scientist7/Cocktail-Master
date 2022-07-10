/*
   hello.c - CGI program for the web
*/

#include <stdio.h>

int main()
{
	printf("content-type: text/html\n\n");

	printf("<html>\n<body>\n");
	printf("<h1> Hello, World! </h1>\n");
	printf("</body>\n</html>\n");

	return 0;
}