#include<Windows.h>
#include<iostream>
#include<thread>
#include<chrono>
using std::cin;
using std::cout;
using std::endl;;
using namespace std::chrono_literals;

bool finish = false;

VOID Function()
{
	while (!finish)
	{
		cout << "Hello Threads from " << GetCurrentThreadId() << endl;
	}
}

struct Point
{
	Point(int x, int y):x(x),y(y) {}
	int x;
	int y;
};
VOID Collision(Point* point)
{
	while (point->x != point->y)
	{
		cout << "X = " << point->x++ << " \tY =" << point->y-- << endl;
		//Sleep(5)
	}
}
VOID Decrement(int* i)
{
	while (i) cout << i-- << "\t";
}

void Plus()
{
	while (!finish)
	{
		cout << "+ ";
		std::this_thread::sleep_for(100ms);
	}
}
void Minus()
{
	while (!finish)
	{
		cout << "- ";
		std::this_thread::sleep_for(100ms);
	}
}

//#define WINDOWS_THREADS_1
//#define WINDOWS_THREADS_2

void main()
{
	setlocale(LC_ALL, "");

#ifdef WINDOWS_THREADS_1
	DWORD dwID = 0;
	HANDLE hThread = CreateThread
	(
		NULL,
		0,
		(LPTHREAD_START_ROUTINE)Function,
		NULL,
		NULL,
		&dwID
	);
	cin.get();
	finish = true;
	cout << "Thread ID from main(): " << dwID << endl;

	WaitForSingleObject(hThread, INFINITE);
#endif // !WINDOWS_THREADS_1

#ifdef WINDOWS_THREADS_2
Point A( 0,10000 );
	int i = 10000;
	DWORD dwTreadID = 0;
	HANDLE hThread = CreateThread
	(
		NULL,
		NULL,
		(LPTHREAD_START_ROUTINE)Decrement,
		(LPVOID)i,
		NULL,
		&dwTreadID
	);
	WaitForSingleObject(hThread, INFINITE);
#endif // WINDOWS_THREADS_2

	
	//Plus();
	//Minus();

	std::thread plus_thread = std::thread(Plus);
	std::thread minus_thread = std::thread(Minus);

	cout << "Start" << endl;
	cin.get();
	finish = true;
	cout << "Finish" << endl;

	if (plus_thread.joinable())plus_thread.join();
	if (minus_thread.joinable())minus_thread.join();

	/*while (true)
	{
		cout << std::this_thread::get_id() << "\t";
		std::this_thread::sleep_for(100ms);
		//Sleep(100);
	}*/
}