#include "monitor.h"
#include<iostream>
#include <cstdio>
#include <queue>
#include <random>
#include <time.h>
#include<unistd.h>

#define MAX_SIZE 9
#define MIN_FOR_CONS 3
#define MAX_SUM_PROD_A 20

class Bufor : public Monitor
{
private:

	std::queue<int> bufor;
	int sum=0;

	Condition consumer_size;
	Condition A;
	Condition B;

public:
	//Bufor();
	//void put(int value, char prod_id);
	void putA(int value);
	void putB(int value);
	//void get(int c_numb);
	void getC(int c_numb);
};

void Bufor::putA(int value)
{
	enter();
	if(bufor.size() >= MAX_SIZE || sum >= MAX_SUM_PROD_A)
		wait(A);
	bufor.push(value);
	sum += value;
	std::cout<<"PROD A    SIZE: "<<bufor.size()<<"\n";
	for(int i=0; i<bufor.size(); ++i)
	{
		std::cout<<bufor.front()<<" ";
		bufor.push(bufor.front());
		bufor.pop();
	}
	std::cout<<std::endl;

	if(bufor.size() == MIN_FOR_CONS+1)
		signal(consumer_size);
	leave();
}

void Bufor::putB(int value)
{
	enter();
	if(bufor.size() >= MAX_SIZE)
		wait(B);
	bufor.push(value);
	sum += value;

	std::cout<<"PROD B    SIZE: "<<bufor.size()<<"\n";
	for(int i=0; i<bufor.size(); ++i)
	{
		std::cout<<bufor.front()<<" ";
		bufor.push(bufor.front());
		bufor.pop();
	}
	std::cout<<std::endl;

	if(bufor.size() < MAX_SIZE && sum < MAX_SUM_PROD_A)
		signal(A);
	if(bufor.size() == MIN_FOR_CONS+1)
		signal(consumer_size);
	leave();
}

void Bufor::getC(int c_numb)
{
	enter();
	if(bufor.size() <= MIN_FOR_CONS)
		wait(consumer_size);
	sum -= bufor.front();
	bufor.pop();

	std::cout<<"CONS "<<c_numb<<"    SIZE: "<<bufor.size()<<std::endl;
	for(int i=0; i<bufor.size(); ++i)
	{
		std::cout<<bufor.front()<<" ";
		bufor.push(bufor.front());
		bufor.pop();
	}
	std::cout<<std::endl;

	signal(B);
	if(bufor.size() < MAX_SIZE && sum < MAX_SUM_PROD_A)
		signal(A);
	if(bufor.size() > MIN_FOR_CONS)
		signal(consumer_size);
	leave();
}

/*void Bufor::put(int value, char prod_id)
{
	enter();
	if (bufor.size() >= MAX_SIZE && prod_id == 'B')
	{
		std::cout<<"Producent "<<prod_id<<" czeka na warunku B\n";
		B_waits = true;
		wait(B);
		B_waits = false;
	}
	else if(prod_id == 'A' && (sum >= MAX_SUM_PROD_A || bufor.size() >= MAX_SIZE))
	{
		A_waits = true;
		std::cout<<"Producent A czeka na warunku A\n";
		wait(A);
		A_waits = false;
		std::cout<<"Producent A wraca z warunku A\n";
	}
	bufor.push(value);
	sum += value;
	std::cout << "Producent " << prod_id << " dodaje " << value << " do bufora\nSuma el: " << sum << "\nLiczba el: "<<bufor.size()<<"\n";
	for(int i=0; i<bufor.size(); ++i)
	{
		std::cout<<bufor.front()<<" ";
		bufor.push(bufor.front());
		bufor.pop();
	}
	std::cout<<std::endl;
	if(prod_id == 'B' && A_waits && bufor.size()<MAX_SIZE && sum < MAX_SUM_PROD_A)
	{
		std::cout<<"Producent B budzi producenta A\n";
		signal(A);
	}
	else if(prod_id == 'A' && B_waits && bufor.size()<MAX_SIZE)
	{
		std::cout<<"Producent A budzi producenta B\n";
		signal(B);
	}
	else if (bufor.size() > MIN_FOR_CONS)
	{
		std::cout << "Producent " << prod_id << " budzi konsumenta\n";
		signal(consumer_size);
	}
	leave();
}*/

/*void Bufor::get(int c_id)
{
	enter();
	if (bufor.size() <= MIN_FOR_CONS)
	{
		std::cout<<"		Konsument "<<c_id<<" czeka na warunku consumer_size\n";
		wait(consumer_size);
	}
	sum -= bufor.front();
	std::cout << "		Konsument " << c_id << " usuwa " << bufor.front() << " z bufora\nSuma el: " << sum << std::endl << "Liczba el: "<<bufor.size()-1<<"\n";
	bufor.pop();

	for(int i=0; i<bufor.size(); ++i)
	{
		std::cout<<bufor.front()<<" ";
		bufor.push(bufor.front());
		bufor.pop();
	}
	std::cout<<std::endl;

	if (sum < MAX_SUM_PROD_A && A_waits && bufor.size() < MAX_SIZE)
	{
		std::cout<<"		Konsument "<< c_id << " budzi producenta A (signal A)\n";
		signal(A);
	}
	else if(B_waits && bufor.size() < MAX_SIZE)
	{
		std::cout<<"		Konsument "<< c_id << " budzi producenta B (signal B)\n";
		signal(B);
	}
	else if(bufor.size() > MIN_FOR_CONS)
	{
		std::cout<<"		Konsument "<< c_id << " budzi konsumentow (signal consumer_size)\n";
		signal(consumer_size);
	}
	leave();
}*/

Bufor mon;

void * producer(void * arg)
{
	while (true)
	{
		usleep(rand() % 100000);
		if(*((char *) arg) - 'A')
			mon.putB(rand() % 20 - 10);
		else
			mon.putA(rand() % 20 - 10);
	}
}

void * consumer(void * arg)
{
	char n = *((char *)arg);
	while (true)
	{
		usleep(rand() % 100000);
		mon.getC(n - 'A');
	}
}

int main()
{
	pthread_t prodA, cons1, prodB, cons2;
	char *a = new char;
	char *b = new char;
	*a = 'A';
	*b = 'B';
	srand(time(NULL));
	pthread_create(&prodA, NULL, producer, a);
	pthread_create(&cons1, NULL, consumer, a);
	pthread_create(&prodB, NULL, producer, b);
	pthread_create(&cons2, NULL, consumer, b);

	pthread_join(prodA, NULL);
	pthread_join(cons1, NULL);
	pthread_join(prodB, NULL);
	pthread_join(cons2, NULL);

	delete a;
	delete b;

	return 0;
}
