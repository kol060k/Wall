#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <ctime>
#include <cmath>

using namespace std;

// Структура данных для thread'ов
struct pthread_data{
	int** arr;
	int Height, Length;
	pthread_mutex_t* mutex;
	int me;
	int Tmin, Tmax;
};

void print_wall(int** arr, int Height, int Length) {
	system("clear");
	for (int i = Height - 1; i >= 0; i--) {
		for (int j = 0; j < 2 * Length; j++) {
			if (arr[i][j] == 0)
				cout << ' ';
			else
				cout << '=';
		}
		cout << endl;
	}
	for (int i = 0; i < 2 * Length; i++)
		cout << '^';
	cout << endl << endl;
}

// Функция постройки стены thread'ом
void* building(void* data_) {
	pthread_data data = *(pthread_data*) data_;
	int cur_h = data.me;	// Начальные положения
	int cur_l = 0;
	while (cur_h < data.Height) {
		// Заходим в критическую секцию и определяем, можно ли класть кирпич на текущую позицию
		pthread_mutex_lock(data.mutex);
		if (data.me == 0) {	// Если это отец
			if (cur_h > 0)
				if (data.arr[cur_h - 1][cur_l + 1] == 0) {
					pthread_mutex_unlock(data.mutex);
					usleep(1000);
					continue;
				}
		}
		else {		// Если это сын
			if ((cur_l == 0) || (cur_l == data.Length * 2 - 1)) {
				if (data.arr[cur_h - 1][cur_l] == 0) {
					pthread_mutex_unlock(data.mutex);
					usleep(1000);
					continue;
				}
			}
			else {
				if (data.arr[cur_h - 1][cur_l + 1] == 0) {
					pthread_mutex_unlock(data.mutex);
					usleep(1000);
					continue;
				}
			}
		}
		pthread_mutex_unlock(data.mutex);

		// Когда мы поняли, что кирпич можно класть, кладём его
		int sleep_time = data.Tmin * 1000000 + round(((double)rand() / RAND_MAX) * (data.Tmax - data.Tmin) * 1000000);
		usleep(sleep_time);

		// Отметим в массиве положенный кирпич и напечатаем стену
		pthread_mutex_lock(data.mutex);
		if (data.me == 0) {	// Если это отец
			data.arr[cur_h][cur_l] = 1;
			data.arr[cur_h][cur_l + 1] = 1;
			if (cur_l < data.Length * 2 - 2)
				cur_l += 2;
			else {
				cur_l = 0;
				cur_h += 2;
			}
		}
		else {		// Если это сын
			if (cur_l == 0) {
				data.arr[cur_h][cur_l] = 1;
				cur_l++;
			} else
			if (cur_l == data.Length * 2 - 1) {
				data.arr[cur_h][cur_l] = 1;
				cur_l = 0;
				cur_h += 2;
			} else
			if ((cur_l > 0) && (cur_l < data.Length * 2 - 1)) {
				data.arr[cur_h][cur_l] = 1;
				data.arr[cur_h][cur_l + 1] = 1;
				cur_l += 2;
			}
		}
		print_wall(data.arr, data.Height, data.Length);
		pthread_mutex_unlock(data.mutex);
	}
}

int main () {
	srand(time(NULL));
	// Ввод начальных данных задачи. 0 - отец, 1 - сын
	int Height, Length;
	int Tmin[2], Tmax[2];
	cin >> Height >> Length;
	cin >> Tmin[0] >> Tmax[0];
	cin >> Tmin[1] >> Tmax[1];
	if ((Height < 0) || (Length < 0) || (Tmin[0] < 0) || (Tmin[1] < 0)
		       || (Tmax[0] < 0) || (Tmax[0] < 0) || (Tmin[0] > Tmax[0]) || (Tmin[1] > Tmax[1])) {
		cout << "Wrong input" << endl;
		return 1;
	}
	if ((Height == 0) || (Length == 0)) {
		cout << "Wall is built!" << endl;
		return 0;
	}
	system("clear");

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

	// Завершение работы
	cout << "Wall is built!" << endl;
	for (int i = 0; i < Height; i++)
		delete[] arr[i];
	delete[] arr;
	return 0;
}
