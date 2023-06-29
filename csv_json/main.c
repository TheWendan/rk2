#include "cJSON.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>


void strStack(char **s, char value) //Добавление символа в любую строку
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

//Считывание строки произвольной длины из консоли 
// Возвращает указатель на эту строчку
char * EnLine() 
{
    char c;
    char *str = (char*)calloc(1,sizeof(char));
    while((c = getchar()) != '\n')
        strStack(&str,c);
    return str;
}


int main()
{
    puts("Enter fileName:");
    char *s = EnLine();
    FILE *f = fopen(s,"r");
    free(s);

    if(f == NULL)
    {
        puts("File is not exist!");
        return 0;
    }

    char c;
    char *bufferStr = (char *)calloc(1,sizeof(char)); // содержимое заголовков
    char fileStr[1024]; // содержимое полей
    int size = 0; // количесвто столбцов

    while((c = fgetc(f)) != '\n') // считывание заголовка изначального файла
    {
        if(c == ',')
            size++; // подсчет столбцов
        strStack(&bufferStr,c);
    }

    FILE *f1 = fopen("output.json","w");

    char **Heads = (char **)calloc(size,sizeof(char*)); // массив заголовков
    char *buffer = strtok(bufferStr,","); // разделение заголовков по запятой

    for(int i = 0;i<size;i++) // проход заголовков столбцов
    {
        Heads[i] = buffer; // заголовки
        buffer = strtok(NULL,",");
    }
    
    fputs("[\n\t", f1);
    int counter = 0;
    int flag = 1;
    while(fgets(fileStr, 1024, f) != NULL) // считывание построчно csv файла
    {
        if(!flag) // условия расстановки фигурных скобок
        {
            fputs("},", f1);
        }
        fputs("\n{\n",f1);
        buffer = strtok(fileStr, ","); // разделение содержания файла (кроме заголовков) по запятой
        while(buffer != NULL)
        {
            if(counter<size && buffer != NULL)
                fprintf(f1,"\"%s\": \"%s\"",Heads[counter++],buffer); // заполнение поля json'а
            buffer = strtok(NULL, ","); // разделение строчки
            
            if(counter < size)
                fputc(',',f1); // расстановка запятых после строк
            fputc('\n',f1);
        }
        counter = 0; // символы поля
        flag = 0; // фигурные скобоки
    }

    fputs("}\n]\n\t", f1);
    fclose(f);
    fclose(f1);
    return 0;
}