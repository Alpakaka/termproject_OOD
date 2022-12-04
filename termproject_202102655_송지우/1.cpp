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

// split 함수 구현
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

	//count 변수
	int count_byte = 0; //바이트 수
	int count_line = 0; //라인 수
	int count_page = 0; //페이지 수
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
	//main용
	int update_count_line = 0;
	int update_i = 0;
	//업데이트용
	vector<vector<vector<char>>> book;
	vector<char> full_line;
	int current_page = 0;
	//command처리용
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
			//마지막 페이지라면
			this->update_i = -1;
			this->message = "마지막 페이지입니다.";
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
			message = "첫 번째 페이지 입니다.";
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
		cout << "저장하고 종료하였습니다." << endl;
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
				this->message = "해당 문자가 파일에 존재하지 않습니다.";
			}
		}
		else {
			this->update_i = -1;
			this->update_count_line = -20;
			this->message = "괄호 안에 입력할 문자를 넣어주세요.";
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
				//입력양식 안맞는 경우 (인자 수가 3개가 아닌 경우)
				this->message = "입력 양식이 잘못되었습니다.(3개의 입력이 필요합니다.)";
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
			if (checker) { //규칙에 맞게 입력했다면
				checker = false; //넣었는지 확인용
				vector<char> update_full_line;

				int temp = 0;
				if (book[current_page].size() < 20) {
					temp = (index[0] - 1 - 20 + book[current_page].size()) * 75 + index[1] + (1500 * current_page);
					if (temp >= full_line.size()) {
						//추가 범위가 음수일때 
						checker = false;
						this->message = "추가 가능 범위를 넘었습니다.";
					}
				}
				else {
					temp = (index[0] - 1) * 75 + index[1] + (1500 * current_page);
				}
				//full_line 업데이트
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
				//업데이트된 full_line으로 book update

				this->book = book_update(book, full_line);
				this->update_i = -1;
				this->update_count_line = -20;

				result.clear();
				index.clear();
				update_full_line.clear();
			}
			else {
				this->message = "입력 양식이 잘못되었습니다.(index를 양식에 맞게 입력해주세요)";
				this->update_i = -1;
				this->update_count_line = -20;
			}
		}
		else {
			this->message = "괄호 안에 입력해주세요";
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
				//입력양식 안맞는 경우 (인자 수가 3개가 아닌 경우)
				this->message = "입력 양식이 잘못되었습니다.(3개의 입력이 필요합니다.)";
				this->update_i = -1;
				this->update_count_line = -20;
				return;
			}
			//index 관련 인자 수로 변환
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
			//화면에 보이는 라인과 index의 크기 확인
			if (index[1] < 0 || index[1] > 75 || index[0] <= 0 || index[0] > 20) {
				checker = false;
			}
			int temp = 0;
			if (book[current_page].size() < 20) {
				temp = (index[0] - 1 - 20 + book[current_page].size()) * 75 + index[1] + (1500 * current_page);
				if (temp < full_line.size()) {
					int tmp = full_line.size() - 1 - temp;
					if (index[2] < 0 || index[2] > full_line.size() - 1 - temp) {
						//삭제 바이트 범위가 남은 값보다 큰 경우 
						this->message = "삭제 범위를 넘었습니다.";
						checker = false;
					}
				}
				else {
					checker = false;
					this->message = "삭제 가능 범위를 넘었습니다.";
				}
			}
			else {
				temp = (index[0] - 1) * 75 + index[1] + (1500 * current_page);
			}
			if (checker) { //규칙에 맞게 입력했다면
				checker = false; //넣었는지 확인용
				vector<char> update_full_line;
				//full_line 없데이트
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
				//업데이트된 full_line으로 book update

				this->book = book_update(book, full_line);

				this->update_i = -1;
				this->update_count_line = -20;

				result.clear();
				index.clear();
				update_full_line.clear();
			}
			else {
				this->message += "입력 양식이 잘못되었습니다.(index를 양식에 맞게 입력해주세요)";
				this->update_i = -1;
				this->update_count_line = -20;
			}
		}
		else {
			this->message = "괄호 안에 입력해주세요";
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
				//입력양식 안맞는 경우 (인자 수가 2개가 아닌 경우)
				this->message = "입력 양식이 잘못되었습니다.(2개의 입력이 필요합니다.)";
				this->update_i = -1;
				this->update_count_line = -20;
				return;
			}
			if (checker) { //규칙에 맞게 입력했다면
				string line = "";
				int count_byte = 0;
				while (count_byte < full_line.size()) {
					line += full_line[count_byte];
					count_byte++;
				}
				full_line.clear();
				//full_line 업데이트
				count_byte = 0;
				line = regex_replace(line, regex(result[0]), result[1]);
				while (count_byte <= line.length()) {
					full_line.push_back(line[count_byte]);
					count_byte++;
				}

				//업데이트된 full_line으로 book update
				this->book = book_update(book, full_line);

				this->update_i = -10000;
				this->update_count_line = -10000;

				result.clear();
			}
			else {
				//인자 관련 잘못된 입력 (인자 타입이 잘못된 경우)
				this->message = "입력 양식이 잘못되었습니다.(양식에 맞게 입력해주세요)";
				this->update_i = -1;
				this->update_count_line = -20;
			}
		}
		else {
			//괄호 관련 잘못된 입력
			this->message = "괄호 안에 입력해주세요";
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

	//count 변수
	int count_byte = 0; //바이트 수
	int count_line = 0; //라인 수
	int count_page = 0; //페이지 수
	if (myfile.is_open())
	{
		//파일에서 읽어오기
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
	count_line = 1; //출력 앞에 쓰는 변수
	count_byte = 0;

	//출력하기
	for (int i = 0; i < count_page; i++) {
		if (book[i].size() < 20) {
			//마지막 줄이 20보다 작은 경우 앞선 줄 출력으로 20줄 맞춰주기
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
		cout << "n:다음페이지, p:이전페이지, i:삽입, d:삭제, c:변경, s:찾기, t:저장후종료\n";
		cout << "----------------------------------------------------------------------------------------\n";
		cout << "(콘솔메시지)";
		cout << message << "\n";
		cout << "----------------------------------------------------------------------------------------\n";
		cout << "입력: ";
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
			message = "옵션 형식이 올바르지 않습니다.";
		}
		delete f;
	}

	return 0;
}