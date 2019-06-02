#include <windows.h>
#include <SFML/Graphics.hpp>

#include <chrono>
#include "es.h"
#include "radix.h"

sf::RenderWindow window;

void LINE(int x, int y, int x1, int y1, sf::Color c) {
	sf::Vertex line[] = { sf::Vertex(sf::Vector2f((float)x, (float)y),c),sf::Vertex(sf::Vector2f((float)x1, (float)y1), c) };
	window.draw(line, 2, sf::Lines); 
}


void test_storage() {

	FEntitys entitles;
	//FEntity& FE = entitles.PushBack();
	FEntity* FE = NULL;
	FE = &entitles.PushBack();
	entitles.Index.set(1, (int)((int)FE - (int)entitles.data));
	FE->id = 1;
	FE->cnt = 0;

	Comp3* Pos = new Comp3();
	Comp3* TestPos = NULL;
	Pos->data[0] = 'a';	Pos->data[1] = 'b';	Pos->data[2] = 'c';	Pos->data[3] = 'd';

	entitles.PushBackComponent(FE, typeid(Comp3).hash_code(),(char*)Pos);

	FE = &entitles.GetEntity(1);
	entitles.PushBackComponent(FE, typeid(Comp3).hash_code(), (char*)Pos);

	FE = &entitles.GetEntity(1);
	TestPos = (Comp3*)entitles.GetComponent(FE, typeid(Comp3).hash_code());

	TestPos = entitles.GetComponent<Comp3>(FE);

	//COORD destCoord;
	//destCoord.X = 0; destCoord.Y = 10;
	//SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), destCoord);
	//std::cout<< entitles.freebuf_info();

	FEntity* FE1 = &entitles.PushBack();
	entitles.Index.set(2, (int)((int)FE1 - (int)entitles.data));
	FE1->id = 2;
	FE1->cnt = 0;
	Pos->data[0] = 'x';	Pos->data[1] = 'y';	Pos->data[2] = 'z';	Pos->data[3] = '!';
	entitles.PushBackComponent(FE1, typeid(Comp3).hash_code(), (char*)Pos);

	FEntity* FE2 = &entitles.PushBack();
	FE2->id = 3;
	FE2->cnt = 0;

	FE = &entitles.GetEntity(1);
	FE1 = &entitles.GetEntity(2);
	FE2 = &entitles.GetEntity(3);





	const int ENT_DRAW = 5000;
	const int ENT_WIDTH = 128;//50*2;
	const int H = 850;

	float t = 0;
	//sf::Sprite* SS;
	//StringComponent* txt;

	sf::Clock Clock;
	sf::Clock deltaClock;

	sf::ContextSettings settings;
	settings.antialiasingLevel = 0;
	window.create(sf::VideoMode(1600, 900), "My window", sf::Style::Default, settings);
	window.setVerticalSyncEnabled(false);// UseVerticalSync(false);
										 //window.setFramerateLimit(60);

										 // run the program as long as the window is open

	bool Auto = false;
	int AutoI = 0;

	while (window.isOpen())
	{
		// check all the window's events that were triggered since the last iteration of the loop
		sf::Event event;
		while (window.pollEvent(event))
		{
			// "close requested" event: we close the window
			if (event.type == sf::Event::Closed)
				window.close();

			if (event.type == sf::Event::KeyPressed) {
				if (event.key.code == sf::Keyboard::Num0) {
					Auto = !Auto;
					std::cout << " Auto :" << Auto << "\n ";
				}

			}

			if (event.type == sf::Event::KeyPressed)
			{
				if (event.key.code == sf::Keyboard::Subtract)
				{
					std::cout << "Adding 1 Entitys \n";
					for (int i = 0; i < 100; i++) {
						float id = (static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) * ENT_DRAW;
						int s = (static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) * static_cast <float>(ENT_WIDTH - (header_size + comp_header_size + sizeof(int)) );
						std::cout << " id:" << id << " size:" << s << "; ";
						if (!entitles.Index.exists((int)id)) {
							entitles.Add(s, (int)id);
						}
					}
				}

				if (event.key.code == sf::Keyboard::Multiply)
				{
					std::cout << "deleting 1 Entitys \n";
					for (int i = 0; i < 100; i++) {
						float id = (static_cast <double> (rand()) / static_cast <double> (RAND_MAX)) * ENT_DRAW;
						int s = (static_cast <double> (rand()) / static_cast <double> (RAND_MAX)) * static_cast <double>(ENT_WIDTH - (header_size + comp_header_size + sizeof(int)));
						std::cout << " id:" << id << " size:" << s << "; ";
						if (entitles.Index.exists(id)) {
							entitles.Erase(&entitles.GetEntity(id));
						}
					}
				}

				if (event.key.code == sf::Keyboard::Divide)
				{
					std::cout << "Clear 1 Entitys \n";
					for (int i = 1; i < ENT_DRAW; i++) {
						//std::cout << " id:" << i << "; ";
						if (entitles.Index.exists(i)) {
							entitles.Erase(&entitles.GetEntity(i));
						}
					}
				}


				if (event.key.code == sf::Keyboard::Num7)
				{
					std::cout << "FreeBloks set\n";
					for (int i = 1; i < ENT_DRAW; i++) {
						//std::cout << " id:" << i << "; ";
						int s = (static_cast <double> (rand()) / static_cast <double> (RAND_MAX)) * static_cast <double>(ENT_WIDTH - (header_size + comp_header_size + sizeof(int)));
						entitles.freeblocks.set(s,i);
					}
				}
				if (event.key.code == sf::Keyboard::Num9)
				{
					std::cout << "FreeBloks clear\n";
					for (int i = 1; i < ENT_DRAW; i++) {
						//std::cout << " id:" << i << "; ";
						int s = (static_cast <double> (rand()) / static_cast <double> (RAND_MAX)) * static_cast <double>(ENT_WIDTH - (header_size + comp_header_size + sizeof(int)));
						entitles.freeblocks.get(s);
					}
				}



			}

		}

		if (Auto) {
			if (AutoI++ > 10) {
				//std::cout << " Auto:" << AutoI << "\n";
				int s = (static_cast <double> (rand()) / static_cast <double> (RAND_MAX)) * static_cast <double>(ENT_WIDTH - (header_size + comp_header_size + sizeof(int)));

				for (int i = 1; i < 5; i++) {

				AutoI = 0;
				float id = (static_cast <double> (rand()) / static_cast <double> (RAND_MAX)) * ENT_DRAW;
				
				//std::cout << " id:" << id << " size:" << s << "; ";
				if (!entitles.Index.exists(id)) {
					entitles.Add(s, id);
				}

				id = (static_cast <double> (rand()) / static_cast <double> (RAND_MAX)) * ENT_DRAW;
				s = (static_cast <double> (rand()) / static_cast <double> (RAND_MAX)) * static_cast <double>(ENT_WIDTH - (header_size + comp_header_size + sizeof(int)));
				//std::cout << " id:" << id << " size:" << s << "; ";
				if (entitles.Index.exists(id)) {
					entitles.Erase(&entitles.GetEntity(id));
				}

				}

			}

		}




		//window.Co
		window.clear(sf::Color::Black);
		LINE(10, 10, 20, 20, sf::Color::Red);

		//sf::Vertex line[] = { sf::Vertex(sf::Vector2f(100, 100)),
		//					  sf::Vertex(sf::Vector2f(100, 200)) }; 
		//	line->color = sf::Color::Red;
		//	window.draw(line, 2, sf::Lines); 
		int i = 1;
		int X = 0;
		int Y = 0;
		int padL = 25;
		int padT = 25;
		for (;i < ENT_DRAW;i++) {
			FE = &entitles.GetEntity(i);
			if (FE == NULL) {
				
				LINE(X + padL, Y + padT, X + padL+ 2, Y + padT, sf::Color::Black);
				
			}
			else {
				
				LINE(X + padL, Y + padT, X + padL + 2, Y + padT, sf::Color::White);
				LINE(X + padL, Y + padT+1, X + padL + 2, Y + padT+1, sf::Color::White);
				//LINE(X + padL + 4, Y + padT, X + padL + 4 + FE->size/4, Y + padT, sf::Color::Yellow);

				int EntityOfs = ((char*)FE - entitles.data);
				int XX = (EntityOfs / H) * 4;
				int YY = (EntityOfs % H);
				int HS = header_size;
				LINE(XX + padL + 4 + 80, YY + padT, XX + padL + 4 + 80, YY + padT + FE->size - 1, sf::Color::Blue);
				LINE(XX + padL + 4 + 81, YY + padT, XX + padL + 4 + 81, YY + padT + HS - 1, sf::Color::Blue);
				LINE(XX + padL + 4 + 82, YY + padT, XX + padL + 4 + 82, YY + padT + FE->size - 1, sf::Color::Blue);


			}


			Y+=1;
			if (Y > H) {
				Y=0;
				X+= 4;
			}
		}
		window.display();
	}

}



