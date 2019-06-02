# FS

Flast Storage - Flat memory heap, with auto de fragmentation.

<pre>

es.h - Simple memory storage, allow to allocate/deallocate varios blocks size, and auto defragment it in memory.
radix.h - Radix index
es.cpp - test app, win msvc15,  SMFL+openGL; control keys: *-+0

Менеджер кучи, позволяющий добавлять/удалять блоки разной длинны, максимально де фрагментируя кучу при этом.

Можно запрашивать любой блок для индекса. Тоесть индекс не обязан быть последовательным.
Ширина индекса = 32бит.

Например мне нужен блок с индексом 7678 и размером 600байт. 
Делаю запрос к куче -
Куча мне его выдает, гарантируя при этом, что блок будет максимально приближен к началу кучи.
Таким образом достигается автоматическая дефрагментация кучи.
Для индексации применяется Radix Index; Индекс можно заменить на B-Tree или любой другой.

es.cpp - Наглядный тест, MSvc15, SMFL. Управление : *-+0

</pre>

<img src="https://raw.githubusercontent.com/USSRcoder/FS/master/thumb.png">
