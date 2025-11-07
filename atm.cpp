#include <algorithm>
#include <fstream>
#include <iostream>
#include <limits>
#include <map>
#include <string>
#include <utility>
#include <vector>
using namespace std;

int input();
class Bank;
class Account {
private:
  string account_number;
  int account_balance;
  string owner_name;
  Bank *issued_bank;
  string card_number;
  string password;

public:
  Account(string account_number, int account_balance, string owner_name,
          Bank *issued_bank, string card_number, string password) {
    this->account_number = account_number;
    this->account_balance = account_balance;
    this->owner_name = owner_name;
    this->issued_bank = issued_bank;
    this->card_number = card_number;
    this->password = password;
  };

  Account() {
    this->account_number = '0';
    this->account_balance = 0;
    this->owner_name = '0';
    this->issued_bank = nullptr;
    this->card_number = '0';
    this->password = '0';
  }
  void balance_change(int changed_amount) {
    account_balance += changed_amount; // 계좌의 잔액을 변동
  }
  string get_account_number() { return account_number; };
  int get_account_balance() { return account_balance; }
  void set_account_balance(int balance);
  string get_owner_name() { return owner_name; }
  Bank *get_issued_bank() { return issued_bank; }
  string get_card_number() { return card_number; }
  string get_password() { return password; }
};

class ATM {
protected:
  string current_language;
  string serial_number;
  bool single_Bank;
  int withdrawal_session;
  int withdrawal_amount;
  static int transaction_id; // 각 transaction의 unique identifier. 모든 ATM에서
                             // 공유, 클래스 외부에서 1로 초기화
  // 초기화 해줘야함
  map<int, int> cash;
  map<int, int> checks;
  Bank *connected_bank;
  string atmTransActionHistory;
  string userActionHistory;
  pair<Account *, bool> inserted_card; // 삽입되어 있는 카드의 정보를 저장, ATM
  // 생성자에서 (nullptr, false)로 초기화

public:
  ATM(string serial_number, bool single_Bank, Bank *connected_bank) {
    this->serial_number = serial_number;
    this->single_Bank = single_Bank;
    this->connected_bank = connected_bank;
    current_language = "English";

    cash[50000] = 0;
    cash[10000] = 0;
    cash[5000] = 0;
    cash[1000] = 0;

    withdrawal_session = 0;
    withdrawal_amount = 0;

    atmTransActionHistory = "";
    userActionHistory = "";
    // transaction_id = 1; // 거래ID를 1로 초기화
    inserted_card = make_pair(nullptr, false);
  }
  virtual ~ATM() {}
  static int next_id;
  virtual void deposit();              // 현금
  virtual void deposit(int check_num); // 수표
  virtual void withdraw();
  virtual void transfer();
  virtual void card_insert();
  virtual bool verify_card_password(Account *user_account);
  virtual void set_language() { return; }
  void admin_menu();
  bool dispenseCash(int amount);
  Account *isCard_valid(const string &card_number);
  void printATM_history(string history);
  void printUserAction() { cout << userActionHistory << endl; }
  string getSerialNumber() { return serial_number; }
  // bool single_Bank;
  int getCashAmount(int denomination) { return cash[denomination]; }
  int getCheckValue(int value) {
    return cash[value];
  } // value에 해당하는 수표 매수를 반환
  Bank *getConnectedBank() { return connected_bank; }
  bool getSingleBank() { return single_Bank; }
  string getAtmTransActionHistory() { return atmTransActionHistory; }
  string getUserActionHistory() { return userActionHistory; }
  map<int, int> &access_cash() { return cash; }
  void reset_userhistory() { userActionHistory = ""; }
  string getCurrentLanguage() { return current_language; }
  virtual string getType() { return "ATM"; }
  // 추가
  void removeCard() { inserted_card = make_pair(nullptr, false); }
  // 추가
  bool isCardInserted() {
    if (!inserted_card.second || inserted_card.first == nullptr) {
      return false;
    }
    return true;
  }
  pair<Account *, bool> getInsertedCard() { return inserted_card; }
};
int ATM::transaction_id = 1;
class BilingualATM;

class Bank {
private:
  string bank_name;
  map<string, Account> accounts; // 계좌 목록 (계좌 번호 -> Account 객체)
  map<string, ATM *> atms; // ATM 목록 (시리얼 번호 -> ATM 포인터)
  // STL들은 모두 동적으로 메모리 할당을해서 값을 집어
  // 넣을때 메모리 걱정은 필요가 없다고 그러네

public:
  Bank(string name);
  ~Bank();
  void createATM();
  void createAccount();
  void chargeATM(string serial);
  string getBankName() { return bank_name; }
  // Account getAccount(string account_number) { return
  // accounts[account_number]; } <<이렇게 하면 key값이 map안에 없을 때
  // account_number로 새로운 element를 만든다고하네. get_ATM도 마찬가지
  Account *find_account(string account_number) // 새로만들어봄
  {
    auto acc_found = accounts.find(account_number);
    if (acc_found != accounts.end()) {
      return &acc_found->second;
    }
    return nullptr;
  }
  ATM *get_ATM(const string &serialnumber);
  bool authorize_user(Account &account, const string &password);
  Account *find_account_by_card_number(const string &card_number);
  bool find_atm_by_serial_number(const string &serial_number);
  map<string, Account> get_acc_map() { return accounts; }
  map<string, ATM *> get_ATM_map() { return atms; }
  void withdrawCashFromATM(const string &serial_number);
};

Bank::~Bank() {
  for (auto &pair : atms) {
    delete pair.second; // 동적으로 할당된 ATM 객체 해제
  }
}

bool Bank::find_atm_by_serial_number(const string &serial_number) {
  return atms.find(serial_number) != atms.end();
}

ATM *Bank::get_ATM(const string &serialnumber) {
  auto it = atms.find(serialnumber);
  if (it != atms.end()) {
    return it->second;
  }
  return nullptr;
}

vector<Bank *> bank_list;

Bank *createBank() {
  string name;
  string choice;

  // 은행 이름 입력
  cout << "은행 이름을 입력하세요: ";
  cin.ignore();
  getline(cin, name);

  // 이름 확인
  cout << "'" << name << "'으로 은행을 생성하시겠습니까?\n";
  cout << "1. 예\n";
  cout << "2. 아니오\n";
  cout << "선택: ";
  cin >> choice;

  if (choice == "2") {
    cout << "은행 생성이 취소되었습니다.\n";
    return nullptr;
  } else if (choice != "1") {
    cout << "잘못된 입력입니다. 은행 생성이 취소되었습니다.\n";
    return nullptr;
  }

  // Bank 객체 생성 및 반환
  Bank *bank = new Bank(name);
  cout << "은행 '" << name << "'가 성공적으로 생성되었습니다.\n";

  return bank;
}

