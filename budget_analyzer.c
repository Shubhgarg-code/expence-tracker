#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

#define MAX_TRANSACTIONS 1000
#define MAX_CATEGORIES 20
#define MAX_NAME 50

typedef struct {
    int id;
    char date[20];
    char category[MAX_NAME];
    float amount;
    char description[100];
} Transaction;

typedef struct {
    char name[MAX_NAME];
    float budget;
    float spent;
    float average;
    int count;
} Category;

typedef struct {
    Transaction transactions[MAX_TRANSACTIONS];
    Category categories[MAX_CATEGORIES];
    int trans_count;
    int cat_count;
    float monthly_budget;
} FinanceData;

// Function prototypes
void initialize_data(FinanceData *data);
void add_transaction(FinanceData *data, char *category, float amount, char *description, char *date);
void add_category(FinanceData *data, char *name, float budget);
void display_menu();
void display_dashboard(FinanceData *data);
void analyze_spending(FinanceData *data);
void detect_anomalies(FinanceData *data);
void set_budget(FinanceData *data);
void display_transactions(FinanceData *data);
void generate_report(FinanceData *data);
float calculate_average(FinanceData *data, char *category);
float calculate_std_dev(FinanceData *data, char *category);
void update_category_stats(FinanceData *data);

// Initialize system
void initialize_data(FinanceData *data) {
    data->trans_count = 0;
    data->cat_count = 0;
    data->monthly_budget = 5000.0;
}

// Add new transaction
void add_transaction(FinanceData *data, char *category, float amount, char *description, char *date) {
    if (data->trans_count >= MAX_TRANSACTIONS) {
        printf("Transaction limit reached!\n");
        return;
    }

    data->transactions[data->trans_count].id = data->trans_count + 1;
    strcpy(data->transactions[data->trans_count].category, category);
    strcpy(data->transactions[data->trans_count].description, description);
    strcpy(data->transactions[data->trans_count].date, date);
    data->transactions[data->trans_count].amount = amount;

    data->trans_count++;
    update_category_stats(data);
    printf("[OK] Transaction added successfully!\n");
}

// Add category
void add_category(FinanceData *data, char *name, float budget) {
    if (data->cat_count >= MAX_CATEGORIES) {
        printf("Category limit reached!\n");
        return;
    }

    strcpy(data->categories[data->cat_count].name, name);
    data->categories[data->cat_count].budget = budget;
    data->categories[data->cat_count].spent = 0;
    data->categories[data->cat_count].count = 0;

    data->cat_count++;
    printf("[OK] Category added successfully!\n");
}

// Update category statistics
void update_category_stats(FinanceData *data) {
    for (int i = 0; i < data->cat_count; i++) {
        data->categories[i].spent = 0;
        data->categories[i].count = 0;
    }

    for (int i = 0; i < data->trans_count; i++) {
        for (int j = 0; j < data->cat_count; j++) {
            if (strcmp(data->transactions[i].category, data->categories[j].name) == 0) {
                data->categories[j].spent += data->transactions[i].amount;
                data->categories[j].count++;
            }
        }
    }

    for (int i = 0; i < data->cat_count; i++) {
        if (data->categories[i].count > 0) {
            data->categories[i].average = data->categories[i].spent / data->categories[i].count;
        }
    }
}

// Display main menu
void display_menu() {
    printf("\n=====================================\n");
    printf("  SMART BUDGET ANALYZER\n");
    printf("=====================================\n");
    printf(" 1. Add Transaction\n");
    printf(" 2. Add Category\n");
    printf(" 3. View Dashboard\n");
    printf(" 4. Analyze Spending Patterns\n");
    printf(" 5. Detect Anomalies\n");
    printf(" 6. View All Transactions\n");
    printf(" 7. Generate Monthly Report\n");
    printf(" 8. Set Monthly Budget\n");
    printf(" 9. Exit\n");
    printf("=====================================\n");
    printf("Enter choice: ");
}

// Display dashboard
void display_dashboard(FinanceData *data) {
    printf("\n=====================================\n");
    printf("     FINANCIAL DASHBOARD\n");
    printf("=====================================\n");

    float total_spent = 0;
    for (int i = 0; i < data->trans_count; i++) {
        total_spent += data->transactions[i].amount;
    }

    printf(" Monthly Budget: $%.2f\n", data->monthly_budget);
    printf(" Total Spent: $%.2f\n", total_spent);
    printf(" Remaining: $%.2f\n", data->monthly_budget - total_spent);
    printf(" Transactions: %d\n", data->trans_count);

    float percentage = (total_spent / data->monthly_budget) * 100;
    printf(" Budget Used: %.1f%%\n", percentage);

    printf("\n--- CATEGORY BREAKDOWN ---\n");

    for (int i = 0; i < data->cat_count; i++) {
        if (data->categories[i].count > 0) {
            float cat_percentage = (data->categories[i].spent / data->monthly_budget) * 100;
            printf(" %s: $%.2f (%.1f%%)\n",
                   data->categories[i].name,
                   data->categories[i].spent,
                   cat_percentage);
        }
    }

    printf("=====================================\n");
}

