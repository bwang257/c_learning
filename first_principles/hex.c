/*
Preparation for problems centered around hexadecimal/binary
*/

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

void uint32_t_to_other_base(uint32_t n, uint32_t base,  char* str){
  if (base > 16){
    fprintf(stderr, "bases above 16 not supported!\n");
    str[0] = '\0';
    return;
  }

  char lookup[] = "0123456789ABCDEF";
  int idx = 0;
  if (n==0){
    str[idx++] = '0';
  } else {
    while (n > 0){
      str[idx++] = lookup[n % base]; // (n % 16) same as (n & 0xF)
      n /= base; // n = n / 16 same as n >>= 4;
    }
    // need to reverse string now, with idx as size
    int l = 0, r = idx - 1;
    while (l < r){
      char temp = str[l];
      str[l] = str[r];
      str[r] = temp;
      ++l;
      --r;
    }
  }
  str[idx] = '\0';
}

size_t hex_to_dec(char* str){
  int len = 0;
  while (str[len] != '\0') ++len;
  size_t res = 0;
  for (int i = 0; i < len; ++i){
    int new_c;
    if ('0' <= str[i] && str[i]<= '9'){
      new_c = str[i] - '0';
    } else {
      new_c = str[i] - 'A' + 10;
    }
    res = (16 * res) + new_c;
  }
  return res;
}

size_t base_to_dec(char* str, int base){
  if (base > 16){
    fprintf(stderr, "Bases greater than 16 not accomodated!\n");
    return 0;
  }
  int len = 0;
  while (str[len] != '\0') ++len;
  size_t res = 0;
  for (int i = 0; i < len; ++i){
    int new_c;
    if ('0' <= str[i] && str[i]<= '9'){
      new_c = str[i] - '0';
    } else {
      new_c = str[i] - 'A' + 10;
    }
    res = (base * res) + new_c;
  }
  return res;
}



int main(void){
  for (uint32_t i = 0; i < 100; ++i){
    char* s = malloc(sizeof(9)); //  32 bits --> 8 slots for hex
    char* s2 = malloc(sizeof(33)); 
    if (!s){
      puts("malloc failed. exiting\n");
      return -1;
    } else if (!s2){
      free(s);
      puts("malloc failed. exiting\n");
      return -1;
    }
    uint32_t_to_other_base(i, 16, s);
    uint32_t_to_other_base(i, 2, s2);
    printf("0x%-5s", s);
    size_t dec = hex_to_dec(s);
    size_t dec2 = base_to_dec(s2, 2);
    assert(dec == dec2 && dec == i);
    printf(" == %4zu", dec);
    printf("   == %9s\n", s2);
    
    
    free(s);
    free(s2);
  }
  return 0;
}