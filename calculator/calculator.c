#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "../util/vector.c"
#include "../util/deque.c"
#include "parsing.c"
#include "../error.h"

#define EXPR_LIMIT 150

int evalRevPolish(deque* revPolish);

int calculate(char* expr){
	
	deque* infix = initDeque();

	int i = 0;
	char recent = 'o';
	while(expr[i] != '\0'){
		if(expr[i] == ' '){
			i++;
			continue;
		}
		if(expr[i] == '-'){
			if(recent == 'o'){
				if(i-1 >= 0 && expr[i-1] == ')'){
					push_back_deque(infix,parseOperator(expr[i]));
					recent = 'o';
					i++;
					continue;
				}
				if(!notOp(expr[i+1])){
					error("Invalid Operator Sequence");
				}
				push_back_deque(infix,parseNum(strtol(expr + i,NULL,0)));
				i++;
				while(notOp(expr[i]))i++;
				recent = 'd';
				continue;
			} else {
				push_back_deque(infix,parseOperator(expr[i]));
				recent = 'o';
			}
		} else if(notOp(expr[i])){
			push_back_deque(infix,parseNum(strtol(expr + i,NULL,0)));
			while(notOp(expr[i]))i++;
			recent = 'd';
			continue;
		} else {
			push_back_deque(infix,parseOperator(expr[i]));
			recent = 'o';
		}
		i++;
	}

	deque* postfix = shuntingYard(infix);

	int result = evalRevPolish(postfix);

	delDeque(infix);
	delDeque(postfix);

	return result;
}