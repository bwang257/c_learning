/**
 * Note: The returned array must be malloced, assume caller calls free().
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>


typedef struct node node;

struct node {
    char* curr_str;
    int l; 
    int b;
    int sz;
    node* next;
};


// recursive func to take it the node, and then add on ( and or )
// 
void get_next(node* n, char add, const int* max, size_t* len){
    int num = *max;
    n->curr_str[n->sz] = add;
    n->sz += 1;
    if (add == '('){
        n->b += 1;
        n->l += 1;
    } else n->b -= 1;

    // if b= 0, you have to add a left
    // if b = n, you have to add rights
    // otherwise you can do both
    if (n->l == num){
        // guaranteed to happen before end of string
        for (size_t i = n->sz; i < (size_t)num *2; ++i) n->curr_str[i] = ')';
        n->curr_str[2*num] = '\0';
    } else if (n->b == 0){
        get_next(n, '(', max, len); 
    } else {
        node* new_n = malloc(sizeof(node)); 
        if (new_n == NULL){
            puts("MALLOC failed\n");
            return;
        }
        char* s = malloc(2*num + 1);
        if (s == NULL){
            puts("MALLOC failed\n");
            free(new_n);
            return;
        }
        *len += 1;
        memcpy(s, n->curr_str, n->sz);
        new_n->curr_str = s;
        new_n->next = n->next;
        new_n->sz = n->sz;
        new_n->b = n->b; 
        new_n->l = n->l;
        n->next = new_n; 

        get_next(n, '(', max, len);
        get_next(new_n, ')', max, len);
    }
}

char** generateParenthesis(int n, int* returnSize) {
    // at each "step", you either add a ( or you add a )
    // if the number of ( = n, then you just add
    node* head = malloc(sizeof(node));
    if (head == NULL) return NULL;
    
    char* s = malloc(2*n + 1);
    if (s == NULL){
        free(head);
        return NULL;
    }

    head->curr_str = s;
    head->b = 0;
    head->l = 0;
    head->sz = 0;
    head->next = NULL;
    size_t len =1;
    get_next(head, '(', &n, &len);

    char** ans = malloc(sizeof(char*) * len);
    if (ans == NULL){
        puts("MALLOC failed\n");
        return NULL;
    }
    size_t i = 0;
    while (head){
        ans[i] = head->curr_str;
        i += 1;
        node* temp  = head->next;
        free(head);
        head = temp;
    }

    *returnSize = (int)len;

    return ans;
}


int main(){
  int returnSize = 0;
  char** ans = generateParenthesis(3, &returnSize);
  puts("[");
  for (int i = 0; i < returnSize; ++i){
    if (i == returnSize - 1) printf("%s", ans[i]);
    else printf("%s, ", ans[i]);
    free(ans[i]);
  }
  puts("]\n");
  free(ans);
}