#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_BOOKS 1000
#define MAX_BORROW_RECORDS 1000
#define TITLE_SIZE 100
#define AUTHOR_SIZE 100
#define NAME_SIZE 100
#define DATA_FILE "library.dat"

// --------- LOGIN CREDENTIALS (you can change) ----------
#define ADMIN_USERNAME "admin"
#define ADMIN_PASSWORD "4321"

// --------- STRUCTS ----------
typedef struct {
    int id;
    char title[TITLE_SIZE];
    char author[AUTHOR_SIZE];
    int totalCopies;
    int availableCopies;
} Book;

typedef struct {
    int recordId;                  // unique borrow record id
    char borrowerName[NAME_SIZE];  // who borrowed
    int bookId;                    // which book
    int isReturned;                // 0 = not returned, 1 = returned
} BorrowRecord;

// --------- GLOBALS ----------
Book books[MAX_BOOKS];
int bookCount = 0;

BorrowRecord borrowRecords[MAX_BORROW_RECORDS];
int borrowCount = 0;

// --------- FUNCTION DECLARATIONS ----------
void clearInputBuffer();
void login();

void loadData();
void saveData();

void addBook();
void listBooks();
void searchBookById();
void searchBookByTitle();
void issueBook();
void returnBook();
void listBorrowRecords();

int findBookIndexById(int id);

// ===================== MAIN =====================
int main() {
    int choice;

    login();       // ask for username & password
    loadData();    // load books + borrow records from file if exist

    while (1) {
        printf("\n===== LIBRARY MANAGEMENT SYSTEM =====\n");
        printf("1. Add Book\n");
        printf("2. List All Books\n");
        printf("3. Search Book by ID\n");
        printf("4. Search Book by Title\n");
        printf("5. Issue (Borrow) Book\n");
        printf("6. Return Book\n");
        printf("7. List All Borrow Records\n");
        printf("8. Save & Exit\n");
        printf("Enter your choice: ");

        if (scanf("%d", &choice) != 1) {
            printf("Invalid input! Please enter a number.\n");
            clearInputBuffer();
            continue;
        }

        clearInputBuffer(); // remove leftover newline

        switch (choice) {
            case 1:
                addBook();
                break;
            case 2:
                listBooks();
                break;
            case 3:
                searchBookById();
                break;
            case 4:
                searchBookByTitle();
                break;
            case 5:
                issueBook();
                break;
            case 6:
                returnBook();
                break;
            case 7:
                listBorrowRecords();
                break;
            case 8:
                saveData();
                printf("Data saved. Exiting program.\n");
                exit(0);
            default:
                printf("Invalid choice! Please select from the menu.\n");
        }
    }

    return 0;
}

// ===================== UTILITY FUNCTIONS =====================
void clearInputBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {
        // discard
    }
}

// Simple username-password login
void login() {
    char username[50];
    char password[50];

    printf("===== LOGIN =====\n");

    while (1) {
        printf("Username: ");
        if (scanf("%49s", username) != 1) {
            clearInputBuffer();
            continue;
        }

        printf("Password: ");
        if (scanf("%49s", password) != 1) {
            clearInputBuffer();
            continue;
        }

        if (strcmp(username, ADMIN_USERNAME) == 0 &&
            strcmp(password, ADMIN_PASSWORD) == 0) {
            printf("Login successful! Welcome, %s.\n", username);
            clearInputBuffer();
            break;
        } else {
            printf("Incorrect username or password. Try again.\n");
            clearInputBuffer();
        }
    }
}

// ===================== FILE I/O =====================

void loadData() {
    FILE *fp = fopen(DATA_FILE, "rb");
    if (fp == NULL) {
        printf("No existing data found. Starting new library.\n");
        return;
    }

    // Read book count
    if (fread(&bookCount, sizeof(int), 1, fp) != 1) {
        printf("Error reading book count.\n");
        fclose(fp);
        bookCount = 0;
        return;
    }

    if (bookCount < 0 || bookCount > MAX_BOOKS) {
        printf("Invalid book count in file. Resetting.\n");
        bookCount = 0;
    } else if (bookCount > 0) {
        if (fread(books, sizeof(Book), bookCount, fp) != (size_t)bookCount) {
            printf("Error reading book records.\n");
            bookCount = 0;
        }
    }

    // Read borrow count
    if (fread(&borrowCount, sizeof(int), 1, fp) != 1) {
        // If file is old format or error, just set borrowCount = 0
        borrowCount = 0;
        printf("No borrow records found (this is OK for first run).\n");
        fclose(fp);
        return;
    }

    if (borrowCount < 0 || borrowCount > MAX_BORROW_RECORDS) {
        printf("Invalid borrow record count in file. Resetting.\n");
        borrowCount = 0;
    } else if (borrowCount > 0) {
        if (fread(borrowRecords, sizeof(BorrowRecord), borrowCount, fp)
            != (size_t)borrowCount) {
            printf("Error reading borrow records.\n");
            borrowCount = 0;
        }
    }

    fclose(fp);
    printf("%d book(s) and %d borrow record(s) loaded from file.\n",
           bookCount, borrowCount);
}

