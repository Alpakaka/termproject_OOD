// reading a text file
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <vector>
#include <string>
#include <sstream>
#include <regex>

#include <typeinfo>
using namespace std;

// split �Լ� ����
vector<string> split(string input, char delimiter) {
	vector<string> result;
	stringstream ss(input);
	string tmp;

	while (getline(ss, tmp, delimiter)) {
		result.push_back(tmp);
	}
	return result;
}

// book update
vector<vector<vector<char>>> book_update(vector<vector<vector<char>>> book, vector<char> full_line) {
	vector<vector<char>> page;
	vector<char> byteline;
	book.clear();

	//count ����
	int count_byte = 0; //����Ʈ ��
	int count_line = 0; //���� ��
	int count_page = 0; //������ ��
	while (count_byte < full_line.size()) {
		if (byteline.size() == 75) {
			if (count_line == 20) {
				book.push_back(page);
				page.clear();
				count_page++;
				count_line = 0;
			}
			page.push_back(byteline);
			count_line++;
			byteline.clear();
		}
		byteline.push_back(full_line[count_byte]);
		count_byte++;
	}
	page.push_back(byteline);
	book.push_back(page);

	byteline.clear();
	page.clear();

	return book;
}

class Option {
public:
	string message = "";
	//main��
	int update_count_line = 0;
	int update_i = 0;
	//������Ʈ��
	vector<vector<vector<char>>> book;
	vector<char> full_line;
	int current_page = 0;
	//commandó����
	string command;

	virtual void print() = 0;
};

class nextpage : public Option {
public:
	nextpage(string command, vector<vector<vector<char>>> book, vector<char> full_line, int i) {
		this->book = book;
		this->full_line = full_line;
		this->current_page = i;
		this->command = command;
	}
	void print() {
		if (current_page == book.size() - 1) {
			//������ ���������
			this->update_i = -1;
			this->message = "������ �������Դϴ�.";
		}
		else {
			this->update_i = 0;
			this->update_count_line = 0;
			this->message = "";
		}
	}
};

class previouspage : public Option {
public:
	previouspage(string command, vector<vector<vector<char>>> book, vector<char> full_line, int i) {
		this->book = book;
		this->full_line = full_line;
		this->current_page = i;
		this->command = command;
	}
	void print() {
		if (current_page == 0) {
			message = "ù ��° ������ �Դϴ�.";
			this->update_count_line = -20;
			this->update_i = -1;
		}
		else {
			this->update_count_line = -40;
			message = "";
			this->update_i = -2;
		}
	}
};
class quitpage : public Option {
public:
	quitpage(string command, vector<vector<vector<char>>> book, vector<char> full_line, int i) {
		this->book = book;
		this->full_line = full_line;
		this->current_page = i;
		this->command = command;
	}
	void print() {
		ofstream writefile;
		writefile.open("test.txt");
		for (int i = 0; i < book.size(); i++) {
			for (int j = 0; j < book[i].size(); j++) {
				for (int k = 0; k < book[i][j].size(); k++) {
					writefile << book[i][j][k];
				}
			}
		}
		writefile.close();
		cout << "�����ϰ� �����Ͽ����ϴ�." << endl;
		exit(0);
	}
};
class searchpage : public Option {
public:
	searchpage(string command, vector<vector<vector<char>>> book, vector<char> full_line, int i) {
		this->book = book;
		this->full_line = full_line;
		this->current_page = i;
		this->command = command;
	}
	void print() {
		if (command.substr(1, 1) == "(" && command.substr(command.size() - 1, 1) == ")") {
			std::string search_str = command.substr(2, command.size() - 3);
			vector<char> search_vec;
			for (int iter = 0; iter < search_str.size(); iter++) {
				search_vec.push_back(search_str[iter]);
			}
			auto it = search(full_line.begin(), full_line.end(), search_vec.begin(), search_vec.end());
			int index = 0;
			if (it != full_line.end()) {
				index = it - full_line.begin();
				vector<char> search_vec;
				for (int i = index; i < full_line.size(); i++) {
					search_vec.push_back(full_line[i]);
				}
				book = book_update(book, search_vec);

				update_i = -10000;
				update_count_line = -10000;

				search_vec.clear();
			}
			else {
				this->update_i = -1;
				this->update_count_line = -20;
				this->message = "�ش� ���ڰ� ���Ͽ� �������� �ʽ��ϴ�.";
			}
		}
		else {
			this->update_i = -1;
			this->update_count_line = -20;
			this->message = "��ȣ �ȿ� �Է��� ���ڸ� �־��ּ���.";
		}
	}
};

