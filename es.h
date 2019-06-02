#pragma once
#include <unordered_map>
#include <iostream>
#include <queue>
#include <functional>     // std::greater
#include <assert.h>
#include "radix.h"


//Entity system; types of Components;
struct PositionComponent {
public:
	PositionComponent() {}
	float x;
	float y;
};

struct Comp3 {
	char data[4];
};


struct RectComponent {
public:
	RectComponent() {}
	int x;
	int y;
	int w;
	int h;
};

struct FpsComponent {
	FpsComponent() {}
	float num;
	float a, b;
};

struct StringComponent {
	sf::Text* text;
};

//Typedefs of RTTI types
std::unordered_map<int, std::string> type_names;	// hash_code() => name
std::unordered_map<int, int> type_size;				// hash_code() => size

void RegisterComponents() {

	type_names[typeid(byte).hash_code()] = typeid(byte).name();
	type_size[typeid(byte).hash_code()] = sizeof(byte);

	type_names[typeid(uint16_t).hash_code()] = typeid(uint16_t).name();
	type_size[typeid(uint16_t).hash_code()] = sizeof(uint16_t);

	
	type_names[typeid(int).hash_code()] = typeid(int).name();
	type_size[typeid(int).hash_code()] = sizeof(int);

	type_names[typeid(float).hash_code()] = typeid(float).name();
	type_size[typeid(float).hash_code()] = sizeof(float);

	type_names[typeid(PositionComponent).hash_code()] = typeid(PositionComponent).name();
	type_size[typeid(PositionComponent).hash_code()] = sizeof(PositionComponent);

	type_names[typeid(Comp3).hash_code()] = typeid(Comp3).name();
	type_size[typeid(Comp3).hash_code()] = sizeof(Comp3);
	

	type_names[typeid(RectComponent).hash_code()] = typeid(RectComponent).name();
	type_size[typeid(RectComponent).hash_code()] = sizeof(RectComponent);

	type_names[typeid(sf::Sprite).hash_code()] = typeid(sf::Sprite).name();
	type_size[typeid(sf::Sprite).hash_code()] = sizeof(sf::Sprite);

	type_names[typeid(StringComponent).hash_code()] = typeid(StringComponent).name();
	type_size[typeid(StringComponent).hash_code()] = sizeof(StringComponent);
}


struct FEntity {
	int size;
	int id;
	int cnt;
};
const int header_size = sizeof(FEntity);

struct FComponent {
	int type;
	int size;
};
const int comp_header_size = sizeof(FComponent);

//typedef std::deque<int> offsets;
// 1. порядок элементов определяется контейнером. (сортированные или не сортированные)
//
// 2. для перечеслений
//		
//		Всех entity с типом t			Index_Fast,	Index_Flat, Index_3, Index_Full
//		Всех компонентов типа t			Index_Fast,	Index_Flat, Index_3, Index_Full
//		Всех компонентов типов t..tn	Index_Fast,	Index_Flat, Index_3, Index_Full
//		Набора Entity					Index_Fast,	Index_Flat, Index_3, Index_Full
//
// 3. для поиска
//		
//		Всех entity с типом t			Index_Fast,	Index_Flat, Index_3, Index_Full
//		Всех компонентов типа t			Index_Fast,	Index_Flat, Index_3, Index_Full
//		Всех компонентов типов t..tn	Index_Fast,	Index_Flat, Index_3, Index_Full
//		Набора Entity					Index_Fast,	Index_Flat, Index_3, Index_Full

struct Index_Flat{
	int entityOfs;
	int componentOfs;
};
struct Index_Full {
	int ID;
	int entityOfs;
	int componentOfs;
	int componentType;
};

struct Index_Fast {
	int componentOfs;
};
struct Index_3 {
	int entityOfs;
	int componentOfs;
	int componentType;
};


template <class T>
struct FEntityIndex {
	std::vector<T> data;
	std::deque<int> types;
	
	void push(T value) {
		data.push_back(value);
	}
	void pop(int indexID) {
		std::memset(data[indexID], 0, sizeof T);
	}
	void erase(int indexID) {
		data.erase(data.begin()+indexID);
	}
};

