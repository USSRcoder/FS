# FS

Flast Storage - Flat memory heap, with auto de fragmentation.

<pre>

es.h - Simple memory storage, allow to allocate/deallocate blocks with varios size, 
and auto defragment it in memory.

radix.h - Radix index
es.cpp - test app, win msvc15,  SMFL+openGL; control keys: *-+0

Менеджер кучи, позволяющий добавлять/удалять блоки разной длинны, максимально 
де фрагментируя кучу при этом. 

Можно запрашивать любой номер для индекса шириной 32 бита. Индекс не обязан быть 
последовательным.


<i>Алгоритм работы</i>
Например, нужен блок с индексом 7678 и размером 600 байт. 
Делаю запрос к куче - куча мне его выдает, гарантируя при этом, что блок будет 
максимально приближен к началу кучи.
Таким образом достигается автоматическая дефрагментация кучи.

<i>Состав</i>
Для достижения целей используется "Массив стеков "свободных блоков" кучи." (std::priority_queue).
При огромном количестве частых вставок/удалений такой метод залагает.

<i>Индексация</i>
Для индексации применяется Radix Index; Индекс можно заменить на B-Tree или любой другой.

<i>Rarix Index</i>
Radix Index это - 65536 страниц по 65536 слов. Память выделяется кратно размеру странице;

При шаге индекса в 65535 (1,65536,131071,...) можно максимально записать 65535 елементов. 
(худший случай использования индекса. Индекс будет занимать 4GB);

При шаге индекса в 1 (1,2,3,...) индекс будет занимать максимально компактное 
пространство (лучший случай использования индекса);

Индекс для 1-65535 будет занимать 128кб, для 65536 - 131070 еще 128кб. 256кб всего;
Размер индекса ограничен свободной памятью в системе;
Вычисление индекса очень быстрое и лайтовое - 2 операции - битовый And + сдвиг;

<i>Stream</i>
Сама куча - непрерывный блок памяти, с динамическим увелечением.
Можно сохранить/загрузить кучу в файл, единым блоком за 1ну операцию mem_move.

<i>Свойства</i>
Index можно перестроить по куче, его можно не сохранять/загружать (draft, leaks).

<i>TO DO</i>
-resize выделенного блока.

es.cpp - Наглядный тест, MSvc15, SFML-2.3.2. Управление: numpad *-+79 [0] - Авто
</pre>

<img src="https://raw.githubusercontent.com/USSRcoder/FS/master/thumb.png">