class insertpage : public Option {
public:
	insertpage(string command, vector<vector<vector<char>>> book, vector<char> full_line, int i) {
		this->book = book;
		this->full_line = full_line;
		this->current_page = i;
		this->command = command;
	}
	void print() {
		if (command.substr(1, 1) == "(" && command.substr(command.size() - 1, 1) == ")") {
			std::string command_src = command.substr(2, command.size() - 3);
			bool checker = true;
			vector<string> result = split(command_src, ',');
			vector<int> index;
			string insert_str = result[2];

			if (result.size() != 3) {
				//�Է¾�� �ȸ´� ��� (���� ���� 3���� �ƴ� ���)
				this->message = "�Է� ����� �߸��Ǿ����ϴ�.(3���� �Է��� �ʿ��մϴ�.)";
				this->update_i = -1;
				this->update_count_line = -20;
				return;
			}
			for (int i = 0; i < 2; i++) {
				int result_int;
				try {
					result_int = stoi(result[i]);
				}
				catch (exception e) {
					checker = false;
					break;
				}
				index.push_back(result_int);
			}
			if (index[1] < 0 || index[1] > 75 || index[0] <= 0 || index[0] > 20) {
				checker = false;
			}
			if (checker) { //��Ģ�� �°� �Է��ߴٸ�
				checker = false; //�־����� Ȯ�ο�
				vector<char> update_full_line;

				int temp = 0;
				if (book[current_page].size() < 20) {
					temp = (index[0] - 1 - 20 + book[current_page].size()) * 75 + index[1] + (1500 * current_page);
					if (temp >= full_line.size()) {
						//�߰� ������ �����϶� 
						checker = false;
						this->message = "�߰� ���� ������ �Ѿ����ϴ�.";
					}
				}
				else {
					temp = (index[0] - 1) * 75 + index[1] + (1500 * current_page);
				}
				//full_line ������Ʈ
				for (int i = 0; i < full_line.size(); i++) {
					if (!checker && temp == i) {
						for (int j = 0; j < insert_str.size(); j++) {
							update_full_line.push_back(insert_str[j]);
						}
						update_full_line.push_back(full_line[i]);
						checker = false;
					}
					else {
						update_full_line.push_back(full_line[i]);
					}
				}
				this->full_line = update_full_line;
				//������Ʈ�� full_line���� book update

				this->book = book_update(book, full_line);
				this->update_i = -1;
				this->update_count_line = -20;

				result.clear();
				index.clear();
				update_full_line.clear();
			}
			else {
				this->message = "�Է� ����� �߸��Ǿ����ϴ�.(index�� ��Ŀ� �°� �Է����ּ���)";
				this->update_i = -1;
				this->update_count_line = -20;
			}
		}
		else {
			this->message = "��ȣ �ȿ� �Է����ּ���";
			this->update_i = -1;
			this->update_count_line = -20;
		}
	}
};
class deletepage : public Option {
public:
	deletepage(string command, vector<vector<vector<char>>> book, vector<char> full_line, int i) {
		this->book = book;
		this->full_line = full_line;
		this->current_page = i;
		this->command = command;
	}
	void print() {
		if (command.substr(1, 1) == "(" && command.substr(command.size() - 1, 1) == ")") {
			std::string command_src = command.substr(2, command.size() - 3);
			bool checker = true;
			vector<string> result = split(command_src, ',');
			vector<int> index;

			if (result.size() != 3) {
				//�Է¾�� �ȸ´� ��� (���� ���� 3���� �ƴ� ���)
				this->message = "�Է� ����� �߸��Ǿ����ϴ�.(3���� �Է��� �ʿ��մϴ�.)";
				this->update_i = -1;
				this->update_count_line = -20;
				return;
			}
			//index ���� ���� ���� ��ȯ
			for (int i = 0; i < 3; i++) {
				int result_int;
				try {
					result_int = stoi(result[i]);
				}
				catch (exception e) {
					checker = false;
					break;
				}
				index.push_back(result_int);
			}
			//ȭ�鿡 ���̴� ���ΰ� index�� ũ�� Ȯ��
			if (index[1] < 0 || index[1] > 75 || index[0] <= 0 || index[0] > 20) {
				checker = false;
			}
			int temp = 0;
			if (book[current_page].size() < 20) {
				temp = (index[0] - 1 - 20 + book[current_page].size()) * 75 + index[1] + (1500 * current_page);
				if (temp < full_line.size()) {
					int tmp = full_line.size() - 1 - temp;
					if (index[2] < 0 || index[2] > full_line.size() - 1 - temp) {
						//���� ����Ʈ ������ ���� ������ ū ��� 
						this->message = "���� ������ �Ѿ����ϴ�.";
						checker = false;
					}
				}
				else {
					checker = false;
					this->message = "���� ���� ������ �Ѿ����ϴ�.";
				}
			}
			else {
				temp = (index[0] - 1) * 75 + index[1] + (1500 * current_page);
			}
			if (checker) { //��Ģ�� �°� �Է��ߴٸ�
				checker = false; //�־����� Ȯ�ο�
				vector<char> update_full_line;
				//full_line ������Ʈ
				for (int i = 0; i < full_line.size(); i++) {
					if (!checker && temp == i) {
						i += index[2] - 1;
						checker = false;
					}
					else {
						update_full_line.push_back(full_line[i]);
					}
				}
				this->full_line = update_full_line;
				//������Ʈ�� full_line���� book update

				this->book = book_update(book, full_line);

				this->update_i = -1;
				this->update_count_line = -20;

				result.clear();
				index.clear();
				update_full_line.clear();
			}
			else {
				this->message += "�Է� ����� �߸��Ǿ����ϴ�.(index�� ��Ŀ� �°� �Է����ּ���)";
				this->update_i = -1;
				this->update_count_line = -20;
			}
		}
		else {
			this->message = "��ȣ �ȿ� �Է����ּ���";
			this->update_i = -1;
			this->update_count_line = -20;
		}
	}
};
class changepage : public Option {
public:
	changepage(string command, vector<vector<vector<char>>> book, vector<char> full_line, int i) {
		this->book = book;
		this->full_line = full_line;
		this->current_page = i;
		this->command = command;
	}
	void print() {
		if (command.substr(1, 1) == "(" && command.substr(command.size() - 1, 1) == ")") {
			std::string command_src = command.substr(2, command.size() - 3);
			bool checker = true;
			vector<string> result = split(command_src, ',');

			if (result.size() != 2) {
				//�Է¾�� �ȸ´� ��� (���� ���� 2���� �ƴ� ���)
				this->message = "�Է� ����� �߸��Ǿ����ϴ�.(2���� �Է��� �ʿ��մϴ�.)";
				this->update_i = -1;
				this->update_count_line = -20;
				return;
			}
			if (checker) { //��Ģ�� �°� �Է��ߴٸ�
				string line = "";
				int count_byte = 0;
				while (count_byte < full_line.size()) {
					line += full_line[count_byte];
					count_byte++;
				}
				full_line.clear();
				//full_line ������Ʈ
				count_byte = 0;
				line = regex_replace(line, regex(result[0]), result[1]);
				while (count_byte <= line.length()) {
					full_line.push_back(line[count_byte]);
					count_byte++;
				}

				//������Ʈ�� full_line���� book update
				this->book = book_update(book, full_line);

				this->update_i = -10000;
				this->update_count_line = -10000;

				result.clear();
			}
			else {
				//���� ���� �߸��� �Է� (���� Ÿ���� �߸��� ���)
				this->message = "�Է� ����� �߸��Ǿ����ϴ�.(��Ŀ� �°� �Է����ּ���)";
				this->update_i = -1;
				this->update_count_line = -20;
			}
		}
		else {
			//��ȣ ���� �߸��� �Է�
			this->message = "��ȣ �ȿ� �Է����ּ���";
			this->update_i = -1;
			this->update_count_line = -20;
		}
	}
};

