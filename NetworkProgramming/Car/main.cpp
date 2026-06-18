#include<iostream>
#include<conio.h>
#include<thread>   // Подключено для работы с потоками (std::thread)
#include<chrono>   // Подключено для работы со временем (std::chrono)
#include<iomanip>  // Подключено для красивого форматирования вывода цифр (std::setprecision)

using std::cin;
using std::cout;
using std::endl;

// Константы клавиш клавиатуры
#define Escape	27
#define Enter	13

// Настройки емкости топливного бака
#define MIN_TANK_CAPACITY	20
#define MAX_TANK_CAPACITY	120

// Класс Топливного Бака (Tank)
class Tank
{
	const int CAPACITY;
	double fuel_level;
public:
	Tank(int capacity) : CAPACITY
	(
		capacity < MIN_TANK_CAPACITY ? MIN_TANK_CAPACITY :
		capacity > MAX_TANK_CAPACITY ? MAX_TANK_CAPACITY :
		capacity
	)
	{
		// По условию задания Ката, машина всегда появляется с 20.0 литрами топлива
		this->fuel_level = 20.0;
		cout << "Бак готов. Адрес памяти: " << this << endl;
	}
	~Tank()
	{
		cout << "Бак уничтожен. Адрес памяти: " << this << endl;
	}
	double get_fuel_level() const
	{
		return fuel_level;
	}
	void Fill(int amount)
	{
		if (amount < 0) return;
		fuel_level += amount;
		if (fuel_level > CAPACITY) fuel_level = CAPACITY;
	}
	// Метод, который уменьшает количество топлива в баке при работе двигателя
	double give_fuel(double amount)
	{
		if (amount < 0) return fuel_level;
		fuel_level -= amount;
		if (fuel_level < 0) fuel_level = 0;
		return fuel_level;
	}

	void info() const
	{
		cout << "Вместимость бака:\t" << CAPACITY << " литров.\n";
		cout << "Текущий уровень топлива:\t" << fuel_level << " литров.\n";
	}
};

// Настройки расхода двигателя
#define MIN_ENGINE_CONSUMPTION	4
#define MAX_ENGINE_CONSUMPTION	30

// Класс Двигателя (Engine)
class Engine
{
	const double CONSUMPTION;
	double consumption_per_second;
public:
	Engine(double consumption) : CONSUMPTION
	(
		consumption < MIN_ENGINE_CONSUMPTION ? MIN_ENGINE_CONSUMPTION :
		consumption > MAX_ENGINE_CONSUMPTION ? MAX_ENGINE_CONSUMPTION :
		consumption
	)
	{
		// Мы поставили расход 0.5 литра в секунду, чтобы в консоли было сразу видно, как уменьшается бензин!
		consumption_per_second = 0.5;
		cout << "Двигатель готов. Адрес памяти:\t" << this << endl;
	}
	~Engine()
	{
		cout << "Двигатель уничтожен. Адрес памяти:\t" << this << endl;
	}

	// Геттер, чтобы класс Car мог узнать секундный расход двигателя
	double get_consumption_per_second() const
	{
		return consumption_per_second;
	}

	void info() const
	{
		cout << "Расход топлива:\t\t" << CONSUMPTION << " л/км.\n";
		cout << "Расход в секунду:\t" << consumption_per_second << " л/сек.\n";
	}
};

// Класс Автомобиля (Car)
class Car
{
	Engine engine;
	Tank tank;
	bool driver_inside;
	bool is_running;          // Состояние: заведен ли двигатель (true/false)
	bool finish_threads;     // Флаг для безопасной остановки потока при закрытии программы
	std::thread fuel_thread; // Поток, который в фоновом режиме уменьшает бензин

public:
	Car(double consumtion, int capacity = 50) : engine(consumtion), tank(capacity)
	{
		driver_inside = false;
		is_running = false;       // По заданию, изначально двигатель НЕ заведен
		finish_threads = false;

		// Запускаем параллельный поток для симуляции расхода бензина
		fuel_thread = std::thread(&Car::engine_life, this);

		cout << "Ваша машина готова к поездке, нажмите Enter, чтобы сесть в нее. Адрес: " << this << endl;
	}

