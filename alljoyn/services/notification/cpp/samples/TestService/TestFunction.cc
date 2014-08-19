/******************************************************************************
 * Copyright (c) 2013-2014, AllSeen Alliance. All rights reserved.
 *
 *    Permission to use, copy, modify, and/or distribute this software for any
 *    purpose with or without fee is hereby granted, provided that the above
 *    copyright notice and this permission notice appear in all copies.
 *
 *    THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 *    WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 *    MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 *    ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 *    WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 *    ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 *    OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 ******************************************************************************/

#include <iostream>
#include "TestFunction.h"

using namespace qcc;

TestFunction::TestFunction() : activateTest(0)
{
}

TestFunction::~TestFunction()
{
}

bool TestFunction::checkRequiredParams(std::map<qcc::String, qcc::String>& params)
{
    if (requiredParams.size() == 0) {
        return true;
    }

    for (std::vector<qcc::String>::const_iterator reqParams_it = requiredParams.begin(); reqParams_it != requiredParams.end(); ++reqParams_it) {
        if (params.find(*reqParams_it) == params.end()) {
            std::cout << "Missing required parameter " << reqParams_it->c_str() << std::endl;
            std::cout << usage.c_str() << std::endl;
            return false;
        }
    }
    return true;
}

void TestFunction::checkOptionalParams(std::map<qcc::String, qcc::String>& params)
{
    for (std::map<qcc::String, qcc::String>::const_iterator params_it = params.begin(); params_it != params.end(); ++params_it) {
        if (find(optionalParams.begin(), optionalParams.end(), params_it->first) == optionalParams.end()) {
            if (find(requiredParams.begin(), requiredParams.end(), params_it->first) == requiredParams.end()) {
                // Element is NOT in either vector so let the user know
                std::cout << "Parameter " << params_it->first.c_str() << " is not a valid parameter for " <<
                functionName.c_str() << ". Ignoring value" << std::endl;
            }
        }
    }
}

