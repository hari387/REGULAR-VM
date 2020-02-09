#pragma once

#include <stdlib.h>
#include <string.h>
#include "../error.h"
#include "../util/deque.c"

//{0 is +, 1 is -, 2 is *, 3 is /, 4 is (, 5 is ) }

int notOp(char c){
	if(isdigit(c) || isalpha(c)){
		return 1;
	} return 0;
}

int isNumber(char* str){
	int i = 0;
	if(strcmp(str,"-") == 0)return 0;
	while(str[i] != '\0'){
		if(!notOp(str[i])){
			if(str[i] != '-' || i != 0){
				return 0;
			}
		}
		i++;
	}return 1;
}

Token parseOperator(char op){
	if(op == '+'){
		return ((Token){0,0});
	}else if(op == '-'){
		return ((Token){1,0});
	}else if(op == '*'){
		return ((Token){2,0});
	}else if(op == '/'){
		return ((Token){3,0});
	}else if(op == '('){
		return ((Token){4,0});
	}else if(op == ')'){
		return ((Token){5,0});
	}
	printf("%c\n",op);
	errExit("Invalid Operator");
	return ((Token){0,1});
}

Token parseNum(int num){
	return ((Token){num,1});
}

Token parseToken(char* tok){
	if(isNumber(tok)){
		return ((Token){strtol(tok,NULL,0),1});
	}else if(strcmp(tok,"+") == 0){
		return ((Token){0,0});
	}else if(strcmp(tok,"-") == 0){
		return ((Token){1,0});
	}else if(strcmp(tok,"*") == 0){
		return ((Token){2,0});
	}else if(strcmp(tok,"/") == 0){
		return ((Token){3,0});
	}else if(strcmp(tok,"(") == 0){
		return ((Token){4,0});
	}else if(strcmp(tok,")") == 0){
		return ((Token){5,0});
	}
	errExit("Invalid Token");
	return ((Token){0,1});
}

deque* shuntingYard(deque* infix){
	int precedence[4] = {1,1,2,2};
	int leftAssociative[4] = {0,1,0,1};
	deque* ops = initDeque();
	deque* revPolish = initDeque();
	deqNode* curr = infix->head;
	while(curr != NULL){
		if(curr->val.isNum){
			push_back_deque(revPolish,curr->val);
		}else if(curr->val.num != 4 && curr->val.num != 5){
			while(ops->size > 0 
				&& ops->tail->val.num != 4 
				&& (precedence[ops->tail->val.num] > precedence[curr->val.num]
					|| (precedence[ops->tail->val.num] == precedence[curr->val.num] && leftAssociative[ops->tail->val.num]))){

				push_back_deque(revPolish,pop_back_deque(ops));
			}
			push_back_deque(ops,curr->val);
		}else if(curr->val.num == 4){
			push_back_deque(ops,curr->val);
		}else if(curr->val.num == 5){
			while(ops->tail->val.num != 4){
				push_back_deque(revPolish,pop_back_deque(ops));
				if(ops->size == 0){
					errExit("Mismatched Parentheses");
				}
			}
			if(ops->tail->val.num == 4){
				pop_back_deque(ops);
			}
		}
		curr = curr->next;
	}

	while(ops->size > 0){
		push_back_deque(revPolish,pop_back_deque(ops));
	}

	delDeque(ops);

	return revPolish;
}

int evalRevPolish(deque* revPolish){
	deque* stack = initDeque();
	deqNode* curr = revPolish->head;
	if(curr == NULL)return 0;
	while(curr != NULL){
		if(curr->val.isNum){
			push_back_deque(stack,curr->val);
		} else {
			int n1,n2;
			n2 = pop_back_deque(stack).num;
			n1 = pop_back_deque(stack).num;
			switch(curr->val.num){
				case 0:
					push_back_deque(stack,(Token){n1+n2,1});
					break;
				case 1:
					push_back_deque(stack,(Token){n1-n2,1});
					break;
				case 2:
					push_back_deque(stack,(Token){n1*n2,1});
					break;
				case 3:
					if(n2 == 0){
						errExit("Divided By Zero");
					}
					push_back_deque(stack,(Token){n1/n2,1});
					break;
				default:
					errExit("Unknown Operator");
					break;
			}
		}
		curr = curr->next;
	}
	if(stack->size > 1)errExit("Missing Operator");
	int res = stack->head->val.num;
	delDeque(stack);
	return res;
}