void slash();
void useATM() {
  // ATM 사용 함수

  string serial_number;
  ATM *choosed_ATM = nullptr; // 사용할 ATM의 포인터 -> 못찾으면 nullptr로 남음

  while (true) // serial_number 찾는 loop
  {
    cout << "ATM의 Serial Number를 입력하세요: ";
    cin >> serial_number;
    bool is_all_num = true;
    for (char ch : serial_number) {
      if (ch < '0' || ch > '9') {
        cout << "입력에 숫자 외의 값이 들어있습니다." << endl;
        is_all_num = false;
        break;
      }
    }
    if (!is_all_num) {
      continue;
    }
    if (serial_number.length() != 6) {
      cout << "입력 길이가 6자리가 아닙니다." << endl;
      continue;
    }
    break;
  }

  for (Bank *bank : bank_list) {
    if (bank->find_atm_by_serial_number(serial_number)) {
      choosed_ATM = bank->get_ATM(serial_number);
      break;
    }
  }

  if (choosed_ATM == nullptr) {
    cout << "해당 Serial Number를 가진 ATM이 존재하지 않습니다." << endl;
    return;
  }

  if (choosed_ATM->getType() == "ATM") {
    while (true) {
      cout << "\n=== ATM Menu ===\n";
      cout << "0. Insert Card\n";
      cout << "1. Deposit\n";
      cout << "2. Withdrawal\n";
      cout << "3. Transfer\n";
      cout << "4. End Transaction\n";
      cout << "/. ATM/Account Snapshot\n";

      // ATM 설정 정보 출력
      cout << "----------------------------------------\n";
      cout << "Bank Mode: "
           << (choosed_ATM->getSingleBank() ? "Single-Bank" : "Multi-Bank")
           << "\n";
      cout << "Connected Bank: "
           << choosed_ATM->getConnectedBank()->getBankName() << "\n";
      cout << "Language Mode: Single Language\n";
      cout << "Card Status: "
           << (choosed_ATM->isCardInserted() ? "Inserted" : "Not Inserted")
           << "\n";
      if (choosed_ATM->isCardInserted()) {
        Account *inserted_account = choosed_ATM->getInsertedCard().first;
        cout << "Card Bank: "
             << inserted_account->get_issued_bank()->getBankName() << "\n";
        cout << "Same Bank: "
             << (inserted_account->get_issued_bank() ==
                         choosed_ATM->getConnectedBank()
                     ? "Yes"
                     : "No")
             << "\n";
        cout << "Card Number: " << inserted_account->get_card_number() << "\n";
        cout << "Account Number: " << inserted_account->get_account_number()
             << "\n";
        cout << "Account Balance: " << inserted_account->get_account_balance()
             << "\n";
      }
      cout << "----------------------------------------\n";

      cout << "Select: ";

      char choice;
      cin.ignore();
      cin >> choice;

      // 입력 값이 숫자가 아닌 경우 예외 처리
      if (cin.fail()) {
        cin.clear(); // 실패 상태 플래그를 리셋
        cin.ignore(numeric_limits<streamsize>::max(),
                   '\n'); // 입력 버퍼 비우기
        cout << "Insert valid input.\n";
        continue;
      }

      if (choice >= '5' && choice != '/') {
        cout << "Insert valid input." << endl;
        continue;
      }

      switch (choice) {

      case '0':
        choosed_ATM->card_insert();
        break;

      case '1':
        if (choosed_ATM->isCardInserted() == false) {
          cout << "No card has been inserted." << endl;
          break;
        }
        int num_check;
        cout << "\nEnter the number of check. If you want to deposit only "
                "cash, insert 0 : ";
        num_check = input();

        if (num_check == 0) {
          choosed_ATM->deposit();
          break;
        }

        if (num_check > 0) {
          choosed_ATM->deposit(num_check);
          break;
        }

        else {
          cout << "Insert valid number." << endl;
          break;
        }

      case '2':
        choosed_ATM->withdraw();
        break;

      case '3':
        choosed_ATM->transfer();
        break;

      case '4':
        cout << "Ending Transaction. Thank You" << endl;
        choosed_ATM->removeCard();
        choosed_ATM->printUserAction();
        choosed_ATM->reset_userhistory();
        return;

      case '/': {
        slash();
        break;
      }

      default:
        cout << "Insert valid input." << endl;
      }
    }
  }

  if (choosed_ATM->getType() == "BilingualATM") {

  initial_stage:
    if (choosed_ATM->getCurrentLanguage() == "Korean") {
      while (true) {
        cout << "\n=== ATM 메뉴 ===\n";
        cout << "0. 카드 삽입\n";
        cout << "1. 입금\n";
        cout << "2. 출금\n";
        cout << "3. 송금\n";
        cout << "4. 언어 선택\n";
        cout << "5. 거래 종료\n";
        cout << "/. ATM, 계좌 현황\n";

        // Bilingual ATM 설정 정보 출력
        cout << "----------------------------------------\n";
        cout << "은행 모드: "
             << (choosed_ATM->getSingleBank() ? "단일은행" : "다중은행")
             << "\n";
        cout << "연결 은행: " << choosed_ATM->getConnectedBank()->getBankName()
             << "\n";
        cout << "언어 모드: 이중언어\n";
        cout << "카드 상태: "
             << (choosed_ATM->isCardInserted() ? "삽입됨" : "미삽입") << "\n";
        if (choosed_ATM->isCardInserted()) {
          Account *inserted_account = choosed_ATM->getInsertedCard().first;
          cout << "카드 발행은행: "
               << inserted_account->get_issued_bank()->getBankName() << "\n";
          cout << "동일 은행 여부: "
               << (inserted_account->get_issued_bank() ==
                           choosed_ATM->getConnectedBank()
                       ? "예"
                       : "아니오")
               << "\n";
          cout << "카드 번호: " << inserted_account->get_card_number() << "\n";
          cout << "계좌 번호: " << inserted_account->get_account_number()
               << "\n";
          cout << "계좌 잔액: " << inserted_account->get_account_balance()
               << "\n";
        }
        cout << "----------------------------------------\n";

        cout << "선택: ";

        char choice;
        cin >> choice;

        // 입력 값이 숫자가 아닌 경우 예외 처리
        if (cin.fail()) {
          cin.clear(); // 실패 상태 플래그를 리셋
          cin.ignore(numeric_limits<streamsize>::max(),
                     '\n'); // 입력 버퍼 비우기
          cout << "유효한 입력값이 아닙니다.\n";
          continue;
        }

        if (choice >= '6' && choice != '/') {
          cout << "유효한 입력값이 아닙니다." << endl;
          continue;
        }

        switch (choice) {

        case '0':
          choosed_ATM->card_insert();
          break;

        case '1':

          if (choosed_ATM->isCardInserted() == false) {
            cout << "카드가 삽입되어 있지 않습니다." << endl;
            break;
          }
          int num_check;
          cout << "\n입금하실 수표의 장수를 입력하세요. 만약 현금 입금만을 "
                  "원하신다면 0을 입력해주세요 : ";
          num_check = input();

          if (num_check == 0) {
            choosed_ATM->deposit();
            break;
          }

          if (num_check > 0) {
            choosed_ATM->deposit(num_check);
            break;
          }

          else {
            cout << "유효한 숫자를 입력해주세요" << endl;
            break;
          }

        case '2':
          choosed_ATM->withdraw();
          break;

        case '3':
          choosed_ATM->transfer();
          break;

        case '4':
          choosed_ATM->set_language();
          goto initial_stage;

        case '5':
          cout << "거래를 종료합니다. 감사합니다." << endl;
          choosed_ATM->removeCard();
          choosed_ATM->printUserAction();
          choosed_ATM->reset_userhistory();
          return;

        case '/': {
          slash();
          break;
        }

        default:
          cout << "유효하지 않은 선택입니다. 다시 시도해주세요" << endl;
        }
      }
    }
    if (choosed_ATM->getCurrentLanguage() == "English") {
      while (true) {
        cout << "\n=== ATM Menu ===\n";
        cout << "0. Insert Card\n";
        cout << "1. Deposit\n";
        cout << "2. Withdrawal\n";
        cout << "3. Transfer\n";
        cout << "4. Set Language\n";
        cout << "5. End Transaction\n";
        cout << "/. ATM/Account Snapshot\n";

        // Bilingual ATM 설정 정보 출력
        cout << "----------------------------------------\n";
        cout << "Bank Mode: "
             << (choosed_ATM->getSingleBank() ? "Single-Bank" : "Multi-Bank")
             << "\n";
        cout << "Connected Bank: "
             << choosed_ATM->getConnectedBank()->getBankName() << "\n";
        cout << "Language Mode: Bilingual\n";
        cout << "Card Status: "
             << (choosed_ATM->isCardInserted() ? "Inserted" : "Not Inserted")
             << "\n";
        if (choosed_ATM->isCardInserted()) {
          Account *inserted_account = choosed_ATM->getInsertedCard().first;
          cout << "Card Bank: "
               << inserted_account->get_issued_bank()->getBankName() << "\n";
          cout << "Same Bank: "
               << (inserted_account->get_issued_bank() ==
                           choosed_ATM->getConnectedBank()
                       ? "Yes"
                       : "No")
               << "\n";
          cout << "Card Number: " << inserted_account->get_card_number()
               << "\n";
          cout << "Account Number: " << inserted_account->get_account_number()
               << "\n";
          cout << "Account Balance: " << inserted_account->get_account_balance()
               << "\n";
        }
        cout << "----------------------------------------\n";

        cout << "Select: ";

        char choice;
        cin >> choice;

        // 입력 값이 숫자가 아닌 경우 예외 처리
        if (cin.fail()) {
          cin.clear(); // 실패 상태 플래그를 리셋
          cin.ignore(numeric_limits<streamsize>::max(),
                     '\n'); // 입력 버퍼 비우기
          cout << "Insert valid input.\n";
          continue;
        }

        if (choice >= '6' && choice != '/') {
          cout << "Insert valid input." << endl;
          continue;
        }

        switch (choice) {

        case '0':
          choosed_ATM->card_insert();
          break;

        case '1':

          if (choosed_ATM->isCardInserted() == false) {
            cout << "No card has been inserted." << endl;
            break;
          }
          int num_check;
          cout << "\nEnter the number of check. If you want to deposit only "
                  "cash, insert 0 : ";
          num_check = input();

          if (num_check == 0) {
            choosed_ATM->deposit();
            break;
          }

          if (num_check > 0) {
            choosed_ATM->deposit(num_check);
            break;
          }

          else {
            cout << "Insert valid number." << endl;
            break;
          }

        case '2':
          choosed_ATM->withdraw();
          break;

        case '3':
          choosed_ATM->transfer();
          break;

        case '4':
          choosed_ATM->set_language();
          goto initial_stage;

        case '5':
          cout << "Ending Transaction. Thank You" << endl;
          choosed_ATM->removeCard();
          choosed_ATM->printUserAction();
          choosed_ATM->reset_userhistory();
          return;

        case '/': // 출력 형식 예시랑 좀 다르게 함
        {
          slash();
          break;
        }

        default:
          cout << "Insert valid number." << endl;
        }
      }
    }
  }
}

void mainMenu() {
  while (true) {
    cout << "\n*****************************************" << endl;
    cout << "*             ATM 시스템               *" << endl;
    cout << "*****************************************" << endl;
    cout << "* 1. 은행 생성                         *" << endl;
    cout << "* 2. 은행 사용                         *" << endl;
    cout << "* 3. ATM 생성                          *" << endl;
    cout << "* 4. ATM 사용                          *" << endl;
    cout << "* 5. 계좌 생성                         *" << endl;
    cout << "* 6. 프로그램 종료                     *" << endl;
    cout << "* /. ATM/Account Snapshot             *" << endl;
    cout << "*****************************************" << endl;
    cout << "선택: ";

    char choice;
    cin >> choice;

    if (cin.fail()) {
      cin.clear();
      cin.ignore(numeric_limits<streamsize>::max(), '\n');
      cout << "유효하지 않은 입력입니다. 다시 입력해주세요.\n";
      continue;
    }

    switch (choice) {
    case '1': {
      Bank *new_bank = createBank();
      if (new_bank != nullptr) {
        bank_list.push_back(new_bank);
      }
      break;
    }
    case '2': {
      while (true) {
        cout << "\n=== 은행 사용 메뉴 ===\n";
        cout << "1. ATM 충전\n";
        cout << "2. ATM에서 돈 빼내기\n";
        cout << "3. 사용자 계좌 잔액 변경\n";
        cout << "4. 은행 사용 종료\n";
        cout << "선택: ";

        char sub_choice;
        cin >> sub_choice;

        if (cin.fail()) {
          cin.clear();
          cin.ignore(numeric_limits<streamsize>::max(), '\n');
          cout << "유효하지 않은 입력입니다. 다시 입력해주세요.\n";
          continue;
        }

        switch (sub_choice) {
        case '1': { // ATM 충전
          cout << "ATM을 충전하실 은행명을 입력하세요: ";
          string bank_name;
          cin >> bank_name;
          bool is_bank_available = false;
          for (Bank *bank : bank_list) {
            if (bank->getBankName() == bank_name) {
              is_bank_available = true;
              cout << "충전하실 ATM의 Serial Number를 입력하세요: ";
              string serial_number;
              cin >> serial_number;
              bank->chargeATM(serial_number);
              break;
            }
          }
          if (!is_bank_available) {
            cout << "해당 은행은 존재하지 않습니다.\n";
          }
          break;
        }
        case '2': { // ATM에서 돈 빼내기
          cout << "ATM에서 돈을 빼내실 은행명을 입력하세요: ";
          string bank_name;
          cin >> bank_name;
          bool is_bank_available = false;
          for (Bank *bank : bank_list) {
            if (bank->getBankName() == bank_name) {
              is_bank_available = true;
              cout << "돈을 빼내실 ATM의 Serial Number를 입력하세요: ";
              string serial_number;
              cin >> serial_number;
              bank->withdrawCashFromATM(serial_number);
              break;
            }
          }
          if (!is_bank_available) {
            cout << "해당 은행은 존재하지 않습니다.\n";
          }
          break;
        }
        case '3': { // 사용자 계좌 잔액 변경
          cout << "잔액을 변경할 계좌 번호를 입력하세요: ";
          string account_number;
          cin >> account_number;
          Account *account = nullptr;
          for (Bank *bank : bank_list) {
            account = bank->find_account(account_number);
            if (account != nullptr) {
              break;
            }
          }
          if (account == nullptr) {
            cout << "해당 계좌가 존재하지 않습니다.\n";
            break;
          }
          cout << "새로운 잔액을 입력하세요: ";
          int new_balance;
          cin >> new_balance;
          if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "유효한 금액을 입력해주세요.\n";
            break;
          }
          account->set_account_balance(new_balance);
          cout << "계좌 잔액이 성공적으로 변경되었습니다.\n";
          break;
        }
        case '4': { // 은행 사용 종료
          cout << "은행 사용을 종료합니다.\n";
          break;
        }
        default:
          cout << "유효하지 않은 선택입니다. 다시 시도해주세요.\n";
        }

        if (sub_choice == '4') {
          break;
        }
      }
      break;
    }
    case '3': {
      cout << "생성하실 ATM의 은행명을 입력하세요: ";
      string bank_name;
      cin >> bank_name;
      bool is_bank_available = false;
      for (Bank *bank : bank_list) {
        if (bank->getBankName() == bank_name) {
          is_bank_available = true;
          bank->createATM();
          break;
        }
      }
      if (!is_bank_available) {
        cout << "해당 은행은 존재하지 않습니다.\n";
      }
      break;
    }
    case '4':
      useATM();
      break;
    case '5': {
      cout << "계좌를 발급하실 은행명을 입력하세요: ";
      string bank_name;
      cin >> bank_name;
      bool is_bank_available = false;
      for (Bank *bank : bank_list) {
        if (bank->getBankName() == bank_name) {
          is_bank_available = true;
          bank->createAccount();
          break;
        }
      }
      if (!is_bank_available) {
        cout << "해당 은행은 존재하지 않습니다.\n";
      }
      break;
    }
    case '6':
      cout << "프로그램을 종료합니다. 감사합니다!\n";
      return;
    case '/':
      slash();
      break;
    default:
      cout << "유효하지 않은 선택입니다. 다시 시도해주세요.\n";
    }
  }
}

