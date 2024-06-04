void swap(char **p, char **q){
  char* t = *p; 
       *p = *q; 
       *q = t;
}
int main(){
      char a1;
      char b1; 
      char *a;
      char *b;
      a = &a1;
      b = &b1;
      swap(&a,&b);
}
