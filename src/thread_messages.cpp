#include <iostream>
#include <pthread.h>
#include <unistd.h>

void * print_hello(void * arg){
    std::cout<< "hola desde este hilo" << std::endl;
    return nullptr;
}

void * print_world(void * arg){
    std::cout << "hola desde otro hilo" << std::endl;
    return nullptr;
}

int main(){
    pthread_t t1, t2;
    pthread_create(&t1, nullptr, print_hello, nullptr);
    pthread_create(&t2, nullptr, print_world, nullptr);

    pthread_join(t1, nullptr);
    pthread_join(t2, nullptr);

    std::cout << "adios! soy el hilo principal" << std::endl;
    return 0;
}