void index_test() {

	const int elements = 1000;

	std::vector<int> pages4;
	pages4.resize(elements * sizeof(int),0);

	auto to = std::chrono::high_resolution_clock::now();
	auto from = to;



	from = std::chrono::high_resolution_clock::now();
	for (int i = 0; i < elements; i++) {
		pages4[i]=i;
	}
	to = std::chrono::high_resolution_clock::now();
	std::cout << "Vector time:\t" << std::chrono::duration_cast<std::chrono::duration<double>>(to - from).count() << "\n";

	from = std::chrono::high_resolution_clock::now();
	for (int i = 0; i < elements; i++) {
		pages4[i] = i;
	}
	to = std::chrono::high_resolution_clock::now();
	std::cout << "Vector time:\t" << std::chrono::duration_cast<std::chrono::duration<double>>(to - from).count() << "\n";

	RadixIndex radixIndex;
	
	

	from = std::chrono::high_resolution_clock::now();
	for (int i = 0; i < elements; i++) {
		int a = abs( (int) ((static_cast <double> (rand()) / static_cast <double> (RAND_MAX)) * (double)2147483647/65536*256));
		radixIndex.set(a,a);
		if (rand() < RAND_MAX / 3*2) {
			radixIndex.del(a);
		}
	}
	to = std::chrono::high_resolution_clock::now();
	std::cout << "Page allocated:\t" << radixIndex.calc_size() << "\n";
	radixIndex.collect_garbage();
	std::cout << "Page allocated:\t" << radixIndex.calc_size() << "\n";
	std::cout << "Radix time:\t" << std::chrono::duration_cast<std::chrono::duration<double>>(to - from).count() << "\n";

	/*
	std::map <int, int> pages3;
	from = std::chrono::high_resolution_clock::now();
	for (int i = 0; i < elements; i++) {
		pages3[i] = i;//(i, i);
	}
	to = std::chrono::high_resolution_clock::now();
	std::cout << "std::map time:\t" << std::chrono::duration_cast<std::chrono::duration<double>>(to - from).count() << "\n";


	from = std::chrono::high_resolution_clock::now();
	for (int i = 0; i < elements; i++) {
		pages3[i] = i;//(i, i);
	}
	to = std::chrono::high_resolution_clock::now();
	std::cout << "std::map time:\t" << std::chrono::duration_cast<std::chrono::duration<double>>(to - from).count() << "\n";
	*/
	system("pause");
}


int main() {

	RegisterComponents();
	
	test_storage();
	//index_test();

	return 0;
}