struct FreeBlocks {
	//typedef std::deque<int>* offsets;
	typedef std::priority_queue<int, std::vector<int>, std::greater<int>> offsets;
	std::map<int, offsets*> Items;

	int get(int size) {

		std::map<int, offsets*>::iterator it = Items.lower_bound(size + header_size);		// search last block equal or greater request size
		if (it != Items.end()) {

			assert(it->second != NULL);

			int freeblsize = it->first;														// free block size

			if (Items.at(freeblsize)->empty()) return 0;

			//			std::deque<int>::iterator it = Items.at(freeblsize)->begin();
			//			std::vector<int> it = (Items.at(freeblsize))->begin();
			//			int EntityOfs = *it;				// pop oldest entity Offset

			int EntityOfs = (Items.at(freeblsize))->top();
			(Items.at(freeblsize))->pop();

			if (Items.at(freeblsize)->empty()) {											// clear block[size] dequie if  empty
																							// delete (it->second);
				delete Items.at(freeblsize);
				Items[freeblsize] = NULL;
				Items.erase(freeblsize);													// remove size from free block cache
			}
			return EntityOfs;
		}
		return 0;
	}

	void set(int size, int offset) {
		if (Items[size] == NULL) {
			Items[size] = new offsets;
		}
		Items[size]->push(offset);
		//std::vector<int>* ofs = Items[size];
		//if (ofs == NULL) {
			//ofs = (offsets*)calloc(sizeof(offsets), 1);// new offsets;
			
			//freeblocks[EF->size] = new offsets;
		//}
		//ofs->push_back(offset);
	}

	void setFront(int size, int offset) {
		if (Items[size] == NULL) {
			Items[size] = new offsets;
		}
		Items[size]->push(offset);
	}


	std::string freebuf_info() {
	/*
		std::string res = "\n* Free buf *\n";

		int nulls = 0;
		int sizes = 0;
		int bytes = 0;
		std::map<int, int> info;		// size, blocks count

		for (auto it = Items.begin(); it != Items.end(); it++) {
			if (it->second == NULL) {
				nulls++;
			}
			else {
				sizes++;
				bytes += ((offsets*)it->second)->size() * sizeof(offsets::value_type);
				info.try_emplace(it->first, ((offsets*)it->second)->size());
			}
		}

		for (auto it = info.begin(); it != info.end(); it++) {
			res += std::to_string(it->first) + " : " + std::to_string(it->second) + "; " + std::to_string(((it->second * sizeof(offsets::value_type)) / bytes) * 100) + "% \n";
		}
		res += "\n --- \n";
		res += "BYTES: " + std::to_string(bytes) + "; LEVELS: " + std::to_string(sizes) + "; NULLS: " + std::to_string(nulls) + "; TOTAL : " + std::to_string(bytes / sizeof(offsets::value_type));
		return res;
		*/
	}

};


struct FEntitys {
	int EntitysCounter;
	int allocated;
	int freeMem;
	char * data;

	char * first;
	char * last;
	bool UseIndex = true;
	RadixIndex Index;
	int LastId = 0;
	FreeBlocks freeblocks;

	FEntitys() { data = NULL, first = NULL, last = NULL, allocated = 0, freeMem = 0, EntitysCounter = 0; }

	void Realloc(int newSize) {
		char * realloc_memory = (char*)calloc(newSize, sizeof(char));
		if (realloc_memory != NULL) {
			memcpy(realloc_memory, data, min(allocated, newSize));
			if (data != NULL)
				free(data);
			data = realloc_memory;
			allocated = newSize;

			int s = last - first;			//calculate LAST FREE VALUE

			assert(s>=0);

			first = data;					// Update pointers;
			last = data + s;				// Update cursor; Курсор не собьется, при realloce
			freeMem = allocated - s;

			assert(freeMem >= 0);
		}
	}

