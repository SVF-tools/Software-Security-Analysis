struct S{
int* f1;
int* f2;
};
int main(){
struct S s;
int a1, a2;
s.f1 = &a1;
s.f2 = &a2;
int* p = s.f1;
int* q = s.f2;
}
