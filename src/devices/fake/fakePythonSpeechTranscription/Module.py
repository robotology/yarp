#!/usr/bin/env python

# SPDX-FileCopyrightText: 2024-2024 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

def open():
    print("Calling open() function")
    return True

def close():
    print("Calling close() function")
    return True

class SpeecTranscriptor:
    def __init__(self, lang, verbose):
        self.lang = lang
        self.verbose = verbose
        print ('Created a SpeecTranscriptor with language: ' , self.lang, ' verbose: ', self.verbose)

    def set_language(self, new_lang):
        self.lang = new_lang
        print ('Setting Language: ', self.lang)
        return True

    def get_language(self):
        print ('Returning Language: ', self.lang)
        return self.lang

    def set_verbose(self, verbose):
        self.verbose = verbose
        print ('Setting verbose: ', self.verbose)
        return True

    def get_verbose(self):
        print ('Getting verbose: ', self.verbose)
        return self.verbose
