#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>  // added for timestamps

#define NAME_LEN  50
#define DIAG_LEN  80
#define LINE_LEN  256

typedef struct {
    int id;
    char name[NAME_LEN];
    int age;
    int priority;              // 1 (critical) .. 5 (low)
    char diagnosis[DIAG_LEN];
    time_t timestamp;          // timestamp when patient added
} Patient;

typedef struct {
    Patient *arr;              // dynamic array
    int size;
    int cap;
} PatientList;

/* ===================== Utilities ===================== */

static void trim_newline(char *s) {
    size_t n = strlen(s);
    if (n && s[n-1] == '\n') s[n-1] = '\0';
}

static void safe_readline(const char *prompt, char *buf, int maxlen) {
    printf("%s", prompt);
    if (!fgets(buf, maxlen, stdin)) {
        buf[0] = '\0';
        return;
    }
    trim_newline(buf);
}

static int read_int(const char *prompt, int minv, int maxv) {
    char line[LINE_LEN];
    while (1) {
        safe_readline(prompt, line, sizeof(line));
        int ok = 1;
        int i = 0;
        if (line[0] == '-' || line[0] == '+') i++;
        for (; line[i]; i++) {
            if (!isdigit((unsigned char)line[i])) { ok = 0; break; }
        }
        if (!ok || line[0] == '\0') {
            printf("Invalid number. Try again.\n");
            continue;
        }
        long v = strtol(line, NULL, 10);
        if (v < minv || v > maxv) {
            printf("Out of range (%d..%d). Try again.\n", minv, maxv);
            continue;
        }
        return (int)v;
    }
}

/* ===================== Dynamic Array (Pointers/Allocation) ===================== */

static void init_list(PatientList *L) {
    L->size = 0;
    L->cap = 8;
    L->arr = (Patient*)malloc(sizeof(Patient) * L->cap);
    if (!L->arr) {
        printf("Memory allocation failed.\n");
        exit(1);
    }
}

static void free_list(PatientList *L) {
    free(L->arr);
    L->arr = NULL;
    L->size = L->cap = 0;
}

static void ensure_cap(PatientList *L) {
    if (L->size < L->cap) return;
    L->cap *= 2;
    Patient *newArr = (Patient*)realloc(L->arr, sizeof(Patient) * L->cap);
    if (!newArr) {
        printf("Memory re-allocation failed.\n");
        free_list(L);
        exit(1);
    }
    L->arr = newArr;
}

/* ===================== Recursion (merge-sort by ID for binary search) ===================== */

static void merge_by_id(Patient *a, int l, int m, int r, Patient *tmp) {
    int i = l, j = m+1, k = l;
    while (i <= m && j <= r) {
        if (a[i].id <= a[j].id) tmp[k++] = a[i++];
        else tmp[k++] = a[j++];
    }
    while (i <= m) tmp[k++] = a[i++];
    while (j <= r) tmp[k++] = a[j++];
    for (i = l; i <= r; i++) a[i] = tmp[i];
}

static void merge_sort_by_id_rec(Patient *a, int l, int r, Patient *tmp) {
    if (l >= r) return;
    int m = l + (r - l)/2;
    merge_sort_by_id_rec(a, l, m, tmp);
    merge_sort_by_id_rec(a, m+1, r, tmp);
    merge_by_id(a, l, m, r, tmp);
}

static void sort_by_id_for_binary(PatientList *L) {
    if (L->size <= 1) return;
    Patient *tmp = (Patient*)malloc(sizeof(Patient) * L->size);
    if (!tmp) { printf("Memory failed.\n"); return; }
    merge_sort_by_id_rec(L->arr, 0, L->size-1, tmp);
    free(tmp);
}

/* ===================== Search (Linear & Binary) ===================== */

static int linear_search_by_name(PatientList *L, const char *needle) {
    for (int i = 0; i < L->size; i++) {
        char nameLower[NAME_LEN], needleLower[NAME_LEN];
        snprintf(nameLower, sizeof(nameLower), "%s", L->arr[i].name);
        snprintf(needleLower, sizeof(needleLower), "%s", needle);
        for (int k = 0; nameLower[k]; k++) nameLower[k] = (char)tolower((unsigned char)nameLower[k]);
        for (int k = 0; needleLower[k]; k++) needleLower[k] = (char)tolower((unsigned char)needleLower[k]);
        if (strstr(nameLower, needleLower)) return i;
    }
    return -1;
}

static int binary_search_by_id(PatientList *L, int id) {
    int lo = 0, hi = L->size - 1;
    while (lo <= hi) {
        int mid = lo + (hi - lo)/2;
        if (L->arr[mid].id == id) return mid;
        if (L->arr[mid].id < id) lo = mid + 1;
        else hi = mid - 1;
    }
    return -1;
}