//factory
class AbstractFactory {
public:
	virtual Option* getOption(string Optiontype, vector<vector<vector<char>>> book, vector<char> full_line, int i) = 0;
};

class notChangeOptionFactory : public AbstractFactory {
public:
	Option* getOption(string command, vector<vector<vector<char>>> book, vector<char> full_line, int i) {
		if (command == "n") {
			return new nextpage(command, book, full_line, i);
		}
		else if (command == "p") {
			return new previouspage(command, book, full_line, i);
		}
		else if (command.substr(0, 1) == "s") {
			return new searchpage(command, book, full_line, i);
		}
		else if (command == "t") {
			return new quitpage(command, book, full_line, i);
		}
		return nullptr;
	}
};
class ChangeOptionFactory : public AbstractFactory {
public:
	Option* getOption(string command, vector<vector<vector<char>>> book, vector<char> full_line, int i) {
		if (command.substr(0, 1) == "i") {
			return new insertpage(command, book, full_line, i);
		}
		else if (command.substr(0, 1) == "d") {
			return new deletepage(command, book, full_line, i);
		}
		else if (command.substr(0, 1) == "c") {
			return new changepage(command, book, full_line, i);
		}
		return nullptr;
	}
};
class FactoryProducer {
public:
	AbstractFactory* getFactory(bool changed) {
		if (changed) {
			return new ChangeOptionFactory();
		}
		else {
			return new notChangeOptionFactory();
		}
	}
};

