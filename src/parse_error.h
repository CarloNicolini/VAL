// Copyright 2019 - University of Strathclyde, King's College London and Schlumberger Ltd
// This source code is licensed under the BSD license found in the LICENSE file in the root directory of this source tree.

#ifndef PARSE_ERROR_H
#define PARSE_ERROR_H

#include <cstdio>
#include <iostream>
#include <list>
#include <string>

#include "ptree.h"

using std::cout;
using std::list;
using std::string;

extern int line_no;             // Line number global
extern char *current_filename;  // file global

namespace VAL {

enum error_severity { E_WARNING, E_FATAL };

class parse_error {
private:
    error_severity severity;
    char *filename;
    int line;
    string description;

public:
    parse_error(error_severity s, const string &d)
        : severity(s), line(line_no), description(d) {
        filename = current_filename;
    };

    // describe error
    void report() {
        cout << filename << ": line: " << line << ": ";

        if (severity == E_FATAL)
            cout << "Error: ";
        else
            cout << "Warning: ";

        cout << description << '\n';
    };
};

// It seems to be more sensible to keep errors and warnings together in the
// same list, as we want to output them in the same order that they were
// found.

class parse_error_list : public list< parse_error * > {
public:
    int errors;
    int warnings;

    parse_error_list() : errors(0), warnings(0) {};

    ~parse_error_list() {
        for (iterator i = begin(); i != end(); ++i) delete (*i);
    };

    // parse_error_list is reponsible for creating and
    // destroying parse_error objects,
    void add(error_severity sev, const string &description) {
        //	Use yacc globals to retrieve file and line number;
        push_back(new parse_error(sev, description));

        if (sev == E_WARNING)
            ++warnings;
        else
            ++errors;
    };

    void report() {
        cout << "\nErrors: " << errors << ", warnings: " << warnings << '\n';

        for (iterator i = begin(); i != end(); ++i) (*i)->report();
    };
};

};  // namespace VAL

#endif /* PARSE_ERROR_H */
