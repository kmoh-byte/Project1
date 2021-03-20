// indexedread.c
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <conio.h>

#define APP_NAME "Week09Demo"

typedef enum { FIRST, LAST, YEAR } IndexKey;

typedef union
{
    char	first[30];
    char	last[30];
    int		age;
} KeyType;

typedef struct
{
    char	firstName[30];
    char	lastName[30];
    int		age;
} Person;

typedef struct
{
    KeyType		key;
    long		filePos;
} IndexRecord;

typedef struct
{
    IndexKey	idxKey;
    char		appName[20];
    int			recCount;
} IndexHeader;


IndexKey indexKey;

long fileSize(FILE* input);

int main(void)
{
    FILE* fpData;
    FILE* fpIndexLastName;
    int		readCount;
    int		recCount;

    Person temp = { "","", 0 };

    IndexHeader idxHeader;
    IndexRecord idxTemp;

    // open data file for reading
    if ((fpData = fopen("people.dat", "rb")) == NULL)
    {
        printf("File could not be opened.\n");
        return 1;
    }

    // open index file for reading
    if ((fpIndexLastName = fopen("lastname.idx", "rb")) == NULL)
    {
        printf("ERROR - can not open index file.\n");
        return 1;
    }

    // calculate record count in data file
    recCount = fileSize(fpData) / sizeof(Person);

    // read header record in index file
    readCount = fread(&idxHeader, sizeof(IndexHeader), 1, fpIndexLastName);

    // check that application versions match
    if (strcmp(idxHeader.appName, APP_NAME) != 0)
    {
        printf("ERROR - Data and Index Files Created by Different Application Versions\n");
        fclose(fpData);
        fclose(fpIndexLastName);
        return 1;
    }

    // check that record count of data file matches index header
    if (idxHeader.recCount != recCount)
    {
        printf("ERROR - Data and Index Files Have a Record Count Mismatch\n");
        fclose(fpData);
        fclose(fpIndexLastName);
        return 1;
    }

    printf("Data File Read in Indexed Order (Last Name)\n\n");
    printf("%-20s%-20s%6s\n", "First Name", "Last Name", "Age");
    printf("----------------------------------------------\n");

    // read first index record
    readCount = fread(&idxTemp, sizeof(IndexRecord), 1, fpIndexLastName);

    // continue processing all index records
    while ((!feof(fpIndexLastName)) && (readCount == 1))
    {
        // seek record in data file based on file position
        //  stored in previously read index record
        if (fseek(fpData, idxTemp.filePos, SEEK_SET) != 0)
        {
            printf("Seek Error\n");
            fclose(fpData);
            fclose(fpIndexLastName);
            return 1;
        }

        // read data record and output
        fread(&temp, sizeof(Person), 1, fpData);
        printf("%-20s%-20s%6d\n", temp.firstName, temp.lastName, temp.age);

        // read next index record
        readCount = fread(&idxTemp, sizeof(IndexRecord), 1, fpIndexLastName);
    }

    // close files and go home
    fclose(fpData);
    fclose(fpIndexLastName);

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
