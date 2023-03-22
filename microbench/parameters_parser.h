//
// Created by Ravil Galiev on 03.03.2023.
//

#ifndef SETBENCH_PARAMETERS_PARSER_H
#define SETBENCH_PARAMETERS_PARSER_H

#include "common.h"
#include "binding.h"
#include "errors.h"

//template<typename K>
class ParametersParser {
protected:
    size_t point;
    size_t argc;
    char **argv;

    Parameters *parameters;
//    KeyGeneratorBuilder<K> keyGeneratorBuilder;

    ParametersParser() = default;

    virtual void parseArg() {
        if (strcmp(argv[point], "-i") == 0) {
            parameters->INS_FRAC = atof(argv[++point]) * 100;
        } else if (strcmp(argv[point], "-d") == 0) {
            parameters->DEL_FRAC = atof(argv[++point]) * 100;
        } else if (strcmp(argv[point], "-insdel") == 0) {
            parameters->INS_FRAC = atof(argv[++point]) * 100;
            parameters->DEL_FRAC = atof(argv[++point]) * 100;
        } else if (strcmp(argv[point], "-rq") == 0) {
            parameters->RQ = atof(argv[++point]);
        } else if (strcmp(argv[point], "-rqsize") == 0) {
            parameters->RQSIZE = atoi(argv[++point]);
        } else if (strcmp(argv[point], "-k") == 0) {
            parameters->MAXKEY = atoi(argv[++point]);
            if (parameters->MAXKEY < 1) {
                setbench_error("key range cannot contain fewer than 1 key");
            }
        } else if (strcmp(argv[point], "-nrq") == 0) {
            parameters->RQ_THREADS = atoi(argv[++point]);
        } else if (strcmp(argv[point], "-nwork") == 0) {
            parameters->WORK_THREADS = atoi(argv[++point]);
        } else if (strcmp(argv[point], "-nprefill") == 0) { // num threads to prefill with
            parameters->PREFILL_THREADS = atoi(argv[++point]);
        } else if (strcmp(argv[point], "-prefill-mixed") == 0) { // prefilling type
            parameters->PREFILL_TYPE = PrefillType::PREFILL_MIXED;
        } else if (strcmp(argv[point], "-prefill-insert") == 0) { // prefilling type
            parameters->PREFILL_TYPE = PrefillType::PREFILL_INSERT;
        } else if (strcmp(argv[point], "-prefill-hybrid") == 0) { // prefilling type
            parameters->PREFILL_TYPE = PrefillType::PREFILL_HYBRID;
        } else if (strcmp(argv[point], "-prefill-hybrid-min-ms") == 0) {
            parameters->PREFILL_HYBRID_MIN_MS = atoi(argv[++point]);
        } else if (strcmp(argv[point], "-prefill-hybrid-max-ms") == 0) {
            parameters->PREFILL_HYBRID_MAX_MS = atoi(argv[++point]);
        } else if (strcmp(argv[point], "-prefillsize") == 0) {
            parameters->DESIRED_PREFILL_SIZE = atol(argv[++point]);
        } else if (strcmp(argv[point], "-t") == 0) {
            parameters->MILLIS_TO_RUN = atoi(argv[++point]);
        } else if (strcmp(argv[point], "-non-shuffle") == 0) {
            parameters->isNonShuffle = true;
        } else if (strcmp(argv[point], "-pin") == 0) { // e.g., "-pin 1.2.3.8-11.4-7.0"
            binding_parseCustom(argv[++point]); // e.g., "1.2.3.8-11.4-7.0"
            std::cout << "parsed custom binding: " << argv[point] << std::endl;
        } else {
            std::cout << "bad argument: " << argv[point] << "\nindex: " << point << std::endl;
            exit(1);
        }
    }

public:

    ParametersParser(size_t _argc, char **_argv, size_t _point = 0)
            : argc(_argc), argv(_argv), point(_point) {
        parameters = new Parameters();
    }

    virtual void parse() {
        while (point < argc) {
            parseArg();
            ++point;
        }

        argc = 0;
        argv = nullptr;
        point = 0;
    }


    virtual Parameters *parse(size_t _argc, char **_argv) {
        argc = _argc;
        argv = _argv;

        while (point < argc) {
            parseArg();
            ++point;
        }

        argc = 0;
        argv = nullptr;
        point = 0;
        return parameters;
    }

    template<typename K>
    static std::pair<KeyGeneratorBuilder<K> *, ParametersParser *>
    parseKeyGeneratorType(size_t _argc, char **_argv, size_t point = 0);


    ParametersParser *getParser(size_t _argc, char **_argv) {
        return new ParametersParser(_argc, _argv);
    }

    virtual ~ParametersParser() {
        argv = nullptr;
    }

};

#include "key_generators/parameters/key_generator_parameters_impls.h"
#include "key_generators/builder/key_generator_builder_impls.h"

