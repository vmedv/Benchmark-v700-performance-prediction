//
// Created by Ravil Galiev on 26.07.2023.
//

#ifndef SETBENCH_GLOBALS_T_H
#define SETBENCH_GLOBALS_T_H

typedef long long test_type;

#ifdef REDIS
    #define VALUE_TYPE test_type
    #define KEY_TO_VALUE(key) key
#else
    #define VALUE_TYPE void *
    #define KEY_TO_VALUE(key) &key /* note: hack to turn a key into a pointer */
#endif

#define DS_ADAPTER_T ds_adapter<test_type, VALUE_TYPE, RECLAIM<>, ALLOC<>, POOL<> >

struct globals_t;

#endif //SETBENCH_GLOBALS_T_H
