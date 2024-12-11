#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>

#ifdef _WIN32
#include <windows.h>
#include <conio.h>
#else
#include <ncurses.h>
#define printf printw
#endif

#define KEY_ARROW_PREFIX 224
#define KEY_ARROW_UP 72
#define KEY_ARROW_DOWN 80
#define KEY_ARROW_LEFT 75
#define KEY_ARROW_RIGHT 77
#define KEY_ESC 27
#define KEY_W 119
#define KEY_S 115

#ifndef KEY_ENTER
#define KEY_ENTER 13
#endif

#ifndef KEY_BACKSPACE
#define KEY_BACKSPACE 8
#endif

#define ANSI_BLUE "\033[34m"
#define ANSI_GREEN "\033[32m"
#define ANSI_RED "\033[31m"
#define ANSI_RESET "\033[0m"

typedef enum { PAYPAL, CREDIT_CARD, DEBIT_CARD, CASH } PaymentType;

typedef enum { WAITING, CANCELLED, COMPLETED } OrderStatus;

typedef enum { BUYER, CASHIER, ADMIN } UserType;

typedef struct Order Order;
typedef struct Item Item;
typedef struct Stock Stock;
typedef struct User User;

// Order is a struct that contains the information of an order
struct Order {
    int id;
    int cashierId;
    PaymentType paymentType;
    OrderStatus orderStatus;
    Item *items;

    Order *next;
    Order *prev;
};

struct Item {
    int id;
    int stockId;
    int quantity;

    Item *next;
    Item *prev;
};

struct Stock {
    int id;
    char name[101];
    int price;
    int quantity;

    Stock *next;
    Stock *prev;
};

struct User {
    int id;
    char name[101];
    char hashedPassword[201];
    UserType type;

    User *next;
    User *prev;
};

typedef struct {
    Order *head;
    Order *tail;
    int length;
} OrderList;

typedef struct {
    Stock *head;
    Stock *tail;
    int length;
} StockList;

typedef struct {
    User *head;
    User *tail;
    int length;
} UserList;

OrderList orders = {NULL, NULL, 0};
StockList stocks = {NULL, NULL, 0};
UserList users = {NULL, NULL, 0};

Item *createItem(int stockId, int quantity);

// linked list functions for orders
Order *createOrder(int cashierId, PaymentType paymentType);

Order *findOrder(int id);

void addOrder(Order *order);

void removeOrder(int id);

Item *findItemFromOrder(int stockId);

void addItemToOrder(Order *order, int stockId, int quantity);

void modifyItemOnOrder(Order *order, int stockId, int quantity);

void setOrderStatus(Order *order, OrderStatus status);

// linked list functions for stocks
void *createStock(char name[], int price, int quantity);

Stock *findStock(int id);

void addStock(Stock *stock);

void removeStock(Stock *stock);

void incrementQuantity(int id, int amount);

void decrementQuantity(int id, int amount);

// linked list functions for users
User *createUser(char name[], char hashedPassword[], UserType type);

User *findUser(int id);

User *findUserByName(char name[]);

void addUser(User *user);

void removeUser(User *user);

void changePassword(User *user, char hashedPassword[]);

// functions for user management
void registerUser(char name[], char password[], UserType type);

void hashPassword(char password[], char hashedPassword[]);

bool verifyPassword(int userId, char password[]);

// functions for file management
void readOrdersFromFile();

void readStocksFromFile();

void readUsersFromFile();

void writeOrdersToFile();

void writeStocksToFile();

void writeUsersToFile();


void clearTerminal();

int mainMenu();

void setCursor(int x, int y);

int menuArrowSelector(int total_option, int *selected);

void printOption(const char *option);

void beginPrintOption();

void pressEnterToContinue();

void printMainMenuOptions();

int loginView();
int registerView();
int creditsView();

void printc(char *text, char *color);

int main() {
#ifndef _WIN32
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE);
    if (has_colors()) {
        start_color();
        use_default_colors();
    }
#endif

    srand(time(NULL));
    int state = 0;
    while (1) {
        while (mainMenu());
    }
    return 0;
}

void clearTerminal() {
#ifdef _WIN32
    system("cls");
    setCursor(0, 0);
#else
        system("clear");
#endif
}

void setCursor(int x, int y) {
#ifdef _WIN32
    const HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    const COORD coord = {x, y};
    SetConsoleCursorPosition(hOut, coord);
#else
        move(y, x);
#endif
}

void printc(char *text, char *color) {
    printf("%s%s%s", color, text, ANSI_RESET);
}

int menuArrowSelector(int total_option, int *selected) {
    for (int i = 0; i < total_option; i++) {
        setCursor(1, i + 2);
        if (i == *selected) {
            printf(ANSI_GREEN);
            printf(">>>");
        } else {
            printf(ANSI_RESET);
            printf("   ");
        }
        printf(ANSI_RESET);
    }

    setCursor(0, 0);

    const int key = getch();
    if (key == KEY_ARROW_PREFIX) {
        const int key2 = getch();
        switch (key2) {
            case KEY_ARROW_UP:
                *selected = (*selected - 1 + total_option) % total_option;
                break;
            case KEY_ARROW_DOWN:
                *selected = (*selected + 1) % total_option;
                break;
        }
        return key2;
    }
    if (key == KEY_W) {
        *selected = (*selected - 1 + total_option) % total_option;
    }
    if (key == KEY_S) {
        *selected = (*selected + 1) % total_option;
    }
    return key;
}

