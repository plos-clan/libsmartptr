#include"stdio.h"
#include"libsmartptr.hpp"

template<typename T> void test_destroy_function(smart_ptr<T>* p) {
    printf("destroyed %p\n", p->get());
    delete p->get();
}

int main() {
    smart_ptr<int> p1(new int, test_destroy_function);
    smart_ptr<int> p2(new int, test_destroy_function);

    printf("constructed %p\n", p1.get());
    printf("constructed %p\n", p2.get());

    *(p1.get()) = 114;
    *(p2.get()) = 514;

    printf("before assign %p <- %d\n", p1.get(), *(p1.get()));
    printf("before assign %p <- %d\n", p2.get(), *(p2.get()));

    p1 = p2;

    printf("after assign %p <- %d\n", p1.get(), *(p1.get()));
    printf("after assign %p <- %d\n", p2.get(), *(p2.get()));

    return 0;
}