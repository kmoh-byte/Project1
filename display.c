#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <conio.h>

#define APP_NAME "Assign08"

typedef enum { FIRST, LAST, YEAR } IndexKey;

typedef union
{
    char	first[30];
    char	last[30];
    int		age;
    int     recCount;
} KeyType;

typedef struct {
    unsigned int acctNum; // account number
    char lastName[15]; // account last name
    char firstName[10]; // account first name
    int  age;
    int  indexHeader;
    int  indexTemp;

    double balance; // account balance
} Person;

typedef struct
{
    KeyType		key;
    long		filePos;
    int         indexTemp;
} IndexRecord;

typedef struct
{
    IndexKey	idxKey;
    char		appName[20];
    int			recCount;
    int         indexTemp;
} IndexHeader;

// function prototypes
long fileSize(FILE* input); //computes file size
int compare(const void* left, const void* right); //used by qsort

IndexKey indexKey;

int main(int argc, char* argv[])
{
    FILE* fpData;
    FILE* fpIndex;
    FILE* accountsIdx;
    int		readCount;
    int		recCount;
    int		writeCount;
    int		dataRecordCount;
    int		indexRecordCount;
    long	filePos;

    IndexHeader	indexHeader;
    IndexRecord* indexTemp;
    Person		temp = { "","", 0 };

    indexKey = LAST;

    printf(" %d", argc);
    printf(" %s", argv[2]);

    if ((fpData = fopen("accounts.dat", "rb")) == NULL)
    {
        printf("ERROR - can not open data file.\n");
        return 1;
    }

    if ((fpIndex = fopen("accounts.idx", "wb")) == NULL)
    {
        printf("ERROR - can not open/create index file.\n");
        return 1;
    }


if ((fpData = fopen("accounts.dat", "rb")) == NULL)
{
    printf("File could not be opened.\n");
    return 1;
}

// open index file for reading
if ((fpIndex = fopen("accounts.idx", "rb")) == NULL)
{
    printf("ERROR - can not open index file.\n");
    return 1;
}

// calculate record count in data file
recCount = fileSize(fpData) / sizeof(Person);

// read header record in index file
readCount = fread(&indexHeader, sizeof(IndexHeader), 1, fpIndex);

// check that application versions match
if (strcmp(indexHeader.appName, APP_NAME) != 0)
{
    printf("ERROR - Data and Index Files Created by Different Application Versions\n");
    fclose(fpData);
    fclose(fpIndex);
    return 1;
}

// check that record count of data file matches index header
if (indexHeader.recCount != recCount)
{
    printf("ERROR - Data and Index Files Have a Record Count Mismatch\n");
    fclose(fpData);
    fclose(fpIndex);
    return 1;
}

printf("Data File Read in Indexed Order (Last Name)\n\n");
printf("%-20s%-20s%6s\n", "First Name", "Last Name", "Age");
printf("----------------------------------------------\n");

// read first index record
readCount = fread(&indexTemp, sizeof(IndexRecord), 1, fpIndex);

// continue processing all index records
while ((!feof(fpIndex)) && (readCount == 1))
{
    // seek record in data file based on file position
    //  stored in previously read index record
    if (fseek(fpData, indexTemp.filePos, SEEK_SET) != 0)
    {
        printf("Seek Error\n");
        fclose(fpData);
        fclose(fpIndex);
        return 1;
    }

    // read data record and output
    fread(&temp, sizeof(Person), 1, fpData);
    printf("%-20s%-20s%6d\n", temp.firstName, temp.lastName, temp.age);

    // read next index record
    readCount = fread(&indexTemp, sizeof(IndexRecord), 1, fpIndex);
}

// close files and go home
fclose(fpData);
fclose(fpIndex);

_getch();
return 0;
}

long fileSize(FILE* input)
{
    long orgPos;
    long startPos;
    long endPos;

    orgPos = ftell(input);          // save orig file position
    rewind(input);
    startPos = ftell(input);
    fseek(input, 0, SEEK_END);
    endPos = ftell(input);
    fseek(input, orgPos, SEEK_SET); // restore orig position

    return(endPos - startPos);
}

