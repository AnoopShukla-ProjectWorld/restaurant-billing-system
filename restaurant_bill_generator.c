#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

#define MAX_ITEMS 100
#define MAX_NAME_LENGTH 50

struct Item {
    char name[MAX_NAME_LENGTH];
    int quantity;
    float price;
};

struct Bill {
    char customerName[MAX_NAME_LENGTH];
    char date[30];
    int numItems;
    struct Item items[MAX_ITEMS];
};

// Function Declare :-
void generateBillHeader(const char *customerName, const char *date);
void generateBillBody(const char *itemName, int quantity, float price);
void generateBillFooter(float total);
void saveBillToFile(struct Bill bill);
void saveBillAsText(struct Bill bill);
void showAllBills();
void searchBillByName(const char *customerName);
void deleteBillByName(const char *customerName);
int getValidatedInt(const char *prompt, int minValue, int maxValue);
float getValidatedFloat(const char *prompt, float minValue, float maxValue);
void getValidatedString(const char *prompt, char *str, int maxLength);
int isValidName(const char *str);
float calculateTotal(struct Bill *bill);
int caseInsensitiveCompare(const char *s1, const char *s2);

int main() {
    int choice;
    char contFlag = 'y';

    while (contFlag == 'y' || contFlag == 'Y') {
#ifdef _WIN32
        system("cls");
#else
        system("clear");
#endif
        printf("\t============ Restaurant Billing System ============\n");
        printf("\n1. Generate a new bill");
        printf("\n2. View all saved bills");
        printf("\n3. Search for a bill by customer name");
        printf("\n4. Delete a bill by customer name");
        printf("\n5. Exit");
        printf("\n\nEnter your choice (1-4): ");

        choice = getValidatedInt("", 1, 5);

        switch (choice) {
            case 1: {
                struct Bill bill;
                float total;

                getValidatedString("Enter customer name (letters and spaces only): ", bill.customerName, MAX_NAME_LENGTH);

                time_t t = time(NULL);
                struct tm tm = *localtime(&t);
                int hour = tm.tm_hour;
                char period[3] = "AM";
                if (hour >= 12) {
                period[0] = 'P';  // PM
                if (hour > 12) hour -= 12;
                } else if (hour == 0) {
                hour = 12;
                }

                snprintf(bill.date, sizeof(bill.date),"%02d-%02d-%04d %02d:%02d:%02d %s",
                tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900,hour, tm.tm_min, tm.tm_sec, period);

                bill.numItems = getValidatedInt("Enter the number of items (1-100): ", 1, MAX_ITEMS);

                for (int i = 0; i < bill.numItems; i++) {
                    printf("\nItem %d:\n", i + 1);
                    getValidatedString("  Enter item name: ", bill.items[i].name, MAX_NAME_LENGTH);
                    bill.items[i].quantity = getValidatedInt("  Enter quantity (1-1000): ", 1, 1000);
                    bill.items[i].price = getValidatedFloat("  Enter price (1-10000): ", 0.01, 10000.0f);
                }

                total = calculateTotal(&bill);

                generateBillHeader(bill.customerName, bill.date);
                for (int i = 0; i < bill.numItems; i++) {
                    generateBillBody(bill.items[i].name, bill.items[i].quantity, bill.items[i].price);
                }
                generateBillFooter(total);

                printf("\nDo you want to save the bill? [y/n]: ");
                char saveOption;
                scanf(" %c", &saveOption);
                while (getchar() != '\n');

                if (saveOption == 'y' || saveOption == 'Y') {
                    saveBillToFile(bill);
                    saveBillAsText(bill);
                }
                break;
            }

            case 2:
                showAllBills();
                break;

            case 3: {
                char searchName[MAX_NAME_LENGTH];
                getValidatedString("\nEnter customer name to search: ", searchName, MAX_NAME_LENGTH);
                searchBillByName(searchName);
                break;
            }

            case 4:{
                char deleteName[MAX_NAME_LENGTH];
                getValidatedString("Enter customer name to delete: ", deleteName, MAX_NAME_LENGTH);
                deleteBillByName(deleteName);
                break;
            }

            case 5:
                printf("\nThank you for using the billing system!\n");
                exit(0);
        }

        printf("\nDo you want to perform another operation? [y/n]: ");
        scanf(" %c", &contFlag);
        while (getchar() != '\n');
    }

    printf("\nThank you for using the billing system!\n");
    return 0;
}