void printOption(const char *option) {
    printf("     %s\n", option);
}

void beginPrintOption() {
    printf("\n\n");
}

void pressEnterToContinue() {
    printf("\nPress enter to continue...");
    while (getchar() != '\n');
}

void printMainMenuOptions() {
    beginPrintOption();
    printOption("Login");
    printOption("Register");
    printOption("Credits");
    printOption("Exit");
}

int mainMenu() {
    clearTerminal();

    printMainMenuOptions();

    int totalOption = 4;

    int selected = 0;
    while (1) {
        const int key = menuArrowSelector(totalOption, &selected);
#ifndef _WIN32
        refresh();
#endif

        if (key == KEY_ESC) {
            exit(0);
        }

        if (key == KEY_ESC) {
            break;
        }

        if (key == KEY_ENTER) {
            clearTerminal();
            switch (selected) {
                case 0:
                case 1:
                case 2:
                case 3:
                    printf("Created by: \n");
                    pressEnterToContinue();
                    return 1;
            }
        }
    }
    return 1;
}

int loginView() {
    clearTerminal();

    printf("Login\n\n");

    printc("Username: ", ANSI_BLUE);
    char username[105];
    scanf("%100s", username);
    getchar();

    printc("Password: ", ANSI_BLUE);
    char password[105];
    scanf("%100s", password);
    getchar();

    for(User *user = users.head; user != NULL; user = user->next) {
        if(strcmp(user->name, username) == 0) {
            if(verifyPassword(user->id, password)) {
                printc("Login successful!\n", ANSI_GREEN);
                pressEnterToContinue();
                return 0;
            }
        }
    }

    printc("Invalid username or password!\n", ANSI_RED);
    return 1;
}

int registerView() {
    clearTerminal();

    printf("Register\n\n");

    char temp[105];
    char username[105];
    char password[105];

    while(1){
        printc("Enter a username: ", ANSI_BLUE);
        scanf("%100s", temp);
        getchar();
        if(findUserByName(temp) != NULL) {
            printc("Username already exists!\n", ANSI_RED);
            continue;
        }
        if(strlen(temp) <= 3){
            printc("Username must be at least 4 characters long!\n", ANSI_RED);
            continue;
        }
        if(strlen(temp) > 20){
            printc("Username must be at most 20 characters long!\n", ANSI_RED);
            continue;
        }
        
        strcpy(username, temp);
        break;
    }

    printc("Enter a password: ", ANSI_BLUE);
    while(1){
        scanf("%100s", temp);
        getchar();
        if(strlen(temp) <= 5){
            printc("Password must be at least 6 characters long!\n", ANSI_RED);
            continue;
        }
        if(strlen(temp) > 50){
            printc("Password must be at most 50 characters long!\n", ANSI_RED);
            continue;
        }

        strcpy(password, temp);
        break;
    }

    clearTerminal();

    printc("Select a user type:\n", ANSI_BLUE);
    beginPrintOption();
    printOption("Buyer");
    printOption("Cashier");
    printOption("Admin");

    int totalOption = 3;
    int selected = 0;

    while(1) {
        const int key = menuArrowSelector(totalOption, &selected);
        #ifndef _WIN32
        refresh();
        #endif

        if(key == KEY_ESC) {
            return 1;
        }

        if(key == KEY_ENTER) {
            registerUser(username, password, selected);
            printc("User registered successfully, you can now login!\n", ANSI_GREEN);
            pressEnterToContinue();
            return 0;
        }
    }

    return 1;
}

int idGenerator(int length = 5) {
    srand(time(NULL));
    int id = rand();
    id = id % (int) pow(10, length);
    return id;
}

Order *createOrder(int cashierId, PaymentType paymentType) {
    Order *order = malloc(sizeof(Order));
    order->id = idGenerator(5);
    order->cashierId = cashierId;
    order->paymentType = paymentType;
    return order;
}

Item *createItem(int stockId, int quantity) {
    Item *item = malloc(sizeof(Item));
    item->id = idGenerator(6);
    item->quantity = quantity;
    item->stockId = stockId;
    item->prev = NULL;
    item->next = NULL;
    return item;
}

Order *findOrder(int id) {
    int currentIteration = 0;
    for (Order *firstOrder = orders.head, *lastOrder = orders.tail;
         firstOrder != NULL && lastOrder != NULL && currentIteration < (orders.length / 2 + 1);
         firstOrder = firstOrder->next, lastOrder = lastOrder->prev) {
        if (firstOrder->id == id) return firstOrder;
        if (lastOrder->id == id) return lastOrder;
        currentIteration++;
    }
    return NULL;
}

void addOrder(Order *order) {
    if (orders.head == NULL) {
        orders.head = order;
        orders.tail = order;
        orders.length = 1;
    } else {
        orders.tail->next = order;
        order->prev = orders.tail;
        orders.tail = order;
        orders.length++;
    }
}

