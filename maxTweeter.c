#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LENGTH 1024
#define MAX_LENGTH_CSV 20000
#define SORTED_LENGTH 10

typedef struct tweeters
{
    char *name;
    int tweetcount;
} twt;

// Function to check that only one arg is inputted
void argChecker(int argc)
{
    if (argc != 2) {
        printf("Invalid Input Format\n");
        exit(0);
    }
}

// Check if file is null
void nullFileChecker(FILE *fptr)
{
    if (fptr == NULL) {
        printf("Invalid Input Format\n");
        exit(0);
    }
}

// Checks that the length of the current line doesnt surpass 1024 character
void line_length_check(char line[])
{
    if ((sizeof(*line)/sizeof(char)) > MAX_LENGTH) {
        printf("Invalid Input Format\n");
        exit(0);
    }
}

char *strtoke(char *str, const char *delim)
{
    // Stores string str for consecutive calls
    static char *start = NULL;
    // Variable to check if token is found
    char *token = NULL;

    // Assign new start in case
    if (str) start = str;

    // Check whether text to parse left
    if (!start) return NULL;

    // Remember current start as found token
    token = start;

    // Find next occurence of delim
    start = strpbrk(start, delim);

    // Replace delim with terminator and move start to follower
    if (start) *start++ = '\0';

    return token;
}

// Check to see that CSV has valid header
void checkHeader(char *str, int* name_count, int* num_fields, int* quoted, int *numCommas)
{
    int experienced_name = 0;
    // Get each word in string
    char *token;

    int count_tokens = 1;
    if (str[0] == ',') {
        count_tokens++;
        (*numCommas)++;
    }

    token = strtoke(str, ",");
    //(*numCommas)++;

    // Check each word for name or "name"
    while (token != NULL) {
        //printf("%s\n", token);
        //printf("\"\"\"name\"\"\"");
        // If it is in tokens, set nameExist to 1
        if (strcmp(token, "\"\"\"name\"\"\"") == 0) {
            experienced_name++;
            (*name_count) = count_tokens;
            // Name field is quoted
            (*quoted) = 1;
        }

        if (strcmp(token, "name") == 0 || strcmp(token, "name\r\n") == 0) {
            (*name_count) = count_tokens;
            experienced_name++;
        }

        // Get next token
        token = strtoke(NULL, ",");
        if (token != NULL){
            (*numCommas)++;
        }
        (*num_fields) = count_tokens;
        count_tokens++;

    }

    if (experienced_name != 1){
        printf("Invalid Input Format\n");
        exit(0);
    }
}

int tweeterexists(struct tweeters stored[] ,char* tweeter, int maxuser)
{
    for(int i = 0; i < maxuser; i++)
    {
        if(stored[i].name)
        {
            // User already tweeted; increase tweet count
            if(strcmp(stored[i].name, tweeter) == 0) {
                stored[i].tweetcount++;
                return 1;
            }
        }
    }
    return 0;
}

char* tokenizeLine(char *str, const int count_name, int *commas)
{
    char *token;

    int count_field = 1;
    token = strtoke(str, ",");
    (*commas)++;

    while (token != NULL) {
            if (count_field == count_name) {
                  return token;
            }

            // Get next token
            token = strtoke(NULL, ",");

        count_field++;
        (*commas)++;
        }
        printf("Invalid Input Format\n");
        exit(0);
}

twt sort_struct_DESC(twt sorted[])
{
    twt lesser_struct;
        // Sorting algo
    for(int i = 0; i < MAX_LENGTH_CSV; i++) {
        if(!sorted[i].name)
            break;
        for(int j = i+1; j < MAX_LENGTH_CSV; j++)
        {
            if(!sorted[j].name)
                break;
            if(sorted[i].tweetcount < sorted[j].tweetcount)
            {
                lesser_struct = sorted[i];
                sorted[i] = sorted[j];
                sorted[j] = lesser_struct;
            }
        }
    }
    // Returns struct w/ sorted usernames
    return *sorted;
}

int main(int argc, char **argv)
{
    int count_of_tweeters = 0;
    twt TweeterArray[MAX_LENGTH_CSV];

    // First check that there is exactly 1 arg
    argChecker(argc);
    int name_exists = 0;
    char *tweeter;

    // Create file ptr and check if exists
    FILE *fptr;
    char line[MAX_LENGTH];
    fptr = fopen(argv[1],"r");
    nullFileChecker(fptr);

    // The field which contains the name field
    int name_count = 0;
    // Number of fields in each row
    int num_fields = 0;
    // Indicates whether or not the name field is quoted
    int quoted = 0;
    // Number of commas
    int commaTot = 0;
// number of commas per line
     int commaCount = 0;
    // Get the first line and check for header
    int lentweeter = 0;
    //used for getting the length of the tweeter name for quoting purposes
    fgets(line, sizeof(line), fptr);
    // Check for header and fill variables
    checkHeader(line, &name_count, &num_fields, &quoted, &commaTot);

    //printf("num commas in header: %d\n", commaTot);

    while (fgets(line, sizeof(line), fptr)) {
        commaCount = 0;
        line_length_check(line);
        tweeter = tokenizeLine(line, name_count, &commaCount);
        lentweeter = (int)strlen(tweeter);
        if (quoted == 1){
            if ( tweeter[0] != '"' && tweeter[lentweeter] != '"'){
                printf("Invalid Input Format\n");
                exit(0);
            }
        } else{
            if ( tweeter[0] == '"' || tweeter[lentweeter] == '"'){
                printf("Invalid Input Format\n");
                exit(0);
            }

        }
        name_exists = tweeterexists(TweeterArray, tweeter, count_of_tweeters);

        if (commaCount != commaTot) {
            printf("Invalid Input Format\n");
            exit(0);
        }

        if (name_exists == 0) {
            TweeterArray[count_of_tweeters].name = (char *) malloc(200);
            strcpy(TweeterArray[count_of_tweeters].name, tweeter);
            TweeterArray[count_of_tweeters].tweetcount = 1;
            count_of_tweeters++;
        }
    }

    sort_struct_DESC(TweeterArray); // sorts the hashmap by number of tweets
    int max_sort_length = 10;
    if (count_of_tweeters <SORTED_LENGTH){
        max_sort_length = count_of_tweeters;
    }
    for(int q = 0; q < max_sort_length; q++) {
        if(!TweeterArray[q].name)
            break;
        printf("%s: %d\n", TweeterArray[q].name, TweeterArray[q].tweetcount);
        }
     for(int j = 0; j < count_of_tweeters; j++) {
         free(TweeterArray[j].name);
     }
    int test = fclose(fptr);

    if(test == -1){
        printf("Invalid Input Format\n");
        exit(0);
    }
}
