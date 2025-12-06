#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <conio.h>   // for getch()

#define STUD_FILE "students.txt"
#define CRE_FILE  "credentials.txt"

char currentUser[50];
char currentRole[20];

/* function prototypes */
void getCurrentDateTime(char *buffer);
void signup();
int  login();
int  isAdmin();

void addStudent();
void displayStudents();
void searchStudent();
void deleteStudent();
void updateMarks();
void getPassword(char *pass);   // password input with *

void getCurrentDateTime(char *buffer) {
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    sprintf(buffer, "%02d-%02d-%04d %02d:%02d:%02d",
            tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900,
            tm.tm_hour, tm.tm_min, tm.tm_sec);
}

/* READ PASSWORD AS ***** */
void getPassword(char *pass) {
    int i = 0;
    char ch;

    while (1) {
        ch = getch();        // read char without echo
        if (ch == 13) {      // Enter key
            break;
        } else if (ch == 8) { // Backspace
            if (i > 0) {
                i--;
                printf("\b \b");
            }
        } else {
            if (i < 49) {    // avoid overflow
                pass[i++] = ch;
                printf("*");
            }
        }
    }
    pass[i] = '\0';
}

/* SIGN UP */
void signup() {
    char username[50], password[50], role[20];
    printf("Enter Username: ");
    scanf("%49s", username);

    printf("Enter Password: ");
    getPassword(password);
    printf("\n");

    printf("Enter Role (admin/staff/guest): ");
    scanf("%19s", role);

    FILE *fp = fopen(CRE_FILE, "a");
    if (!fp) {
        printf("Error: Unable to open credentials file!\n");
        return;
    }

    fprintf(fp, "%s %s %s\n", username, password, role);
    fclose(fp);

    printf("Signup completed! You can now log in.\n");
}

/* LOGIN */
int login() {
    char u[50], p[50], r[20];
    char inUser[50], inPass[50];

    printf("\n=== LOGIN MENU ===\n");
    printf("Enter Username: ");
    scanf("%49s", inUser);

    printf("Enter Password: ");
    getPassword(inPass);
    printf("\n");

    FILE *fp = fopen(CRE_FILE, "r");
    if (!fp) {
        printf("No credentials found. Please sign up first.\n");
        return 0;
    }

    while (fscanf(fp, "%49s %49s %19s", u, p, r) == 3) {
        if (strcmp(inUser, u) == 0 && strcmp(inPass, p) == 0) {
            strcpy(currentUser, u);
            strcpy(currentRole, r);
            fclose(fp);
            return 1;
        }
    }

    fclose(fp);
    return 0;
}

/* CHECK IF CURRENT USER IS ADMIN */
int isAdmin() {
    return (strcmp(currentRole, "admin") == 0 ||
            strcmp(currentRole, "ADMIN") == 0);
}

/* ADD STUDENT */
void addStudent() {
    FILE *fp = fopen(STUD_FILE, "a");
    if (!fp) { printf("Error: Unable to open student file!\n"); return; }

    int roll, sem;
    float mark;
    char name[50], course[20], degree[20], datetime[50];

    printf("Enter Roll Number: ");
    if (scanf("%d", &roll) != 1) { printf("Invalid roll number.\n"); fclose(fp); return; }
    getchar();

    printf("Enter Name: ");
    fgets(name, 50, stdin);
    if (name[0] == '\n') { printf("Name cannot be empty.\n"); fclose(fp); return; }
    if (name[strlen(name)-1]=='\n') name[strlen(name)-1]='\0';

    printf("Enter Course: ");
    scanf("%19s", course);

    printf("Enter Degree: ");
    scanf("%19s", degree);

    printf("Enter Semester: ");
    if (scanf("%d", &sem) != 1) { printf("Invalid semester.\n"); fclose(fp); return; }

    printf("Enter Marks: ");
    if (scanf("%f", &mark) != 1) { printf("Invalid marks.\n"); fclose(fp); return; }

    getCurrentDateTime(datetime);

    fprintf(fp, "%d %s %s %s %d %.2f %s\n",
            roll, name, course, degree, sem, mark, datetime);

    fclose(fp);

    printf("Student record added successfully!\n");
}

/* DISPLAY STUDENTS */
void displayStudents() {
    FILE *fp = fopen(STUD_FILE, "r");
    if (!fp) { printf("No student records found!\n"); return; }

    int roll, sem;
    float mark;
    char name[50], course[20], degree[20], datetime[100];

    printf("\nRoll\tName\tCourse\tDegree\tSem\tMarks\tTimestamp\n");
    printf("---------------------------------------------------------------\n");

    while (fscanf(fp, "%d %49s %19s %19s %d %f %99[^\n]",
                  &roll, name, course, degree, &sem, &mark, datetime) == 7) {
        printf("%d\t%s\t%s\t%s\t%d\t%.2f\t%s\n",
               roll, name, course, degree, sem, mark, datetime);
    }

    fclose(fp);
}