int main() {
  mainMenu();
  // 동적으로 할당된 Bank 객체를 삭제
  for (Bank *bank : bank_list) {
    delete bank;
  }
  bank_list.clear();

  return 0;
}

void Account::set_account_balance(int balance) { account_balance = balance; }

void Bank::createAccount() {
  string accountNumber, ownerName, cardNumber, cardPassword;
  int initialBalance;

  while (true) // 계좌번호 설정 loop
  {
    cout << "계좌 번호 숫자 12자리를 입력하세요: ";
    cin >> accountNumber;
    bool is_all_num = true;
    for (char ch : accountNumber) {
      if (ch < '0' || ch > '9') {
        cout << "입력에 숫자 외의 값이 들어있습니다." << endl;
        is_all_num = false;
        break;
      }
    }
    if (!is_all_num) {
      continue;
    }
    if (accountNumber.length() != 12) {
      cout << "입력 길이가 12자리가 아닙니다." << endl;
      continue;
    }

    bool isAccNumDuplicated = false;

    for (Bank *bank : bank_list) // 수정함
    {
      Account *acc_found = bank->find_account(accountNumber);
      if (acc_found != nullptr) {
        isAccNumDuplicated = true;
        break;
      }
    }
    if (isAccNumDuplicated) {
      cout << "이미 존재하는 계좌번호입니다." << endl;
      continue;
    }
    break;
  }

  while (true) // 카드번호 설정 loop
  {
    cout << "카드번호 숫자 16자리를 입력하세요: ";
    cin >> cardNumber;
    bool is_all_num = true;
    for (char ch : cardNumber) {
      if (ch < '0' || ch > '9') {
        cout << "입력에 숫자 외의 값이 들어있습니다." << endl;
        is_all_num = false;
        break;
      }
    }
    if (!is_all_num) {
      continue;
    }
    if (cardNumber.length() != 16) {
      cout << "입력 길이가 16자리가 아닙니다." << endl;
      continue;
    }

    bool isCardNumDuplicated = false;

    for (Bank *bank : bank_list) {
      Account *acc_found = bank->find_account_by_card_number(cardNumber);
      if (acc_found != nullptr) {
        isCardNumDuplicated = true;
        break;
      }
    }

    if (isCardNumDuplicated) {
      cout << "이미 존재하는 카드번호입니다." << endl;
      continue;
    }
    break;
  }

  while (true) {
    cout << "카드 비밀번호로 설정할 숫자 4자리를 입력하세요: ";
    cin >> cardPassword;
    bool is_all_num = true;
    for (char ch : cardPassword) {
      if (ch < '0' || ch > '9') {
        cout << "입력에 숫자 외의 값이 들어있습니다." << endl;
        is_all_num = false;
        break;
      }
    }
    if (!is_all_num) {
      continue;
    }
    if (cardPassword.length() != 4) {
      cout << "입력 길이가 4자리가 아닙니다." << endl;
      continue;
    }
    break;
  }

  cout << "소유자 이름을 입력하세요: ";
  cin.ignore(numeric_limits<streamsize>::max(), '\n');
  getline(cin, ownerName);

  cout << "초기 잔액을 입력하세요: ";
  cin >> initialBalance;
  if (cin.fail()) {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cout << "유효한 숫자를 입력해주세요.\n";
    return;
  }

  Account account_obj(accountNumber, initialBalance, ownerName, this,
                      cardNumber, cardPassword);

  accounts[accountNumber] = account_obj; // 은행의 계좌목록에 새 계좌 추가

  cout << "계좌가 성공적으로 생성되었습니다.\n";
  return;
}

Bank::Bank(string name) : bank_name{name} {}

void ATM::deposit() {
  if (!inserted_card.second || inserted_card.first == nullptr) {
    cout << "No card has been inserted." << endl;
    return;
  }

  Account *user = inserted_card.first;
  int temp_1000{0}, temp_5000{0}, temp_10000{0}, temp_50000{0}, temp_deposit{0},
      bill_count{0}, fee{0}, temp_fee{0};
  string click;
  bool confirm;
  int user_balance = user->get_account_balance();

  // 수수료 지정 파트 - 같은 은행이면 fee는 1000, 다른 은행이면 2000으로
  // 설정하는 파트
  fee = (connected_bank == user->get_issued_bank()) ? 1000 : 2000;

insertcash_stage:

  cout << "\nEnter the number of 50,000 KRW bills to deposit: ";
  temp_50000 = input();
  if (temp_50000 < 0) {
    cout << "Please enter a valid value." << endl;
    cout << "Exit: 1 \nModify deposit amount: 2" << endl;
    cin >> click;
    if (click == "1")
      return;
    else if (click == "2")
      goto insertcash_stage;
  }

  cout << "Enter the number of 10,000 KRW bills to deposit: ";
  temp_10000 = input();
  if (temp_10000 < 0) {
    cout << "Please enter a valid value." << endl;
    cout << "Exit: 1 \nModify deposit amount: 2" << endl;
    cin >> click;
    if (click == "1")
      return;
    else if (click == "2")
      goto insertcash_stage;
  }

  cout << "Enter the number of 5,000 KRW bills to deposit: ";
  temp_5000 = input();
  if (temp_5000 < 0) {
    cout << "Please enter a valid value." << endl;
    cout << "Exit: 1 \nModify deposit amount: 2" << endl;
    cin >> click;
    if (click == "1")
      return;
    else if (click == "2")
      goto insertcash_stage;
  }

  cout << "Enter the number of 1,000 KRW bills to deposit: ";
  temp_1000 = input();
  if (temp_1000 < 0) {
    cout << "Please enter a valid value." << endl;
    cout << "Exit: 1 \nModify deposit amount: 2" << endl;
    cin >> click;
    if (click == "1")
      return;
    else if (click == "2")
      goto insertcash_stage;
  }

  bill_count = temp_50000 + temp_10000 + temp_5000 + temp_1000;

  if (bill_count == 0) {
    cout << "No bills have been inserted. Please try again." << endl;
    return;
  }

  temp_deposit = temp_50000 * 50000 + temp_10000 * 10000 + temp_5000 * 5000 +
                 temp_1000 * 1000;

  // 총 금액이 일정 금액 이상이면 돌아가기 or 종료하기 이건 고민해봐야할듯
  if (bill_count > 50) {
    cout << "\nThe number of bills to deposit is too large. Please deposit 50 "
            "or fewer bills."
         << endl;
    temp_deposit = 0;
    bill_count = 0;
    goto insertcash_stage; // or return
  }

  cout << "\nDo you want to deposit " << temp_deposit << " KRW?" << "\t Yes: 1"
       << "\t No: 0 \t";

  cin >> confirm; // 1이 확인 0이면 다시 입금하기 - 혹시 시연할때 잘못 입력하는
  // 경우랑 현실성 생각해서 확인버튼 만듦

  if (confirm) {

    cout << "\nPlease deposit the fee of " << fee
         << " KRW: "; // 1000원짜리 지폐로만 가능 - 한장이거나 두장이거나
    cin >> temp_fee;
    if (temp_fee != fee) {
      cout << "Inserted money is different with fee" << endl;
      return;
    }

    cash[1000] += fee / 1000;

    // 계좌 잔액 올리기
    user_balance += (temp_deposit);
    user->set_account_balance(user_balance);

    // ATM의 보유 현금량 조정
    cash[50000] += temp_50000;
    cash[10000] += temp_10000;
    cash[5000] += temp_5000;
    cash[1000] += temp_1000;

    cout << temp_deposit << " KRW has been deposited." << endl;
    cout << "Your current balance is " << user_balance
         << " KRW. Transaction complete." << endl;

    string record;
    record += "Transaction ID: " + to_string(transaction_id) +
              ", Card Number: " + inserted_card.first->get_card_number() +
              ", Transaction Type: Cash deposit, Amount: " +
              to_string(temp_deposit - fee) + ", Fee: " + to_string(fee) + "\n";
    atmTransActionHistory += record; // ATM 거래내역에 추가함
    transaction_id += 1;

    string temp_history = "\nDeposit: " + to_string(temp_deposit) +
                          "Won,\tBalance: " + to_string(user_balance) + "Won";
    userActionHistory += temp_history;
    return;
  }

  goto insertcash_stage;
}

// 수표 입금 부분
void ATM::deposit(int num_check) {
  if (!inserted_card.second || inserted_card.first == nullptr) {
    cout << "No card has been inserted." << endl;
    return;
  }

  Account *user = inserted_card.first;
  int temp_deposit{0}, total_deposit{0}, fee{0}, temp_fee{0};
  bool confirm;
  int user_balance = user->get_account_balance();

  fee = (connected_bank == user->get_issued_bank()) ? 1000 : 2000;

  /*
  numcheck_stage:
      cout << "\n입금하실 수표의 장수를 입력하세요 : "; // 얘는 밖으로
  빠져나가기 cin >> num_check;
  */

  if (num_check > 30) {
    cout << "The number of checks you want to deposit is too large. Please "
            "deposit 30 or fewer checks."
         << endl;
    return;
  }

  for (int cycle = 0; cycle < num_check; cycle++) {
  insertcheck_stage:
    cout << "\nInsert the check to deposit: ";
    temp_deposit = input();

    if (temp_deposit < 100000) {
      cout << "The deposited amount is less than 100,000 KRW. "
              "Please try again."
           << endl;
      goto insertcheck_stage;
    }

    total_deposit += temp_deposit;
  }

  cout << "\nDo you want to deposit " << total_deposit << " KRW?" << "\t Yes: 1"
       << "\t No: 0 \t";
  cin >> confirm;

  if (confirm == 0) {
    total_deposit = 0;
    return;
  }

  cout << "\nPlease deposit the fee of " << fee
       << " KRW: "; // 1000원짜리 지폐로만 가능 - 한장이거나 두장이거나
  cin >> temp_fee;
  if (temp_fee != fee) {
    cout << "Inserted money is different with fee" << endl;
    return;
  }
  cash[1000] += fee / 1000;

  // 계좌 잔액 올리기
  user_balance += total_deposit;
  user->set_account_balance(user_balance);
  // checks[temp_deposit] += 1;  일단 수표 map으로 저장해서 넣긴 넣었는데 이거
  // 굳이 인거같기도?

  cout << "Your current balance is " << user_balance
       << " KRW. Transaction complete." << endl;

  string record;
  record +=
      "Transaction ID: " + to_string(transaction_id) +
      ", Card Number: " + inserted_card.first->get_card_number() +
      ", Transaction Type: Check Deposit, Amount: " + to_string(total_deposit) +
      ", Fee: " + to_string(fee) + "\n";
  atmTransActionHistory += record; // ATM 거래내역에 추가함
  transaction_id += 1;

  string temp_history = "\nDeposit: " + to_string(total_deposit) +
                        "Won,\tBalance: " + to_string(user_balance) + "Won";
  userActionHistory += temp_history;
  return;
}

