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
    Stock *head;
    Stock *tail;
    int length;
} UserList;

OrderList orders = {NULL, NULL, 0};
StockList stocks = {NULL, NULL, 0};
UserList users = {NULL, NULL, 0};

// linked list functions for orders
Order *createOrder(int id, int cashierId, PaymentType paymentType);
Order *findOrder(int id);
void addOrder(Order *order);
void removeOrder(int id);
Item *findItemFromOrder(int stockId);
void addItemToOrder(Order *order, int stockId, int quantity);
void modifyItemOnOrder(Order *order, int stockId, int quantity);
void setOrderStatus(Order *order, OrderStatus status);

// linked list functions for stocks
void *createStock(int id, char name[], int price, int quantity);
Stock *findStock(int id);
void addStock(Stock *stock);
void removeStock(Stock *stock);
void incrementQuantity(int id, int amount);
void decrementQuantity(int id, int amount);

// linked list functions for users
void *createUser(int id, char name[], char hashedPassword[], UserType type);
User *findUser(int id);
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
    if(key == KEY_W) {
        *selected = (*selected - 1 + total_option) % total_option;
    }
    if(key == KEY_S) {
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

    beginPrintOption();
    printOption("Login");
    printOption("Register");
    printOption("Credits");
    printOption("Exit");

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

Order *createOrder(int id, int cashierId, PaymentType paymentType) {
    Order *order = malloc(sizeof(Order));
    order->id = id;
    order->cashierId = cashierId;
    order->paymentType = paymentType;
    return order;
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
    Stock *targetItem = findStock(stockId);
    if (targetItem == NULL) return;

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