	FEntity &GetEntity(int EntityID) {
		//FEntity& E = *(FEntity*)data;
		if (UseIndex && Index.exists(EntityID) ){
				char * E = data + (Index.get(EntityID));
				return *(FEntity*)E;
		} else if (!UseIndex) {
		
			char * E = data + 1;
			for (; E < last;) {
				//std::cout << "get ent:\t" << (int)(((int)E) - (int)data) << "\tID:\t" << (*(FEntity*)E).id << "\n";

				assert(E > data);

				if ( (*(FEntity*)E).id == EntityID) {
					return *(FEntity*)E;
				}

				assert((*(FEntity*)E).size > 0);

				E += (*(FEntity*)E).size;
			}
		}
		return *(FEntity*)NULL;
	}

	void Add(FEntity * Entity, int ComponentType, char* ComponentData, int size) {

		if (size > allocated) {
			char * realloc_memory = (char*)calloc(size, sizeof(char));
			if (realloc_memory != NULL) {
			}
		}
	}

	// =====================================================
	// Добавляет entity Размером size;
	// * id должен быть уникальным
	// * проверяет наличие в кеше свободных блоков
	// * вставляет в найденный свободный блок или в конец 
	// 

	FEntity& Add(int size,int id) {

		int EntityOfs = freeblocks.get(size);

		//FreeBlocks::iterator it;
		//it = freeblocks.lower_bound(size + header_size);		//search last block equal or greater request size
		//if (it != freeblocks.end()) {					

		//	assert(it->second != NULL);

		//	int freeblsize = it->first;							// free block size

		//	offsets* ofs = NULL;
		//	ofs = ((offsets*)freeblocks[freeblsize]);

		//	int EntityOfs = ofs->front();				// pop oldest entity Offset
		//	ofs->pop();							//

		//	if (ofs->empty()) {							// clear block[size] dequie if  empty
				//delete (it->second);
		//		free (ofs);
		//		ofs = NULL;
		//		freeblocks.erase(freeblsize);					//remove size from free block cache
		//	}

		//if (EntityOfs == 0) {std::cout << "[-1-]";}else{std::cout << "[-0-]";}

		if (EntityOfs > 0) {
			if (UseIndex) assert(!Index.exists(id) );			// check for dublicate id
			if (UseIndex) Index.set(id, EntityOfs);

			FEntity* E = (FEntity*)(data + EntityOfs);
			int freeblsize = E->size;
			E->id = id;
			E->cnt = 0;
			E->size = size + header_size;


			// reamain process
			int remainBytes = freeblsize - (size + header_size);
			if (remainBytes > 0) {
																// minimal free block size
				int MinFreeBlockSize = header_size + comp_header_size + sizeof(int);

				if (remainBytes < MinFreeBlockSize) {
					E->size += remainBytes;						// if remainder is very small, make it part of current entity; 
				}
				else {											
																// new free block
					FEntity* EF = (FEntity*)(data + EntityOfs + E->size);
					EF->id = 0;
					EF->cnt = -1;
					EF->size = remainBytes;

					assert(remainBytes > 0);

					freeblocks.setFront(remainBytes, EntityOfs + E->size);

					int FreeEntityOfs = (char*)EF - data;
					//ofs = (offsets*)freeblocks[EF->size];
					//if (ofs == NULL) {
					//	ofs = (offsets*)calloc(sizeof(offsets), 1);// new offsets;
					//	freeblocks[EF->size] = (int)ofs;
						//freeblocks[EF->size] = new offsets;
					//}
					//ofs->push(FreeEntityOfs);
				}
			}
			return *E;

		}
		else {
			// push back
			FEntity* E = &PushBack(size);

			if (UseIndex)
				assert(!Index.exists(id));			// check for dublicate id

			if (UseIndex) Index.set(id, (int)((int)E - (int)data));
			E->id = id;
			E->cnt = 0;
			E->size = size + header_size;

			return *E;
		}


	}

