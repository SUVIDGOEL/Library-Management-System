#include<iostream>
#include<chrono>
#include<vector>
#include<ctime>
#include<fstream>
#include<algorithm>
#include<iomanip>
#include<cctype>

using namespace std;
#define ll long long

int monthDays[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

string currentdate(){
    auto now = chrono::system_clock::now();
    time_t present_time = chrono::system_clock::to_time_t(now);
    tm *localTime = localtime(&present_time);

    stringstream ss;
    ss << setfill('0') << setw(2) << localTime->tm_mday << "/" << setfill('0') << setw(2) << (localTime->tm_mon + 1) << "/"
       << (localTime->tm_year + 1900);

    return ss.str();
}

string formatdate(tm &time_s){
    stringstream ss;
    ss << setfill('0') << setw(2) << time_s.tm_mday << "/" << setfill('0') << setw(2) << (time_s.tm_mon + 1) << "/"
       << (time_s.tm_year + 1900);

    return ss.str();
}

string getfuturedate(int d) {

    auto now = chrono::system_clock::now();

    auto futureTime = now + chrono::hours(24*d);
    time_t future_time = chrono::system_clock::to_time_t(futureTime);
    tm* localTime = localtime(&future_time);
    return formatdate(*localTime);
}

bool isLeap(int y) {
    return (y % 4 == 0 && y % 100 != 0) || (y % 400 == 0);
}

int countDays(int d, int m, int y) {
    int total = y * 365 + (y / 4) - (y / 100) + (y / 400);
    for (int i = 0; i < m - 1; i++) total += monthDays[i];
    if (m > 2 && isLeap(y)) total++;
    return total + d;
}

int daysBetween(string date1, string date2) {
    int d1 = stoi(date1.substr(0, 2)), m1 = stoi(date1.substr(3, 2)), y1 = stoi(date1.substr(6, 4));
    int d2 = stoi(date2.substr(0, 2)), m2 = stoi(date2.substr(3, 2)), y2 = stoi(date2.substr(6, 4));
    return abs(countDays(d2, m2, y2) - countDays(d1, m1, y1));
}

class bookstatus{
public:
    static const int AVAILABLE = 0;
    static const int BORROWED = 1;
    static const int RESERVED = 2;    
};

string actualbookstatus(int status){
    if(status == bookstatus::AVAILABLE) return "Available";
    if(status == bookstatus::BORROWED) return "Borrowed";
    if(status == bookstatus::RESERVED) return "Reserved";
    else return "Status not Known";
}

class Book{
public:
    int id,year;
    int reserverid;
    string title, author, publisher, ISBN;
    int status;
    time_t reserveTime;

    Book (int id, string title, string author, string publisher, int year, string ISBN){
        this->id = id;
        this->title = title;
        this->author = author;
        this->publisher = publisher;
        this->year = year;
        this->ISBN = ISBN;
        this->status = bookstatus::AVAILABLE;
        this->reserveTime = 0;
        this->reserverid = -1;
    }

    void print(){
        cout << "BookID: " << id << "\n"
             << "Title: " << title << "\n"
             << "Author: " << author << "\n"
             << "Publisher: " << publisher << "\n"
             << "Year: " << year << "\n"
             << "ISBN: " << ISBN << "\n"
             << "Status: " << actualbookstatus(status);
        if(status == bookstatus::RESERVED) {
            if(reserveTime != 0) {
                char buff[80];
                strftime(buff, sizeof(buff), "%d/%m/%Y", localtime(&reserveTime));
                cout << " - Reserved on: " << buff << "\n";
            }
            else
                cout << "\n";
        }
        else cout << "\n";
        cout << "----------------------\n";      
    }
};

class borrowedbook{
public:
    int bookid;
    string borrowdate;
    borrowedbook(int bookid, string borrowdate){
        this->bookid = bookid;
        this->borrowdate = borrowdate;
    }
};

class returnedbook{
public:
    int bookid;
    string borrowdate;
    string returndate;
    returnedbook(int bookid, string borrowdate, string returndate){
        this->bookid = bookid;
        this->borrowdate = borrowdate;
        this->returndate = returndate;
    }
};

class reservedbook{
public:    
    int bookid;
    int userid;
    string reservedate;
    reservedbook(int bookid, int userid, string reservedate){
        this->bookid = bookid;
        this->userid = userid;
        this->reservedate = reservedate;
    }
};

class Account{
private:
    vector<borrowedbook> current;
    vector<returnedbook> history;
    vector<reservedbook> reserved;
    ll fine;
public:
    Account (){
        this->fine = 0;
    }

    void borrowBook(int bookid, string borrowdate, int reserverid){
        current.push_back({bookid, borrowdate});
        if(reserverid!=-1){
            int n = reserved.size();
            for(int i=0;i<n;i++){
                if(reserved[i].bookid == bookid){
                    reserved.erase(reserved.begin() + i);
                }
            }
        }
    }

    bool returnBook(int bookid, string returndate, int allowedDays, int fineperday){
        int n = current.size();
        for(int i=0;i<n;i++){
            if(current[i].bookid == bookid){
                string borrdate = current[i].borrowdate;
                int keptDays = daysBetween(borrdate, returndate);
                if(keptDays > allowedDays){
                    fine += (keptDays-allowedDays)*fineperday;
                }
                history.push_back({bookid, borrdate, returndate});
                current.erase(current.begin()+i);
                return true;
            }
            else return false;
        }
        return false;
    }

    void reserveBook(int bookid, int userid, string reservedate){
        reserved.push_back({bookid, userid, reservedate});
    }

    void finepayment(){
        //Fine has been paid, therefore set fine to zero
        fine = 0;
        cout << "Fine has been Cleared Successfully!\n";
    }

    vector<borrowedbook>& getcurrentbooks() {
        return current;
    }

    vector<returnedbook>& getreturnedbooks(){
        return history;
    } 

    vector<reservedbook>& getreservedbooks(){
        return reserved;
    } 

    void setFine(ll f) {
        fine = f;
    }

    ll getfine() const {
        return fine;
    }

};

class Library;

class User{
private:
    string username, password;
    Account account;

public:
    Account& getaccount() {
        return account;
    }

    const Account& getaccount()const{
        return account;
    }
    int id;
    string name, type;
   

    User(int id, string name, string type, string username, string password){
        this->id = id;
        this->name = name;
        this->type = type;
        this->username = username;
        this->password = password;
    }

    void changename(string name){
        this->name = name;
    }

    string getusername() const {
        return this->username;
    }

    string getpassword() const{
        return this->password;
    }

    void changeusername(string username) {
        this->username = username;
        cout << "Username changed Successfully!\n";
    }

    void changepassword(string password) {
        this->password = password;
        cout << "Password changed Successfully!\n";
    }


    //Functions to check User's Username and and Password are correct or not
    bool checkusername(string username){
        return (this->username == username);
    }

    bool checkpassword(string password){
        return (this->password == password);
    }


    virtual bool borrowbook(Library &lib, int bookid, string borrowdate) = 0;
    virtual bool returnbook(Library &lib, int bookid, string returndate) = 0;
    virtual bool reservebook(Library &lib, int bookId, string reservedate) = 0;

    void payfine(){
        if(getaccount().getfine() == 0){
            cout << "There is no Outstanding Fine!\n";
            return;
        }
        cout << "Outstanding Amount: " << getaccount().getfine() << " rupees\n";
        return getaccount().finepayment();
    }

    virtual void print(){
        cout << "User ID: " << id << "\n" << "Name: " << name << "\n" << "Type: " << type << "\n-----------------\n";
    }

    void printaccount(Library *lib){
        cout << "User ID: " << id << "\n";
        cout << "Name: " << name << "\n";
        cout << "Type: " << type << "\n";
        cout << "Username: " << getusername() << "\n";
        cout << "Password: " << getpassword() << "\n";
        if(this->type != "Librarian"){
            cout << "Books Borrowed Currently: " << getaccount().getcurrentbooks().size() <<"\n";
            if(getaccount().getcurrentbooks().size() != 0){
                cout << "Borrowed Book ID's: ";
                int j = 0;
                for (auto &rec : getaccount().getcurrentbooks()){
                    cout << rec.bookid;
                    if(j!=(getaccount().getcurrentbooks().size() - 1)) cout << ", ";
                    j++;
                }
                cout << "\n";
            }
            cout << "Books Borrowed and Returned Previously: " << getaccount().getreturnedbooks().size() << "\n";
            if(getaccount().getreturnedbooks().size()!=0){
                cout << "Returned Book ID's: ";
                int j = 0;
                for (auto &rec : getaccount().getreturnedbooks()){
                    cout << rec.bookid;
                    if(j!=(getaccount().getreturnedbooks().size() - 1)) cout << ", ";
                    j++;
                }
                cout << "\n";
            }
            cout << "Books currently Reserved: " << getaccount().getreservedbooks().size() << "\n";
            if(getaccount().getreservedbooks().size()!=0){
                cout << "Reserved Book ID's: ";
                int j = 0;
                for (auto &rec : getaccount().getreservedbooks()){
                    cout << rec.bookid;
                    if(j!=(getaccount().getreservedbooks().size() - 1)) cout << ", ";
                    j++;
                }
                cout << "\n";
            }
            cout << "Outstanding Fine: " << getaccount().getfine() << " rupees\n";
        }
    }

    int getuserID(){
        return this->id;
    }
};


class Student : public User{
public:
    const int maxbooks = 3;
    const int maxreserve = 2;
    const int allowedDays = 15;
    const int fineperday = 10;

    Student(int id, string name, string username, string password) : User(id, name, "Student", username, password){}

    bool borrowbook(Library &lib, int bookid, string borrowdate) override;
    bool returnbook(Library &lib, int bookid, string returndate) override;
    bool reservebook(Library &lib, int bookId, string reservedate) override;

};

class Faculty:public User{
public:
    const int maxbooks = 5;
    const int maxreserve = 3;
    const int allowedDays = 30;

    Faculty(int id, string name, string username, string password) : User(id, name, "Faculty", username, password) {}

    bool borrowbook(Library &lib, int bookid, string borrowdate) override;
    bool returnbook(Library &lib, int bookid, string returndate) override;
    bool reservebook(Library &lib, int bookId, string reservedate) override;

};

class Librarian:public User{
public:
    Librarian(int id, string name, string username, string password) : User(id, name, "Librarian", username, password){}

    bool borrowbook(Library &lib, int bookId, string currentDay)override{
        cout << "Librarians cannot borrow books." << endl;
        return false;
    }
    bool returnbook(Library &lib, int bookId, string returnDay)override{
        cout << "Librarians do not return books." << endl;
        return false;
    }
    bool reservebook(Library &lib, int bookId, string reservedate) override {
        cout << "Librarians do not reserve books." << endl;
        return false;
    }

    void addbook(Library &lib, const Book &book);
    void removebook(Library &lib, int bookID);
    void updatebook(Library &lib, int bookID, Book &updatedbook) ;
    void adduser(Library &lib, User* user);
    void removeuser(Library& lib, int userID);
};

string serializeBorrowedBooks(const vector<borrowedbook>& current) {
    string s;
    for (size_t i = 0; i < current.size(); i++){
        s += to_string(current[i].bookid) + "," + current[i].borrowdate;
        if(i != current.size()-1) s += ";";
    }
    return s;
}

string serializeReturnedBooks(const vector<returnedbook>& history) {
    string s;
    for (size_t i = 0; i < history.size(); i++){
        s += to_string(history[i].bookid) + "," + history[i].borrowdate + "," + history[i].returndate;
        if(i != history.size()-1) s += ";";
    }
    return s;
}

string serializeReservedBooks(const vector<reservedbook>& reserved) {
    string s;
    for (size_t i = 0; i < reserved.size(); i++){
        s += to_string(reserved[i].bookid) + "," + to_string(reserved[i].userid) + "," + reserved[i].reservedate;
        if(i != reserved.size()-1) s += ";";
    }
    return s;
}

vector<borrowedbook> deserializeBorrowedBooks(const string& s) {
    vector<borrowedbook> curr;
    if(s.empty()) return curr;
    size_t start = 0, end;
    while((end = s.find(';', start)) != string::npos) {
        string token = s.substr(start, end - start);
        size_t commaPos = token.find(',');
        if(commaPos != string::npos) {
            int bookid = stoi(token.substr(0, commaPos));
            string borrowdate = token.substr(commaPos+1);
            curr.push_back(borrowedbook(bookid, borrowdate));
        }
        start = end + 1;
    }
    // Last record
    if(start < s.size()){
        string token = s.substr(start);
        size_t commaPos = token.find(',');
        if(commaPos != string::npos) {
            int bookid = stoi(token.substr(0, commaPos));
            string borrowdate = token.substr(commaPos+1);
            curr.push_back(borrowedbook(bookid, borrowdate));
        }
    }
    return curr;
}

vector<returnedbook> deserializeReturnedBooks(const string& s) {
    vector<returnedbook> hist;
    if(s.empty()) return hist;
    size_t start = 0, end;
    while((end = s.find(';', start)) != string::npos) {
        string token = s.substr(start, end - start);
        size_t first = token.find(',');
        size_t second = token.find(',', first+1);
        if(first != string::npos && second != string::npos) {
            int bookid = stoi(token.substr(0, first));
            string borrowdate = token.substr(first+1, second - first - 1);
            string returndate = token.substr(second+1);
            hist.push_back(returnedbook(bookid, borrowdate, returndate));
        }
        start = end + 1;
    }
    if(start < s.size()){
        string token = s.substr(start);
        size_t first = token.find(',');
        size_t second = token.find(',', first+1);
        if(first != string::npos && second != string::npos) {
            int bookid = stoi(token.substr(0, first));
            string borrowdate = token.substr(first+1, second - first - 1);
            string returndate = token.substr(second+1);
            hist.push_back(returnedbook(bookid, borrowdate, returndate));
        }
    }
    return hist;
}

vector<reservedbook> deserializeReservedBooks(const string& s) {
    vector<reservedbook> res;
    if(s.empty()) return res;
    size_t start = 0, end;
    while((end = s.find(';', start)) != string::npos) {
        string token = s.substr(start, end - start);
        size_t first = token.find(',');
        size_t second = token.find(',', first+1);
        if(first != string::npos && second != string::npos) {
            int bookid = stoi(token.substr(0, first));
            int userid = stoi(token.substr(first+1, second - first - 1));
            string reservedate = token.substr(second+1);
            res.push_back(reservedbook(bookid, userid, reservedate));
        }
        start = end + 1;
    }
    if(start < s.size()){
        string token = s.substr(start);
        size_t first = token.find(',');
        size_t second = token.find(',', first+1);
        if(first != string::npos && second != string::npos) {
            int bookid = stoi(token.substr(0, first));
            int userid = stoi(token.substr(first+1, second - first - 1));
            string reservedate = token.substr(second+1);
            res.push_back(reservedbook(bookid, userid, reservedate));
        }
    }
    return res;
}


class Library{
public: 
    vector<User*> users;
    vector<Book> books;

    ~Library(){
        for(User* u : users) delete u;
        users.clear();
    }

    void loadData() {
        loadBooks();
        loadUsers();
    }

    void saveData() {
        saveBooks();
        saveUsers();
    }

    void loadBooks(){
        ifstream ifs("books.txt");
        if(!ifs){
            books.clear();
            books.push_back(Book(1, "C++ Primer", "Lippman", "Addison-Wesley", 2012, "ISBN001"));
            books.push_back(Book(2, "Effective Modern C++", "Meyers", "O'Reilly", 2014, "ISBN002"));
            books.push_back(Book(3, "Design Patterns", "Gamma et al.", "Addison-Wesley", 1994, "ISBN003"));
            books.push_back(Book(4, "Clean Code", "Robert C. Martin", "Prentice Hall", 2008, "ISBN004"));
            books.push_back(Book(5, "The C++ Programming Language", "Stroustrup", "Addison-Wesley", 2013, "ISBN005"));
            books.push_back(Book(6, "The C++ Standard Library", "Josuttis", "O'Reilly", 2001, "ISBN006"));
            books.push_back(Book(7, "Accelerated C++", "Koenig & Moo", "Addison-Wesley", 2000, "ISBN007"));
            books.push_back(Book(8, "C++ Concurrency in Action", "Anthony Williams", "Manning", 2012, "ISBN008"));
            books.push_back(Book(9, "Modern C++ Design", "Andrei Alexandrescu", "Addison-Wesley", 2001, "ISBN009"));
            books.push_back(Book(10, "Programming: Principles and Practice Using C++", "Bjarne Stroustrup", "Addison-Wesley", 2014, "ISBN010"));
            return;
        }
        books.clear();
        string line;
        while(getline(ifs, line)){
            if(line.empty()) continue;
            istringstream iss(line);
            string token;
            int id, year, status, reserverid;
            string title, author, publisher, isbn;
            time_t reserveTime;
            getline(iss, token, '|'); id = stoi(token);
            getline(iss, title, '|');
            getline(iss, author, '|');
            getline(iss, publisher, '|');
            getline(iss, token, '|'); year = stoi(token);
            getline(iss, isbn, '|');
            getline(iss, token, '|'); status = stoi(token);
            getline(iss, token, '|'); reserveTime = static_cast<time_t>(stoll(token));
            getline(iss, token, '|'); reserverid = stoi(token);
            Book b(id, title, author, publisher, year, isbn);
            b.status = status;
            b.reserveTime = reserveTime;
            b.reserverid = reserverid;
            books.push_back(b);
        }
        ifs.close();
    }

    void saveBooks(){
        ofstream ofs("books.txt");
        if(!ofs){
            cerr << "Error opening books.txt for writing." << endl;
            return;
        }
        for(const auto &book: books){
            ofs << book.id << "|"
                << book.title << "|"
                << book.author << "|"
                << book.publisher << "|"
                << book.year << "|"
                << book.ISBN << "|"
                << book.status << "|"
                << book.reserveTime << "|"
                << book.reserverid << "\n";
        }
        ofs.close();
    }

    void saveUsers(){
        ofstream ofs("users.txt");
        if(!ofs){
            cerr << "Error opening users.txt for writing." << endl;
            return;
        }
        for(User* user : users){
            // Serialize account data:
            string curr = "";
            string ret = "";
            string res = "";
            // borrowed books
            {
                vector<borrowedbook> cb = user->getaccount().getcurrentbooks();
                for(size_t i = 0; i < cb.size(); i++){
                    curr += to_string(cb[i].bookid) + "," + cb[i].borrowdate;
                    if(i != cb.size()-1) curr += ";";
                }
            }
            // returned books
            {
                vector<returnedbook> rb = user->getaccount().getreturnedbooks();
                for(size_t i = 0; i < rb.size(); i++){
                    ret += to_string(rb[i].bookid) + "," + rb[i].borrowdate + "," + rb[i].returndate;
                    if(i != rb.size()-1) ret += ";";
                }
            }
            // reserved books
            {
                vector<reservedbook> rs = user->getaccount().getreservedbooks();
                for(size_t i = 0; i < rs.size(); i++){
                    res += to_string(rs[i].bookid) + "," + to_string(rs[i].userid) + "," + rs[i].reservedate;
                    if(i != rs.size()-1) res += ";";
                }
            }
            ofs << user->id << "|"
                << user->name << "|"
                << user->type << "|"
                << user->getusername() << "|"
                << user->getpassword() << "|"
                << user->getaccount().getfine() << "|"
                << curr << "|"
                << ret << "|"
                << res << "\n";
        }
        ofs.close();
    }
    

    void loadUsers(){
        ifstream ifs("users.txt");
        if(!ifs){
            users.clear();
            users.push_back(new Student(1, "Alice", "alice", "pass"));
            users.push_back(new Student(2, "Bob", "bob", "pass"));
            users.push_back(new Student(3, "Charlie", "charlie", "pass"));
            users.push_back(new Student(4, "Dana", "dana", "pass"));
            users.push_back(new Student(5, "Eve", "eve", "pass"));
            users.push_back(new Faculty(6, "Dr. Smith", "drsmith", "pass"));
            users.push_back(new Faculty(7, "Dr. Johnson", "drjohnson", "pass"));
            users.push_back(new Faculty(8, "Dr. Williams", "drwilliams", "pass"));
            users.push_back(new Librarian(9, "Mr. Librarian", "librarian", "pass"));
            return;
        }
        users.clear();
        string line;
        while(getline(ifs, line)){
            // if(line.empty()) continue;
            istringstream iss(line);
            string token;
            int id;
            string name, type, username, password;
            ll fine;
            string curr, ret, res;
            getline(iss, token, '|'); id = stoi(token);
            getline(iss, name, '|');
            getline(iss, type, '|');
            getline(iss, username, '|');
            getline(iss, password, '|');
            getline(iss, token, '|'); fine = stoll(token);
            getline(iss, curr, '|');
            getline(iss, ret, '|');
            getline(iss, res, '|');
            User* u = nullptr;
            if(type == "Student")
                u = new Student(id, name, username, password);
            else if(type == "Faculty")
                u = new Faculty(id, name, username, password);
            else if(type == "Librarian")
                u = new Librarian(id, name, username, password);
            if(u){
                Account &acc = u->getaccount();
                vector<borrowedbook> currBooks = vector<borrowedbook>();
                vector<returnedbook> retBooks = vector<returnedbook>();
                vector<reservedbook> resBooks = vector<reservedbook>();
                // Deserialize current borrowed books
                if(!curr.empty()){
                    size_t start = 0, end;
                    while((end = curr.find(';', start)) != string::npos){
                        string token = curr.substr(start, end - start);
                        size_t commaPos = token.find(',');
                        if(commaPos != string::npos){
                            int bid = stoi(token.substr(0, commaPos));
                            string bdate = token.substr(commaPos+1);
                            currBooks.push_back(borrowedbook(bid, bdate));
                        }
                        start = end + 1;
                    }
                    if(start < curr.size()){
                        string token = curr.substr(start);
                        size_t commaPos = token.find(',');
                        if(commaPos != string::npos){
                            int bid = stoi(token.substr(0, commaPos));
                            string bdate = token.substr(commaPos+1);
                            currBooks.push_back(borrowedbook(bid, bdate));
                        }
                    }
                }
                // Deserialize returned books
                if(!ret.empty()){
                    size_t start = 0, end;
                    while((end = ret.find(';', start)) != string::npos){
                        string token = ret.substr(start, end - start);
                        size_t first = token.find(',');
                        size_t second = token.find(',', first+1);
                        if(first != string::npos && second != string::npos){
                            int bid = stoi(token.substr(0, first));
                            string bdate = token.substr(first+1, second - first - 1);
                            string rdate = token.substr(second+1);
                            retBooks.push_back(returnedbook(bid, bdate, rdate));
                        }
                        start = end + 1;
                    }
                    if(start < ret.size()){
                        string token = ret.substr(start);
                        size_t first = token.find(',');
                        size_t second = token.find(',', first+1);
                        if(first != string::npos && second != string::npos){
                            int bid = stoi(token.substr(0, first));
                            string bdate = token.substr(first+1, second - first - 1);
                            string rdate = token.substr(second+1);
                            retBooks.push_back(returnedbook(bid, bdate, rdate));
                        }
                    }
                }
                // Deserialize reserved books
                if(!res.empty()){
                    size_t start = 0, end;
                    while((end = res.find(';', start)) != string::npos){
                        string token = res.substr(start, end - start);
                        size_t first = token.find(',');
                        size_t second = token.find(',', first+1);
                        if(first != string::npos && second != string::npos){
                            int bid = stoi(token.substr(0, first));
                            int uid = stoi(token.substr(first+1, second - first - 1));
                            string rdate = token.substr(second+1);
                            resBooks.push_back(reservedbook(bid, uid, rdate));
                        }
                        start = end + 1;
                    }
                    if(start < res.size()){
                        string token = res.substr(start);
                        size_t first = token.find(',');
                        size_t second = token.find(',', first+1);
                        if(first != string::npos && second != string::npos){
                            int bid = stoi(token.substr(0, first));
                            int uid = stoi(token.substr(first+1, second - first - 1));
                            string rdate = token.substr(second+1);
                            resBooks.push_back(reservedbook(bid, uid, rdate));
                        }
                    }
                }
                for(auto &cb : currBooks) acc.getcurrentbooks().push_back(cb);
                for(auto &rb : retBooks) acc.getreturnedbooks().push_back(rb);
                for(auto &rs : resBooks) acc.getreservedbooks().push_back(rs);
                acc.setFine(fine);
                users.push_back(u);
            }
        }
        ifs.close();
    }
    
    

    void unreserveExpiredBooks() {
        time_t now = chrono::system_clock::to_time_t(chrono::system_clock::now());
        const int timeamount = 10 * 24 * 3600;
        for (auto &book : books) {
            if (book.status == bookstatus::RESERVED) {
                if (difftime(now, book.reserveTime) >= timeamount) {
                    book.status = bookstatus::AVAILABLE;
                    book.reserveTime = 0;
                    cout << "Book ID " << book.id << " has been automatically unreserved." << endl;
                }
            }
        }
    }

    Book* getbookbyID(int id) {
        for (auto &book : books) {
            if (book.id == id) return &book;
        }
        return nullptr;
    }

    User* getuserbyID(int id){
        for(User* user: users){
            if(user->getuserID() == id) return user;
        }
        return nullptr;
    }

    User* getuserby_uname_pass(string username, string password){
        for(User* user: users){
            if((user->getusername() == username) && (user->getpassword() == password)){
                return user;
            }
        }
        return nullptr;
    }

    void showbooks(){
        cout << "-------All Books in Library-------\n";
        for(auto &book : books){
            book.print();
        }
    }

    void showusers(){
        cout << "-------All Registered Users-------\n";
        for(User* user: users){
            user->print();
        }
    }

};



//Student class Member Functions-------------START

bool Student::borrowbook(Library &lib, int bookid, string borrowdate) {
    if(getaccount().getfine() > 0){
        cout << "Please Clear outstanding dues first before borrowing.\n";
        return false;
    }
    if(getaccount().getcurrentbooks().size() >= maxbooks){
        cout << "Maximum Borrowing Limit Reached (" << maxbooks << " Books)\n";
        return false;
    }
    Book* book = lib.getbookbyID(bookid);

    if(book == nullptr){
        cout << "Book not found.\n";
        return false;
    }
    if(book->status == bookstatus::BORROWED){
        cout << "Book is not Available.\n";
        return false;
    }
    if(book->status == bookstatus::RESERVED){
        if(this->id == book->reserverid){
            getaccount().borrowBook(bookid, borrowdate, book->reserverid);
            book->reserverid = -1;
            book->status = bookstatus::BORROWED;
            cout << "Book has been successfully Borrowed!\n";
            cout << "Borrow Date: " << currentdate() << "\n";
            cout << "Please return the Book by " << getfuturedate(15) << "\n";
            return true;
        }
        else{
            cout << "Book has been Reserved by someone else.\n";
            return false;
        }
    }
    book->status = bookstatus::BORROWED;
    getaccount().borrowBook(bookid, borrowdate, -1);
    cout << "Book has been successfully Borrowed!\n";
    cout << "Borrow Date: " << currentdate() << "\n";
    cout << "Please return the Book by " << getfuturedate(15) << "\n";
    return true;
}

bool Student::returnbook(Library &lib, int bookid, string returndate) {
    Book* book = lib.getbookbyID(bookid);
    if(!book){
        cout << "Book not found!\n";
        return false;
    }
    if(book->status != bookstatus::BORROWED){
        cout << "This book has not been borrowed.\n";
        return false;
    }
    if(getaccount().returnBook(bookid, returndate, allowedDays, fineperday)){
        cout << "Book Returned Successfully!\n";
        cout << "Return Date: " << currentdate() << "\n";
        book->status = bookstatus::AVAILABLE;
        if(getaccount().getfine() > 0){
            cout << "Outstanding Fine: " << getaccount().getfine() << " rupees.\n";
        }
        return true;
    }
    cout << "Error Ocurred while processing Return.\n";
    return false;
}

bool Student::reservebook(Library &lib, int bookId, string reservedate){
    if(getaccount().getfine() > 0){
        cout << "Please Clear outstanding dues first before Reserving.\n";
        return false;
    }
    if(getaccount().getreservedbooks().size() >= maxreserve){
        cout << "Maximum Reserving Limit Reached (" << maxreserve << " Books)\n";
        return false;
    }
    Book* book = lib.getbookbyID(bookId);
    if (!book) {
        cout << "Book not found." << endl;
        return false;
    }
    if (book->status != bookstatus::AVAILABLE) {
        cout << "Book is not available for reservation." << endl;
        return false;
    }
    getaccount().reserveBook(bookId, id, reservedate);
    book->status = bookstatus::RESERVED;
    book->reserverid = id;
    book->reserveTime = chrono::system_clock::to_time_t(chrono::system_clock::now());
    cout << "Book reserved successfully." << endl;
    cout << "Book will be reserved till: " << getfuturedate(10) << "\n";
    return true;
}

//---------------END

//Faculty Class Member Functions-----------START

bool Faculty::borrowbook(Library &lib, int bookid, string borrowdate){
    if(getaccount().getcurrentbooks().size() >= maxbooks){
        cout << "Maximum Borrowing Limit Reached (" << maxbooks << " Books)\n";
        return false;
    }
    Book* book = lib.getbookbyID(bookid);

    if(book == nullptr){
        cout << "Book not found.\n";
        return false;
    }
    if(book->status == bookstatus::BORROWED){
        cout << "Book is not Available.\n";
        return false;
    }
    if(book->status == bookstatus::RESERVED){
        if(this->id == book->reserverid){
            getaccount().borrowBook(bookid, borrowdate, book->reserverid);
            book->reserverid = -1;
            book->status = bookstatus::BORROWED;
            cout << "Book has been successfully Borrowed!\n";
            cout << "Borrow Date: " << currentdate() << "\n";
            cout << "Please return the Book by " << getfuturedate(30) << "\n";
            return true;
        }
        else{
            cout << "Book has been Reserved by someone else.\n";
            return false;
        }
    }
    book->status = bookstatus::BORROWED;
    getaccount().borrowBook(bookid, borrowdate, -1);
    cout << "Book has been successfully Borrowed!\n";
    cout << "Borrow Date: " << currentdate() << "\n";
    cout << "Please return the Book by " << getfuturedate(30) << "\n";
    return true;
}

bool Faculty::returnbook(Library &lib, int bookid, string returndate) {
    Book* book = lib.getbookbyID(bookid);
    if(!book){
        cout << "Book not found.\n";
        return false;
    }
    if(book->status != bookstatus::BORROWED){
        cout << "This book has not been borrowed!\n";
        return false;
    }
    if(getaccount().returnBook(bookid, returndate, allowedDays, 0)){
        cout << "Book Returned Successfully!\n";
        cout << "Return Date: " << currentdate() << "\n";
        book->status = bookstatus::AVAILABLE;
        return true;
    }
    cout << "Error Ocurred while processing Return.\n";
    return false;
}

bool Faculty::reservebook(Library &lib, int bookId, string reservedate){
    if(getaccount().getreservedbooks().size() >= maxreserve){
        cout << "Maximum Reserving Limit Reached (" << maxreserve << " Books)\n";
        return false;
    }
    Book* book = lib.getbookbyID(bookId);
    if (!book) {
        cout << "Book not found." << endl;
        return false;
    }
    if (book->status != bookstatus::AVAILABLE) {
        cout << "Book is not available for reservation." << endl;
        return false;
    }
    getaccount().reserveBook(bookId, id, reservedate);
    book->status = bookstatus::RESERVED;
    book->reserverid = id;
    book->reserveTime = chrono::system_clock::to_time_t(chrono::system_clock::now());
    cout << "Book reserved successfully." << endl;
    cout << "Book will be reserved till: " << getfuturedate(10) << "\n";
    return true;
}
//------------END

//Librarian Class Member Functions---------START
void Librarian::addbook(Library &lib, const Book &book) {
    lib.books.push_back(book);
    cout << "Book added by librarian." << endl;
}

void Librarian::removebook(Library &lib, int bookID){
    vector<Book>::iterator it = remove_if(lib.books.begin(), lib.books.end(),[bookID](const Book &b) { return b.id == bookID;});

    if(it==lib.books.end()){
        cout << "Book has not been found!\n";
        return;
    }
    lib.books.erase(it);
    cout << "Book has been removed successfully!\n";
}

void Librarian::updatebook(Library &lib, int bookID, Book &updatedbook){
    for(Book &book : lib.books){
        if(book.id == bookID){
            book = updatedbook;
            cout << "Book has been succesfully Updated!\n";
            return;
        }
        cout << "Book Not Found\n";
    }
}

void Librarian::adduser(Library &lib, User* user){
    lib.users.push_back(user);
    cout << "New User has been successfully Added!\n";
}

void Librarian::removeuser(Library& lib, int userID){
    vector<User*>::iterator it = remove_if(lib.users.begin(), lib.users.end(),[userID](User* user) { return user->id == userID;});
    if(it==lib.users.end()){
        cout << "User Not Found\n";
        return;
    }
    delete *it;
    lib.users.erase(it);
    cout << "User has been removed Successfully!\n";
}

//---------END

bool isintegernumber(string s) {
    if (s.empty())
        return false; 
    for (char c : s) {
        if (!isdigit(c)) return false;
    }
    return true;
}


int main(){
    Library lib;
    lib.loadData();
    cout << "--Library Management System--\n";
    int option;
    while(true){
        lib.unreserveExpiredBooks();
        string temp;
        cout << "Press 1. to Login and 2. to Exit\n";
        getline(cin, temp);
        if(temp == "1")option = 1;
        else if(temp == "2")option = 2;
        else{
            cout << "Invalid Input!!\n\n";
            continue;
        }
        if(option==1){
            string choice;
            cout << "Are you Logging In for first time: YES or NO(in uppercase): ";
            getline(cin, choice);
            cout << "\n";
            if(choice == "YES"){
                cout << "Your Username is your name in lower letters without spaces.\n";
                cout << "Your Password is \"pass\" without double quotes.\n";
                cout << "Please Change your username and Password, once you have Logged in to your Account!\n\n";
            }
            else if(choice != "NO"){
                cout << "Invalid Input!\n\n";
                continue;
            }
            string uname, pass;
            cout << "Please Enter your Username(without spaces): ";
            getline(cin, uname);
            cout << "Please Enter your Password(without spaces): ";
            getline(cin, pass);
            cout << "\n";
            User* user = lib.getuserby_uname_pass(uname, pass);
            if(!user){
                cout << "Incorrect Username and Password!!\n\n";
                continue;
            }

            cout << "Welcome " << user->name << "("<< user->type << ") to Library Management System\n\n";
            if(user->type == "Student" || user->type == "Faculty"){
                int useropt;
                string temp1;
                while(true){
                    cout << "1. Borrow Book\n";
                    cout << "2. Reserve Book\n";
                    cout << "3. Return Book\n";
                    cout << "4. Pay Fine\n";
                    cout << "5. View Account\n";
                    cout << "6. Change Username\n";
                    cout << "7. Change Password\n";
                    cout << "8. Logout\n";
                    cout << "Enter your choice(choice number): ";
                    getline(cin, temp1);
                    if((temp1.size() == 1) && (isdigit(temp1[0]))){
                        useropt = temp1[0] - '0';
                    }
                    else useropt = -1;
                    if(useropt == 1){
                        if(lib.books.size() == 0){
                            cout << "No Books to Borrow!\n\n";
                            continue;
                        }
                        lib.showbooks();
                        int bookid;
                        string temp2;
                        cout << "Enter Book ID: ";
                        getline(cin, temp2);
                        if(isintegernumber(temp2)){
                            bookid = stoi(temp2, nullptr, 10);
                        }
                        else {
                            cout << "Invalid Input!\n\n";
                            continue;
                        }
                        user->borrowbook(lib, bookid, currentdate());
                    }
                    else if(useropt == 2){
                        if(lib.books.size() == 0){
                            cout << "No Books to Reserve!\n\n";
                            continue;
                        }
                        lib.showbooks();
                        int bookid;
                        string temp2;
                        cout << "Enter Book ID: ";
                        getline(cin, temp2);
                        if(isintegernumber(temp2)){
                            bookid = stoi(temp2, nullptr, 10);
                        }
                        else {
                            cout << "Invalid Input!\n\n";
                            continue;
                        }
                        user->reservebook(lib, bookid, currentdate());
                    }
                    else if(useropt == 3){
                        int bookid;
                        string temp2;
                        cout << "Enter Book ID: ";
                        getline(cin, temp2);
                        if(isintegernumber(temp2)){
                            bookid = stoi(temp2, nullptr, 10);
                        }
                        else {
                            cout << "Invalid Input!\n\n";
                            continue;
                        }
                        user->returnbook(lib, bookid, currentdate());
                    }
                    else if(useropt == 4){
                        user->payfine();
                    }
                    else if(useropt==5){
                        user->printaccount(&lib);
                    }
                    else if(useropt == 6){
                        string u_name;
                        cout << "Enter you earlier Username: ";
                        getline(cin, u_name);
                        if(user->checkusername(u_name)){
                            string new_uname;
                            cout << "Enter new Username: ";
                            getline(cin, new_uname);
                            user->changeusername(new_uname);
                        }
                        else{
                            cout << "Invalid Username!\n\n";
                            continue;
                        }
                    }
                    else if(useropt == 7){
                        string passw;
                        cout << "Enter you earlier Password: ";
                        getline(cin, passw);
                        if(user->checkpassword(passw)){
                            string new_pass;
                            cout << "Enter new Password: ";
                            getline(cin, new_pass);
                            user->changepassword(new_pass);
                        }
                        else{
                            cout << "Invalid Password!\n\n";
                            continue;
                        }
                    }
                    else if(useropt == 8){
                        cout << "Logged Out....\n\n";
                        break;
                    }
                    else{
                        cout << "Invalid Option!!!\n\n";
                    }
                }
            }
            else if(user->type == "Librarian"){
                int libopt;
                while(true){
                    cout << "1. Add Book\n";
                    cout << "2. Remove Book\n";
                    cout << "3. Update Book\n";
                    cout << "4. Add User\n";
                    cout << "5. Remove User\n";
                    cout << "6. View All Books\n";
                    cout << "7. View All Users\n";
                    cout << "8. Change Username\n";
                    cout << "9. Change Password\n";
                    cout << "10. View Account Details\n";
                    cout << "11. Logout\n";
                    cout << "Enter your choice: ";
                    cin >> libopt;
                    cin.ignore();

                    if(libopt == 1){
                        int id = lib.books.size() + 1;
                        string title, author, publisher, ISBN;
                        int year;
                        string temp2;
                        cout << "Enter Book's Title: ";
                        getline(cin, title);
                        cout << "Enter Author of the Book: ";
                        getline(cin, author);
                        cout << "Enter Publisher of the Book: ";
                        getline(cin, publisher);
                        cout << "Enter the ISBN number of Book: ";
                        getline(cin, ISBN);
                        cout << "Enter year in which Book was published(in integer): ";
                        getline(cin, temp2);
                        if(isintegernumber(temp2)){
                            year = stoi(temp2, nullptr, 10);
                        }
                        else {
                            cout << "Invalid Input!\n\n";
                            continue;
                        }

                        Book newbook(id, title, author, publisher, year, ISBN);
                        static_cast<Librarian*> (user)->addbook(lib, newbook);
                    }
                    else if(libopt==2){
                        if(lib.books.size() == 0){
                            cout << "No Books to Remove!\n\n";
                            continue;
                        }
                        int id;
                        string temp2;
                        cout << "Enter Book's ID: ";
                        getline(cin, temp2);
                        if(isintegernumber(temp2)){
                            id = stoi(temp2, nullptr, 10);
                        }
                        else {
                            cout << "Invalid Input!\n\n";
                            continue;
                        }
                        static_cast<Librarian*>(user)->removebook(lib, id);
                    }
                    else if(libopt == 3){
                        int id;
                        string temp2;
                        cout << "Enter book ID to update: ";
                        getline(cin, temp2);
                        if(isintegernumber(temp2)){
                            id = stoi(temp2, nullptr, 10);
                        }
                        else {
                            cout << "Invalid Input!\n\n";
                            continue;
                        }
                        Book* book = lib.getbookbyID(id);
                        if(book){
                            cout << "Enter new title (leave blank to keep current): ";
                            string newtitle;
                            getline(cin, newtitle);
                            if (!newtitle.empty()) book->title = newtitle;

                            cout << "Enter new author (leave blank to keep current): ";
                            string newauthor;
                            getline(cin, newauthor);
                            if (!newauthor.empty()) book->author = newauthor;

                            cout << "Enter new publisher (leave blank to keep current): ";
                            string newpublisher;
                            getline(cin, newpublisher);
                            if (!newpublisher.empty()) book->publisher = newpublisher;

                            string temp2;
                            cout << "Enter new year (0 to keep current): ";
                            int newyear;
                            getline(cin, temp2);
                            if(isintegernumber(temp2)){
                                newyear = stoi(temp2, nullptr, 10);
                            }
                            else {
                                cout << "Invalid Input!\n\n";
                                continue;
                            }
                            if (newyear != 0)
                                book->year = newyear;
                            cout << "Enter new ISBN (leave blank to keep current): ";
                            string newISBN;
                            getline(cin, newISBN);
                            if (!newISBN.empty()) book->ISBN = newISBN;
                            cout << "Book updated successfully.\n\n";
                        }
                        else cout << "Book not Found!!\n\n";
                    }
                    else if(libopt == 4){
                        int type;
                        cout << "Enter User Type (1. for Student, 2. for Faculty, 3. for Librarian): ";
                        cin >> type;
                        cin.ignore();
                        string name;
                        cout << "Enter Name: ";
                        cin >> name;
                        string username, pass;
                        for(auto u : name){
                            username += tolower(u);
                        }
                        pass = "pass";
                        int newid = lib.users.size() + 1;
                        if(type==1) lib.users.push_back(new Student(newid, name,username, pass));
                        else if(type==2) lib.users.push_back(new Faculty(newid, name, username, pass));
                        else if(type==3) lib.users.push_back(new Librarian(newid, name, username, pass));
                        cout << "User added with ID: " << newid << " Successfully!\n\n";
                    }
                    else if(libopt == 5){
                        int userid;
                        string temp2;
                        cout << "Enter ID of user to be removed: ";
                        getline(cin, temp2);
                        if(isintegernumber(temp2)){
                            userid = stoi(temp2, nullptr, 10);
                        }
                        else {
                            cout << "Invalid Input!\n\n";
                            continue;
                        }
                        static_cast<Librarian*>(user)->removeuser(lib, userid);
                    }
                    else if(libopt == 6){
                        lib.showbooks();
                    }
                    else if(libopt == 7){
                        lib.showusers();
                    }
                    else if(libopt == 8){
                        string u_name;
                        cout << "Enter you earlier Username: ";
                        getline(cin, u_name);
                        if(user->checkusername(u_name)){
                            string new_uname;
                            cout << "Enter new Username: ";
                            getline(cin, new_uname);
                            user->changeusername(new_uname);
                        }
                        else{
                            cout << "Invalid Username!\n\n";
                            continue;
                        }
                    }
                    else if(libopt == 9){
                        string passw;
                        cout << "Enter you earlier Password: ";
                        getline(cin, passw);
                        if(user->checkpassword(passw)){
                            string new_pass;
                            cout << "Enter new Password: ";
                            getline(cin, new_pass);
                            user->changepassword(new_pass);
                        }
                        else{
                            cout << "Invalid Password!\n\n";
                            continue;
                        }
                    }
                    else if(libopt == 10){
                        user->printaccount(&lib);
                    }
                    else if(libopt == 11){
                        cout << "Logged Out....\n\n";
                        break;
                    }
                    else{
                        cout << "Invalid Choice!!\n\n";
                    }
                }
            }
        }
        else if(option == 2)break;
        else{
            cout << "Invalid Option!!\n\n";
        }
        lib.unreserveExpiredBooks();
    }
    lib.unreserveExpiredBooks();
    lib.saveData();
    cout << "Exiting Library Management System!!!\n";
    return 0;
}