template<typename K>
std::pair<KeyGeneratorBuilder<K> *, ParametersParser *>
ParametersParser::parseKeyGeneratorType(size_t _argc, char **_argv, size_t point) {
    ParametersParser *parameterParser = nullptr;
//    Parameters *parameters;
    KeyGeneratorBuilder<K> *keyGeneratorBuilder = nullptr;
    if (strcmp(_argv[point], "-skewed-sets") == 0) {
//        parameters = new SkewedSetsParameters();
        parameterParser = new SkewedSetsParametersParser(_argc, _argv, ++point);
//            parameters->keygenType = KeyGeneratorType::SKEWED_SETS;
//        parameterParser->point = ++point;
//        parameterParser->parameters = parameters;

        keyGeneratorBuilder = new SkewedSetsKeyGeneratorBuilder<K>(
                (SkewedSetsParameters *) parameterParser->parameters
        );
    } else if (strcmp(_argv[point], "-creakers-and-wave") == 0) {
//        parameters = new CreakersAndWaveParameters();
        parameterParser = new CreakersAndWaveParametersParser(_argc, _argv, ++point);
//            parameters->keygenType = KeyGeneratorType::CREAKERS_AND_WAVE;
//        parameterParser->point = ++point;
//        parameterParser->parameters = parameters;

        keyGeneratorBuilder = new CreakersAndWaveKeyGeneratorBuilder<K>(
                (CreakersAndWaveParameters *) parameterParser->parameters
        );
    } else if (strcmp(_argv[point], "-temporary-skewed") == 0
               || strcmp(_argv[point], "-temp-skewed") == 0) {
//        parameters = new TemporarySkewedParameters();
        parameterParser = new TemporarySkewedParametersParser(_argc, _argv, ++point);
//            parameters->keygenType = KeyGeneratorType::TEMPORARY_SKEWED;
//        parameterParser->point = ++point;
//        parameterParser->parameters = parameters;

        keyGeneratorBuilder = new TemporarySkewedKeyGeneratorBuilder<K>(
                (TemporarySkewedParameters *) parameterParser->parameters
        );
    } else if (strcmp(_argv[point], "-delete-speed-test") == 0) {
        // todo add workloads
//            parameters = new Parameters(_argc, _argv);
//            parameters->workloadType = WorkloadType::DELETE_SPEED_TEST;
//            parameters->keygenType = KeyGeneratorType::NONE;
//            parameters->numMilliseconds = 0;
//            parameters->point = ++point;
//            parameterParser->parameters = parameters;
//
//            keyGeneratorBuilder = new NoneKeyGeneratorBuilder(parameters);
    } else if (strcmp(_argv[point], "-delete-leafs") == 0) {
        // todo add workloads
//            parameters = new Parameters(_argc, _argv);
//            parameters->workloadType = WorkloadType::DELETE_LEAFS;
//            parameters->keygenType = KeyGeneratorType::NONE;
//                        parameters->point = ++point;
//
//            keyGeneratorBuilder = new NoneKeyGeneratorBuilder(parameters);
    } else if (strcmp(_argv[point], "-leaf-insert") == 0) {
//            parameters = new Parameters(_argc, _argv);
//            parameters->keygenType = KeyGeneratorType::LEAF_INSERT;
//            parameters->point = ++point;
//
//            keyGeneratorBuilder = new LeafInsertKeyGeneratorBuilder(parameters);
    } else if (strcmp(_argv[point], "-leafs-handshake") == 0) {
//            parameters = new LeafsHandshakeParameters(_argc, _argv);
//            parameters->keygenType = KeyGeneratorType::LEAFS_HANDSHAKE;
//            parameters->point = ++point;
//
//            keyGeneratorBuilder = new LeafsHandshakeKeyGeneratorBuilder(parameters);
    } else if (strcmp(_argv[point], "-leafs-extension-handshake") == 0) {
//            parameters = new LeafsHandshakeParameters(_argc, _argv);
//            parameters->keygenType = KeyGeneratorType::LEAFS_EXTENSION_HANDSHAKE;
//            parameters->point = ++point;
//
//            keyGeneratorBuilder = new LeafsExtensionHandshakeKeyGeneratorBuilder(parameters);
    } else if (strcmp(_argv[point], "-temporary-operation") == 0
               || strcmp(_argv[point], "-temp-oper") == 0) {
        // todo add workloads
//
//            keyGeneratorBuilder = parseWorkload(_argc, _argv, ++point );
//            parameters = new TemporaryOperationsParameters(_argc, _argv);
//            parameters->workloadType = WorkloadType::TEMPORARY_OPERATIONS;
//            parameters->keygenType = keyGeneratorBuilder->parameters->keygenType;
//
//            keyGeneratorBuilder.parameters = parameters;
    } else {
//        parameters = new SimpleParameters();
        parameterParser = new SimpleParametersParser(_argc, _argv, point);
//            parameters->keygenType = KeyGeneratorType::SIMPLE_KEYGEN;
//        parameterParser->point = point;
//        parameterParser->parameters = parameters;

        keyGeneratorBuilder = new SimpleKeyGeneratorBuilder<K>(
                (SimpleParameters *) parameterParser->parameters
        );
    }

    return {keyGeneratorBuilder, parameterParser};
}

#endif //SETBENCH_PARAMETERS_PARSER_H
