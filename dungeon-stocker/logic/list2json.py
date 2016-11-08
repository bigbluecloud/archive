"""
Utility file used to convert list files that have duplicate for randoming to
json formatted files that can be used for weighted randoming
TODO - Allow user to specify the directory and have the code iterate through
        all files in that directory
"""

import os
import json
from collections import Counter

FILE_LIST = [
            "obstacle",
            "trap_effect",
            "trap_severity",
            "trap_trigger",
            "trick_effect",
            "trick_object",
            "odor",
            "noises",
            "general_furnishings",
            "mage_furnishings"
        ]
SUFFIX = ".json"
DIRECTORY = "table_files/old_tablefiles/"

def workFiles():
    global FILE_LIST
    for f in FILE_LIST:
        currentList = []

        with open(DIRECTORY + f, "r") as data_file:
            currentList = data_file.read().splitlines()

        jsonFormat = Counter(currentList)  # Count unique entries

        with open(f + SUFFIX, "w") as fd:
            fd.write(json.dumps(jsonFormat, sort_keys=True, indent=4))


if __name__ == "__main__":
    workFiles()
