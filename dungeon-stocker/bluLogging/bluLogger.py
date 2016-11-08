import os
import json
import logging
import logging.config

_default_msg_fmt = "[ {asctime} ] {name:10} {levelname:14} {message}"
_default_date_fmt = "%Y/%m/%d %H:%M:%S"
_default_config_path = "default_bluLog_config.json"


#  Loads the default config file provided with the module; default_config.json
def load_default_config():
    config_path = ""
    if os.path.exists("bluLogging/"):  # Module path is a child directory
        config_path = "bluLogging/" + _default_config_path
    else:
        config_path = _default_config_path
    with open(config_path, "r") as fd:
        logging.config.dictConfig(json.load(fd))


#  Allows the user to specify a different JSON config file and load it
def load_custom_json_config(file_path):
    with open(file_path, "r") as fd:
        logging.config.dictConfig(json.load(fd))


#  Allows the basic config to be specified in python code
#  At minimum the user only has to specify the file name they want to write to
def log_set_basic_config(log_name, log_mode="a", log_style="{",
                        log_level=logging.INFO, log_format=_default_msg_fmt,
                        log_datefmt=_default_date_fmt):
        logging.basicConfig(filename=log_name, filemode=log_mode,
                            style=log_style, level=log_level,
                            format=log_format, datefmt=log_datefmt)


if __name__ == "__main__":
    if not(os.path.exists("logs/")):
        os.mkdir("logs/")

    #  Really simple test code to ensure that the logging is working as intended
    load_default_config()

    logger_list = [
        "debug_logger",
        "info_logger",
        "warn_logger",
        "error_logger",
        "root"
        ]
    log_levels = {
        10: "DEBUG",
        20: "INFO",
        30: "WARN",
        40: "ERROR",
        50: "CRITICAL"
        }

    for log_name in logger_list:
        logger = logging.getLogger(log_name)
        for level in log_levels:
            logger.log(level, "Message: " + log_levels[level])

else:
    if not(os.path.exists("logs/")):
        os.mkdir("logs/")