/* SEARCH STUDENT */
void searchStudent() {
    int choice;
    printf("Search Options: 1.Roll  2.Name\nChoose: ");
    if (scanf("%d", &choice) != 1) { printf("Invalid choice.\n"); return; }
    getchar();

    int roll, sem, found = 0;
    float mark;
    char name[50], course[20], degree[20], datetime[100], searchName[50];

    FILE *fp = fopen(STUD_FILE, "r");
    if (!fp) { printf("No student records available.\n"); return; }

    if (choice == 1) {
        int sroll;
        printf("Enter Roll Number: ");
        if (scanf("%d", &sroll) != 1) { printf("Invalid roll number.\n"); fclose(fp); return; }

        while (fscanf(fp, "%d %49s %19s %19s %d %f %99[^\n]",
                      &roll, name, course, degree, &sem, &mark, datetime) == 7) {
            if (roll == sroll) {
                printf("Student record found:\n");
                printf("%d %s %s %s %d %.2f %s\n",
                       roll, name, course, degree, sem, mark, datetime);
                found = 1;
                break;
            }
        }
    } else {
        printf("Enter Name: ");
        fgets(searchName, 50, stdin);
        if (searchName[strlen(searchName)-1]=='\n')
            searchName[strlen(searchName)-1]='\0';

        while (fscanf(fp, "%d %49s %19s %19s %d %f %99[^\n]",
                      &roll, name, course, degree, &sem, &mark, datetime) == 7) {
            if (strcmp(name, searchName) == 0) {
                printf("Student record found:\n");
                printf("%d %s %s %s %d %.2f %s\n",
                       roll, name, course, degree, sem, mark, datetime);
                found = 1;
            }
        }
    }

    if (!found) printf("No matching student found.\n");

    fclose(fp);
}

/* DELETE STUDENT */
void deleteStudent() {
    int droll;
    printf("Enter Roll Number to delete: ");
    if (scanf("%d", &droll) != 1) { printf("Invalid roll number.\n"); return; }

    FILE *fp = fopen(STUD_FILE, "r");
    if (!fp) { printf("No student records found!\n"); return; }

    FILE *temp = fopen("temp.txt", "w");
    if (!temp) { printf("Error: Unable to open temp file!\n"); fclose(fp); return; }

    int roll, sem, deleted = 0;
    float mark;
    char name[50], course[20], degree[20], datetime[100];

    while (fscanf(fp, "%d %49s %19s %19s %d %f %99[^\n]",
                  &roll, name, course, degree, &sem, &mark, datetime) == 7) {
        if (roll == droll) {
            deleted = 1;
            continue; // skip writing this record
        }
        fprintf(temp, "%d %s %s %s %d %.2f %s\n",
                roll, name, course, degree, sem, mark, datetime);
    }

    fclose(fp);
    fclose(temp);

    remove(STUD_FILE);
    rename("temp.txt", STUD_FILE);

    if (deleted)
        printf("Student record deleted successfully!\n");
    else
        printf("No record found for the given roll number.\n");
}

/* UPDATE MARKS */
void updateMarks() {
    int uroll;
    printf("Enter Roll Number to update marks: ");
    if (scanf("%d", &uroll) != 1) { printf("Invalid roll number.\n"); return; }

    FILE *fp = fopen(STUD_FILE, "r");
    if (!fp) {
        printf("No student records found!\n");
        return;
    }

    FILE *temp = fopen("temp.txt", "w");
    if (!temp) {
        printf("Error: Unable to open temp file!\n");
        fclose(fp);
        return;
    }

    int roll, sem, updated = 0;
    float mark, newMarks;
    char name[50], course[20], degree[20], datetime[100], newDatetime[50];

    while (fscanf(fp, "%d %49s %19s %19s %d %f %99[^\n]",
                  &roll, name, course, degree, &sem, &mark, datetime) == 7) {

        if (roll == uroll) {
            printf("Current Record:\n");
            printf("%d %s %s %s %d %.2f %s\n",
                   roll, name, course, degree, sem, mark, datetime);

            printf("Enter New Marks: ");
            if (scanf("%f", &newMarks) != 1) {
                printf("Invalid marks input.\n");
                fclose(fp);
                fclose(temp);
                remove("temp.txt");
                return;
            }

            getCurrentDateTime(newDatetime);

            fprintf(temp, "%d %s %s %s %d %.2f %s\n",
                    roll, name, course, degree, sem, newMarks, newDatetime);

            updated = 1;
        } else {
            fprintf(temp, "%d %s %s %s %d %.2f %s\n",
                    roll, name, course, degree, sem, mark, datetime);
        }
    }

    fclose(fp);
    fclose(temp);

    remove(STUD_FILE);
    rename("temp.txt", STUD_FILE);

    if (updated)
        printf("Marks updated successfully!\n");
    else
        printf("No record found for the given roll number.\n");
}

/* MAIN */
int main() {
    int choice;
    printf("1. Sign Up\n2. Login\nChoose Option: ");
    if (scanf("%d", &choice) != 1) { printf("Invalid choice.\n"); return 0; }
    getchar();

    if (choice == 1)
        signup();

    if (!login()) {
        printf("Login failed! Please try again.\n");
        return 0;
    }

    printf("\nLogin successful! Role: %s\n", currentRole);

    int menu;
    while (1) {
        printf("\nMain Menu:\n");
        printf("1.Add Student  2.Display  3.Search  4.Delete  5.Update Marks  6.Exit\n");
        printf("Choose an option: ");
        if (scanf("%d", &menu) != 1) { printf("Invalid option.\n"); break; }
        getchar();

        if (menu == 1) {
            if (isAdmin())
                addStudent();
            else
                printf("Access denied! Only ADMIN can add students.\n");
        } else if (menu == 2) {
            displayStudents();
        } else if (menu == 3) {
            searchStudent();
        } else if (menu == 4) {
            if (isAdmin())
                deleteStudent();
            else
                printf("Access denied! Only ADMIN can delete students.\n");
        } else if (menu == 5) {
            if (isAdmin() || strcmp(currentRole, "staff") == 0 || strcmp(currentRole, "STAFF") == 0) {
                updateMarks();
            } else {
                printf("Access denied! Only ADMIN or STAFF can update marks.\n");
            }
        } else {
            printf("Exiting. Goodbye!\n");
            break;
        }
    }
    return 0;
}