// Calculate average spending for category
float calculate_average(FinanceData *data, char *category) {
    float sum = 0;
    int count = 0;

    for (int i = 0; i < data->trans_count; i++) {
        if (strcmp(data->transactions[i].category, category) == 0) {
            sum += data->transactions[i].amount;
            count++;
        }
    }

    return count > 0 ? sum / count : 0;
}

// Calculate standard deviation
float calculate_std_dev(FinanceData *data, char *category) {
    float avg = calculate_average(data, category);
    float sum = 0;
    int count = 0;

    for (int i = 0; i < data->trans_count; i++) {
        if (strcmp(data->transactions[i].category, category) == 0) {
            sum += (data->transactions[i].amount - avg) * (data->transactions[i].amount - avg);
            count++;
        }
    }

    return count > 1 ? sqrt(sum / (count - 1)) : 0;
}

// Detect anomalies in spending
void detect_anomalies(FinanceData *data) {
    printf("\n=====================================\n");
    printf("  ANOMALY DETECTION REPORT\n");
    printf("=====================================\n\n");

    int anomalies_found = 0;

    for (int i = 0; i < data->cat_count; i++) {
        float avg = calculate_average(data, data->categories[i].name);
        float std_dev = calculate_std_dev(data, data->categories[i].name);

        if (avg > 0) {
            printf("[*] Category: %s\n", data->categories[i].name);
            printf("    Average: $%.2f | Std Dev: $%.2f\n", avg, std_dev);

            for (int j = 0; j < data->trans_count; j++) {
                if (strcmp(data->transactions[j].category, data->categories[i].name) == 0) {
                    float z_score = (data->transactions[j].amount - avg) / (std_dev + 0.01);

                    if (fabs(z_score) > 2.0) {
                        printf("    [!] ANOMALY: $%.2f on %s\n",
                               data->transactions[j].amount,
                               data->transactions[j].date);
                        printf("        Description: %s\n", data->transactions[j].description);
                        printf("        Z-Score: %.2f\n\n", z_score);
                        anomalies_found++;
                    }
                }
            }
        }
    }

    if (anomalies_found == 0) {
        printf("[OK] No anomalies detected! Your spending is consistent.\n");
    } else {
        printf("\n[WARNING] Found %d anomalous transactions!\n", anomalies_found);
    }
}

// Analyze spending patterns
void analyze_spending(FinanceData *data) {
    printf("\n=====================================\n");
    printf("  SPENDING PATTERN ANALYSIS\n");
    printf("=====================================\n\n");

    if (data->trans_count == 0) {
        printf("No transactions to analyze.\n");
        return;
    }

    for (int i = 0; i < data->cat_count; i++) {
        if (data->categories[i].count > 0) {
            float cat_percentage = (data->categories[i].spent / (data->monthly_budget + 0.01)) * 100;
            float avg = data->categories[i].average;

            printf("[%s]\n", data->categories[i].name);
            printf("    Total: $%.2f | Average per transaction: $%.2f\n", data->categories[i].spent, avg);
            printf("    Transactions: %d | Budget usage: %.1f%%\n", data->categories[i].count, cat_percentage);

            if (cat_percentage > 50) {
                printf("    [!] HIGH SPENDING - Consider reducing expenses in this category\n");
            } else if (cat_percentage > 30) {
                printf("    [*] MODERATE SPENDING - Keep monitoring\n");
            } else {
                printf("    [OK] HEALTHY SPENDING\n");
            }
            printf("\n");
        }
    }
}

// Display all transactions
void display_transactions(FinanceData *data) {
    printf("\n=====================================\n");
    printf("      ALL TRANSACTIONS\n");
    printf("=====================================\n\n");

    if (data->trans_count == 0) {
        printf("No transactions recorded.\n");
        return;
    }

    for (int i = 0; i < data->trans_count; i++) {
        printf("[%d] %s | %s | $%.2f\n",
               data->transactions[i].id,
               data->transactions[i].date,
               data->transactions[i].category,
               data->transactions[i].amount);
        printf("    Description: %s\n\n", data->transactions[i].description);
    }
}

