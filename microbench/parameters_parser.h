//
// Created by Ravil Galiev on 03.03.2023.
//

#ifndef SETBENCH_PARAMETERS_PARSER_H
#define SETBENCH_PARAMETERS_PARSER_H

#include "common.h"
#include "binding.h"
#include "errors.h"
#include "parse_argument.h"

//template<typename K>
class ParametersParser {
protected:
//    size_t point;
//    size_t argc;
//    char **argv;
    ParseArgument *args;

    Parameters *parameters;
//    KeyGeneratorBuilder<K> keyGeneratorBuilder;

    ParametersParser() = default;

//    virtual void parseArg() {
//        if (strcmp(args->getCurrent(), "-i") == 0) {
//            parameters->INS_FRAC = atof(args->getNext()) * 100;
//        } else if (strcmp(args->getCurrent(), "-d") == 0) {
//            parameters->DEL_FRAC = atof(args->getNext()) * 100;
//        } else if (strcmp(args->getCurrent(), "-insdel") == 0) {
//            parameters->INS_FRAC = atof(args->getNext()) * 100;
//            parameters->DEL_FRAC = atof(args->getNext()) * 100;
//        } else if (strcmp(args->getCurrent(), "-rq") == 0) {
//            parameters->RQ = atof(args->getNext());
//        } else if (strcmp(args->getCurrent(), "-rqsize") == 0) {
//            parameters->RQSIZE = atoi(args->getNext());
//        } else if (strcmp(args->getCurrent(), "-k") == 0) {
//            parameters->MAXKEY = atoi(args->getNext());
//            if (parameters->MAXKEY < 1) {
//                setbench_error("key range cannot contain fewer than 1 key");
//            }
//        } else if (strcmp(args->getCurrent(), "-nrq") == 0) {
//            parameters->RQ_THREADS = atoi(args->getNext());
//        } else if (strcmp(args->getCurrent(), "-nwork") == 0) {
//            parameters->WORK_THREADS = atoi(args->getNext());
//        } else if (strcmp(args->getCurrent(), "-nprefill") == 0) { // num threads to prefill with
//            parameters->PREFILL_THREADS = atoi(args->getNext());
//        } else if (strcmp(args->getCurrent(), "-prefill-mixed") == 0) { // prefilling type
//            parameters->PREFILL_TYPE = PrefillType::PREFILL_MIXED;
//        } else if (strcmp(args->getCurrent(), "-prefill-insert") == 0) { // prefilling type
//            parameters->PREFILL_TYPE = PrefillType::PREFILL_INSERT;
//        } else if (strcmp(args->getCurrent(), "-prefill-hybrid") == 0) { // prefilling type
//            parameters->PREFILL_TYPE = PrefillType::PREFILL_HYBRID;
//        } else if (strcmp(args->getCurrent(), "-prefill-hybrid-min-ms") == 0) {
//            parameters->PREFILL_HYBRID_MIN_MS = atoi(args->getNext());
//        } else if (strcmp(args->getCurrent(), "-prefill-hybrid-max-ms") == 0) {
//            parameters->PREFILL_HYBRID_MAX_MS = atoi(args->getNext());
//        } else if (strcmp(args->getCurrent(), "-prefillsize") == 0) {
//            parameters->DESIRED_PREFILL_SIZE = atol(args->getNext());
//        } else if (strcmp(args->getCurrent(), "-t") == 0) {
//            parameters->MILLIS_TO_RUN = atoi(args->getNext());
//        } else if (strcmp(args->getCurrent(), "-non-shuffle") == 0) {
//            parameters->isNonShuffle = true;
//        } else if (strcmp(args->getCurrent(), "-pin") == 0) { // e.g., "-pin 1.2.3.8-11.4-7.0"
//            binding_parseCustom(args->getNext()); // e.g., "1.2.3.8-11.4-7.0"
//            std::cout << "parsed custom binding: " << args->getCurrent() << std::endl;
//        } else if (parameters->threadLoopParameters->parseArg(args)) {
//            std::cout << "bad argument: " << args->getCurrent() << "\nindex: " << args->pointer << std::endl;
//            exit(1);
//        }
//    }

public:

    ParametersParser(ParseArgument *_args)
            : args(_args) {
        parameters = new Parameters();
    }