void ATM::withdraw() {
  int atm_cash = cash[1000] * 1000 + cash[5000] * 5000 + cash[10000] * 10000 +
                 cash[50000] * 50000;
  string click;

  if (!inserted_card.second || inserted_card.first == nullptr) {
    cout << "No card has been inserted." << endl;
    return;
  }

  Account *user = inserted_card.first;
  int user_balance = user->get_account_balance();
  int fee = (connected_bank == user->get_issued_bank()) ? 1000 : 2000;

  // 수수료 안내 추가
  if (connected_bank == user->get_issued_bank()) {
    cout << "Fee: 1,000 KRW (Same bank transaction)" << endl;
  } else {
    cout << "Fee: 2,000 KRW (Different bank transaction)" << endl;
  }

  if (withdrawal_session >= 3) {
    cout << "A maximum of 3 withdrawals are allowed per session. Further "
            "withdrawals are restricted."
         << endl;
    return;
  }

  if (atm_cash == 0) {
    cout << "The ATM is out of cash. Withdrawal functionality is restricted."
         << endl;
    return;
  }

enter_amount:
  int amount;
  cout << "Please enter the withdrawal amount: ";
  amount = input();

  // 숫자가 아닌 값이 입력될 경우 예외 처리
  if (amount == -1 || amount == 0) {
    cout << "Please enter a valid value." << endl;
    cout << "Exit: 1 \nModify withdrawal amount: 2" << endl;
    cin >> click;
    if (click == "1")
      return;
    else if (click == "2")
      goto enter_amount;
    else {
      cout << "Invalid input. Withdrawal is terminated." << endl;
      return;
    }
  }

  if (amount > 500000) {
    cout << "The maximum withdrawal amount per transaction is 500,000 KRW.\n";
    cout << "Exit: 1 \nModify withdrawal amount: 2" << endl;
    cin >> click;
    if (click == "1")
      return;
    else if (click == "2")
      goto enter_amount;
    else {
      cout << "Invalid input. Withdrawal is terminated." << endl;
      return;
    }
  }

  if (amount + fee > user_balance) {
    cout << "Insufficient account balance.\n"
         << "Your current balance is " << user_balance << " KRW.\n"
         << "The maximum amount available for withdrawal is "
         << user_balance - fee << " KRW (including " << fee << " KRW fee)."
         << endl;
    cout << "Exit: 1 \nModify withdrawal amount: 2" << endl;
    cin >> click;
    if (click == "1")
      return;
    else if (click == "2")
      goto enter_amount;
    else {
      cout << "Invalid input. Withdrawal is terminated." << endl;
      return;
    }
  }

  if (amount > atm_cash) {
    cout << "The ATM does not have sufficient cash.\n"
         << "The maximum amount available for withdrawal is " << atm_cash
         << " KRW." << endl;
    cout << "Exit: 1 \nModify withdrawal amount: 2" << endl;
    cin >> click;
    if (click == "1")
      return;
    else if (click == "2")
      goto enter_amount;
    else {
      cout << "Invalid input. Withdrawal is terminated." << endl;
      return;
    }
  }
  // 1000원의 배수가 아닌지 확인
  if (amount % 1000 != 0) {
    cout << "The withdrawal amount must be a multiple of 1000 KRW. Please "
            "enter again."
         << endl;
    cout << "Exit: 1 \nModify withdrawal amount: 2" << endl;
    cin >> click;
    if (click == "1")
      return;
    else if (click == "2")
      goto enter_amount;
    else {
      cout << "Invalid input. Withdrawal is terminated." << endl;
      return;
    }
  }
  // 출금 진행
  if (!dispenseCash(amount)) {
    cout << "The requested amount cannot be provided by the ATM. Please enter "
            "a different amount."
         << endl;
    return;
  }
  user->set_account_balance(user->get_account_balance() - amount - fee);
  withdrawal_session += 1;
  withdrawal_amount += amount;

  cout << amount << " KRW has been withdrawn. (Fee: " << fee << " KRW)\n"
       << "Your current account balance is " << user->get_account_balance()
       << " KRW.\n"
       << "Number of withdrawals: " << withdrawal_session << " / " << 3 << endl;

  string record;
  record += "Transaction ID: " + to_string(transaction_id) +
            ", Card Number: " + inserted_card.first->get_card_number() +
            ", Transaction Type: Withdrawal, Amount: " + to_string(amount) +
            ", Fee: " + to_string(fee) + "\n";
  atmTransActionHistory += record; // ATM 거래내역에 추가함
  transaction_id += 1;

  string temp_history =
      "\nWithdrawal: " + to_string(amount) +
      "Won,\tBalance: " + to_string(user->get_account_balance()) + "Won";
  userActionHistory += temp_history;
}

bool ATM::dispenseCash(int amount) {
  int denominations[] = {50000, 10000, 5000, 1000};
  map<int, int> bills_to_dispense;
  int remaining_amount = amount;

  for (int denom : denominations) {
    int num_bills = min(remaining_amount / denom, cash[denom]);
    if (num_bills > 0) {
      bills_to_dispense[denom] = num_bills;
      cash[denom] -= num_bills;
      remaining_amount -= denom * num_bills;
    }
  }

  if (remaining_amount > 0) {
    for (auto &pair : bills_to_dispense) {
      cash[pair.first] += pair.second;
    }
    return false;
  }

  if (current_language == "Korean") {
    cout << "지폐 지급 내역:" << endl;
    for (auto &pair : bills_to_dispense) {
      cout << pair.first << "원권: " << pair.second << "장" << endl;
    }
  } else {
    cout << "Dispensed bills:" << endl;
    for (auto &pair : bills_to_dispense) {
      cout << pair.first << " KRW bills: " << pair.second << endl;
    }
  }

  return true;
}

void ATM::transfer() {
  string account_or_cash;
  string destination_account_num;
  Account *destination_account = nullptr;
  int transfer_fee;
  cout << "Press 1 for account transfer, 2 for cash transfer, or 3 to cancel: ";
  cin >> account_or_cash;
  if (account_or_cash == "1") {
    // 계좌이체
    // 카드 들어와있는지 확인 >> 현금이체처럼 상대계좌 입력받기(수수료 설정) >>
    // 송금액 받아서 처리;
    if (!inserted_card.second) {
      cout << "No card has been inserted." << endl;
      return;
    }
    while (true) // 상대방 계좌를 입력받음
    {
      cout << "Enter the 12-digit account number of the recipient: ";
      cin >> destination_account_num;
      bool is_all_num = true;
      for (char ch : destination_account_num) {
        if (ch < '0' || ch > '9') {
          cout << "The input contains non-numeric characters." << endl;
          is_all_num = false;
          break;
        }
      }
      if (!is_all_num) {
        continue;
      }
      if (destination_account_num.length() != 12) {
        cout << "The input length is not 12 digits." << endl;
        continue;
      }
      bool is_valid_destination = false;
      for (Bank *bank : bank_list) // 상대방 계좌가 유효한지 확인
      {
        Account *acc_found = bank->find_account(destination_account_num);
        if (acc_found != nullptr) {
          is_valid_destination = true;
          destination_account = acc_found;
          break;
        }
      }
      if (!is_valid_destination) {
        cout << "The recipient's account number is invalid." << endl;
        return;
      } else if (destination_account == inserted_card.first) {
        cout << "You cannot transfer to the same account as inserted card"
             << endl;
        continue;
      }
      if (connected_bank == inserted_card.first->get_issued_bank() &&
          connected_bank == destination_account->get_issued_bank()) {
        transfer_fee = 2000;
      } // 타행송금 가능하도록 수정함
      else if (connected_bank == inserted_card.first->get_issued_bank() ||
               connected_bank == destination_account->get_issued_bank()) {
        transfer_fee = 3000;
      } else if (connected_bank != inserted_card.first->get_issued_bank() &&
                 connected_bank != destination_account->get_issued_bank()) {
        transfer_fee = 4000;
      } else {
        transfer_fee = 0;
      }
      break;
    }
    int transfer_amount;
    cout << "The transfer fee is " << transfer_fee
         << " KRW. Please enter the amount to transfer: ";
    transfer_amount = input();
    if (transfer_amount < 0) {
      cout << "Invalid input" << endl;
      return;
    } else if (transfer_amount == 0) {
      cout << "You cannot transfer 0 Won" << endl;
      return;
    }
    int total_amount = transfer_amount + transfer_fee;
    if (inserted_card.first->get_account_balance() <
        total_amount) // 계좌 잔액 고려하도록 수정
    {
      cout << "Insufficient account balance." << endl;
      return;
    }
    cout << "You entered " << transfer_amount << " KRW. The transfer fee is "
         << transfer_fee << " KRW. A total of " << total_amount
         << " KRW will be deducted from your account." << endl;

    cout << "Do you want to approve this transaction? Yes: 1, No: 0" << endl;
    string approval;
    cin >> approval;
    if (approval == "1") {
      cout << "Transaction Approved" << endl;
    }
    else if (approval == "0") {
      cout << "Transaction Canceled" << endl;
      return;
    }
    else {
      cout << "Invalid input" << endl;
      return;
    }
    inserted_card.first->balance_change(-total_amount);
    destination_account->balance_change(transfer_amount);
    cout << "Transfer completed successfully." << endl;

    string record = "";
    record += "Transaction ID: " + to_string(transaction_id) +
              ", Card Number: " + inserted_card.first->get_card_number() +
              ", Transaction Type: Account Transfer, Amount: " +
              to_string(transfer_amount) + ", Fee: " + to_string(transfer_fee) +
              ", Source Account: " + inserted_card.first->get_account_number() +
              ", Destination Account: " + destination_account_num + "\n";
    atmTransActionHistory += record; // ATM 거래내역에 추가함
    transaction_id += 1;
    return;
  } else if (account_or_cash == "2") {
    // 현금이체
    if (!inserted_card.second) // 현금이체도 카드 삽입해야지만 가능
    {
      cout << "No card has been inserted." << endl;
      return;
    }
    while (true) // 상대방 계좌를 입력받음
    {
      cout << "Enter the 12-digit account number of the recipient: ";
      cin >> destination_account_num;
      bool is_all_num = true;
      for (char ch : destination_account_num) {
        if (ch < '0' || ch > '9') {
          cout << "The input contains non-numeric characters." << endl;
          is_all_num = false;
          break;
        }
      }
      if (!is_all_num) {
        continue;
      }
      if (destination_account_num.length() != 12) {
        cout << "The input length is not 12 digits." << endl;
        continue;
      }
      bool is_valid_destination = false;
      for (Bank *bank : bank_list) // 상대방 계좌가 유효한지 확인
      {
        Account *acc_found = bank->find_account(destination_account_num);
        if (acc_found != nullptr) {
          is_valid_destination = true;
          destination_account = acc_found;
          break;
        }
      }
      if (!is_valid_destination) {
        cout << "The recipient's account number is invalid." << endl;
        return;
      } else if (destination_account == inserted_card.first) {
        cout << "You cannot transfer to the same account as inserted card"
             << endl;
        continue;
      }
      transfer_fee = 1000;
      transfer_fee = 1000;
      break;
    }
    int won_1000, won_5000, won_10000, won_50000, entered_fee;
    cout << "The transfer fee is " << transfer_fee
         << " KRW. Please enter the number of bills for each denomination."
         << endl;
    cout << "Enter the number of 1,000 KRW bills: ";
    won_1000 = input();
    if (won_1000 < 0) {
      cout << "Invalid input" << endl;
      return;
    }
    cout << "Enter the number of 5,000 KRW bills: ";
    won_5000 = input();
    if (won_5000 < 0) {
      cout << "Invalid input" << endl;
      return;
    }
    cout << "Enter the number of 10,000 KRW bills: ";
    won_10000 = input();
    if (won_10000 < 0) {
      cout << "Invalid input" << endl;
      return;
    }
    cout << "Enter the number of 50,000 KRW bills: ";
    won_50000 = input();
    if (won_50000 < 0) {
      cout << "Invalid input" << endl;
      return;
    }
    int total_inserted = won_1000 * 1000 + won_5000 * 5000 + won_10000 * 10000 +
                         won_50000 * 50000;
    if (total_inserted <= 0) {
      cout << "Cash is not inserted" << endl;
      return;
    }
    cout << "Please enter the fee " << transfer_fee << " KRW: ";
    entered_fee = input();
    if (entered_fee < 0) {
      cout << "Invalid input" << endl;
      return;
    } else if (entered_fee != transfer_fee) {
      cout << "Transfer fee is not matched" << endl;
      return;
    }

    cout << "You inserted a total of " << total_inserted + transfer_fee
         << " KRW. The transfer fee is " << transfer_fee << " KRW. "
         << total_inserted
         << " KRW will be transferred to the destination account." << endl;

    cout << "Do you want to approve this transaction? Yes: 1, No: 0" << endl;
    string approval;
    cin >> approval;
    if (approval == "1") {
      cout << "Transaction Approved" << endl;
    }
    else if (approval == "0") {
      cout << "Transaction Canceled" << endl;
      return;
    }
    else {
      cout << "Invalid input" << endl;
      return;
    }

    cash[1000] += won_1000;
    cash[5000] += won_5000;
    cash[10000] += won_10000;
    cash[50000] += won_50000;
    cash[1000] += (transfer_fee / 1000); // 수수료로 받은 지폐를 atm에 추가

    destination_account->balance_change(total_inserted);
    cout << "Transfer completed successfully." << endl;

    string record = "";
    record += "Transaction ID: " + to_string(transaction_id) +
              ", Card Number: " + inserted_card.first->get_card_number() +
              ", Transaction Type: Cash Transfer, Amount: " +
              to_string(total_inserted) + ", Fee: " + to_string(transfer_fee) +
              ", Source Account: " + inserted_card.first->get_account_number() +
              ", Destination Account: " + destination_account_num + "\n";
    atmTransActionHistory += record; // ATM 거래내역에 추가함
    string temp_history =
        "\nTransfer: " + to_string(total_inserted) + "Won,\tBalance: " +
        to_string(inserted_card.first->get_account_balance()) + "Won";
    userActionHistory += temp_history;

    transaction_id += 1;
    return;
  } else if (account_or_cash == "3") {
    cout << "Transfer cancelled." << endl;
    return;
  } else {
    cout << "Invalid input. Please enter 1, 2, or 3." << endl;
    return;
  }
}

