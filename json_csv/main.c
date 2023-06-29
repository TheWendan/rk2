#include "cJSON.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>


void strStack(char **s, char value) //Добавление символа в строку
{
    int len = strlen(*s);
    char *buffer = (char *)malloc((len + 2) * sizeof(char));
    for (int i = 0; i < len; i++)
    {
        buffer[i] = (*s)[i];
    }
    buffer[len] = value;
    buffer[len + 1] = '\0';
    free(*s);
    *s = buffer;
}

char * EnLine() //Считывание строки произвольной длины из консоли
{
    char c;
    char *str = (char*)calloc(1,sizeof(char));
    while((c = getchar()) != '\n')
        strStack(&str,c);
    return str;
}

void FormColumns(cJSON *head,FILE *f) // создание столбцов
{
    int flag = 0;
    if(head->child !=NULL)
    {
        flag = 1;
        FormColumns(head->child,f);
    }
    if(head->next!=NULL)
        FormColumns(head->next,f);
    if(!flag)
        fprintf(f,"%s,",head->string);
}

void FormStrs(cJSON *head,FILE *f) // заполнение строчек
{
    int flag = 0;
    if(head->child !=NULL)
    {
        flag = 1;
        FormStrs(head->child,f);
    }
    if(head->next!=NULL)
        FormStrs(head->next,f);
    if(!flag)
    {
        if(head->type == cJSON_String)
            fprintf(f,"%s,",head->valuestring);
        else if(head->type == cJSON_Number)  
            fprintf(f,"%g,",head->valuedouble);
    }
}

int main()
{
    puts("Filename:");
    char *s = EnLine();

    FILE *f = fopen(s,"r");
    free(s);
    if(f == NULL)
    {
        puts("File is not exist!");
        return 0;
    }
    
    char c;
    char *fileStr = (char *)calloc(1,sizeof(char)); // полное считыввание файла
    while ((c = fgetc(f)) != EOF)
        strStack(&fileStr,c);
    fclose(f);
    cJSON *json = cJSON_Parse(fileStr); // Создание массива элементов типа cjson на основе файла json

    if(json == NULL)
    {
        puts("Error parcing!");
        return 0;
    }

    if(!cJSON_IsArray(json)) // проверка на корректность 
    {
        puts("Error! JSON is incorrect!");
        return 0;
    }

    int size = cJSON_GetArraySize(json); // размер массива (строка 82)
    printf("size = %d\n", size);
    cJSON **data = (cJSON **) malloc(size * sizeof(cJSON*)); 
    cJSON *buffer; 
    for (int i = 0; i < size; i++) // Перебор массива 
    {
        buffer = cJSON_GetArrayItem(json, i);
        data[i] = buffer;
    }

    f = fopen("output.csv","w");

    FormColumns(data[0]->child,f); // создание первой строчки csv

    fputc('\n',f); // переход на новую строчку

    // создание всех остальных строчек
    for(int i = 0;i<size;i++)
    {
        FormStrs(data[i]->child,f); 
        fputc('\n',f);
    } 
    fclose(f);
    puts("Success!");
    return 0;
}