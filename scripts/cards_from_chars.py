from pymongo import MongoClient, collection
import random
import string

def generate_random_code(length):
    return ''.join(random.choices(string.ascii_uppercase + string.digits, k=length))

def check_entries_with_value(collection : collection, field_name, value):
    cursor = collection.find({})
    for document in cursor:
        if document[field_name] == value:
            return True
    return False
    
    # return any(entry.get(field_name) == value for entry in collection.entries)

def get_random_unique_code(collection : collection):
    max_size_attempts = 5
    code_length = 3
    
    while True:
        for _ in range(max_size_attempts):
            test_code = "ca" + generate_random_code(code_length)
            
            code_exists = check_entries_with_value(collection, "uuid", test_code)

            if not code_exists:
                return test_code

        max_size_attempts *= 2  # Double the attempts for the next length
        code_length += 1  # Increase code length

def getTierFromName(name :str) -> int:
    if "common" in name:
        return 0
    elif "rare" in name:
        return 1
    elif "epic" in name:
        return 2
    elif "illustration" in name:
        return 3
    elif "set_1" in name:
        return 4
    
def getMultipleFromTier(tier : int):
    if tier == 0:
        return 1
    elif tier == 1:
        return 2
    elif tier == 2:
        return 3
    elif tier == 3:
        return 5
    elif tier == 4:
        return 7
    elif tier == 5:
        return 11


IMAGE_LIST = ["common.png", "rare.png", "epic.png", "illustration.gif", "set_1.gif"]



client = MongoClient("mongodb://localhost:27017/")
db = client["hakari"]
char_collection = db["characters"]
card_collection = db["cards"]



for character in char_collection.find():
    char_id = character["_id"]
    rank = character["rank"]
    
    for image in IMAGE_LIST:
        tier = getTierFromName(image)
        card_entry = {
            "uuid" : get_random_unique_code(card_collection),
            "name" : character["name"],
            "character_id" : char_id,
            "set_id" : "1",
            "tier" : tier,
            "image_url" : image,
            "num_acquired" : 0,
        }

        card_collection.insert_one(card_entry)

        print(f"inserted {card_entry}")