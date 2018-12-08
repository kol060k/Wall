#include <iostream>
#include <pthread.h>

using namespace std;

// Структура данных для thread'ов
struct pthread_data{
	int** arr;
	int Height, Length;
	pthread_mutex_t* mutex;
	int me;
	int Tmin, Tmax;
};

// Функция постройки стены thread'ом
void* building(void* data_) {
	pthread_data data = *(pthread_data*) data_;
	if (data.me == 0)
		cout << "Father is here!" << endl;
	else
		cout << "Son is here!" << endl;
//	int cur_h = i;	// Начальные положения
//	int cur_l = 0;
//	while (cur_h < data.Height) {
//		
//	}
}

int main () {
	// Ввод начальных данных задачи. 0 - отец, 1 - сын
	int Height, Length;
	int Tmin[2], Tmax[2];
	cin >> Height >> Length;
	cin >> Tmin[0] >> Tmax[0];
	cin >> Tmin[1] >> Tmax[1];

	// Инициализация переменных и массивов, инициализация thread'ов
	int** arr = new int*[Height];
	for (int i = 0; i < Height; i++)
		arr[i] = new int[Length * 2];
	for (int i = 0; i < Height; i++)
		for (int j = 0; j < Length * 2; j++)
			arr[i][j] = 0;
	pthread_data data[2];
	pthread_mutex_t mutex;
	pthread_mutex_init(&mutex, NULL);
	for (int i = 0; i < 2; i++) {
		data[i].mutex = &mutex;
		data[i].me = i;
		data[i].Tmin = Tmin[i];
		data[i].Tmax = Tmax[i];
		data[i].Height = Height;
		data[i].Length = Length;
		data[i].arr = arr;
	}
	pthread_t threads[2];
	for (int i = 0; i < 2; i++)
		pthread_create(&threads[i], NULL, building, &data[i]);
	for (int i = 0; i < 2; i++)
		pthread_join(threads[i], NULL);

	for (int i = 0; i < Height; i++)
		delete[] arr[i];
	delete[] arr;
	return 0;
}
