/******************************************************************************
 * Copyright (c) 2013, AllSeen Alliance. All rights reserved.
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

#ifndef TESTFUNCTION_H_
#define TESTFUNCTION_H_

#include <algorithm>
#include <map>
#include <vector>
#include <qcc/String.h>

/**
 * TestFunction class used to represent on of the possible API calls
 * in the TestService application
 */
class TestFunction {
  public:

    /**
     * Constructor
     */
    TestFunction();

    /**
     * Destructor
     */
    virtual ~TestFunction();

    /**
     * Validate that required params were added
     * @param params
     * @return true/false
     */
    bool checkRequiredParams(std::map<qcc::String, qcc::String>& params);

    /**
     * Check the optional parameters
     * @param params
     */
    void checkOptionalParams(std::map<qcc::String, qcc::String>& params);

    /**
     * The name of the function
     */
    qcc::String functionName;

    /**
     * The usage string
     */
    qcc::String usage;

    /**
     * The required Parameters
     */
    std::vector<qcc::String> requiredParams;

    /**
     * The optional Parameters
     */
    std::vector<qcc::String> optionalParams;

    /**
     * Preconditions - steps that need to be taken before this API call
     */
    std::vector<qcc::String> requiredSteps;

    /**
     * functionPointer to function that will execute the API call
     * @param params
     * @return
     */
    bool (*activateTest)(std::map<qcc::String, qcc::String>& params);
};

#endif /* TESTFUNCTION_H_ */