int main() {
	string line;
	string command = "";
	ifstream myfile("test.txt");
	vector<char> byteline;
	vector<vector<char>> page;
	vector<vector<vector<char>>> book;
	vector<char> full_line;
	string message = "";

	//count ����
	int count_byte = 0; //����Ʈ ��
	int count_line = 0; //���� ��
	int count_page = 0; //������ ��
	if (myfile.is_open())
	{
		//���Ͽ��� �о����
		getline(myfile, line);
		while (count_byte <= line.length()) {
			if (byteline.size() == 75) {
				if (count_line == 20) {
					book.push_back(page);
					page.clear();
					count_page++;
					count_line = 0;
				}
				page.push_back(byteline);
				count_line++;
				byteline.clear();
			}
			full_line.push_back(line[count_byte]);
			byteline.push_back(line[count_byte]);
			count_byte++;
		}
		page.push_back(byteline);
		book.push_back(page);
		byteline.clear();
		page.clear();
	}
	else cout << "Unable to open file";
	myfile.close();

	count_page++;
	count_line = 1; //��� �տ� ���� ����
	count_byte = 0;

	//����ϱ�
	for (int i = 0; i < count_page; i++) {
		if (book[i].size() < 20) {
			//������ ���� 20���� ���� ��� �ռ� �� ������� 20�� �����ֱ�
			count_line = ((book.size() - 1) * 20) - book[i - 1].size() + book[i].size() + 1;
			for (int a = book[i].size(); a < book[i - 1].size(); a++) {
				cout << count_line << "| ";
				for (int b = 0; b < book[i - 1][a].size(); b++) {
					cout << book[i - 1][a][b];
				}
				count_line++;
				cout << "\n";
			}
		}
		for (int j = 0; j < book[i].size(); j++) {
			cout << count_line << "| ";
			for (int k = 0; k < book[i][j].size(); k++) {
				cout << book[i][j][k];
			}
			count_line++;
			cout << "\n";
		}
		cout << "----------------------------------------------------------------------------------------\n";
		cout << "n:����������, p:����������, i:����, d:����, c:����, s:ã��, t:����������\n";
		cout << "----------------------------------------------------------------------------------------\n";
		cout << "(�ָܼ޽���)";
		cout << message << "\n";
		cout << "----------------------------------------------------------------------------------------\n";
		cout << "�Է�: ";
		getline(cin, command);
		cout << "----------------------------------------------------------------------------------------\n";
		command.erase(remove(command.begin(), command.end(), ' '), command.end());

		FactoryProducer* f = new FactoryProducer();
		AbstractFactory* shapeFactory;
		if (command.substr(0, 1) == "s" || command.substr(0, 1) == "n" || command.substr(0, 1) == "p" || command.substr(0, 1) == "t") {
			shapeFactory = f->getFactory(false);
		}
		else {
			shapeFactory = f->getFactory(true);
		}
		Option* option = shapeFactory->getOption(command, book, full_line, i);
		if (option != nullptr) {
			option->print();
			message = option->message;
			count_line += option->update_count_line;
			if (count_line < 1) {
				count_line = 1;
			}
			i += option->update_i;
			if (i < -1) {
				i = -1;
			}

			book = option->book;
			full_line = option->full_line;
		}
		else {
			count_line -= 20;
			i -= 1;
			message = "�ɼ� ������ �ùٸ��� �ʽ��ϴ�.";
		}
		delete f;
	}

	return 0;
}