    template<typename K>
    static KeyGeneratorBuilder<K> *parse(ParseArgument *args) {
//        ParseArgument *args = new ParseArgument(argc, argv);
        KeyGeneratorBuilder<K> *keyGeneratorBuilder = parseThreadLoop<K>(args);

        while (args->hasNext()) {
            keyGeneratorBuilder->parameters->parseArg(args);
            args->next();
        }
        keyGeneratorBuilder->parameters->build();
        return keyGeneratorBuilder;
    }

//    virtual void parse() {
//        while (args->hasNext()) {
//            parseArg();
//            args->next();
//        }
//    }

    template<typename K>
    static KeyGeneratorBuilder<K> *parseKeyGenerator(ParseArgument *args);

    template<typename K>
    static KeyGeneratorBuilder<K> *parseThreadLoop(ParseArgument *args);

//
//    ParametersParser *getParser(size_t _argc, char **_argv) {
//        return new ParametersParser(_argc, _argv);
//    }

    virtual ~ParametersParser() {
        delete args;
    }

};

#include "key_generators/parameters/key_generator_parameters_impls.h"

#include "key_generators/builder/key_generator_builder_impls.h"
#include "thread_loops/thread_loop_parameters_impls.h"

template<typename K>
KeyGeneratorBuilder<K> *ParametersParser::parseKeyGenerator(ParseArgument *args) {
//    ParametersParser *parametersParser = nullptr;
//    Parameters *parameters;

    KeyGeneratorBuilder<K> *keyGeneratorBuilder = nullptr;
    if (strcmp(args->getCurrent(), "-skewed-sets") == 0) {
        SkewedSetsParameters* parameters = new SkewedSetsParameters();
        keyGeneratorBuilder = new SkewedSetsKeyGeneratorBuilder<K>(parameters);
    } else if (strcmp(args->getCurrent(), "-creakers-and-wave") == 0) {
        CreakersAndWaveParameters *parameters = new CreakersAndWaveParameters();
        keyGeneratorBuilder = new CreakersAndWaveKeyGeneratorBuilder<K>(parameters);
    } else if (strcmp(args->getCurrent(), "-temporary-skewed") == 0
               || strcmp(args->getCurrent(), "-temp-skewed") == 0) {
        TemporarySkewedParameters* parameters = new TemporarySkewedParameters();
        keyGeneratorBuilder = new TemporarySkewedKeyGeneratorBuilder<K>(parameters);
    } else if (strcmp(args->getCurrent(), "-leaf-insert") == 0) {
//            parameters = new Parameters(_argc, _argv);
//            parameters->keygenType = KeyGeneratorType::LEAF_INSERT;
//            parameters->point = ++point;
//
//            keyGeneratorBuilder = new LeafInsertKeyGeneratorBuilder(parameters);
    } else if (strcmp(args->getCurrent(), "-leafs-handshake") == 0) {
//            parameters = new LeafsHandshakeParameters(_argc, _argv);
//            parameters->keygenType = KeyGeneratorType::LEAFS_HANDSHAKE;
//            parameters->point = ++point;
//
//            keyGeneratorBuilder = new LeafsHandshakeKeyGeneratorBuilder(parameters);
    } else if (strcmp(args->getCurrent(), "-leafs-extension-handshake") == 0) {
//            parameters = new LeafsHandshakeParameters(_argc, _argv);
//            parameters->keygenType = KeyGeneratorType::LEAFS_EXTENSION_HANDSHAKE;
//            parameters->point = ++point;
//
//            keyGeneratorBuilder = new LeafsExtensionHandshakeKeyGeneratorBuilder(parameters);
    } else {
        SimpleParameters *parameters = new SimpleParameters();
        return new SimpleKeyGeneratorBuilder<K>(parameters);
    }

    args->next();
    return keyGeneratorBuilder;
}

template<typename K>
KeyGeneratorBuilder<K> *ParametersParser::parseThreadLoop(ParseArgument *args) {
    KeyGeneratorBuilder<K> *keyGeneratorBuilder;
    if (strcmp(args->getCurrent(), "-temp-oper") == 0
        || strcmp(args->getCurrent(), "-temporary-operation") == 0) {
        keyGeneratorBuilder = parseKeyGenerator<K>(args->next());
        keyGeneratorBuilder->parameters->threadLoopParameters = new TemporaryOperationThreadLoopParameters();
    } else {
        keyGeneratorBuilder = parseKeyGenerator<K>(args);
        keyGeneratorBuilder->parameters->threadLoopParameters = new DefaultThreadLoopParameters();
    }
    return keyGeneratorBuilder;
}

#endif //SETBENCH_PARAMETERS_PARSER_H