void ATM::card_insert() {
  string entered_card_number;
  int attempts = 0;
  const int max_attempts = 3;

  if (inserted_card.second) {
    cout << "A card is already inserted." << endl;
    return;
  }

  // 카드 삽입 안내
  cout << "Please insert your card (Enter card number): ";
  cin >> entered_card_number;

  // admin card인지 확인
  if (entered_card_number == "admin") // admin의 카드번호는 "admin"으로 가정
  {
    admin_menu();
    return;
  }

  // 카드가 유효한지 확인
  Account *user_account = isCard_valid(entered_card_number);
  if (user_account == nullptr) {
    cout << "Invalid card number." << endl;
    return;
  }

  // 단일은행 ATM인 경우 카드 발급 은행 확인
  if (single_Bank && user_account->get_issued_bank() != connected_bank) {
    cout << "This ATM only accepts cards issued by "
         << connected_bank->getBankName() << "." << endl;
    return;
  }

  // 비밀번호 입력 및 인증 요청
  while (attempts < max_attempts) {
    if (verify_card_password(user_account)) {
      // 인증 성공 시 세션 시작
      cout << "Authentication successful. You can now start your transaction."
           << endl;
      inserted_card = {user_account, true}; // 인증된 사용자와 세션 시작
      withdrawal_session = 0;
      withdrawal_amount = 0;
      return;
    } else {
      attempts++;
      cout << "Incorrect password. (" << attempts << "/" << max_attempts << ")"
           << endl;
    }
  }

  // 비밀번호 입력 시도 초과 시 세션 종료
  cout << "Maximum password attempts exceeded. Session terminated, and the "
          "card is returned."
       << endl;
}

// 카드 번호를 통해 계좌 찾기.
Account *Bank::find_account_by_card_number(const string &card_number) {
  for (auto &act_pair : accounts) {
    if (act_pair.second.get_card_number() == card_number) {
      return &act_pair.second;
    }
  }
  return nullptr;
}

// 카드 유효성 검사 함수
Account *ATM::isCard_valid(const string &card_number) {
  for (Bank *bank : bank_list) {
    Account *user_account = bank->find_account_by_card_number(card_number);
    if (user_account != nullptr) {
      return user_account; // 유효한 카드일 경우 사용자 계정을 반환
    }
  }
  return nullptr; // 유효하지 않은 카드
}

//  카드 비밀번호 인증 함수
bool ATM::verify_card_password(Account *user_account) {
  string password;
  cout << "Enter your password: ";
  cin >> password;

  // 카드가 속한 은행을 통해 인증 요청
  return user_account->get_issued_bank()->authorize_user(*user_account,
                                                         password);
}

bool Bank::authorize_user(Account &account, const string &password) {
  // 계좌가 이 은행에서 발급된 계좌인지 확인
  if (&account != find_account(account.get_account_number()))
    return false;

  // 비밀번호가 올바르면 true 리턴.
  return account.get_password() == password;
}

void Bank::chargeATM(string serial) {
  ATM *atm = get_ATM(serial);
  if (atm == nullptr) {
    cout << "해당 시리얼 번호의 ATM이 존재하지 않습니다.\n";
    return;
  }

  int amount;
  cout << "\n=== ATM 현금 충전 ===\n";

  for (int denomination : {50000, 10000, 5000, 1000}) {
    while (true) {
      cout << denomination << "원권 충전 수량을 입력하세요: ";
      cin >> amount;

      if (cin.fail()) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "유효한 숫자를 입력해주세요.\n";
        continue;
      }

      if (amount < 0) {
        cout << "0 이상의 수량을 입력해주세요.\n";
        continue;
      }

      atm->access_cash()[denomination] += amount;
      break;
    }
  }
  cout << "ATM 현금 충전이 완료되었습니다.\n";
}

class BilingualATM : public ATM {
public:
  void set_language() override;
  BilingualATM(string serial_number, bool single_Bank, Bank *connected_bank)
      : ATM(serial_number, single_Bank, connected_bank) {}
  void deposit() override;              // 현금
  void deposit(int check_num) override; // 수표
  void withdraw() override;
  void transfer() override;
  void card_insert() override;
  bool verify_card_password(Account *user_account) override;
  string getType() override { return "BilingualATM"; }
};

void BilingualATM::set_language() {
  int choice;

  while (true) {
    // 현재 언어에 따라 언어 선택 프롬프트 출력
    if (current_language == "English") {
      cout << "\nPlease select a language for the ATM:\n";
      cout << "1. Korean\n";
      cout << "2. English\n";
      cout << "Choice (1 or 2): ";
    } else if (current_language == "Korean") {
      cout << "\n언어 변경을 원하시면 다음 중 하나를 선택해주세요:\n";
      cout << "1. 한국어\n";
      cout << "2. 영어\n";
      cout << "선택 (1 또는 2): ";
    } else {
      // 현재 언어에 오류가 발생할 가능성 고려 필요한지 애매
      cout << "\nUnknown current language. Defaulting to English.\n";
      current_language = "English";
      continue;
    }

    // 사용자 입력 받기
    cin >> choice;

    // 입력 스트림 상태 확인 및 예외 처리
    if (cin.fail()) {
      cin.clear();
      cin.ignore(numeric_limits<streamsize>::max(), '\n');
      if (current_language == "English") {
        cout << "Invalid input. Please enter a number (1 or 2)." << endl;
      } else {
        cout << "유효한 숫자를 입력해주세요 (1 또는 2)." << endl;
      }
      continue;
    }

    // 입력에 따른 언어 설정
    if (choice == 1) {
      current_language = "Korean";
      cout << "언어가 한국어로 설정되었습니다." << endl;
      break;
    } else if (choice == 2) {
      current_language = "English";
      cout << "Language has been set to English." << endl;
      break;
    } else {
      // 잘못된 선택 처리
      if (current_language == "English") {
        cout << "Invalid choice. Please enter 1 or 2." << endl;
      } else {
        cout << "잘못된 선택입니다. 1 또는 2를 입력해주세요." << endl;
      }
    }
  }
}