void removeOrder(int id) {
    Order *order = findOrder(id);
    if (order == NULL) return;
    if (order == orders.head) {
        orders.head = order->next;
        orders.head->prev = NULL;
    } else if (order == orders.tail) {
        orders.tail = order->prev;
        orders.tail->next = NULL;
    } else {
        order->prev->next = order->next;
        order->next->prev = order->prev;
    }
    free(order);
    orders.length--;
}

Item *findItemFromOrder(int stockId) {
    return NULL; // TODO
}

void addItemToOrder(Order *order, int stockId, int quantity) {
    Stock *stock = findStock(stockId);
    if (stock == NULL) return;

    int found = 0;
    for (Item *item = order->items; item != NULL; item = item->next) {
        if (item->stockId == stockId) {
            item->quantity += quantity;
            found = 1;
            break;
        }
    }

    if (!found) {
        Item *item = createItem(stockId, quantity);
        item->next = order->items;
        if (order->items != NULL) {
            order->items->prev = item;
        }
        order->items = item;
    }
}

Stock *findStock(int id) {
    int currentIteration = 0;
    for (Stock *firstStock = stocks.head, *lastStock = stocks.tail;
         firstStock != NULL && lastStock != NULL && currentIteration < (stocks.length / 2 + 1);
         firstStock = firstStock->next, lastStock = lastStock->prev) {
        if (firstStock->id == id) return firstStock;
        if (lastStock->id == id) return lastStock;
        currentIteration++;
    }
    return NULL;
}

void addStock(Stock *stock) {
    if (stocks.head == NULL) {
        stocks.head = stock;
        stocks.tail = stock;
        stocks.length = 1;
    } else {
        stocks.tail->next = stock;
        stock->prev = stocks.tail;
        stocks.tail = stock;
        stocks.length++;
    }
}

void removeStock(Stock *stock) {
    if (stocks.head != NULL && stock->id == stocks.head->id) {
        stocks.head = stock->next;
        stocks.head->prev = NULL;
    } else if (stocks.tail != NULL && stock->id == stocks.tail->id) {
        stocks.tail = stock->prev;
        stocks.tail->next = NULL;
    } else {
        stock->prev->next = stock->next;
        stock->next->prev = stock->prev;
    }
    free(stock);
    stocks.length--;
}

void incrementQuantity(int stockId, int quantity) {
    Stock *stock = findStock(stockId);
    if (stock == NULL) return;
    stock->quantity += quantity;
}

void decrementQuantity(int stockId, int quantity) {
    Stock *stock = findStock(stockId);
    if (stock == NULL) return;
    stock->quantity -= quantity;
}

User *createUser(char name[], char hashedPassword[], UserType type) {
    User *user = malloc(sizeof(User));
    user->id = idGenerator(7);
    strcpy(user->name, name);
    strcpy(user->hashedPassword, hashedPassword);
    user->type = type;
    return user;
}

User *findUser(int id) {
    int currentIteration = 0;
    for (User *firstUser = users.head, *lastUser = users.tail;
         firstUser != NULL && lastUser != NULL && currentIteration < (users.length / 2 + 1);
         firstUser = firstUser->next, lastUser = lastUser->prev) {
        if (firstUser->id == id) return firstUser;
        if (lastUser->id == id) return lastUser;
        currentIteration++;
    }
    return NULL;
}

void addUser(User *user) {
    if (users.head == NULL) {
        users.head = user;
        users.tail = user;
        users.length = 1;
    } else {
        users.tail->next = user;
        user->prev = users.tail;
        users.tail = user;
        users.length++;
    }
}

void removeUser(User *user) {
    if (users.head != NULL && user->id == users.head->id) {
        users.head = user->next;
        users.head->prev = NULL;
    } else if (users.tail != NULL && user->id == users.tail->id) {
        users.tail = user->prev;
        users.tail->next = NULL;
    } else {
        user->prev->next = user->next;
        user->next->prev = user->prev;
    }
    free(user);
    users.length--;
}

void changePassword(User *user, char hashedPassword[]) {
    strcpy(user->hashedPassword, hashedPassword);
}

void registerUser(char name[], char password[], UserType type) {
    User *user = createUser(name, password, type);
    addUser(user);
}

void hashPassword(char password[], char hashedPassword[]) {
    // let's say abc, where a = 97, b = 98, c = 99
    int passwordLength = strlen(password);
    for (int i = 0; i < passwordLength; i++) {
        hashedPassword[i] = password[i] + (i * 7) % 26;
        // a = 97 + (0 * 7) % 26 = 97 + (0 * 7) = 97
        // b = 98 + (1 * 7) % 26 = 98 + (1 * 7) = 105
        // c = 99 + (2 * 7) % 26 = 99 + (2 * 7) = 106
    }
    hashedPassword[passwordLength] = '\0';
}

bool verifyPassword(User *user, char password[]) {
    char hashedPassword[201];
    hashPassword(password, hashedPassword);
    if (strcmp(user->hashedPassword, hashedPassword) == 0) return true;
    else return false;
}
