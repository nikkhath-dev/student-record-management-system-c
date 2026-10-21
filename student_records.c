/*
 * Student Record Management System
 * Language: C
 * Concepts: structures, linked list, dynamic memory allocation,
 *           file handling, searching, sorting, modular functions
 *
 * Compile: gcc student_records.c -o student_records
 * Run:     ./student_records
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NAME_LEN 50
#define DATA_FILE "students.dat"

/* A student record, also a node of the linked list */
typedef struct Student {
    int roll;
    char name[NAME_LEN];
    float marks;
    struct Student *next;
} Student;

/* ---------- Helper input functions ---------- */

void clearBuffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) { }
}

int readInt(const char *prompt) {
    int value;
    printf("%s", prompt);
    while (scanf("%d", &value) != 1) {
        clearBuffer();
        printf("Invalid input. %s", prompt);
    }
    clearBuffer();
    return value;
}

float readFloat(const char *prompt) {
    float value;
    printf("%s", prompt);
    while (scanf("%f", &value) != 1) {
        clearBuffer();
        printf("Invalid input. %s", prompt);
    }
    clearBuffer();
    return value;
}

void readString(const char *prompt, char *buffer, int size) {
    printf("%s", prompt);
    if (fgets(buffer, size, stdin) != NULL) {
        buffer[strcspn(buffer, "\n")] = '\0';
    }
}

/* ---------- Linked list operations ---------- */

Student *findStudent(Student *head, int roll) {
    Student *cur = head;
    while (cur != NULL) {
        if (cur->roll == roll) {
            return cur;
        }
        cur = cur->next;
    }
    return NULL;
}

/* Adds a new student at the end of the list */
Student *addStudent(Student *head, int roll, const char *name, float marks) {
    Student *node = (Student *)malloc(sizeof(Student));
    if (node == NULL) {
        printf("Memory allocation failed.\n");
        return head;
    }
    node->roll = roll;
    strncpy(node->name, name, NAME_LEN - 1);
    node->name[NAME_LEN - 1] = '\0';
    node->marks = marks;
    node->next = NULL;

    if (head == NULL) {
        return node;
    }
    Student *cur = head;
    while (cur->next != NULL) {
        cur = cur->next;
    }
    cur->next = node;
    return head;
}

void displayAll(Student *head) {
    if (head == NULL) {
        printf("No records found.\n");
        return;
    }
    printf("\n%-10s %-25s %-8s\n", "Roll No", "Name", "Marks");
    printf("-------------------------------------------\n");
    for (Student *cur = head; cur != NULL; cur = cur->next) {
        printf("%-10d %-25s %-8.2f\n", cur->roll, cur->name, cur->marks);
    }
}

Student *deleteStudent(Student *head, int roll) {
    Student *cur = head, *prev = NULL;
    while (cur != NULL && cur->roll != roll) {
        prev = cur;
        cur = cur->next;
    }
    if (cur == NULL) {
        printf("Student with roll number %d not found.\n", roll);
        return head;
    }
    if (prev == NULL) {
        head = cur->next;
    } else {
        prev->next = cur->next;
    }
    free(cur);
    printf("Record deleted.\n");
    return head;
}

/* Bubble sort on the list by swapping data (not pointers) */
void sortByMarks(Student *head) {
    if (head == NULL) {
        return;
    }
    int swapped;
    Student *cur, *last = NULL;
    do {
        swapped = 0;
        cur = head;
        while (cur->next != last) {
            if (cur->marks < cur->next->marks) {
                int tRoll = cur->roll;
                float tMarks = cur->marks;
                char tName[NAME_LEN];

                strcpy(tName, cur->name);
                cur->roll = cur->next->roll;
                cur->marks = cur->next->marks;
                strcpy(cur->name, cur->next->name);

                cur->next->roll = tRoll;
                cur->next->marks = tMarks;
                strcpy(cur->next->name, tName);
                swapped = 1;
            }
            cur = cur->next;
        }
        last = cur;
    } while (swapped);
}

/* Frees every node to avoid memory leaks */
void freeList(Student *head) {
    while (head != NULL) {
        Student *temp = head;
        head = head->next;
        free(temp);
    }
}

/* ---------- File handling ---------- */

void saveToFile(Student *head) {
    FILE *fp = fopen(DATA_FILE, "wb");
    if (fp == NULL) {
        printf("Could not open file for saving.\n");
        return;
    }
    for (Student *cur = head; cur != NULL; cur = cur->next) {
        /* write only the data fields, not the pointer */
        fwrite(&cur->roll, sizeof(int), 1, fp);
        fwrite(cur->name, sizeof(char), NAME_LEN, fp);
        fwrite(&cur->marks, sizeof(float), 1, fp);
    }
    fclose(fp);
}

Student *loadFromFile(void) {
    Student *head = NULL;
    FILE *fp = fopen(DATA_FILE, "rb");
    if (fp == NULL) {
        return NULL; /* first run: no file yet */
    }
    int roll;
    char name[NAME_LEN];
    float marks;
    while (fread(&roll, sizeof(int), 1, fp) == 1 &&
           fread(name, sizeof(char), NAME_LEN, fp) == NAME_LEN &&
           fread(&marks, sizeof(float), 1, fp) == 1) {
        head = addStudent(head, roll, name, marks);
    }
    fclose(fp);
    return head;
}

/* ---------- Menu actions ---------- */

Student *menuAdd(Student *head) {
    int roll = readInt("Enter roll number: ");
    if (findStudent(head, roll) != NULL) {
        printf("Roll number already exists.\n");
        return head;
    }
    char name[NAME_LEN];
    readString("Enter name: ", name, NAME_LEN);
    float marks = readFloat("Enter marks: ");
    head = addStudent(head, roll, name, marks);
    printf("Record added.\n");
    return head;
}

void menuSearch(Student *head) {
    int roll = readInt("Enter roll number to search: ");
    Student *s = findStudent(head, roll);
    if (s == NULL) {
        printf("Student not found.\n");
    } else {
        printf("Found: Roll %d | Name: %s | Marks: %.2f\n",
               s->roll, s->name, s->marks);
    }
}

void menuUpdate(Student *head) {
    int roll = readInt("Enter roll number to update: ");
    Student *s = findStudent(head, roll);
    if (s == NULL) {
        printf("Student not found.\n");
        return;
    }
    readString("Enter new name: ", s->name, NAME_LEN);
    s->marks = readFloat("Enter new marks: ");
    printf("Record updated.\n");
}

int main(void) {
    Student *head = loadFromFile();
    int choice;

    do {
        printf("\n===== Student Record Management System =====\n");
        printf("1. Add student\n");
        printf("2. Display all students\n");
        printf("3. Search student\n");
        printf("4. Update student\n");
        printf("5. Delete student\n");
        printf("6. Sort by marks (high to low)\n");
        printf("7. Save and exit\n");
        choice = readInt("Enter your choice: ");

        switch (choice) {
            case 1: head = menuAdd(head); break;
            case 2: displayAll(head); break;
            case 3: menuSearch(head); break;
            case 4: menuUpdate(head); break;
            case 5:
                head = deleteStudent(head, readInt("Enter roll number to delete: "));
                break;
            case 6:
                sortByMarks(head);
                printf("Sorted by marks.\n");
                displayAll(head);
                break;
            case 7:
                saveToFile(head);
                printf("Records saved. Goodbye!\n");
                break;
            default:
                printf("Invalid choice. Try again.\n");
        }
    } while (choice != 7);

    freeList(head);
    return 0;
}
