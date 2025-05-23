#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>

using namespace std;

int const HASHSIZE = 16;//размер хэш-таблицы

/*Описание записи хэш - таблицы(узла связанного списка). Содержащего пару
	ключ-значение и указатель на следующий узел(элемент хэш-таблицы).*/
class Node                                             
{
	string key;
	string value;
	Node* next;

public:
	//Конструктор класса, который меняет значение полей ключ-значение и ставит указатель на следующей узел в Null
	Node(string key, string value)
	{
		this->key = key;
		this->value = value;
		next = nullptr;
	}


	/*Дальше идут геттеры и сеттеры для
	доступа к приватным полям класса*/
	string GetKey()
	{
		return key;
	}
	string GetValue()
	{
		return value;
	}
	void SetValue(string& value)
	{
		this->value = value;
	}
	Node* GetNext()
	{
		return next;
	}
	void SetNext(Node* obj)
	{
		next = obj;
	}
};

/*Описание полной хэш-таблицы, как структуры. Содержит статический массив указателей на Node(на записи хэш-таблицы)*/
class HashTable
{
	Node* hashtable[HASHSIZE] = {nullptr};

	//Функция находит хэш ключа с помощью перевода из 62-ой сс в 10-ую и деления на размер хэш-таблицы.
	unsigned int find_hash(string key)
	{
		unsigned long long result = 0;

		//тут происходит перевод из 62-й сс в 10-ю
		int size_key = key.size();
		for (int i = 0; i < size_key; ++i)
		{
			if (isalpha(key[i]))
				result += (islower(key[i]) ? key[i] - 'a' + 36 : key[i] - 'A' + 10) * pow(62, size_key - 1 - i);
			else
				result += (key[i] - '0') * pow(62, size_key - 1 - i);
		}

		return result % HASHSIZE;

	}

public:
	//Функция push добавляет элемент в хэш-таблицу. Если вдруг у разных ключей один хэш, то коллизия решается с помощью метода цепочек(связанный список).
	void push(string key, string value)
	{
		unsigned int hash = find_hash(key);
		
		if (hashtable[hash] == nullptr)
		{
			hashtable[hash] = new Node(key, value);
		}
		else
		{
			Node* node = hashtable[hash];
			while (node->GetNext() != nullptr)
			{
				if (node->GetKey() == key) //Меняет значение ключа, если он уже есть в нашей хэш-таблице.
				{
					node->SetValue(value);
					return;
				}

				node = node->GetNext();
			}

			if (node->GetKey() == key) 
			{
				node->SetValue(value);
				return;
			}

			Node* obj = new Node(key, value);
			node->SetNext(obj);
		}
	}

	//Функция get находит значение по ключу и возвращает его
	string get(string key)
	{
		unsigned int hash = find_hash(key);

		if (hashtable[hash])
		{
			Node* node = hashtable[hash];
			while (node != nullptr)
			{
				if (node->GetKey() == key)
				{
					return node->GetValue();
				}

				node = node->GetNext();
			}

			return "Ключ не найден(";
		}
	}

	//Деструктор, который удаляет каждую запись из хэш-таблицы.
	~HashTable()
	{
		for (int i = 0; i < HASHSIZE; ++i)
		{
			Node* node = hashtable[i];
			while (node != nullptr)
			{
				Node* next_el = node->GetNext();
				delete node;
				node = next_el;
			}
		}
	}
};



int main()
{
	ifstream inputFile;
	inputFile.open("inputfile.txt"); //Открываем файл с входными данными

	if (!inputFile.is_open())
		cerr << "error: not open file" << endl;
	else
	{
		vector<string> words_arr;
		vector<string> macross_arr;

		string line;
		string word;
		
		/*В цикле while мы находим директивы #define и вытаскиваем из них ключ-значение(добавляя их в macross_arr)
		или разделяем слова друг от друга(добавляя их в words_arr)*/
		while (getline(inputFile, line))
		{
			int size_line = line.size(); //Узнает размер считанной строки
			bool flag = true; //Указывает есть ли символы в строке

			for (int i = 0; i < size_line; ++i)
			{
				/*Проверяем, если символ буква, цифра, + или #, то тогда добавляем в word*/
				if (isalnum(line[i]) || line[i] == '+' || line[i] == '#' || line[i] == '_' || ispunct(line[i]) || line[i] == '\n')
				{
					if (isalnum(line[i]) || line[i] == '+' || line[i] == '#' || line[i] == '_')
						word.push_back(line[i]);
					else
					{
						words_arr.push_back(word);
						word.clear();

						string symbol;
						symbol = line[i];
						words_arr.push_back(symbol);
					}
				}

				else if (!word.empty())
				{
					//Проверяем не нашли ли мы #define
					if (line[0] == '#' && word == "#define") 
					{
						word.clear();
						int index_value = 0;
						/*Если мы нашли define, то нужно запустить новый цикл, начиная с i,
						чтобы вытащить ключ-значение*/
						for (int j = i; j < size_line; ++j)
						{
							if (isalnum(line[j]) || line[j] == '_' || line[j] == '+')
								word.push_back(line[j]);

							else if (!word.empty()) //Добавление ключа 
							{
								macross_arr.push_back(word);
								word.clear();

								index_value = j;
								break;
							} 
						}

						for (int i = index_value + 1; i < size_line; ++i)
							word.push_back(line[i]);

						if (!word.empty()) //Добавление значения
						{
							macross_arr.push_back(word);
							word.clear();
						}

						/*Ставим flag в false, потому что дошли до конца строки
						и можно выйти из цикла, чтобы начать рассматривать следующую строку*/
						flag = false; 
						break;
					}
					
					//Добавляет обычное слово в список(words_arr)(не ключ-значение)
					else 
					{
						words_arr.push_back(word);
						word.clear();
					}
				}
			}

			//Если флаг false, тогда мы уже доабвили последнее слово и нам не нужно снова это делать(если вдруг после директив у нас нет текста)
			if (!flag) 
				continue;

			if (!word.empty())
			{
				words_arr.push_back(word);
			}

		}



		HashTable hashtable;
		int size_words = words_arr.size(), size_macross = macross_arr.size();

		//Открываем файл, в который будем записывать
		ofstream outputFile;
		outputFile.open("output-file.txt");

		//Добавляем макроссы в хэш-таблицу
		for (int i = 0; i < size_macross; i+=2) 
		{
			string key = macross_arr[i], value = macross_arr[i + 1];
			hashtable.push(key, value);
		}


		//Бегаем по словам из текста и сверяемся с директивами.
		for (int i = 0; i < size_words - 1; ++i)
		{
			for (int j = 0; j < size_macross; j += 2)
			{
				if (words_arr[i] == macross_arr[j])
				{
					string new_value = hashtable.get(macross_arr[j]);
					words_arr[i] = new_value;
				}
			}


			//Корректный вывод в файл
			if (words_arr[i + 1] == "," || words_arr[i + 1] == "." || words_arr[i + 1] == ":" ||
				words_arr[i + 1] == "!" || words_arr[i + 1] == "?" || words_arr[i + 1] == ")" || words_arr[i + 1] == "(")
				outputFile << words_arr[i];
			else
				outputFile << words_arr[i] << " ";

			if (words_arr[i] == ".")
				outputFile << endl;
		}
	}
}
