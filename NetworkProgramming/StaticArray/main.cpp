#include<iostream>
using namespace std;

#define MTU	5

void main()
{
	char recv_buffer[MTU] = {};
	cout << &recv_buffer;
}