void BilingualATM::deposit() {

  if (current_language == "English") {
    ATM::deposit();
    return;
  }

  if (!inserted_card.second || inserted_card.first == nullptr) {
    cout << "카드가 삽입되어 있지 않습니다." << endl;
    return;
  }

  Account *user = inserted_card.first;

  // 현재 언어가 영어면 기본 ATM 함수 호출 한국어라면 아래로 진행
  // 세부 내용은 동일하지만 사용자 입장에서 언어가 한국어

  int temp_1000{0}, temp_5000{0}, temp_10000{0}, temp_50000{0}, temp_deposit{0},
      bill_count{0}, fee{0}, temp_fee{0};
  string click;
  bool confirm;
  int user_balance = user->get_account_balance();

  // 카드 넣어야 입금 시작 - 카드 삽입, 유효검사 파트

  // 수수료 지정 파트 - 같은 은행이면 fee는 1000, 다른 은행이면 2000으로
  // 설정하는 파트
  fee = (connected_bank == user->get_issued_bank()) ? 1000 : 2000;

insertcash_stage:
  cout << "\n입금하실 50000원 지폐의 수를 입력하세요 : ";
  temp_50000 = input();
  if (temp_50000 < 0) {
    cout << "잘못된 입력입니다. 올바른 수를 입력하세요." << endl;
    cout << "종료 : 1 \n출금액 수정 : 2" << endl;
    cin >> click;
    if (click == "1")
      return;
    else if (click == "2")
      goto insertcash_stage;
  }

  cout << "입금하실 10000원 지폐의 수를 입력하세요 : ";
  temp_10000 = input();
  if (temp_10000 < 0) {
    cout << "잘못된 입력입니다. 올바른 수를 입력하세요." << endl;
    cout << "종료 : 1 \n출금액 수정 : 2" << endl;
    cin >> click;
    if (click == "1")
      return;
    else if (click == "2")
      goto insertcash_stage;
  }

  cout << "입금하실 5000원 지폐의 수를 입력하세요 : ";
  temp_5000 = input();
  if (temp_5000 < 0) {
    cout << "잘못된 입력입니다. 올바른 수를 입력하세요." << endl;
    cout << "종료 : 1 \n출금액 수정 : 2" << endl;
    cin >> click;
    if (click == "1")
      return;
    else if (click == "2")
      goto insertcash_stage;
  }

  cout << "입금하실 1000원 지폐의 수를 입력하세요 : ";
  temp_1000 = input();
  if (temp_1000 < 0) {
    cout << "잘못된 입력입니다. 올바른 수를 입력하세요." << endl;
    cout << "종료 : 1 \n출금액 수정 : 2" << endl;
    cin >> click;
    if (click == "1")
      return;
    else if (click == "2")
      goto insertcash_stage;
  }

  bill_count = temp_50000 + temp_10000 + temp_5000 + temp_1000;

  if (bill_count == 0) {
    cout << "0원이 삽입되었습니다. 다시 삽입해주세요" << endl;
    return;
  }

  temp_deposit = temp_50000 * 50000 + temp_10000 * 10000 + temp_5000 * 5000 +
                 temp_1000 * 1000;

  // 총 금액이 일정 금액 이상이면 돌아가기 or 종료하기 이건 고민해봐야할듯
  if (bill_count > 50) {
    cout << "\n현재 입금하고자 하는 지폐의 장수가 너무 큽니다. 50장 이하의 "
            "장수를 입금해주세요"
         << endl;
    temp_deposit = 0;
    bill_count = 0;
    goto insertcash_stage; // or return
  }

  cout << "\n"
       << temp_deposit << "원을 입금하시겠습니까?" << "\t 네: 1"
       << "\t 아니요: 0 \t"; // \n은 가독성땜에 추가함

  cin >> confirm; // 1이 확인 0이면 다시 입금하기 - 혹시 시연할때 잘못 입력하는
  // 경우랑 현실성 생각해서 확인버튼 만듦

  if (confirm) {
    cout << "\n수수료 " << fee
         << "원을 입금하세요 : "; // 1000원짜리 지폐로만 가능 - 한장이거나
                                  // 두장이거나
    cin >> temp_fee;
    if (temp_fee != fee) {
      cout << "삽입된 금액이 수수료와 다릅니다." << endl;
      return;
    }
    cash[1000] += fee / 1000;

    // 계좌 잔액 올리기
    user_balance += (temp_deposit);
    user->set_account_balance(user_balance);

    // ATM의 보유 현금량 조정
    cash[50000] += temp_50000;
    cash[10000] += temp_10000;
    cash[5000] += temp_5000;
    cash[1000] += temp_1000;

    cout << temp_deposit << "원을 입금합니다" << endl;
    cout << "현재 잔액은 " << user_balance << "입니다. 거래를 종료합니다"
         << endl;

    string record;
    record +=
        "Transaction ID: " + to_string(transaction_id) +
        ", Card Number: " + inserted_card.first->get_card_number() +
        ", Transaction Type: Cash deposit, Amount: " + to_string(temp_deposit) +
        ", Fee: " + to_string(fee) + "\n";
    atmTransActionHistory += record; // ATM 거래내역에 추가함
    transaction_id += 1;

    string temp_history = "\n입금: " + to_string(temp_deposit) +
                          "원,\t잔액: " + to_string(user_balance) + "원";
    userActionHistory += temp_history;
    return;
  }

  goto insertcash_stage;
}

// 수표 입금 부분
void BilingualATM::deposit(int num_check) {

  if (current_language == "English") {
    ATM::deposit(num_check);
    return;
  }

  if (!inserted_card.second || inserted_card.first == nullptr) {
    cout << "카드가 삽입되어 있지 않습니다." << endl;
    return;
  }

  Account *user = inserted_card.first;

  int temp_deposit{0}, total_deposit{0}, fee{0}, temp_fee{0};
  bool confirm;
  int user_balance = user->get_account_balance();

  fee = (connected_bank == user->get_issued_bank()) ? 1000 : 2000;

  /*
  numcheck_stage:
      cout << "\n입금하실 수표의 장수를 입력하세요 : "; // 얘는 밖으로
  빠져나가기 cin >> num_check;
  */

  if (num_check > 30) {
    cout << "현재 입금하고자 하는 수표의 장수가 너무 큽니다. 30장 이하의 "
            "장수를 입금해주세요"
         << endl;
    return;
  }

  for (int cycle = 0; cycle < num_check; cycle++) {
  insertcheck_stage:
    cout << "\n입금하실 수표를 삽입하세요 : ";
    temp_deposit = input();

    if (temp_deposit < 100000) {
      cout << "100,000원 미만의 금액이 삽입되었습니다. 다시 삽입해주세요"
           << endl;
      goto insertcheck_stage;
    }

    total_deposit += temp_deposit;
  }

  cout << "\n"
       << total_deposit << "원을 입금하시겠습니까?" << "\t 네: 1"
       << "\t 아니요: 0 \t";
  cin >> confirm;

  if (confirm == 0) {
    total_deposit = 0;
    return;
  }

  cout << "\n수수료 " << fee
       << "원을 입금하세요 : "; // 1000원짜리 지폐로만 가능 - 한장이거나
  // 두장이거나
  cin >> temp_fee;
  if (temp_fee != fee) {
    cout << "삽입된 금액이 수수료와 다릅니다." << endl;
    return;
  }
  cash[1000] += fee / 1000;

  // 계좌 잔액 올리기
  user_balance += total_deposit;
  user->set_account_balance(user_balance);
  // checks[temp_deposit] += 1;  일단 수표 map으로 저장해서 넣긴 넣었는데 이거
  // 굳이 인거같기도?

  cout << "현재 잔액은 " << user_balance << "입니다. 거래를 종료합니다" << endl;

  string record;
  record +=
      "Transaction ID: " + to_string(transaction_id) +
      ", Card Number: " + inserted_card.first->get_card_number() +
      ", Transaction Type: Check Deposit, Amount: " + to_string(total_deposit) +
      ", Fee: " + to_string(fee) + "\n";
  atmTransActionHistory += record; // ATM 거래내역에 추가함
  transaction_id += 1;

  string temp_history = "\n입금: " + to_string(total_deposit) +
                        "원,\t잔액: " + to_string(user_balance) + "원";
  userActionHistory += temp_history;
  return;
}

void BilingualATM::withdraw() {
  if (current_language == "English") {
    ATM::withdraw();
    return;
  }

  int atm_cash = cash[1000] * 1000 + cash[5000] * 5000 + cash[10000] * 10000 +
                 cash[50000] * 50000;
  string click;

  if (!inserted_card.second || inserted_card.first == nullptr) {
    cout << "카드가 삽입되어 있지 않습니다." << endl;
    return;
  }

  Account *user = inserted_card.first;
  int user_balance = user->get_account_balance();
  int fee = (connected_bank == user->get_issued_bank()) ? 1000 : 2000;

  // 수수료 안내 추가
  if (connected_bank == user->get_issued_bank()) {
    cout << "수수료: 1,000원 (동일 은행 거래)" << endl;
  } else {
    cout << "수수료: 2,000원 (타 은행 거래)" << endl;
  }

  if (withdrawal_session >= 3) {
    cout << "한 세션 당 최대 3번의 출금이 가능합니다. 이를 초과해 출금이 "
            "제한됩니다."
         << endl;
    return;
  }

  if (atm_cash == 0) {
    cout << "ATM에 현금이 없습니다. 출금 기능이 제한됩니다." << endl;
    return;
  }

enter_amount:
  int amount;
  cout << "원하시는 출금액을 입력하세요 : ";
  amount = input();

  // 숫자가 아닌 값이 입력될 경우 예외 처리
  if (amount == -1) {
    cout << "잘못된 입력입니다. 올바른 수를 입력하세요." << endl;
    goto enter_amount;
  }

  if (amount > 500000) {
    cout << "한 거래당 최대 출금 금액은 50만원입니다.\n";
    cout << "종료 : 1 \n출금액 수정 : 2" << endl;
    cin >> click;
    if (click == "1")
      return;
    else if (click == "2")
      goto enter_amount;
    else {
      cout << "잘못된 입력입니다. 출금이 종료됩니다." << endl;
      return;
    }
  }

  if (amount + fee > user_balance) {
    cout << "계좌의 잔액이 부족합니다.\n"
         << "현재 잔액은 " << user_balance << "원 입니다.\n"
         << "출금 가능한 최대 금액은 " << user_balance - fee
         << "원 입니다.(수수료 " << fee << "원 포함)" << endl;
    cout << "종료 : 1 \n출금액 수정 : 2" << endl;
    cin >> click;
    if (click == "1")
      return;
    else if (click == "2")
      goto enter_amount;
    else {
      cout << "잘못된 입력입니다. 출금이 종료됩니다." << endl;
      return;
    }
  }

  if (amount > atm_cash) {
    cout << "ATM에 현금이 부족합니다.\n"
         << "출금 가능한 액수는 " << atm_cash << "원 입니다." << endl;
    cout << "종료 : 1 \n출금액 수정 : 2" << endl;
    cin >> click;
    if (click == "1")
      return;
    else if (click == "2")
      goto enter_amount;
    else {
      cout << "잘못된 입력입니다. 출금이 종료됩니다." << endl;
      return;
    }
  }

  // 1000원의 배수가 아닌지 확인
  if (amount % 1000 != 0) {
    cout << "출금 금액은 1000원의 배수여야 합니다. 다시 입력해주세요." << endl;
    goto enter_amount;
  }

  // 출금 진행
  if (!dispenseCash(amount)) {
    cout << "요청하신 금액을 현재 ATM이 제공할 수 없습니다. 다른 금액을 "
            "입력해주세요"
         << endl;
    return;
  }
  user->set_account_balance(user->get_account_balance() - amount - fee);
  withdrawal_session += 1;
  withdrawal_amount += amount;

  cout << amount << "원이 출금되었습니다.(수수료: " << fee << "원)\n"
       << "현재 계좌 잔액은 " << user->get_account_balance() << "원 입니다.\n"
       << "출금 횟수 : " << withdrawal_session << " / " << 3 << endl;

  string record;
  record += "Transaction ID: " + to_string(transaction_id) +
            ", Card Number: " + inserted_card.first->get_card_number() +
            ", Transaction Type: Withdrawal, Amount: " + to_string(amount) +
            ", Fee: " + to_string(fee) + "\n";
  atmTransActionHistory += record; // ATM 거래내역에 추가함
  transaction_id += 1;

  string temp_history = "\n출금: " + to_string(amount) +
                        "원,\t잔액: " + to_string(user->get_account_balance()) +
                        "원";
  userActionHistory += temp_history;
}