// Generate monthly report
void generate_report(FinanceData *data) {
    printf("\n=====================================\n");
    printf("  MONTHLY FINANCIAL REPORT\n");
    printf("=====================================\n\n");

    float total_spent = 0;
    for (int i = 0; i < data->trans_count; i++) {
        total_spent += data->transactions[i].amount;
    }

    printf("Total Transactions: %d\n", data->trans_count);
    printf("Total Spent: $%.2f\n", total_spent);
    printf("Monthly Budget: $%.2f\n", data->monthly_budget);
    printf("Remaining Budget: $%.2f\n\n", data->monthly_budget - total_spent);

    float percentage = (total_spent / (data->monthly_budget + 0.01)) * 100;
    printf("Budget Utilization: %.1f%%\n\n", percentage);

    printf("Category Breakdown:\n");
    for (int i = 0; i < data->cat_count; i++) {
        if (data->categories[i].count > 0) {
            printf("  %s: $%.2f (%.1f%% of budget)\n",
                   data->categories[i].name,
                   data->categories[i].spent,
                   (data->categories[i].spent / (data->monthly_budget + 0.01)) * 100);
        }
    }

    printf("\nRecommendations:\n");
    if (percentage > 90) {
        printf("[!] You are using 90%%+ of your budget. Reduce spending immediately!\n");
    } else if (percentage > 75) {
        printf("[*] You are using 75%%+ of your budget. Start cutting down expenses.\n");
    } else {
        printf("[OK] Your spending is under control. Keep it up!\n");
    }
}

// Set monthly budget
void set_budget(FinanceData *data) {
    printf("Enter monthly budget: $");
    scanf("%f", &data->monthly_budget);
    printf("[OK] Budget set to $%.2f\n", data->monthly_budget);
}

// Interactive initial setup: categories and optional initial transactions
void interactive_setup(FinanceData *data) {
    int ccount = 0;
    char line[128];

    printf("Would you like to set up your categories now? (y/n): ");
    if (fgets(line, sizeof(line), stdin) == NULL) return;
    if (line[0] == 'y' || line[0] == 'Y') {
        printf("How many categories do you want to add (max %d): ", MAX_CATEGORIES);
        if (fgets(line, sizeof(line), stdin) == NULL) return;
        ccount = atoi(line);
        if (ccount < 0) ccount = 0;
        if (ccount > MAX_CATEGORIES) ccount = MAX_CATEGORIES;

        for (int i = 0; i < ccount; i++) {
            char name[MAX_NAME];
            float budget = 0.0f;
            printf("Enter name for category #%d: ", i + 1);
            if (fgets(name, sizeof(name), stdin) == NULL) break;
            name[strcspn(name, "\n")] = '\0';

            printf("Enter monthly budget for '%s': $", name);
            if (fgets(line, sizeof(line), stdin) == NULL) break;
            budget = (float)atof(line);

            add_category(data, name, budget);
        }
    }

    printf("Would you like to add initial transactions now? (y/n): ");
    if (fgets(line, sizeof(line), stdin) == NULL) return;
    if (line[0] == 'y' || line[0] == 'Y') {
        while (1) {
            char category[MAX_NAME];
            char description[100];
            char date[20];
            float amount = 0.0f;

            printf("Enter category for transaction (or just Enter to stop): ");
            if (fgets(category, sizeof(category), stdin) == NULL) break;
            if (category[0] == '\n') break;
            category[strcspn(category, "\n")] = '\0';

            printf("Enter amount: $");
            if (fgets(line, sizeof(line), stdin) == NULL) break;
            amount = (float)atof(line);

            printf("Enter description: ");
            if (fgets(description, sizeof(description), stdin) == NULL) break;
            description[strcspn(description, "\n")] = '\0';

            printf("Enter date (YYYY-MM-DD): ");
            if (fgets(date, sizeof(date), stdin) == NULL) break;
            date[strcspn(date, "\n")] = '\0';

            add_transaction(data, category, amount, description, date);
        }
    }
}
// Main function
int main() {
    FinanceData data;
    initialize_data(&data);

    // Interactive setup: let the user define categories and optionally add initial transactions
    interactive_setup(&data);

    int choice;
    char category[MAX_NAME], description[100], date[20];
    float amount;

    while (1) {
        display_menu();
        scanf("%d", &choice);
        getchar();

        switch (choice) {
            case 1:
                printf("Enter category: ");
                fgets(category, MAX_NAME, stdin);
                category[strcspn(category, "\n")] = 0;

                printf("Enter amount: $");
                scanf("%f", &amount);
                getchar();

                printf("Enter description: ");
                fgets(description, 100, stdin);
                description[strcspn(description, "\n")] = 0;

                printf("Enter date (YYYY-MM-DD): ");
                fgets(date, 20, stdin);
                date[strcspn(date, "\n")] = 0;

                add_transaction(&data, category, amount, description, date);
                break;

            case 2:
                printf("Enter category name: ");
                fgets(category, MAX_NAME, stdin);
                category[strcspn(category, "\n")] = 0;

                printf("Enter budget for this category: $");
                scanf("%f", &amount);

                add_category(&data, category, amount);
                break;

            case 3:
                display_dashboard(&data);
                break;

            case 4:
                analyze_spending(&data);
                break;

            case 5:
                detect_anomalies(&data);
                break;

            case 6:
                display_transactions(&data);
                break;

            case 7:
                generate_report(&data);
                break;

            case 8:
                set_budget(&data);
                break;

            case 9:
                printf("\nThank you for using Smart Budget Analyzer!\n");
                return 0;

            default:
                printf("Invalid choice! Please try again.\n");
        }
    }

    return 0;
}