int getValidatedInt(const char *prompt, int minValue, int maxValue) {
    int value;
    while (1) {
        printf("%s", prompt);
        if (scanf("%d", &value) != 1 || value < minValue || value > maxValue) {
            printf("Invalid input. Please enter a number between %d and %d.\n", minValue, maxValue);
            while (getchar() != '\n');
        } else {
            while (getchar() != '\n');
            return value;
        }
    }
}

float getValidatedFloat(const char *prompt, float minValue, float maxValue) {
    float value;
    while (1) {
        printf("%s", prompt);
        if (scanf("%f", &value) != 1 || value < minValue || value > maxValue) {
            printf("Invalid input. Please enter a valid number between %.2f and %.2f.\n", minValue, maxValue);
            while (getchar() != '\n');
        } else {
            while (getchar() != '\n');
            return value;
        }
    }
}

void getValidatedString(const char *prompt, char *str, int maxLength) {
    while (1) {
        printf("%s", prompt);
        fgets(str, maxLength, stdin);
        if (str[strlen(str) - 1] != '\n') while (getchar() != '\n');
        str[strcspn(str, "\n")] = 0;

        if (strlen(str) == 0 || !isValidName(str)) {
            printf("Invalid input. Only letters and spaces allowed.\n");
        } else {
            break;
        }
    }
}

int isValidName(const char *str) {
    int hasLetter = 0;
    for (int i = 0; str[i]; i++) {
        if (isalpha(str[i])) hasLetter = 1;
        else if (str[i] != ' ') return 0;
    }
    return hasLetter;
}

int caseInsensitiveCompare(const char *s1, const char *s2) {
    while (*s1 && *s2) {
        if (tolower(*s1) != tolower(*s2)) return 0;
        s1++;
        s2++;
    }
    return *s1 == *s2;
}

float calculateTotal(struct Bill *bill) {
    float total = 0;
    for (int i = 0; i < bill->numItems; i++) {
        total += bill->items[i].quantity * bill->items[i].price;
    }
    return total;
}

void generateBillHeader(const char *customerName, const char *date) {
    printf("\n\n\t=========== RESTAURANT BILL ===========\n");
    printf("Date: %s\n", date);
    printf("Customer Name: %s\n", customerName);
    printf("-----------------------------------------------------------\n");
    printf("| %-20s | %8s | %8s | %8s |\n", "Item", "Quantity", "Price", "Total");
    printf("-----------------------------------------------------------\n");
}

void generateBillBody(const char *itemName, int quantity, float price) {
    printf("| %-20s | %8d | %8.2f | %8.2f |\n", itemName, quantity, price, quantity * price);
}

void generateBillFooter(float total) {
    float discount = 0.1f * total;
    float netTotal = total - discount;
    float tax = 0.18f * netTotal;
    float grandTotal = netTotal + tax;

    printf("-----------------------------------------------------------\n");
    printf("| %-32s %20.2f |\n", "Sub Total", total);
    printf("| %-32s %20.2f |\n", "Discount (10%)", discount);
    printf("| %-32s %20.2f |\n", "Net Total", netTotal);
    printf("| %-32s %20.2f |\n", "GST (18%)", tax);
    printf("-----------------------------------------------------------\n");
    printf("| %-32s %20.2f |\n", "Grand Total", grandTotal);
    printf("-----------------------------------------------------------\n");
}


void saveBillToFile(struct Bill bill) {
    FILE *file = fopen("RestaurantBills.dat", "ab");
    if (!file) {
        perror("Error saving the bill");
        return;
    }
    fwrite(&bill, sizeof(struct Bill), 1, file);
    fclose(file);
    printf("\nBinary bill saved successfully.\n");
}