	void PushBackComponent(FEntity * Entity, int ComponentType, char* ComponentData) {

		int Componentsize = type_size[ComponentType];
		if ((int)(data) + allocated < (int)(Entity) + Entity->size + comp_header_size + Componentsize) {
			int Eid = Entity->id;
			Realloc(allocated + comp_header_size + Componentsize);
			Entity = &GetEntity(Eid);
		}

		//FComponent& comp = *(FComponent*)((Entity)+Entity->size);
		//comp.type = ComponentType;
		//comp.size = Componentsize;

		char* componentdata = (char*)(Entity) + Entity->size;				//move cursor to Entity->size
		*(int*)componentdata = ComponentType;								//store component type
		componentdata += sizeof(int);
		*(int*)componentdata = Componentsize;								//store component size
		componentdata += sizeof(int);
		//char* componentdata = (char*)(Entity)+Entity->size + sizeof(FComponent);
		memcpy(componentdata, ComponentData, Componentsize);				//store component data

		Entity->size+= Componentsize + comp_header_size;				//update Entity size;
		Entity->cnt++;														//update Entity components;

		last = last + Componentsize + comp_header_size;				//move up last position
		freeMem = freeMem - (Componentsize + comp_header_size);		//decrease free mem in data
	}

	FEntity& PushBack(int EntitySize=0) {

		if (last !=NULL) {
			//if ((int)&data + allocated < (int)&current + header_size) {
				//Realloc(allocated + header_size);
			//}
				if (freeMem < (header_size + EntitySize)) {
					Realloc(allocated + (header_size+ EntitySize));
				}
		}
		else {
			Realloc( 1 + allocated + (header_size + EntitySize));
			last+=1;
		}

		FEntity& FE = *(FEntity*)last;
		FE.size = header_size + EntitySize;
		FE.id = 0;
		FE.cnt = 0;
		last = last + header_size + EntitySize;
		freeMem -= (header_size + EntitySize);
		std::cout << "New ent:\t" << (int)((int)&FE - (int)data) << "\n";
		return FE;
	}

	void Erase(FEntity * Entity) {

		if (Entity == NULL) return;

		assert((char*)Entity >= data);
		assert((char*)Entity < last);

		if (UseIndex && Index.exists(Entity->id)) 
			Index.del(Entity->id);

		Entity->id = 0;
		Entity->cnt = -1;


		char * E = ((char *)Entity) + header_size;
		char * end = ((char *)E) + Entity->size - header_size;

		for (; E < end; E++) {
			*(char*)E = 0;
		}

		
		/*FEntity* NextFreeEntity = (FEntity*)GetNextFree(Entity);
		if (NextFreeEntity != NULL) {
			//merge with next;
			Entity->size += NextFreeEntity->size;

			//clear next data;
			NextFreeEntity->cnt = 0;
			NextFreeEntity->id = 0;
			NextFreeEntity->size = 0;
		}
		*/
		
		

		//offsets * freeofs = NULL;
		//if (freeblocks.find(Entity->size) == freeblocks.end()) {
		//	if (freeblocks[Entity->size] == NULL) {
		//		freeofs = (offsets*)calloc(sizeof(offsets),1);// new offsets;
		//	}
		//	freeblocks[Entity->size] = (int)freeofs;
		//}
		//freeofs = (offsets*)freeblocks[Entity->size];

		//assert(freeofs !=NULL );
		
		int EntityOfs = (char*)Entity - data;
		//freeofs->push(EntityOfs);
		freeblocks.set(Entity->size, EntityOfs);
	}

	char* GetNextFree(FEntity * Entity) {

		char * end = ((char *)Entity) + Entity->size;
		FEntity* FE = &*(FEntity*)end;
		if (FE->id == 0 && FE->cnt == -1) {
			
			return (char*)FE;
			int EntityOfs = end - data;
		}
		return NULL;

	}

	char* GetComponent(FEntity * Entity, int ComponentType) {

		char * E = ((char *)Entity) + header_size;
		char * end = ((char *)E) + Entity->size;

		for (int i=0; E < end && i <= Entity->cnt;i++) {

			if ((*(FComponent*)E).type == ComponentType) {

				E += comp_header_size;
				return (char*)E;

			} else {

				E += (*(FComponent*)E).size + comp_header_size;
			}
			//std::cout << "get ent:\t" << (int)(((int)E) - (int)data) << "\tID:\t" << (*(FEntity*)E).id << "\n";
		}
		return (char*)NULL;
	}

	template <class T>
	T* GetComponent(FEntity * Entity) {
		return (T*)GetComponent(Entity, typeid(T).hash_code() );
	}

};


