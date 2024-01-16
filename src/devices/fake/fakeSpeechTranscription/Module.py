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

class Pet:
    def __init__(self, race, age, mood):
        self.race = race
        self.age = age
        self.mood = mood
        print ('Created a ' , self.race, ' Pet with ', self.age, ' years')

    def set_race(self, new_race):
        self.race = new_race
        print ('Setting a new race: ', self.race)
        return self.race
    
    def set_age(self, new_age):
        self.age = new_age
        print ('Setting new age: ', self.age)
        return self.age

    def set_mood(self, new_mood):
        self.mood = new_mood
        print ('Setting new mood: ', self.mood)