void BilingualATM::transfer() {

  if (current_language == "English") {
    ATM::transfer();
    return;
  }
  string account_or_cash;
  string destination_account_num;
  Account *destination_account = nullptr;
  int transfer_fee;
  cout << "계좌 이체를 하시려면 1, 현금 이체를 하시려면 2, 취소하시려면 3을 "
          "입력해주세요: ";
  cin >> account_or_cash;
  if (account_or_cash == "1") {
    // 계좌이체
    // 카드 들어와있는지 확인 >> 현금이체처럼 상대계좌 입력받기(수수료 설정) >>
    // 송금액 받아서 처리;
    if (!inserted_card.second) {
      cout << "카드가 삽입되어 있지 않습니다." << endl;
      return;
    }
    while (true) // 상대방 계좌를 입력받음
    {
      cout << "상대방의 계좌번호 12자리를 입력해주세요: ";
      cin >> destination_account_num;
      bool is_all_num = true;
      for (char ch : destination_account_num) {
        if (ch < '0' || ch > '9') {
          cout << "입력에 숫자 외의 값이 들어있습니다." << endl;
          is_all_num = false;
          break;
        }
      }
      if (!is_all_num) {
        continue;
      }
      if (destination_account_num.length() != 12) {
        cout << "입력 길이가 12자리가 아닙니다." << endl;
        continue;
      }
      bool is_valid_destination = false;
      for (Bank *bank : bank_list) // 상대방 계좌가 유효한지 확인
      {
        Account *acc_found = bank->find_account(destination_account_num);
        if (acc_found != nullptr) {
          is_valid_destination = true;
          destination_account = acc_found;
          break;
        }
      }
      if (!is_valid_destination) {
        cout << "상대방의 계좌번호가 유효하지 않습니다." << endl;
        return;
      } else if (destination_account == inserted_card.first) {
        cout << "카드와 동일한 계좌로 송금할 수 없습니다" << endl;
        continue;
      }
      if (connected_bank == inserted_card.first->get_issued_bank() &&
          connected_bank == destination_account->get_issued_bank()) {
        transfer_fee = 2000;
      } // 타행송금 가능하도록 수정함
      else if (connected_bank == inserted_card.first->get_issued_bank() ||
               connected_bank == destination_account->get_issued_bank()) {
        transfer_fee = 3000;
      } else if (connected_bank != inserted_card.first->get_issued_bank() &&
                 connected_bank != destination_account->get_issued_bank()) {
        transfer_fee = 4000;
      } else {
        transfer_fee = 0;
      }
      break;
    }
    int transfer_amount;
    cout << "송금 수수료는 " << transfer_fee
         << "원입니다. 송금하실 금액을 입력해주세요: ";
    transfer_amount = input();
    if (transfer_amount < 0) {
      cout << "유효한 입력이 아닙니다" << endl;
      return;
    } else if (transfer_amount == 0) {
      cout << "0원을 송금할 수 없습니다" << endl;
      return;
    }
    int total_amount = transfer_amount + transfer_fee;
    if (inserted_card.first->get_account_balance() <
        total_amount) // 계좌 잔액 고려하도록 수정
    {
      cout << "계좌 잔액이 부족합니다." << endl;
      return;
    }
    cout << "입력하신 금액은 " << transfer_amount << "원이며, 송금 수수료는 "
         << transfer_fee << "원입니다. 고객님의 계좌에서 " << total_amount
         << "원이 차감됩니다." << endl;

    cout << "이 거래를 승인하시겠습니까? 네: 1, 아니오: 0" << endl;
    string approval;
    cin >> approval;
    if (approval == "1") {
      cout << "거래가 승인되었습니다" << endl;
    }
    else if (approval == "0") {
      cout << "거래가 취소되었습니다" << endl;
      return;
    }
    else {
      cout << "유효한 입력이 아닙니다" << endl;
      return;
    }

    inserted_card.first->balance_change(-total_amount);
    destination_account->balance_change(transfer_amount);
    cout << "송금이 완료되었습니다." << endl;

    string record = "";
    record += "Transaction ID: " + to_string(transaction_id) +
              ", Card Number: " + inserted_card.first->get_card_number() +
              ", Transaction Type: Account Transfer, Amount: " +
              to_string(transfer_amount) + ", Fee: " + to_string(transfer_fee) +
              ", Source Account: " + inserted_card.first->get_account_number() +
              ", Destination Account: " + destination_account_num + "\n";
    atmTransActionHistory += record; // ATM 거래내역에 추가함
    transaction_id += 1;

    string temp_history =
        "\n송금: " + to_string(transfer_amount) +
        "원,\t잔액: " + to_string(inserted_card.first->get_account_balance()) +
        "원";
    userActionHistory += temp_history;
    return;
  } else if (account_or_cash == "2") {
    // 현금이체
    if (!inserted_card.second) // 현금이체도 카드 삽입해야지만 가능
    {
      cout << "카드가 삽입되어 있지 않습니다." << endl;
      return;
    }
    while (true) // 상대방 계좌를 입력받음
    {
      cout << "상대방의 계좌번호 12자리를 입력해주세요: ";
      cin >> destination_account_num;
      bool is_all_num = true;
      for (char ch : destination_account_num) {
        if (ch < '0' || ch > '9') {
          cout << "입력에 숫자 외의 값이 들어있습니다." << endl;
          is_all_num = false;
          break;
        }
      }
      if (!is_all_num) {
        continue;
      }
      if (destination_account_num.length() != 12) {
        cout << "입력 길이가 12자리가 아닙니다." << endl;
        continue;
      }
      bool is_valid_destination = false;
      for (Bank *bank : bank_list) // 상대방 계좌가 유효한지 확인
      {
        Account *acc_found = bank->find_account(destination_account_num);
        if (acc_found != nullptr) {
          is_valid_destination = true;
          destination_account = acc_found;
          break;
        }
      }
      if (!is_valid_destination) {
        cout << "상대방의 계좌번호가 유효하지 않습니다." << endl;
        return;
      } else if (destination_account == inserted_card.first) {
        cout << "카드와 동일한 계좌로 송금할 수 없습니다" << endl;
        continue;
      }
      transfer_fee = 1000;
      break;
    }
    int won_1000, won_5000, won_10000, won_50000, entered_fee;
    cout << "송금 수수료는 " << transfer_fee
         << "원입니다. 권종별로 송금하실 장수를 입력해주세요." << endl;
    cout << "송금하실 1000원권의 매수를 입력하세요: ";
    won_1000 = input();
    if (won_1000 < 0) {
      cout << "유효한 입력이 아닙니다" << endl;
      return;
    }
    cout << "송금하실 5000원권의 매수를 입력하세요: ";
    won_5000 = input();
    if (won_5000 < 0) {
      cout << "유효한 입력이 아닙니다" << endl;
      return;
    }
    cout << "송금하실 10000원권의 매수를 입력하세요: ";
    won_10000 = input();
    if (won_10000 < 0) {
      cout << "유효한 입력이 아닙니다" << endl;
      return;
    }
    cout << "송금하실 50000원권의 매수를 입력하세요: ";
    won_50000 = input();
    if (won_50000 < 0) {
      cout << "유효한 입력이 아닙니다" << endl;
      return;
    }

    int total_inserted = won_1000 * 1000 + won_5000 * 5000 + won_10000 * 10000 +
                         won_50000 * 50000;
    if (total_inserted <= 0) {
      cout << "현금이 투입되지 않았습니다" << endl;
      return;
    }
    cout << "수수료 " << transfer_fee << " KRW를 투입해주세요: ";
    entered_fee = input();
    if (entered_fee < 0) {
      cout << "유효한 입력이 아닙니다" << endl;
      return;
    } else if (entered_fee != transfer_fee) {
      cout << "수수료와 같은 금액이 아닙니다" << endl;
      return;
    }

    cout << "투입하신 총 금액은 " << total_inserted + transfer_fee
         << "원이며, 송금 수수료는 " << transfer_fee << "원입니다. "
         << total_inserted << "원이 송금됩니다." << endl;

    cout << "이 거래를 승인하시겠습니까? 네: 1, 아니오: 0" << endl;
    string approval;
    cin >> approval;
    if (approval == "1") {
      cout << "거래가 승인되었습니다" << endl;
    }
    else if (approval == "0") {
      cout << "거래가 취소되었습니다" << endl;
      return;
    }
    else {
      cout << "유효한 입력이 아닙니다" << endl;
      return;
    }

    cash[1000] += won_1000;
    cash[5000] += won_5000;
    cash[10000] += won_10000;
    cash[50000] += won_50000;
    cash[1000] += (transfer_fee / 1000); // 수수료로 받은 지폐를 atm에 추가

    destination_account->balance_change(total_inserted);
    cout << "송금이 완료되었습니다." << endl;

    string record = "";
    record += "Transaction ID: " + to_string(transaction_id) +
              ", Card Number: " + inserted_card.first->get_card_number() +
              ", Transaction Type: Cash Transfer, Amount: " +
              to_string(total_inserted) + ", Fee: " + to_string(transfer_fee) +
              ", Source Account: " + inserted_card.first->get_account_number() +
              ", Destination Account: " + destination_account_num + "\n";
    atmTransActionHistory += record; // ATM 거래내역에 추가함

    string temp_history =
        "\n송금: " + to_string(total_inserted) +
        "원,\t잔액: " + to_string(inserted_card.first->get_account_balance()) +
        "원";
    userActionHistory += temp_history;

    transaction_id += 1;
    return;
  } else if (account_or_cash == "3") {
    cout << "송금을 취소합니다." << endl;
    return;
  } else {
    cout << "1, 2, 3 이외의 값이 입력되었습니다." << endl;
    return;
  }
}

