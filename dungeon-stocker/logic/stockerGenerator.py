import random
import bisect

from . import config


#  Weighted random function
def w_random(rand_table):
    x = random.random() * rand_table["weight_total"]
    i = bisect.bisect(rand_table["weightings"], x)
    return rand_table["choices"][i]


def roll_on_table(config_key):
    count = 0
    desc_str = []

    rand_table = config.TABLES[config_key]["choices"]
    rand_chance = config.CONFIG[config_key]["chance"]
    num_of_rolls = config.CONFIG[config_key]["num"]

    while count < num_of_rolls:
        if random.randint(0, 100) < rand_chance:
            desc_str.append(w_random(config.TABLES[config_key]))
        count += 1

    return desc_str


def create_rooms():
    room_num = 1
    floor_desc = {}
    while room_num <= config.CONFIG["rooms"]:
        floor_desc["Room" + str(room_num)] = {}
        current_room = floor_desc["Room" + str(room_num)]
        if random.randint(0, 100) < config.CONFIG["empty_room_chance"]:
             current_room["empty"] = True
        else:
            current_room["chamber_state"] = roll_on_table("chamber_state")
            current_room["odor"] = roll_on_table("odor")
            current_room["general_features"] = roll_on_table("general_features")
            current_room["obstacle"] = roll_on_table("obstacle")
            current_room["general_furnishings"] = roll_on_table("general_furnishings")
            current_room["mage_furnishings"] = roll_on_table("mage_furnishings")
            current_room["books"] = roll_on_table("books")

            # Traps
            count = 0
            trap_count = 0
            current_room["traps"] = {}
            while count < config.CONFIG["traps"]["num"]:
                if random.randint(0, 100) <= config.CONFIG["traps"]["chance"]:
                    current_room["traps"][trap_count] = {}
                    curren_trap = current_room["traps"][trap_count]
                    curren_trap["severity"] = w_random(config.TABLES["trap_severity"])
                    curren_trap["trigger"] = w_random(config.TABLES["trap_trigger"])
                    curren_trap["effect"] = w_random(config.TABLES["trap_effect"])
                    trap_count += 1
                count += 1

            # Trick
            count = 0
            trick_count = 0
            current_room["tricks"] = {}
            while count < config.CONFIG["tricks"]["num"]:
                if random.randint(0, 100) < config.CONFIG["tricks"]["chance"]:
                    current_room["tricks"][trick_count] = {}
                    curren_trick = current_room["tricks"][trick_count]
                    curren_trick["object"] = w_random(config.TABLES["trick_object"])
                    curren_trick["effect"] = w_random(config.TABLES["trick_effect"])
                    trick_count += 1
                count += 1
        room_num += 1
    return floor_desc


def generate_floor_events():
    floor_events = {}
    floor_events["air"] = w_random(config.TABLES["air"])
    if random.randint(0, 100) < 24:
        floor_events["entry_noise"] = w_random(config.TABLES["noises"])
    return floor_events


def create_floor(thread_id, floors):
    current_floor = thread_id
    while current_floor < config.CONFIG["floors"]:
        desc = {}
        desc["floor_events"] = generate_floor_events()
        desc["rooms"] = create_rooms()

        floors["Floor" + str(current_floor + 1)] = desc
        current_floor += config.THREAD_NUM


"""
Old code for text file formatting

def roll_on_table(desc_prefix, config_key, rand_chance=None):
    rand_table = config.TABLES[config_key]["choices"]
    desc_str = ""

    rand_chance = config.CONFIG[config_key]["chance"]
    num_of_rolls = config.CONFIG[config_key]["num"]
    count = 0
    desc_str = "\t" + desc_prefix + ":\n"
    while count < num_of_rolls:
        if random.randint(0, 100) < rand_chance:
            desc_str += "\t\t" + w_random(config.TABLES[config_key]) + "\n"
        count += 1

    return desc_str


def create_rooms():
    room_num = 1
    floor_desc = ""
    while room_num <= config.CONFIG["rooms"]:
        floor_desc += "\n"
        if random.randint(0, 100) < config.CONFIG["empty_room_chance"]:
            floor_desc += "Room " + str(room_num) + " is an empty room.\n"
        else:
            floor_desc += "Room " + str(room_num) + ":\n"
            floor_desc += roll_on_table("Room state", "chamber_state")
            floor_desc += roll_on_table("Odor", "odor")
            floor_desc += roll_on_table("Room feature", "general_features")
            floor_desc += roll_on_table("Obstacle", "obstacle")
            floor_desc += roll_on_table("Furnishings", "general_furnishings")
            floor_desc += roll_on_table("Mage furnishings", "mage_furnishings")
            floor_desc += roll_on_table("Books", "books")

            # Traps
            count = 0
            while count < config.CONFIG["traps"]["num"]:
                if random.randint(0, 100) < config.CONFIG["traps"]["chance"]:
                    floor_desc += "\t" + w_random(config.TABLES["trap_severity"]) + " trap. Triggered by " + w_random(config.TABLES["trap_trigger"]) + ". Effect: " \
                        + w_random(config.TABLES["trap_effect"]) + ".\n"
                count += 1

            # Trick
            if random.randint(0, 100) < config.CONFIG["tricks"]["chance"]:
                floor_desc += "\tTrick: " + w_random(config.TABLES["trick_object"]) + " that " + w_random(config.TABLES["trick_effect"]) + ".\n"
        room_num += 1
    return floor_desc


def generate_floor_events():
    floor_events = "Floor air is " + w_random(config.TABLES["air"]) + ".\n"
    if random.randint(0, 100) < 24:
        floor_events += "On entering this floor the party hears " + w_random(config.TABLES["noises"]) + ".\n"
    return floor_events

writing to file:
    with open(config.OUTPUT_DIR_PREFIX + "Floor" + str(current_floor + 1) + ".txt", "w") as room_file:
        room_file.write(desc)
"""
