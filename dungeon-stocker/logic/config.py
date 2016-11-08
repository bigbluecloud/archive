import os
import sys
import json
from collections import OrderedDict

CONFIG_FILE = "logic/config_default.json"
CONFIG = {}

OUTPUT_DIR_PREFIX = "dungeon/"
TABLE_DIR_PREFIX = "static/table_files/"
FILE_SUFFIX = ".json"
FILE_LIST = [
            "air",
            "books",
            "chamber_state",
            "general_features",
            "general_furnishings",
            "mage_furnishings",
            "noises",
            "obstacle",
            "odor",
            "trap_effect",
            "trap_severity",
            "trap_trigger",
            "trick_effect",
            "trick_object"
        ]
TABLES = {}
THREAD_NUM = 4


def load_stocker_config():
    global CONFIG
    with open(CONFIG_FILE, "r") as conf:
        CONFIG = json.load(conf)


def load_tables():
    global TABLES
    for f in FILE_LIST:
        data = {}
        #  Retrieve data
        with open(TABLE_DIR_PREFIX + f + FILE_SUFFIX, "r") as fd:
            data = json.load(fd, object_pairs_hook=OrderedDict)

        #  Build weight distribution
        cumulative_weights = []
        weight_total = 0;
        choices = []
        for key in data:
            weight_total += data[key]
            cumulative_weights.append(weight_total)
            choices.append(key)
        #  Insert the lists into the tables dict
        TABLES[f] = {"weight_total": weight_total,
                    "weightings": cumulative_weights,
                    "choices": choices}

if __name__ == "__main__":
    print("To run dungeon_stocker invoke the stocker.py file.")
