#pragma once

#include <stdlib.h>
#include <stdint.h>

#include "ASMtypedefs.c"

Label* initLabel(char* name,int address){
	Label* label = (Label*)malloc(sizeof(Label));
	label->name = (char*)malloc(strlen(name)+1);
	strcpy(label->name,name);
	label->address = address;
	label->next = NULL;
	return label;
}

int labelHash(char* name){
	int sum = 0;
	int i = 0;
	while(name[i] != '\0'){
		sum += name[i];
		i++;
	}
	return sum%labelTableSize;
}

void insertLabel(Label* label,Label** labelTable){
	int hashedVal = labelHash(label->name);
	Label* iter;
	if(labelTable[hashedVal] == NULL){
		labelTable[hashedVal] = label;
	} else {
		iter = labelTable[hashedVal];
		while(iter->next != NULL){
			iter = iter->next;
		}
		iter->next = label;
	}
	return;
}

Label* retrieveLabel(char* name,Label** labelTable){
	Label* mylabel = labelTable[labelHash(name)];
	while(mylabel != NULL && strcmp(name,mylabel->name) != 0){
		mylabel = mylabel->next;
	}
	return mylabel;
}

void labelParse(FILE* file,Label** labelTable,int* lineNumber){
	char line[256];
	char* token;
	FILE* header;
	while(fgets(line,sizeof(line),file)){
		line[strlen(line)-2] = '\0';
		token = strtok(line," ");
		if(strcmp(token,"label") == 0){
			token = strtok(NULL," ");
			Label* label = initLabel(token,*lineNumber);
			insertLabel(label,labelTable);
			(*lineNumber)++;
		} else if(strcmp(token,"include") == 0){
			token = strtok(NULL," ");
			header = fopen(token,"r");
			labelParse(header,labelTable,lineNumber);
			fclose(header);
		} else if(strcmp(token,"pushc") == 0){
			(*lineNumber)+=4;
		} else if(strcmp(token,"pushr") == 0){
			(*lineNumber)+=3;
		} else if(strcmp(token,"pushret") == 0){
			(*lineNumber)+=5;
		}  else if(strcmp(token,"pop") == 0){
			(*lineNumber)+=3;
		} else {
			(*lineNumber)++;
		}
	}
}