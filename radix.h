#pragma once

// https://stackoverflow.com/questions/3198112/fastest-c-map

typedef int Item;

struct page {
	Item data[65536];
};

struct PagesList {
	page* data[65536];
};

struct RadixIndex {
	PagesList* pages;			// list of pages
	uint16_t PagesIndex[65536]; // sentinel index

	RadixIndex () {
		pages = new PagesList;
		for (int i = 0; i < 65536; i++) {
			pages->data[i] = NULL;
			PagesIndex[i] = 0;
		}
	}

	~RadixIndex() {
		for (int i = 0; i < 65536; i++) {
			//if (pages->data[i]!=NULL) 
				free(pages->data[i]);
		}
		free(pages);
	}

	Item& get(const int index) {
		return pages->data[index >> 16]->data[index & 0xFFFF];
	}

	Item& operator[](int const& index) {
		return pages->data[index >> 16]->data[index & 0xFFFF];
	}

	const Item& operator[](int const& index) const {
		return pages->data[index >> 16]->data[index & 0xFFFF];
	}

	void set(const int index, Item value) {
		uint32_t index16 = index >> 16;
		if (pages->data[index16] == NULL) {
			pages->data[index16] = allocate_new_null_filled_page();
		}
		pages->data[index16]->data[index & 0xFFFF] = value;
		PagesIndex[index16]++;
	}

	void del(const int index) {
		uint32_t index16 = index >> 16;
		pages->data[index16]->data[index & 0xFFFF] = 0;
		PagesIndex[index16]--;
		if (PagesIndex[index16] == 0) {
			free(pages->data[index16]);
			pages->data[index16] = NULL;
		}
	}

	bool exists(const int index) {
		uint32_t index16 = index >> 16;
		return PagesIndex[index16] == 0 ? false : pages->data[index16]->data[index & 0xFFFF] != 0;
	}

	page* allocate_new_null_filled_page() {
		return (page*)calloc(65536, sizeof(Item)); //page* ref = (page*)calloc(65536,sizeof(Item));
	}

	size_t calc_size() {
		size_t ret = 0;
		for (int i = 0; i < 65536; i++) {
			if (pages->data[i] != NULL) ret++;
		}
		return ret; /* * sizeof(page) */
	}

	void collect_garbage() {
		for (int i = 1; i < 65536; i++) {
			if (PagesIndex[i] == 0 && pages->data[i] != NULL) {
				free(pages->data[i]);
				pages->data[i] = NULL;
				//std::cout << "clean page2 " << i << " " << "" << calc_size() << "\n";
			}
		}
	}

	int get_value_index(int value) {
		for (int i = 1; i < 65536; i++) {
			if (PagesIndex[i] != 0 && pages->data[i] != NULL) {
				for (int j = 1; j < 65536; j++) {
					if (pages->data[i]->data[j] == value)
						return (i << 16) + j;
				}
			}
		}
		return 0;
	}



};