void BilingualATM::card_insert() {

  if (current_language == "English") {
    ATM::card_insert();
    return;
  }

  string entered_card_number;
  int attempts = 0;
  const int max_attempts = 3;

  if (inserted_card.second) {
    cout << "이미 카드가 삽입되어 있습니다." << endl;
    return;
  }

  // 카드 삽입 안내
  cout << "카드를 삽입하세요 (카드 번호 입력): ";
  cin >> entered_card_number;

  if (entered_card_number == "admin") // admin의 카드번호는 "admin"으로 가정
  {
    admin_menu();
    return;
  }

  // 카드가 유효한지 확인
  Account *user_account = isCard_valid(entered_card_number);
  if (user_account == nullptr) {
    cout << "존재하지 않는 카드번호입니다." << endl;
    return;
  }

  // 단일은행 ATM인 경우 카드 발급 은행 확인
  if (single_Bank && user_account->get_issued_bank() != connected_bank) {
    cout << "이 ATM은 " << connected_bank->getBankName()
         << "에서 발급한 카드만 사용 가능합니다." << endl;
    return;
  }

  // 비밀번호 입력 및 인증 요청
  while (attempts < max_attempts) {
    if (verify_card_password(user_account)) {
      // 인증 성공 시 세션 시작
      cout << "인증이 완료되었습니다. 거래를 시작할 수 있습니다." << endl;
      inserted_card = {user_account, true}; // 인증된 사용자와 세션 시작
      withdrawal_session = 0;
      withdrawal_amount = 0;
      return;
    } else {
      attempts++;
      cout << "잘못된 비밀번호입니다. (" << attempts << "/" << max_attempts
           << ")" << endl;
    }
  }

  // 비밀번호 입력 시도 초과 시 세션 종료
  cout
      << "비밀번호 입력 횟수를 초과했습니다. 세션이 종료되고 카드가 반환됩니다."
      << endl;
}

bool BilingualATM::verify_card_password(Account *user_account) {
  if (current_language == "English") {
    return ATM::verify_card_password(user_account);
  }
  string password;
  cout << "비밀번호를 입력하세요: ";
  cin >> password;

  // 카드가 속한 은행을 통해 인증 요청
  return user_account->get_issued_bank()->authorize_user(*user_account,
                                                         password);
}

void ATM::printATM_history(string history) {
  cout << history << endl;
  ofstream outFile("transaction_history.txt");
  if (outFile.is_open()) {
    outFile << history << endl;
    outFile.close();
  } else {
    cout << "거래내역 파일 출력이 실패하였습니다." << endl;
  }
  return;
}

void ATM::admin_menu() {
  while (true) {
    string admin_input;
    cout << "관리자 모드입니다. 거래 내역을 출력하시려면 1, 취소하시려면 2를 "
            "입력하세요: ";
    cin >> admin_input;
    if (admin_input == "1") {
      string all_atm_history = "";
      for (Bank *bank : bank_list) {
        for (auto &pair : bank->get_ATM_map()) {
          all_atm_history +=
              ("ATM [SN: " + pair.second->getSerialNumber() + "]" + "\n");
          all_atm_history += pair.second->atmTransActionHistory;
          cout << endl;
        }
      }
      printATM_history(all_atm_history);
      cout << "출력이 완료되었습니다. 관리자 모드를 종료합니다." << endl;
      return;
    } else if (admin_input == "2") {
      cout << "관리자 모드를 종료합니다." << endl;
      return;
    } else {
      cout << "유효하지 않은 입력입니다. 다시 입력해주세요." << endl;
      continue;
    }
  }
}
void slash() {
  cout << "Printing ATM/Account Status." << endl;
  cout << "Currently there are " << bank_list.size() << " banks." << endl;
  for (Bank *bank : bank_list) {
    cout << "Bank: " << bank->getBankName() << endl;
    cout << "ATM: " << bank->get_ATM_map().size()
         << ", Account: " << bank->get_acc_map().size()
         << endl; // Print number of ATMs and accounts for this bank
    for (auto &pair : bank->get_ATM_map()) {
      cout << "ATM [SN: " << pair.second->getSerialNumber() << "] ";
      if (pair.second->getSingleBank()) {
        cout << "Single Bank";
      } else {
        cout << "Multi Bank";
      }
      cout << ", " << pair.second->getType();
      cout << ", remaining cash: {KRW 50000: ";
      cout << pair.second->getCashAmount(50000)
           << ", KRW 10000: " << pair.second->getCashAmount(10000);
      cout << ", KRW 5000: " << pair.second->getCashAmount(5000)
           << ", KRW 1000: " << pair.second->getCashAmount(1000) << "}" << endl;
    }
    for (auto &pair : bank->get_acc_map()) {
      cout << "Account[Bank: " << bank->getBankName()
           << ", No: " << pair.second.get_account_number()
           << ", Card No: " << pair.second.get_card_number();
      cout << ", Owner: " << pair.second.get_owner_name()
           << "] balance: " << pair.second.get_account_balance() << "}" << endl;
    }
  }
  return;
}

void Bank::createATM() {
  string serial_number;
  bool single_Bank;
  bool bilingual;

  while (true) // serial_number 설정 loop
  {
    cout << "ATM의 Serial Number를 입력하세요: ";
    cin >> serial_number;
    bool is_all_num = true;
    for (char ch : serial_number) {
      if (ch < '0' || ch > '9') {
        cout << "입력에 숫자 외의 값이 들어있습니다." << endl;
        is_all_num = false;
        break;
      }
    }
    if (!is_all_num) {
      continue;
    }
    if (serial_number.length() != 6) {
      cout << "입력 길이가 6자리가 아닙니다." << endl;
      continue;
    }

    bool isATMNumDuplicated = false;
    // ATM 중복 관리할때 -> 다른 은행끼리는 serialnumber중복 가능하면 뱅크
    // 안에서 검사, 아니면 따로 리스트를 만들어야할듯
    for (Bank *bank : bank_list) {
      isATMNumDuplicated = bank->find_atm_by_serial_number(serial_number);
      if (isATMNumDuplicated) {
        cout << "이미 존재하는 Serial Number입니다." << endl;
        break; // 중복 발견 시 루프 종료
      }
    }
    if (isATMNumDuplicated) {
      // 새로운 시리얼 번호를 다시 입력받거나 함수 종료
      continue;
    }
    break;
  }

  while (true) {
    cout << "단일은행 ATM입니까? 맞으면 1, 아니면 0을 입력해주세요: ";
    cin >> single_Bank;
    if (cin.fail()) {
      cin.clear();
      cin.ignore(numeric_limits<streamsize>::max(), '\n');
      cout << "유효하지 않은 입력입니다. 다시 입력해주세요.\n";
      continue;
    }
    if (single_Bank != 0 && single_Bank != 1) {
      cout << "0 또는 1을 입력해주세요.\n";
      continue;
    }
    break;
  }

  while (true) {
    cout << "다중언어 ATM입니까? 맞으면 1, 아니면 0을 입력해주세요: ";
    cin >> bilingual;
    if (cin.fail()) {
      cin.clear();
      cin.ignore(numeric_limits<streamsize>::max(), '\n');
      cout << "유효하지 않은 입력입니다. 다시 입력해주세요.\n";
      continue;
    }
    if (bilingual != 0 && bilingual != 1) {
      cout << "0 또는 1을 입력해주세요.\n";
      continue;
    }
    break;
  }

  if (bilingual == 0) {
    ATM *temp_ATM = new ATM(serial_number, single_Bank, this);
    atms[serial_number] = temp_ATM;
    cout << "ATM이 생성되었습니다. 시리얼 번호: " << serial_number << endl
         << (single_Bank ? "단일은행" : "다중은행")
         << ", 단일언어 ATM입니다.\n";

    // ATM 생성 후 충전 여부 질문
    char charge_choice;
    while (true) {
      cout << "ATM을 충전하시겠습니까? 예: 1, 아니오: 0: ";
      cin >> charge_choice;

      if (charge_choice == '1') {
        chargeATM(serial_number);
        break;
      } else if (charge_choice == '0') {
        cout << "ATM 충전을 건너뜁니다.\n";
        break;
      } else {
        cout << "유효한 입력이 아닙니다. 다시 입력해주세요.\n";
      }
    }
    return;
  }

  if (bilingual == 1) {
    ATM *temp_ATM = new BilingualATM(serial_number, single_Bank, this);
    atms[serial_number] = temp_ATM;
    cout << "ATM이 생성되었습니다. 시리얼 번호: " << serial_number << endl
         << (single_Bank ? "단일은행" : "다중은행")
         << ", 다중언어 ATM입니다.\n";

    // ATM 생성 후 충전 여부 질문
    char charge_choice;
    while (true) {
      cout << "ATM을 충전하시겠습니까? 예: 1, 아니오: 0: ";
      cin >> charge_choice;

      if (charge_choice == '1') {
        chargeATM(serial_number);
        break;
      } else if (charge_choice == '0') {
        cout << "ATM 충전을 건너뜁니다.\n";
        break;
      } else {
        cout << "유효한 입력이 아닙니다. 다시 입력해주세요.\n";
      }
    }
    return;
  }
}

int input() {
  // 0보다 같거나 큰 정수를 입력을 받는 함수 - 성공하면 입력받은 정수를,
  // 실패하면 -1을 반환

  double input_number;
  cin >> input_number;

  // 입력 실패 처리
  if (cin.fail()) {
    cin.clear(); // 스트림 상태 플래그 초기화
    cin.ignore(numeric_limits<streamsize>::max(), '\n'); // 잘못된 입력 제거
    return -1;                                           // 실패 반환
  }

  // 소수인지 확인
  if (input_number != static_cast<int>(input_number)) {
    return -1; // 실패 반환
  }

  // 음수일 경우 처리
  if (input_number < 0) {
    return -1; // 실패 반환
  }

  return static_cast<int>(input_number); // 성공 반환
}

// Bank 클래스에 ATM에서 돈 빼내는 함수 추가
void Bank::withdrawCashFromATM(const string &serial_number) {
  ATM *atm = get_ATM(serial_number);
  if (atm == nullptr) {
    cout << "해당 Serial Number를 가진 ATM이 존재하지 않습니다.\n";
    return;
  }

  cout << "\n=== ATM 현금 인출 ===\n";

  int temp_1000, temp_5000, temp_10000, temp_50000;
  cout << "인출하실 50000원 지폐의 수를 입력하세요: ";
  cin >> temp_50000;
  cout << "인출하실 10000원 지폐의 수를 입력하세요: ";
  cin >> temp_10000;
  cout << "인출하실 5000원 지폐의 수를 입력하세요: ";
  cin >> temp_5000;
  cout << "인출하실 1000원 지폐의 수를 입력하세요: ";
  cin >> temp_1000;

  // 지폐 수량이 ATM에 충분한지 확인
  if (atm->getCashAmount(50000) < temp_50000 ||
      atm->getCashAmount(10000) < temp_10000 ||
      atm->getCashAmount(5000) < temp_5000 ||
      atm->getCashAmount(1000) < temp_1000) {
    cout << "ATM에 지정하신 지폐의 수량이 충분하지 않습니다.\n";
    return;
  }

  // ATM의 현금 수량을 감소시킴
  atm->access_cash()[50000] -= temp_50000;
  atm->access_cash()[10000] -= temp_10000;
  atm->access_cash()[5000] -= temp_5000;
  atm->access_cash()[1000] -= temp_1000;

  int total_withdraw = temp_50000 * 50000 + temp_10000 * 10000 +
                       temp_5000 * 5000 + temp_1000 * 1000;

  cout << "총 " << total_withdraw << "원 인출되었습니다.\n";
}