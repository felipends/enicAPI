#ifndef FUNC_DELETE_VECTOR_H
#define FUNC_DELETE_VECTOR_H

template<class T>
void deleteVector(vector<T*> *vec) {
    for (auto it = vec->begin(); it != vec->end();) {
        auto aux = *it;
        it = vec->erase(it);
        delete aux;
    }
    vec->clear();
    delete vec;
};

template<class T>
void deleteVectorElements(vector<T*> *vec) {
    for (auto it = vec->begin(); it != vec->end();) {
        auto aux = *it;
        it = vec->erase(it);
        delete aux;
    }
    vec->clear();
};

#endif