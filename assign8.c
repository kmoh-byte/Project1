
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
} KeyType;

typedef struct {
    unsigned int acctNum; // account number
    char lastName[15]; // account last name
    char firstName[10]; // account first name
    int  age;
    double balance; // account balance
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
    int		writeCount;
    int		dataRecordCount;
    int		indexRecordCount;
    long	filePos;

    IndexHeader	indexHeader;
    IndexRecord* indexRecords;
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

    // calculate record count in data file
    dataRecordCount = fileSize(fpData) / sizeof(Person);

    indexRecords = (IndexRecord*)calloc(dataRecordCount, sizeof(IndexRecord));

    indexHeader.idxKey = indexKey;
    strcpy(indexHeader.appName, APP_NAME);
    indexHeader.recCount = dataRecordCount;

    rewind(fpData);
    filePos = ftell(fpData);
    indexRecordCount = 0;

    readCount = fread(&temp, sizeof(Person), 1, fpData);

    while (!feof(fpData) && (readCount == 1))
    {
        // populate index record from data record
        // would need to change last argument accordingly in strcpy invocation below if
        //       sorting by another field
        strcpy(indexRecords[indexRecordCount].key.last, temp.lastName);
        indexRecords[indexRecordCount].filePos = filePos;

        // added a record - increment to next element
        indexRecordCount++;

        // store file pos before next read advances it
        filePos = ftell(fpData);

        // read next record
        readCount = fread(&temp, sizeof(Person), 1, fpData);
    }

    qsort(indexRecords, indexRecordCount, sizeof(IndexRecord), compare);

    printf("Index Header Size: %d\nIndex Record Size: %d\n\n"
        , sizeof(IndexHeader), sizeof(IndexRecord));

    printf("Creating Index File\n");

    writeCount = fwrite(&indexHeader, sizeof(IndexHeader), 1, fpIndex);

    // Write Index Records to file
    writeCount = fwrite(indexRecords, sizeof(IndexRecord), indexRecordCount, fpIndex);

    printf("%d Records Processed\n", writeCount);

    printf("\nFileSize: %d\n\n", fileSize(fpIndex));

    free(indexRecords);

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

int compare(const void* left, const void* right)
{
    // cast pointer parameters to IndexRecord
    IndexRecord* pLeft = (IndexRecord*)left;
    IndexRecord* pRight = (IndexRecord*)right;

    // would need to be updated if sorting on another key
    return strcmp((pLeft->key.last), (pRight->key.last));
}




  

   // if ((accountsDat = fopen(accountsDatFile, "rb+")) == NULL) {
    //    printf("File could not be opened.");
      //  return 1;
   // }

    // open index file for writing
   // if ((accountsIdx = fopen(accountsIdxFile, "wb")) == NULL)
    //{
       // printf("ERROR - can not open/create index file.\n");
        //return 1;
   // }


  //  return 0;
//}