	~Car()
	{
		// Сигнализируем фоновому потоку, что нужно остановиться, и ждем его завершения
		finish_threads = true;
		if (fuel_thread.joinable())
		{
			fuel_thread.join();
		}
		cout << "Машина уничтожена. Адрес: " << this << endl;
	}

	void get_in()
	{
		driver_inside = true;
		cout << "\n=== ПАНЕЛЬ ПРИБОРОВ ===" << endl;
		cout << "Нажмите 'i' чтобы завести двигатель. Esc - выход." << endl;
		panel();
	}

	void get_out()
	{
		driver_inside = false;
		if (is_running)
		{
			is_running = false;
			cout << "\n[Система] Вы вышли из машины. Двигатель заглушен." << endl;
		}
	}

	// Функция фонового потока, которая каждую секунду уменьшает топливо при заведенном моторе
	void engine_life()
	{
		while (!finish_threads)
		{
			std::this_thread::sleep_for(std::chrono::seconds(1));

			if (is_running)
			{
				double burn = engine.get_consumption_per_second();
				tank.give_fuel(burn);

				// Если бензин закончился — глушим мотор
				if (tank.get_fuel_level() <= 0)
				{
					is_running = false;
					if (driver_inside) panel();
					cout << "\n[Внимание] Бензин закончился! Двигатель заглох сам." << endl;
				}
			}
		}
	}

	// Метод управления кнопками (без зависания программы благодаря _kbhit)
	void control()
	{
		char key = 0;
		do
		{
			// Проверяем нажатие клавиши БЕЗ остановки всей программы
			if (_kbhit())
			{
				key = _getch();
				switch (key)
				{
				case Enter:
					if (driver_inside) get_out();
					else get_in();
					break;

				case 'i':
				case 'I':
					if (driver_inside)
					{
						if (!is_running)
						{
							if (tank.get_fuel_level() > 0)
							{
								is_running = true;
								cout << "\n[Двигатель] Врррм! Двигатель заведен кнопкой 'i'." << endl;
								panel();
							}
							else
							{
								cout << "\n[Ошибка] Нет бензина!" << endl;
							}
						}
						else
						{
							is_running = false;
							cout << "\n[Двигатель] Двигатель заглушен кнопкой 'i'." << endl;
							panel();
						}
					}
					else
					{
						cout << "\n[Система] Сначала нажмите Enter, чтобы сесть в машину!" << endl;
					}

					// Очищаем буфер клавиатуры, чтобы избежать эффекта "дребезга кнопок"
					while (_kbhit()) _getch();

					break;
				}
			}

			// Если машина работает и водитель внутри — обновляем панель каждую секунду прямо здесь
			if (is_running && driver_inside)
			{
				panel();
				std::this_thread::sleep_for(std::chrono::seconds(1));
			}
			else
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(50));
			}

		} while (key != Escape);
	}

	// Вывод текущего состояния бензина на экран
	void panel()
	{
		if (driver_inside)
		{
			// Символ \r возвращает каретку в начало строки, а std::flush мгновенно обновляет текст на месте
			cout << "\r[Панель приборов] Уровень топлива: " << std::fixed << std::setprecision(2) << tank.get_fuel_level() << " л. | Двигатель: " << (is_running ? "РАБОТАЕТ " : "ВЫКЛЮЧЕН") << "    " << std::flush;
		}
	}
};

void main()
{
	// Настройка локализации для поддержки русского языка в консоли
	setlocale(LC_ALL, "");

	Car bmw(10, 70); // Создаем машину с расходом 10 и баком 70 литров
	bmw.control();   // Запускаем цикл управления
}