/* ===================== Bubble Sort (Queue order by priority then arrival) ===================== */

static void bubble_sort_by_priority(PatientList *L) {
    for (int i = 0; i < L->size - 1; i++) {
        for (int j = 0; j < L->size - 1 - i; j++) {
            if (L->arr[j].priority > L->arr[j+1].priority) {
                Patient t = L->arr[j];
                L->arr[j] = L->arr[j+1];
                L->arr[j+1] = t;
            }
        }
    }
}

/* ===================== 2D Arrays (Statistics by priority & age groups) ===================== */

static void show_stats_2d(PatientList *L) {
    int stats[5][4] = {0};
    for (int i = 0; i < L->size; i++) {
        int p = L->arr[i].priority;
        int age = L->arr[i].age;
        int col = 0;
        if (age <= 17) col = 0;
        else if (age <= 40) col = 1;
        else if (age <= 60) col = 2;
        else col = 3;
        if (p >= 1 && p <= 5) stats[p-1][col]++;
    }
    printf("\n--- Stats (2D Array): Priority x AgeGroup ---\n");
    printf("AgeGroups: [0-17] [18-40] [41-60] [61+]\n");
    for (int pr = 0; pr < 5; pr++) {
        printf("Priority %d:    %5d  %6d  %6d  %5d\n",
               pr+1, stats[pr][0], stats[pr][1], stats[pr][2], stats[pr][3]);
    }
}

/* ===================== Core Queue Ops ===================== */

static int id_exists_linear(PatientList *L, int id) {
    for (int i = 0; i < L->size; i++) if (L->arr[i].id == id) return 1;
    return 0;
}

static void add_patient(PatientList *L) {
    Patient p;
    p.id = read_int("Enter ID (1..999999): ", 1, 999999);
    if (id_exists_linear(L, p.id)) {
        printf("ID already exists.\n");
        return;
    }
    safe_readline("Enter Name: ", p.name, sizeof(p.name));
    if (p.name[0] == '\0') { printf("Name cannot be empty.\n"); return; }
    p.age = read_int("Enter Age (0..120): ", 0, 120);
    p.priority = read_int("Enter Priority (1=critical .. 5=low): ", 1, 5);
    safe_readline("Enter Diagnosis: ", p.diagnosis, sizeof(p.diagnosis));
    if (p.diagnosis[0] == '\0') snprintf(p.diagnosis, sizeof(p.diagnosis), "N/A");

    p.timestamp = time(NULL);  // store time

    ensure_cap(L);
    L->arr[L->size++] = p;

    bubble_sort_by_priority(L);

    printf("Patient added to queue.\n");
}

static void serve_next(PatientList *L) {
    if (L->size == 0) {
        printf("Queue is empty.\n");
        return;
    }
    Patient served = L->arr[0];
    for (int i = 1; i < L->size; i++) L->arr[i-1] = L->arr[i];
    L->size--;

    char buf[26];
    ctime_r(&served.timestamp, buf);
    buf[strcspn(buf, "\n")] = 0;

    printf("\n--- Served Patient ---\n");
    printf("ID: %d | Name: %s | Age: %d | Priority: %d | Dx: %s | Added: %s\n",
           served.id, served.name, served.age, served.priority, served.diagnosis, buf);
}

static void display_queue(PatientList *L) {
    if (L->size == 0) {
        printf("Queue is empty.\n");
        return;
    }
    printf("\n--- Current Queue (Priority Order) ---\n");
    printf("%-6s | %-20s | %-3s | %-8s | %-30s | %-20s\n", "ID", "Name", "Age", "Priority", "Diagnosis", "Added");
    printf("------------------------------------------------------------------------------------------\n");
    for (int i = 0; i < L->size; i++) {
        char buf[26];
        ctime_r(&L->arr[i].timestamp, buf);
        buf[strcspn(buf, "\n")] = 0;
        printf("%-6d | %-20s | %-3d | %-8d | %-30s | %-20s\n",
               L->arr[i].id, L->arr[i].name, L->arr[i].age, L->arr[i].priority, L->arr[i].diagnosis, buf);
    }
}

/* ===================== Search ===================== */