void saveData() {
    FILE *fp = fopen(DATA_FILE, "wb");
    if (fp == NULL) {
        printf("Error opening file for writing.\n");
        return;
    }

    // Save books
    if (fwrite(&bookCount, sizeof(int), 1, fp) != 1) {
        printf("Error writing book count.\n");
        fclose(fp);
        return;
    }

    if (bookCount > 0) {
        if (fwrite(books, sizeof(Book), bookCount, fp) != (size_t)bookCount) {
            printf("Error writing book records.\n");
            fclose(fp);
            return;
        }
    }

    // Save borrow records
    if (fwrite(&borrowCount, sizeof(int), 1, fp) != 1) {
        printf("Error writing borrow record count.\n");
        fclose(fp);
        return;
    }

    if (borrowCount > 0) {
        if (fwrite(borrowRecords, sizeof(BorrowRecord), borrowCount, fp)
            != (size_t)borrowCount) {
            printf("Error writing borrow records.\n");
        }
    }

    fclose(fp);
}

// ===================== BOOK FUNCTIONS =====================

int findBookIndexById(int id) {
    for (int i = 0; i < bookCount; i++) {
        if (books[i].id == id) {
            return i;
        }
    }
    return -1;
}

void addBook() {
    if (bookCount >= MAX_BOOKS) {
        printf("Library is full! Cannot add more books.\n");
        return;
    }

    Book b;

    printf("Enter Book ID (integer): ");
    if (scanf("%d", &b.id) != 1) {
        printf("Invalid ID.\n");
        clearInputBuffer();
        return;
    }
    clearInputBuffer(); // remove leftover newline

    // Check if ID already exists
    if (findBookIndexById(b.id) != -1) {
        printf("A book with this ID already exists!\n");
        return;
    }

    printf("Enter Book Title: ");
    fgets(b.title, TITLE_SIZE, stdin);
    b.title[strcspn(b.title, "\n")] = '\0';

    printf("Enter Author Name: ");
    fgets(b.author, AUTHOR_SIZE, stdin);
    b.author[strcspn(b.author, "\n")] = '\0';

    printf("Enter Total Copies: ");
    if (scanf("%d", &b.totalCopies) != 1 || b.totalCopies < 1) {
        printf("Invalid number of copies.\n");
        clearInputBuffer();
        return;
    }

    b.availableCopies = b.totalCopies;

    books[bookCount] = b;
    bookCount++;

    printf("Book added successfully!\n");
}

void listBooks() {
    if (bookCount == 0) {
        printf("No books in the library.\n");
        return;
    }

    printf("\n----- List of Books -----\n");
    printf("%-5s | %-30s | %-20s | %-10s | %-10s\n",
           "ID", "Title", "Author", "Total", "Available");
    printf("-------------------------------------------------------------------------------\n");

    for (int i = 0; i < bookCount; i++) {
        printf("%-5d | %-30s | %-20s | %-10d | %-10d\n",
               books[i].id,
               books[i].title,
               books[i].author,
               books[i].totalCopies,
               books[i].availableCopies);
    }
}

void searchBookById() {
    if (bookCount == 0) {
        printf("No books in the library.\n");
        return;
    }

    int id;
    printf("Enter Book ID to search: ");
    if (scanf("%d", &id) != 1) {
        printf("Invalid ID.\n");
        clearInputBuffer();
        return;
    }

    int index = findBookIndexById(id);
    if (index == -1) {
        printf("No book found with ID %d.\n", id);
        return;
    }

    Book *b = &books[index];
    printf("\nBook Found:\n");
    printf("ID: %d\n", b->id);
    printf("Title: %s\n", b->title);
    printf("Author: %s\n", b->author);
    printf("Total Copies: %d\n", b->totalCopies);
    printf("Available Copies: %d\n", b->availableCopies);
}

