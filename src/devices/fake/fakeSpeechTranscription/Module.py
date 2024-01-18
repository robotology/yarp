#!/usr/bin/env python

def open():
    print("Opening python module")
    return True

def close():
    print("Closing python module")
    return True

def get_language(lang):
    print("getting: " + lang)
    return lang

def set_language(lang):
    print("setting: " + lang)
    return lang

class Dummy:
    def __init__(self, lang, age, mood):
        self.lang = lang
        self.age = age
        self.mood = mood
        print ('Created a dummy with lang ' , self.lang, ' Age: ', self.age, ' Mood ', self.mood)

    def set_languag(self, new_lang):
        self.lang = new_lang
        print ('Setting Language: ', self.lang)
        return self.lang
    
    def set_age(self, new_age):
        self.age = new_age
        print ('Setting new age: ', self.age)
        return self.age

    def set_mood(self, new_mood):
        self.mood = new_mood
        print ('Setting new mood: ', self.mood)
