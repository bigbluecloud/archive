import os
import json
import logging

# import timing  # Currently prints time to console, find a way to log it
from flask import Flask, url_for, render_template, request

from bluLogging import bluLogger
from bluThreading import bluThreads
from logic import config, stockerGenerator

__author__ = 'bigbluecloud'
app = Flask(__name__)
logger = None


def root_path():
    return os.path.dirname(__file__)


@app.before_first_request  # Run setup on server launch
def setup():
    global logger
    bluLogger.load_default_config()
    logger = logging.getLogger("debug_logger")

    logger.info("Loading config file...")
    config.load_stocker_config()
    logger.info("Loading random tables...")
    config.load_tables()
    logger.info("Done!")


@app.route('/')
def home():
    return render_template('index.html')


@app.route('/create')
def create_dungeon():
    floor_dict = {"floors": {}}

    if not os.path.exists(config.OUTPUT_DIR_PREFIX):
        os.mkdir(config.OUTPUT_DIR_PREFIX)

    logger.debug("Creating dungeon...")
    bluThreads.start_threaded_job(stockerGenerator.create_floor,
        thread_num=config.THREAD_NUM, **floor_dict)
    bluThreads.wait_for_threads(logger=logger)
    logger.debug("Done!")
    return json.dumps(floor_dict["floors"], sort_keys=True,indent=4)


if __name__ == "__main__":
    print("To run Dungeon Stocker please use startup.sh")