void searchBookByTitle() {
    if (bookCount == 0) {
        printf("No books in the library.\n");
        return;
    }

    char title[TITLE_SIZE];
    printf("Enter Book Title (or part of it) to search: ");
    fgets(title, TITLE_SIZE, stdin);
    title[strcspn(title, "\n")] = '\0';

    int found = 0;
    for (int i = 0; i < bookCount; i++) {
        if (strstr(books[i].title, title) != NULL) {
            if (!found) {
                printf("\nBooks matching \"%s\":\n", title);
                printf("%-5s | %-30s | %-20s | %-10s | %-10s\n",
                       "ID", "Title", "Author", "Total", "Available");
                printf("-------------------------------------------------------------------------------\n");
            }

            printf("%-5d | %-30s | %-20s | %-10d | %-10d\n",
                   books[i].id,
                   books[i].title,
                   books[i].author,
                   books[i].totalCopies,
                   books[i].availableCopies);

            found = 1;
        }
    }

    if (!found) {
        printf("No books found with title containing \"%s\".\n", title);
    }
}

// ===================== BORROW / RETURN FUNCTIONS =====================

void issueBook() {
    if (bookCount == 0) {
        printf("No books in the library.\n");
        return;
    }
    if (borrowCount >= MAX_BORROW_RECORDS) {
        printf("Cannot issue more books, borrow records are full.\n");
        return;
    }

    int id;
    char borrowerName[NAME_SIZE];

    printf("Enter Book ID to issue: ");
    if (scanf("%d", &id) != 1) {
        printf("Invalid ID.\n");
        clearInputBuffer();
        return;
    }
    clearInputBuffer();

    int bookIndex = findBookIndexById(id);
    if (bookIndex == -1) {
        printf("No book found with ID %d.\n", id);
        return;
    }

    Book *b = &books[bookIndex];
    if (b->availableCopies <= 0) {
        printf("No copies available to issue.\n");
        return;
    }

    printf("Enter Borrower Name: ");
    fgets(borrowerName, NAME_SIZE, stdin);
    borrowerName[strcspn(borrowerName, "\n")] = '\0';

    // Create borrow record
    BorrowRecord br;
    br.recordId = borrowCount + 1;
    strcpy(br.borrowerName, borrowerName);
    br.bookId = id;
    br.isReturned = 0;

    borrowRecords[borrowCount] = br;
    borrowCount++;

    b->availableCopies--;

    printf("Book issued successfully to %s! Remaining available copies: %d\n",
           borrowerName, b->availableCopies);
}

void returnBook() {
    if (bookCount == 0) {
        printf("No books in the library.\n");
        return;
    }
    if (borrowCount == 0) {
        printf("No borrow records. Nothing to return.\n");
        return;
    }

    int id;
    char borrowerName[NAME_SIZE];

    printf("Enter Book ID to return: ");
    if (scanf("%d", &id) != 1) {
        printf("Invalid ID.\n");
        clearInputBuffer();
        return;
    }
    clearInputBuffer();

    printf("Enter Borrower Name: ");
    fgets(borrowerName, NAME_SIZE, stdin);
    borrowerName[strcspn(borrowerName, "\n")] = '\0';

    int recordIndex = -1;
    for (int i = 0; i < borrowCount; i++) {
        if (borrowRecords[i].bookId == id &&
            borrowRecords[i].isReturned == 0 &&
            strcmp(borrowRecords[i].borrowerName, borrowerName) == 0) {
            recordIndex = i;
            break;
        }
    }

    if (recordIndex == -1) {
        printf("No active borrow record found for %s with Book ID %d.\n",
               borrowerName, id);
        return;
    }

    int bookIndex = findBookIndexById(id);
    if (bookIndex == -1) {
        printf("Book not found in library data. (Data error)\n");
        return;
    }

    // Mark record as returned
    borrowRecords[recordIndex].isReturned = 1;

    // Increase available copies
    books[bookIndex].availableCopies++;

    printf("Book returned successfully by %s. Available copies: %d\n",
           borrowerName, books[bookIndex].availableCopies);
}

void listBorrowRecords() {
    if (borrowCount == 0) {
        printf("No borrow records found.\n");
        return;
    }

    printf("\n----- Borrow Records -----\n");
    printf("%-5s | %-20s | %-7s | %-10s\n",
           "ID", "Borrower", "BookID", "Status");
    printf("--------------------------------------------------------\n");

    for (int i = 0; i < borrowCount; i++) {
        printf("%-5d | %-20s | %-7d | %-10s\n",
               borrowRecords[i].recordId,
               borrowRecords[i].borrowerName,
               borrowRecords[i].bookId,
               borrowRecords[i].isReturned ? "Returned" : "Borrowed");
    }
}