void saveBillAsText(struct Bill bill) {
    char fileName[100];
    snprintf(fileName, sizeof(fileName), "%s_%s.txt", bill.customerName, bill.date);

    snprintf(fileName, sizeof(fileName), "%s.txt", bill.customerName);

    for (int i = 0; fileName[i]; i++) {
        if (fileName[i] == ' ') fileName[i] = '_';
    }

    FILE *file = fopen(fileName, "a");  // OPEN IN APPEND MODE
    if (!file) {
        perror("Error creating text bill");
        return;
    }

    float total = calculateTotal(&bill);
    float discount = 0.1f * total;
    float netTotal = total - discount;
    float tax = 0.18f * netTotal;
    float grandTotal = netTotal + tax;

    // Header
    fprintf(file, "\n\n\t=========== RESTAURANT BILL ===========\n");
    fprintf(file, "Date: %s\n", bill.date);
    fprintf(file, "Customer Name: %s\n", bill.customerName);
    fprintf(file, "-----------------------------------------------------------\n");
    fprintf(file, "| %-20s | %8s | %8s | %8s |\n", "Item", "Quantity", "Price", "Total");
    fprintf(file, "-----------------------------------------------------------\n");

    // Body
    for (int i = 0; i < bill.numItems; i++) {
        fprintf(file, "| %-20s | %8d | %8.2f | %8.2f |\n",
                bill.items[i].name,
                bill.items[i].quantity,
                bill.items[i].price,
                bill.items[i].quantity * bill.items[i].price);
    }

    // Footer
    fprintf(file, "-----------------------------------------------------------\n");
    fprintf(file, "| %-32s %20.2f |\n", "Sub Total", total);
    fprintf(file, "| %-32s %20.2f |\n", "Discount (10%%)", discount);
    fprintf(file, "| %-32s %20.2f |\n", "Net Total", netTotal);
    fprintf(file, "| %-32s %20.2f |\n", "GST (18%%)", tax);
    fprintf(file, "-----------------------------------------------------------\n");
    fprintf(file, "| %-32s %20.2f |\n", "Grand Total", grandTotal);
    fprintf(file, "-----------------------------------------------------------\n");

    fclose(file);
    printf("Text bill appended to file: %s\n", fileName);
}



void showAllBills() {
    FILE *file = fopen("RestaurantBills.dat", "rb");
    struct Bill bill;
    if (!file) {
        printf("\nNo bills found.\n");
        return;
    }

    printf("\n\t=========== All Saved Bills ===========\n");
    while (fread(&bill, sizeof(struct Bill), 1, file)) {
        float total = calculateTotal(&bill);
        generateBillHeader(bill.customerName, bill.date);
        for (int i = 0; i < bill.numItems; i++) {
            generateBillBody(bill.items[i].name, bill.items[i].quantity, bill.items[i].price);
        }
        generateBillFooter(total);
        printf("\n");
    }
    fclose(file);
}

void searchBillByName(const char *customerName) {
    FILE *file = fopen("RestaurantBills.dat", "rb");
    struct Bill bill;
    int found = 0;

    if (!file) {
        printf("\nNo bills found.\n");
        return;
    }

    while (fread(&bill, sizeof(struct Bill), 1, file)) {
        if (caseInsensitiveCompare(bill.customerName, customerName)) {
            float total = calculateTotal(&bill);
            generateBillHeader(bill.customerName, bill.date);
            for (int i = 0; i < bill.numItems; i++) {
                generateBillBody(bill.items[i].name, bill.items[i].quantity, bill.items[i].price);
            }
            generateBillFooter(total);
            found = 1;
            printf("\n");
        }
    }

    if (!found) {
        printf("\nNo bill found for customer: %s\n", customerName);
    }

    fclose(file);
}

void deleteBillByName(const char *customerName) {
    FILE *file = fopen("RestaurantBills.dat", "rb");
    FILE *temp = fopen("temp.dat", "wb");
    struct Bill bill;
    int found = 0;

    if (!file || !temp) {
        printf("\nError opening files.\n");
        return;
    }

    while (fread(&bill, sizeof(struct Bill), 1, file)) {
        if (!caseInsensitiveCompare(bill.customerName, customerName)) {
            fwrite(&bill, sizeof(struct Bill), 1, temp);
        } else {
            found = 1;
        }
    }

    fclose(file);
    fclose(temp);

    if (found) {
        remove("RestaurantBills.dat");
        rename("temp.dat", "RestaurantBills.dat");

        // delete associated .txt file
        char textFileName[100];
        snprintf(textFileName, sizeof(textFileName), "%s.txt", customerName);
        for (int i = 0; textFileName[i]; i++) {
            if (textFileName[i] == ' ') textFileName[i] = '_';
        }
        if (remove(textFileName) == 0) {
            printf("\nDeleted bill file: %s\n", textFileName);
        } else {
            printf("\nNo matching text file found or couldn't delete.\n");
        }

        printf("\nAll data for '%s' has been deleted successfully.\n", customerName);
    } else {
        remove("temp.dat");
        printf("\nNo bill found for customer: %s\n", customerName);
    }
}