static void search_menu(PatientList *L) {
    int choice = read_int("\nSearch by: 1) ID (Binary)  2) Name (Linear)  => ", 1, 2);
    if (choice == 1) {
        if (L->size == 0) { printf("Queue is empty.\n"); return; }
        int id = read_int("Enter ID: ", 1, 999999);
        sort_by_id_for_binary(L);
        int idx = binary_search_by_id(L, id);
        if (idx == -1) printf("Not found.\n");
        else {
            Patient *p = &L->arr[idx];
            char buf[26];
            ctime_r(&p->timestamp, buf);
            buf[strcspn(buf, "\n")] = 0;
            printf("FOUND: ID=%d | Name=%s | Age=%d | Priority=%d | Dx=%s | Added: %s\n",
                   p->id, p->name, p->age, p->priority, p->diagnosis, buf);
        }
        bubble_sort_by_priority(L);
    } else {
        char name[NAME_LEN];
        safe_readline("Enter part of name: ", name, sizeof(name));
        int idx = linear_search_by_name(L, name);
        if (idx == -1) printf("Not found.\n");
        else {
            Patient *p = &L->arr[idx];
            char buf[26];
            ctime_r(&p->timestamp, buf);
            buf[strcspn(buf, "\n")] = 0;
            printf("FOUND: ID=%d | Name=%s | Age=%d | Priority=%d | Dx=%s | Added: %s\n",
                   p->id, p->name, p->age, p->priority, p->diagnosis, buf);
        }
    }
}

/* ===================== File Operations ===================== */

static void save_to_file(PatientList *L, const char *filename) {
    FILE *fp = fopen(filename, "w");
    if (!fp) { printf("Cannot open file for writing.\n"); return; }

    fprintf(fp, "%d\n", L->size);
    for (int i = 0; i < L->size; i++) {
        fprintf(fp, "%d|%s|%d|%d|%s|%ld\n",
                L->arr[i].id, L->arr[i].name, L->arr[i].age,
                L->arr[i].priority, L->arr[i].diagnosis,
                (long)L->arr[i].timestamp);
    }
    fclose(fp);
    printf("Saved to %s\n", filename);
}

static int parse_line_to_patient(const char *line, Patient *p) {
    char tmp[LINE_LEN];
    snprintf(tmp, sizeof(tmp), "%s", line);

    char *tok = strtok(tmp, "|");
    if (!tok) return 0;
    p->id = atoi(tok);

    tok = strtok(NULL, "|");
    if (!tok) return 0;
    snprintf(p->name, sizeof(p->name), "%s", tok);

    tok = strtok(NULL, "|");
    if (!tok) return 0;
    p->age = atoi(tok);

    tok = strtok(NULL, "|");
    if (!tok) return 0;
    p->priority = atoi(tok);

    tok = strtok(NULL, "|");
    if (!tok) return 0;
    snprintf(p->diagnosis, sizeof(p->diagnosis), "%s", tok);

    tok = strtok(NULL, "\n");
    if (!tok) return 0;
    p->timestamp = (time_t)atol(tok);

    if (p->id <= 0 || p->age < 0 || p->age > 120 || p->priority < 1 || p->priority > 5) return 0;
    if (p->name[0] == '\0') return 0;
    return 1;
}

static void load_from_file(PatientList *L, const char *filename) {
    FILE *fp = fopen(filename, "r");
    if (!fp) { printf("No file found: %s\n", filename); return; }

    int n = 0;
    if (fscanf(fp, "%d\n", &n) != 1 || n < 0) {
        printf("Corrupted file.\n");
        fclose(fp);
        return;
    }

    L->size = 0;
    char line[LINE_LEN];
    for (int i = 0; i < n; i++) {
        if (!fgets(line, sizeof(line), fp)) break;
        trim_newline(line);
        Patient p;
        if (parse_line_to_patient(line, &p)) {
            ensure_cap(L);
            L->arr[L->size++] = p;
        }
    }
    fclose(fp);
    bubble_sort_by_priority(L);
    printf("Loaded %d patients from %s\n", L->size, filename);
}

/* ===================== Main Menu ===================== */

static void print_menu(void) {
    printf("\n=========== Hospital Patient Queue System ===========\n");
    printf("1) Add Patient\n");
    printf("2) Serve Next Patient\n");
    printf("3) Display Queue\n");
    printf("4) Search (Binary by ID / Linear by Name)\n");
    printf("5) Stats (2D Array report)\n");
    printf("6) Save to File\n");
    printf("7) Load from File\n");
    printf("0) Exit\n");
    printf("=====================================================\n");
}

int main(void) {
    PatientList L;
    init_list(&L);

    const char *DATAFILE = "patients.txt";

    while (1) {
        print_menu();
        int ch = read_int("Choose: ", 0, 7);

        if (ch == 0) break;
        else if (ch == 1) add_patient(&L);
        else if (ch == 2) serve_next(&L);
        else if (ch == 3) display_queue(&L);
        else if (ch == 4) search_menu(&L);
        else if (ch == 5) show_stats_2d(&L);
        else if (ch == 6) save_to_file(&L, DATAFILE);
        else if (ch == 7) load_from_file(&L, DATAFILE);
    }

    free_list(&L);
    printf("Goodbye.\n");
    return 0;
}
